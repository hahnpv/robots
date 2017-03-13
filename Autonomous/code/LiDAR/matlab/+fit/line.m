classdef line < handle
    properties
        normVector
    end
    methods
        function out = fit(self,sample)
            %% FIT fits a test function to the sample set
            kLine = sample(2,:)-sample(1,:);% two points relative distance
            kLineNorm = kLine/norm(kLine);
            self.normVector = [-kLineNorm(2),kLineNorm(1)];%Ax+By+C=0 A=-kLineNorm(2),B=kLineNorm(1)
            out.m = -self.normVector(1)/self.normVector(2);
            out.b = sample(1,2)-out.m*sample(1,1);
        end
        
        function [inlierIdx, inlierNum] = test(self, threshDist, number, data, sample)
            %% TEST is a test function which tests data points against a fit
            distance = self.normVector*(data - repmat(sample(1,:)',1,number)')'; % TODO fix these transposes!
            inlierIdx = find(abs(distance)<=threshDist);
            inlierNum = length(inlierIdx);
        end
    end
    methods (Static)
        function [update, endpoints] = merge(pa, frame, endpoints, fit)
            %% MERGE will merge features which are subsets of a single feature
            %% line_merge finds collinear lines and recalculates endpoints
            % TODO: this will be errant until we use bool vecs instead of rangesSim for
            % endpoints. Islands will get mixed in.
            
            update = false;
            
            %diff consecutive lines; small error = collinear
            dF = abs(difc(fit));        % delta
            pct = dF ./ fit;            % percent
            threshold = 1;             % percent
            meets_threshold = abs(pct(:,:)) < threshold;    % TODO got to be a simpler way ...
            meets_threshold = bitand(meets_threshold(:,1),meets_threshold(:,2));
            
            if any(meets_threshold)
                update = true;
                for i = numel(meets_threshold):-1:1
                    if meets_threshold(i) == 1
                        ip1 = i+1;
                        if ip1 > size(endpoints,1)
                            ip1 = 1;
                        end
                        % merge endpoints
                        endpoints(i,2) = endpoints(ip1,2);
                        endpoints(ip1,:) = [];
                    end
                end
            end
            
            
            % alternative TODO implement and test
            for i = 1:size(endpoints,1)
                for j = 1:size(endpoints,1)
                    if i~=j
                        % TODO another method to merge collinear lines
                        % http://programmers.stackexchange.com/questions/230682/algorithm-for-detection-of-overlapping-between-vectors
                        %               P1 = P2
                        %    => A1 + k1·D1 = A2 + k2·D2
                        %    =>          0 = (A2 - A1) + (-k1·D1 ) + k2·D2
                        % d1, d2 would be opposing endpoints from successive lines
                    end
                end
            end
        end
    end
end