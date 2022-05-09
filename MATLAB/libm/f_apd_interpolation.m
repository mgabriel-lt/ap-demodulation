
%                       C O P Y R I G H T   N O T I C E
%
% Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
% All Rights Reserved. The underlying technology is protected by PCT Patent
% Application No. PCT/EP2021/054650.
%
% This file is part of the AP Demodulation library, which is free software: you can
% redistribute it and/or modify it under the terms of the GNU General Public License
% as published by the Free Software Foundation in version 2.
%
% This program is distributed in the hope that it will be useful, but WITHOUT ANY
% WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
% PARTICULAR PURPOSE. See the GNU General Public License v2 for more details. You
% should have received a copy of the GNU General Public License v2 along with this
% program. If not, see https://www.gnu.org/licenses/.
%
% Contact the Technology Transfer Office, IST Austria, Am Campus 1,
% A-3400 Klosterneuburg, Austria, +43-(0)2243 9000, twist@ist.ac.at, for commercial
% licensing opportunities.
%
% See https://github.com/mgabriel-lt/ap-demodulation for the latest version of the
% code and user-friendly explanations on the working principle, domains of
% application, and advice on the usage of different AP Demodulation algorithms in
% practice.



function [out_s, out_Ub, out_ix] = f_apd_interpolation (s, Par, Ub, t)
% >
% P U R P O S E
%
% Interpolates the input signal on a refined uniform grid following Eq. 23 in the
% paper cited in the header of the present file).
%
%
% I N P U T   A R G U M E N T S
%
% [s] - input signal.
%
% [Par] - parameters characterizing the signal and demodulation procedure. This is a
%         variable of the structure type. Its fields are as follows:
%
%         .Nr - numbers of sample points on the refined uniform grid used for
%               interpolating nonuniformly sampled signals. This is an array of D
%               elements.
%
% [Ub] - array with the upper bound constraints on the modulator. This array must be
%        empty or have the same number of elements as the input signal array, s.
%
% [t] - sampling coordinates. This is a real 2D array, with the number of rows equal
%       to the number of signal sample points, and the number of columns equal to the
%       dimension of the input signal.
%
%
% O U T P U T   A R G U M E N T S
%
% [out_s] - interpolated signal.
%
% [out_Ub] - interpolated upper bounds on the modulator.
%
% [out_ix] - linear indexes of the elements of an interpolated signal corresponding
%            to every element of the original signal, i.e., mapping between the
%            linear indexes of the original and interpolated signals.
%
%
% M A T L A B - E X T E R N A L   F U N C T I O N S   U S E D
%
% None.



% Overall length of the original signal

ns = numel(s); 
    


% Cumulative product of the dimensions of the interpolated signal

cumNr = cumprod(Par.Nr(1:end-1));

cumNr = cumNr(:);
    


% New and old arrays of the signal and modulator's upper bound

if numel(Par.Nr) == 1

    out_s = zeros(Par.Nr,1);

else

    out_s = zeros(Par.Nr);

end
    
if ~isempty(Ub)

    if numel(Par.Nr) == 1

        out_Ub = zeros(Par.Nr,1) + Inf;

    else

        out_Ub = zeros(Par.Nr) + Inf;

    end
    
else
    
    out_Ub = Ub;

end
    
    

% Bounds and step sizes of the time coordinates of the interpolated signal sample
% points

t = t';

tmin = min(t,[],2);

dt = (max(t,[],2) - tmin) ./ (Par.Nr(:) - 1);



% Interpolation

i_all = 1:ns;

r_all = zeros(1,ns);

out_ix = zeros(1,ns);
    
    
for i=1:ns

    % The closest point on the new grid
    
    ix = round((t(:,i)-tmin) ./ dt) + 1;
    
    
    % The correponding distance
    
    r = norm((t(:,i)-tmin) - (ix-1).*dt); 

    
    % Linear index of the new point
    
    if sum(size(out_s)~=1) > 1

        ix = ix(1) + (ix(2:end)-1)' * cumNr; 

    end
    
    
    % If the current closest point on the new grid has not been assigned yet, it is
    % assigned the value of the current point on the old grid. Otherwise, the current
    % closest point on the new grid is reassigned, if additionally, the distance to
    % the current point on the old grid is smaller than the previously assigned one.

    if ~any(out_ix(1:i-1) == ix)

        out_s(ix) = s(i);

        if ~isempty(out_Ub)

            out_Ub(ix) = Ub(i);

        end

        out_ix(i) = ix;

        r_all(i) = r;

    else

        i_old = i_all(out_ix(1:i-1) == ix);

        if r < r_all(i_old)

            out_s(ix) = s(i);

            if ~isempty(out_Ub)

                out_Ub(ix) = Ub(i);

            end

            out_ix(i) = ix;

            r_all(i) = r;

            out_ix(i_old) = -out_ix(i_old); % eliminates from further comparisons

        else

            out_ix(i) = -out_ix(i); % eliminates from further comparisons

        end

    end

end

