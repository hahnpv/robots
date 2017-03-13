function [bestFit,bestInlierIdx] = ransac(data,num,iter,threshDist,inlierRatio,FitClass)
% Source: https://en.wikipedia.org/wiki/RANSAC#Algorithm
% data: a 2xn dataset with #n data points
% num: the minimum number of points. For line fitting problem, num=2
% iter: the number of iterations
% threshDist: the threshold of the distances between points and the fitting line
% inlierRatio: the threshold of the number of inliers
number = size(data,1); % Total number of points
bestInNum = 0; % Best fitting line with largest number of inliers
for i=1:iter
    %% Randomly select 2 points
    idx = randperm(number,num); sample = data(idx,:);
    
    %% Compute the distances between all points with the fitting line
    testFit = FitClass.fit(sample);
    
    %% Compute the inliers with distances smaller than the threshold
    [inlierIdx, inlierNum] = FitClass.test(threshDist, number, data, sample);
    
    %% Update the number of inliers and fitting model if better model is found
    if inlierNum>=round(inlierRatio*number) && inlierNum>bestInNum
        bestInNum = inlierNum;
        bestInlierIdx = inlierIdx;
        bestFit = testFit;
    end
end
end
