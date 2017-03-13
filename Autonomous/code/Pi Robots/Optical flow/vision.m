% matlab script in process to develop coplotting capability

width  = 640;
height = 480;
cols   = (width/16) + 1; % 16+1, there's always an extra column
rows   = (height/16);    % 16
frames = 198;

m     = memmapfile('motion.data','Format',{'int8',[1],'x'; 'int8',[1],'y';'uint16',[1],'SAD'});
data  = m.Data;
array = reshape(m.Data, [rows, cols, frames]);

x    = reshape([data.x], [rows, cols, frames]);
y    = reshape([data.y], [rows, cols, frames]);
SAD  = reshape([data.SAD], [rows, cols, frames]);

for i = 1:frames
    quiver(x(:,:,i),y(:,:,i));
    drawnow;
    disp(['frame ' num2str(i)]);
    pause(0.05);
end


for i = 1:frames
    surf(SAD(:,:,i));
    drawnow;
    disp(['frame ' num2str(i)]);
    pause(0.05);
end



%{
v = VideoReader('motion.mp4')
x = readFrame(v,1);
x = readFrame(v);
movie(x)
%}

%{ Richard's example code: 
xyloObj = VideoReader('testvideo.mp4');
% xyloObj = VideoReader('testvideo_one_MECO.mp4');
nFrames = xyloObj.NumberOfFrames;
vidHeight = xyloObj.Height;
vidWidth = xyloObj.Width;
total=vidHeight*vidWidth;
vidtime=1/29.48*[1:nFrames];
writerObj = VideoWriter('H1R1b.avi');
open(writerObj);
set(gca,'nextplot','replacechildren');
set(gcf,'Renderer','zbuffer');
buildframes=[nFrames/2:5:nFrames*3/4];
for i=1:size(buildframes,2),
    frm=read(xyloObj,buildframes(i));
    tt(i,1)=1.0/total*sum(sum(frm(:,:,1))');
    tt(i,2)=1.0/total*sum(sum(frm(:,:,2))');
    tt(i,3)=1.0/total*sum(sum(frm(:,:,3))');
subplot(4,1,[1:3])
h=image(frm); hold all;axis off;
subplot(4,1,4)
plot(tt(1:i,:));xlim([0,size(buildframes,2)]);ylim([65,85]);axis on;
xlabel('frames/5');
frame=getframe(gcf);
writeVideo(writerObj,frame);
end;
close(writerObj);
% save('movie_data');
% movie2avi(M,'test_movie'
);
%}
