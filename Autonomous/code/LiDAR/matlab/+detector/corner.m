function endpoints = corner_detector(pa, frame, endpoints, showPlot, debug)
% TODO a corner detector, pre-input to RANSAC

endpointCount = 1;
for j = 1:size(endpoints,1)
    points = endpoints(j,1):1:endpoints(j,2); % told you bool arrays would be useful
    if ( endpoints(j,1) > endpoints(j,2))     % wraps around
        points = [endpoints(j,1):1:numel(frame(:,1)),1:1:endpoints(j,2)];
    end
    [ymax,imax,ymin,imin] = extrema(frame(points,2));
    if showPlot && debug
        figure; plot(points,frame(points,2));hold all;
        plot(points(imax),ymax,'rd');
        plot(points(imin),ymin,'bd');
    end
    
    % confirm persistence
    localmax = arrayfun(@(y,i) y >= max(frame(max(points(i)-5,1):min(points(i)+5,size(frame,1)),2)),ymax,imax);
    localmin = arrayfun(@(y,i) y <= min(frame(max(points(i)-5,1):min(points(i)+5,size(frame,1)),2)),ymin,imin);
    
    ymax(~localmax) = [];
    ymin(~localmin) = [];
    imax(~localmax) = [];
    imin(~localmin) = [];
    
    nodes = sort([points(imax),points(imin)]);

    % reorder nodes
    i = find(nodes>endpoints(j,1),1);
    nodes = circshift(nodes',[numel(nodes)-i+1, 0]); % this syntax is important
    
    %elminiate matches if the exist
    nodes(nodes==endpoints(j,1)) = [];
    nodes(nodes==endpoints(j,2)) = [];

    if(showPlot)
        [x,y] = pol2cart(frame(nodes,1)*C_DEG,double(frame(nodes,2)));
        scatter(pa, x, y, 250, 'ko');
    end
    
    % add to breakpoints
    if any(nodes)
        for i = 1:numel(nodes)
            if i == 1
                revised_endpoints(endpointCount,:) = [endpoints(j,1) nodes(i)];
                endpointCount = endpointCount + 1;
            end
            if i == numel(nodes)
                revised_endpoints(endpointCount,:) = [nodes(i) endpoints(j,2)];
                endpointCount = endpointCount + 1;
            else
                revised_endpoints(endpointCount,:) = nodes(i:i+1);
                endpointCount = endpointCount + 1;
            end
        end
    else
        revised_endpoints(endpointCount,:) = endpoints(j,:);
    end
end
endpoints = revised_endpoints;
end