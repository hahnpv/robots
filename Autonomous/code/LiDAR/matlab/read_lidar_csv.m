function frames = read_lidar_csv(fname, varargin)
parser = inputParser;
parser.addParameter('Mask',                 [],@(x)isempty(x)||isvector(x));
parser.addParameter('CompleteFramesOnly', true,@isscalar);
parser.addParameter('FrameRange',           [],@isvector);
parser.addParameter('InvalidFrames',      true,@isscalar);
parser.addParameter('Statistics',         true,@isscalar);
parser.parse(varargin{:});

LiDAR = csvread(fname);

frame_start = cellfun(@(x) x<0, num2cell(diff(LiDAR(:,1))));
frame_indx  = find(frame_start)+1;
frame_size  = diff(frame_indx);

%% Build frames
%j = 1;
frames = cell(numel(frame_size),1);
for i = 1:numel(frame_size)
    frames{i} = LiDAR(frame_indx(i):frame_indx(i+1)-1,:);
end

%% Select frame range if desired
if parser.Results.FrameRange
    frames = frames{parser.Results.FrameRange};
end

%% Downselect to complete frames if desired
if parser.Results.CompleteFramesOnly
    frames(frame_size~=360) = [];
end


%% Mask out angles if desired
if parser.Results.Mask
    for i = 1:numel(frames)
        frames{i}(parser.Results.Mask,:) = [];
    end
end


%% Remove invalid data if desired
if parser.Results.InvalidFrames
    for i = 1:numel(frames)
        invalid = logical(frames{i}(:,4));
        frames{i}(invalid,:) = [];
    end
end


if parser.Results.Statistics
    for i = 1:numel(frames)
        readings = numel(frames{i}(:,1));
        invalid  = frames{i}(:,4);
        strength = frames{i}(:,5);        
        disp(['Frame ' num2str(i) ': ' num2str(readings) ' data points, ' num2str(sum(invalid)) ' invalid, ' num2str(sum(strength)) ' strength warnings']);
    end
end
end