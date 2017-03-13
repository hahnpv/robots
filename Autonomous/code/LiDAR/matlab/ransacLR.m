function [bestFit,bestInlierIdx] = ransacLR(dataL,dataR,num,iter,threshDist,inlierRatio,FitClass)
%% TODO: rename. Google. Sure this has been done before
% this version of RANSAC compares two different inputs
% Source: https://en.wikipedia.org/wiki/RANSAC#Algorithm
% data: a 2xn dataset with #n data points
% num: the minimum number of points. For line fitting problem, num=2
% iter: the number of iterations
% threshDist: the threshold of the distances between points and the fitting line
% inlierRatio: the threshold of the number of inliers
%assert(size(dataL)==size(dataR));       % TODO this doesnt have to be true
number = size(dataL,1); % Total number of points
bestInNum = 0; % Best fitting line with largest number of inliers
for i=1:iter
    %% Randomly select points
    idx = randperm(number,num); sampleL = dataL(idx,:);
    idx = randperm(number,num); sampleR = dataR(idx,:);
    
    %% Fit the function to the selected points
    testFit = FitClass.fit(sampleL, sampleR);
    
    %% Compute the inliers with distances smaller than the threshold
    [inlierIdx, inlierNum] = FitClass.test(threshDist, number, dataL, dataR, sampleL, sampleR);
    
    %% Update the number of inliers and fitting model if better model is found
    if inlierNum>=round(inlierRatio*number) && inlierNum>bestInNum
        bestInNum = inlierNum;
        bestInlierIdx = inlierIdx;
        bestFit = testFit;
    end
end
end
