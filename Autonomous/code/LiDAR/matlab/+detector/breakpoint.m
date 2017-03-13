function endpoints = breakpoint_detector(pa, frame, varargin)
%% breakpoint_detector creates initial line segments for further analysis
% This isn't meant to be the be-all end-all of line detection it merely
% provides segments broken by major jumps in the LiDAR scan which can then
% be subdivided if necessary and parsed into features
% Breakpoints themselves arent even authoritative a smart algorithm might
% take a line segment found within a breakpoint and discover points outside
% the breakpoints which fit the feature, the breakpoint was errant due to
% low signal return (surface) or data dropout or a nearer feature obscuring
% continuity.
%
% TL;DR: Give your algorithms a good guess and don't tune this too hard.
%
% This function works unless there is a data mask. TODO fix to be general
%
% ExtendEnds true takes repeat left/right nodes and extends them. If not true,
% ExtendEnds will treat them as islands
parser = inputParser;
parser.addParameter('ExtendEnds',  true,@isvector); % currently hardcoded to extend
parser.addParameter('Plot',        true,@isscalar);
parser.parse(varargin{:});

r        = frame(:,2);
[x,y]    = pol2cart(frame(:,1)*C_DEG,double(frame(:,2)));

dx = difc(x);
dy = difc(y);
dl = sqrt(dx.^2 + dy.^2);
threshold = 0.05 .* r;
indxr = arrayfun(@(x,y) x>y, dl,threshold);                    % right sided
indxl = arrayfun(@(x,y) x>y,[dl(end); dl(1:end-1)],threshold); % left sided

islands = bitand(indxl, indxr);             % island nodes have no connection

% remove islands from left and right
left  = find(indxl - islands);
right = find(indxr - islands);

%% this by default extends the repeat edge nodes out
ixl = arrayfun(@(x) x==1,diff([0; indxl-islands]));     % this is identical to il
backwards = diff([0; indxr(end:-1:1)-islands(end:-1:1)]);    % TODO got to be a better way!
ixr = arrayfun(@(x) x==1,backwards(end:-1:1));

left = find(ixl);
right = find(ixr);
all = sort([left;right]);
    
[~, ir, ~] = intersect(all,right);      % TODO make bitops
[~, il, ~] = intersect(all,left);

ir = [0; arrayfun(@(x) x==1,diff(ir))];
il = [arrayfun(@(x) x==1,diff(il)); 0];
if any(ir)                      % TODO remove this later; for now it lets us breakpoint right/left
    right = right(~ir);         % todo verify shift is proper
end
if any(il)
    left = left(~il);
end

if parser.Results.Plot
    scatter(pa,x(left(:,1)),y(left(:,1)),250,'ro'); hold all; % left
    scatter(pa,x(right(:,1)),y(right(:,1)),250,'rx'); hold all; % right
    text(x(left),y(left),num2str(left))
    text(x(right),y(right),num2str(right))
end

% Shift left vector if a feature wraps around
if left(1) > right(1)
    left = [left(end); left(1:end-1)];
end
    
endpoints = [left,right];       % TODO make array of bool vecs to deal w/distontinuities etc. frame
end
