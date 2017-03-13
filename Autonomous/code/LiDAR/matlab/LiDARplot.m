function LiDARplot(varargin)
parser = inputParser;
parser.addParameter('Mask',                         [],@(x)isempty(x)||isvector(x));
parser.addParameter('Filename', '../data/bathroom.csv',@isstr);
parser.addParameter('Animate',                   false,@isscalar);
parser.addParameter('Statistics',                false,@isscalar);
parser.parse(varargin{:});

%frames = read_lidar_csv('data/frontroom/capture.csv','Mask',200:360); % poor quality
%frames = read_lidar_csv('data/motion/capture.csv','Mask',200:360);
%frames = read_lidar_csv('data/bathroom/capture.csv');
%frames = read_lidar_csv('data/hallway/capture.csv');

frames = read_lidar_csv(parser.Results.Filename,...
                        'Mask',parser.Results.Mask);

%% Plot complete frames

figure;
pa = gca; %fig.CurrentAxes;
axis equal;
axis([-5000,2000,-2000,4000])
mode = 'mapping';      %  motion; mapping; movement

vidObj = VideoWriter('lidar.avi');
if parser.Results.Animate
    vidObj.FrameRate = 5;
    open(vidObj);
end
for i = 1:numel(frames) % frame 7 has issue which needs fixing numel(frame)
    switch mode
        case 'motion'       % Motion tracker
            % TBD... anchor a room then track moving actor
            % likely want to track outliers from ransac fits and test
            % if they significantly deviate, then track that as a separate
            % feature
        case 'mapping'      % Map out the environment
            cla
            plot.frame(pa, frames{i});
            %            endpoints = detector.breakpoint(pa, frames{i});
            endpoints = [1,size(frames{i},1)]; % grr this performs better than breakpont_detector... look for bugs
            endpoints = detector.corner(pa, frames{i}, endpoints, true, false);
            update = true;
            while update==true
                linefit = lidar_ransac(pa, endpoints,frames{i},true);
                [update, endpoints] = fit.line.merge(pa, frames{i}, endpoints, linefit);
                if update
                    disp('Need to re-ransac') % note we're recalcing all just need
                end                           % to re-calc merges
            end
            disp(['Completed Frame ' num2str(i)]);
            pause(0.005);   % enough to refresh figure prior to cla
        case 'replay'       % Replay a video of motion captured
            plot_frame(pa, frames{i});
            pause(0.2)      % quasi-real time as spin rate is 5 Hz
    end
    
    if parser.Results.Animate
        writeVideo(vidObj,getframe);
    end
end
if parser.Results.Animate
    close(vidObj);
end
end

function linefit = lidar_ransac(pa, endpoints, frame, showPlot)
%% lidar_ransac takes enpoints and runs a line fitting ransac algorithm
% eventually this will be generalized into a generalized ransac algorithm
% with several different fit functions
linefit = zeros(size(endpoints));
for j = 1:size(endpoints,1)
    points = endpoints(j,1):1:endpoints(j,2); % told you bool arrays would be useful
    if ( endpoints(j,1) > endpoints(j,2))     % wraps around
        points = [endpoints(j,1):1:numel(frame(:,1)),1:1:endpoints(j,2)];
    end
    [x,y] = pol2cart(frame(points,1)*C_DEG,double(frame(points,2)));
    data = [x,y];
    
    % TODO assess other fit models for lines
    % TODO assess non-line fits (curves, hulls, etc.)
    [output,inliers] = ransac(data,2,1000,25,0.25,fit.line);
    
    m = output.m;   % TODO refactor this
    b = output.b;
    
    if showPlot
        xAxis = data(inliers,1);
        yAxis = m*xAxis + b;
        plot(pa,xAxis,yAxis,'r-','LineWidth',2);
        plot(data(inliers,1),data(inliers,2),'rx');
    end
    
    linefit(j,:) = [m, b];
end
end