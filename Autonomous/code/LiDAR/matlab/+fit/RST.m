classdef RST < handle
    %% RST provides a RANSAC method for testing rotation, scaling, translation
    % between two point clouds or feature sets of LiDAR data. Limitation is
    % mapping i:i between sets, so if features or points are misordered you
    % won't be able to find a valid fit. You can massage to a point but
    % even an off by one error will make it impossible to match. Adding an 
    % outer loop to roll different vec start/end points is costly This is of
    % limited practical utility.
    properties
        phi;
        T;
    end
    methods
        function out = fit(self, X1, X2)
            %{
            4 %
            5 % DESC:
            6 % computes the RST transformation between the point pairs X1, X2
            7 %
            8 % VERSION:
            9 % 1.0.1
            10 %
            11 % INPUT:
            12 % X1, X2 = point matches (cartesian coordinates)
            13 % normalization = true (default) or false to enable/disable point
            14 % normalzation
            15 %
            16 % OUTPUT:
            17 % H = homography representing the RST transformation
            18 % s = scaling
            19 % phi = rotation angle
            20 % T = translation vector
            21
            22 % ADAPTED FROM:
            23 % AUTHOR:
            24 % Marco Zuliani, email: marco.zuliani@gmail.com
            25 % Copyright (C) 2011 by Marco Zuliani
            26 %
            27 % LICENSE:
            28 % This toolbox is distributed under the terms of the GNU GPL.
            29 % Please refer to the files COPYING.txt for more information.
            %}
            
            %{
            if (nargin < 3)
                normalization = true;
            end
            %}
            
            N = size(X1, 2);
            
            if (size(X2, 2) ~= N)
                error('RSTLS:inputError', ...
                    'The set of input points should have the same cardinality')
            end
            if N < 2
                error('RSTLS:inputError', ...
                    'At least 2 point correspondences are needed')
            end
            %{
            %% Normalize the input
            if (normalization) && (N > 2)
                % fprintf('nnNormalizing...')
                [X1, T1] = normalize_points(X1);
                [X2, T2] = normalize_points(X2);        % TODO find this if we need it. dont think we do
            end
            %}
            
            %% Esimation
            if (N == 2)
                
                % fast estimation
                Theta = zeros(4,1);
                % 2 additions
                MM = X1(:,1) - X1(:,2);
                % 1 additions, 2 multiplication
                detMM = MM(1)*MM(1) + MM(2)*MM(2);
                % 2 multiplications
                MMi = MM / detMM;
                % 2 additions
                Delta = X2(:,1) - X2(:,2);
                % 1 additions, 2 multiplications
                Theta(1) = MMi(1)*Delta(1) + MMi(2)*Delta(2);
                % 1 additions, 2 multiplications
                Theta(2) = MMi(1)*Delta(2) - MMi(2)*Delta(1);
                % 2 additions, 2 multiplications
                Theta(3) = X2(1,2) - Theta(1)*X1(1,2) + Theta(2)*X1(2,2);
                % 2 additions, 2 multiplications
                Theta(4) = X2(2,2) - Theta(1)*X1(2,2) - Theta(2)*X1(1,2);
                
            else
                a = sum(X1(:).^2);
                Theta(1) = sum( X1(1, :).*X2(1, :) + X1(2, :).*X2(2, :) ) / a;
                Theta(2) = sum( -X1(2, :).*X2(1, :) + X1(1, :).*X2(2, :) ) / a;
                Theta(3) = 0;
                Theta(4) = 0;
            end
            
            % compute the corresponding homography
            H = [Theta(1) -Theta(2) Theta(3); Theta(2) Theta(1) Theta(4); 0 0 1];
            
            %% De-normalize parameters
            %{
            if (normalization) && (N > 2)
                H = T2\H*T1;
            end
            %}
            out.H = H/H(9);
            out.s = sqrt(H(1,1)*H(1,1) + H(2,1)*H(2,1));
            out.phi = atan2(H(2,1), H(1,1));
            out.T = H(1:2, 3);
            
            self.phi = out.phi;
            self.T   = out.T;
        end
        
        function [inlierIdx, inlierNum] = test(self, threshDist, number, dataL, dataR, ~, ~)
            % Will have to be some munging here, convert angle to index and
            % compare where data exists for both sets, drop out points
            % which have no corresponding point in the other set.
            dcm = [ cos(self.phi), sin(self.phi);...
                   -sin(self.phi), cos(self.phi)];

            % TODO should be arrayfun for this?
            for i =1:size(dataR,1)
                dataRR(i,:)= dcm * dataR(i,:)';
            end
            
            dataRR(:,1) = dataRR(:,1) - self.T(1);
            dataRR(:,2) = dataRR(:,2) - self.T(2);
          
            err = dataL - dataRR;
            inlierIdx = [];
            for i =1:size(dataR,1)
                dl = sqrt( err(i,1)^2 + err(i,2)^2);
                if dl < threshDist
                    inlierIdx(end+1) = i;
                end
            end
            inlierNum = numel(inlierIdx);
        end
    end
end
