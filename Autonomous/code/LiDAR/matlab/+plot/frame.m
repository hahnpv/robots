function frame(pa, frame)
% Plot the data, color for invalid/strength warning
% TODO find nearest neighbor would find lines which are broken by spurious
% readings.
C_DEG = pi/180;
[x,y] = pol2cart(frame(:,1)*C_DEG,double(frame(:,2)));
quality  = frame(:,3);
invalid  = frame(:,4);
strength = frame(:,5);
scatter(pa,x,y,25,quality);
hold all
% now plot invaids in red
xx = x(invalid==1);
yy = y(invalid==1);
scatter(pa,xx,yy,25,'ro');
% plot strength warnings in black
xxx = x(strength==1);
yyy = y(strength==1);
scatter(pa,xxx,yyy,25,'ko');
end
