
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
% All other inquiries should be directed to the author, Mantas Gabrielaitis,
% mgabriel@ist.ac.at



function [m_out, e_out, iter] = f_ap_basic (s, Par, Ub, ix)
% >
% S H O R T   D E S C R I P T I O N
%
% Calculates the modulator of a signal by using the AP-Basic algorithm. Signals
% defined in up to 3 dimensions are allowed.
%
%
% I N P U T   A R G U M E N T S
%
% [s] - input signal. This is a real array of a chosen dimension D ≤ 3.
%
% [Par] - parameters characterizing the signal and demodulation procedure. This is a
%         variable of the structure type. Its fields are as follows:
%
%         .Fs - sampling frequencies for each dimension of the signal. This is an
%               array of D elements.
%
%         .Fc - cutoff frequencies of the modulator for each dimension of the signal.
%               This is an array of D elements.
%
%         .Et - infeasibility error tolerance used to control the termination of
%               the demodulation algorithm. The iterative process is stopped when the
%               infeasibility error, ϵ, drops to the level of .Et or below. If
%               .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
%               below), is completed.
%
%         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
%               chosen AP algorithm is iterated not more than .Ni times independent
%               of whether ϵ drops at or below .Et.
%
%         .Ns   numbers of sample points of the original input signal (before any
%               possible interpolation) in every dimension. This is an array with the
%               number of elements equal to the number of signal dimensions.
%
%         .Nx - numbers of sample points of the provided input signal (possibly
%               interpolated) in every dimension. This is an array with the number
%               of elements equal to the number of signal dimensions.
%
%         .im - array with the iteration numbers at which the modulator estimates 
%               have to be saved for the output. If .im is empty, only the final
%               modulator estimate is saved. The same is true when numel(i.im)=1 and
%               .im(1)=.Ni. This field is optional (.im=[] is assumed by default).
%
%         .ie - array with the iteration numbers at which the infeasibility error, ϵ,
%               values have to be saved for the output. If .ie is empty, only the
%               final error is saved. The same is true when numel(i.im)=1 and
%               .im(1)=.Ni. This field is optional (.ie=[] is assumed by default).
%
% [Ub] - array with the upper bound constraints on the modulator. If Ub is an empty
%        array, no upper bound constraint is assumed. Otherwise, Ub must be an array
%        with the same dimensions as s. This input argument is optional (the default
%        is Ub=[]).
%
% [ix] - linear indexes of the elements of an interpolated signal corresponding to
%        every element of the original signal, i.e., mapping between the linear
%        indexes of the original and interpolated signals.
%
%
% O U T P U T   A R G U M E N T S
%
% [m_out] - modulator estimates at predefined interations (D+1 array).
%
% [e_out] - infeasibility error estimates at predefined iterations (1D array).
%
% [Ni_out] - number of iterations actually used (scalar).



% [1] DEFINITION AND INITIALIZATION OF RELEVANT VARIABLES

% Dimensionality of the input signal

D = sum(size(s)~=1);



% Numbers of sample points of the original and possibly interpolated signals

ns = prod(Par.Ns);

nx = prod(Par.Nx);



% Full-wave rectification and scaling of the signal

if D == 1
    
    s = s(:);
    
end

s_abs = abs(s); % absolute value of the signal

max_s = max(s_abs(:)); % maximum of the absolute-value signal

s_abs = s_abs / max_s; % scaled absolute signal

if ~isempty(Ub)
    
    Ub = Ub / max_s;
    
end



% Frequencies to filter out with the projection onto Mw

fL = 1 + ceil(Par.Fc(:) ./ (Par.Fs(:)./Par.Nx));

fR = Par.Nx - fL + 2;

i_cutoff = cell(D,1);

for i=1:D

    i_cutoff{i} = fL(i)+1:fR(i)-1;
    
end


% Initialization of the infeasibility error variable

if Par.Et > 0

    Etol = (Par.Et / max_s)^2;
    
else
    
    Etol = Par.Et;
    
end

E = sum(s_abs(:).^2) / nx;



% Initialization of the output arguments and related variables

iter_m = 1; % iteration variable for storing the modulator estimates

nim = numel(Par.im);


m_out = zeros([Par.Ns; max(numel(Par.im),1)]'); % output array with modulator estimates 

if ~isempty(Par.im) && Par.im(iter_m) == 1
    
    m_out(1+(iter_m-1)*nx:iter_m*nx) = s_abs;
    
    iter_m = iter_m + 1;
    
end



iter_e = 1; % iteration variable for storing the infeasibility errors

nie = numel(Par.ie);


e_out = zeros(numel(Par.ie),1); % output array with the infeasibility errors

if ~isempty(Par.ie) && Par.ie(iter_e) == 1
    
    e_out(iter_e) = E;
    
    iter_e = iter_e + 1;
    
end



% [3] CALCULATIONS

iter = 0;

m = s_abs;

    
while E > Etol && iter < Par.Ni

    iter = iter + 1;


    % Projection onto Mw

    a = m;

    a = fftn(a);

    if D == 1

        a(i_cutoff{1}) = 0;

    elseif D == 2

        a(i_cutoff{1},:) = 0;

        a(:,i_cutoff{2}) = 0;

    elseif D == 3

        a(i_cutoff{1},:,:) = 0;

        a(:,i_cutoff{2},:) = 0;

        a(:,:,i_cutoff{3}) = 0;

    end

    a = ifftn(a);


    % Projection onto Cd

    m = a;

    m(m < s_abs) = s_abs(m < s_abs);

    if ~isempty(Ub)

        m(m > Ub) = Ub(m > Ub);

    end


    % Infeasibility error

    E = sum((a(:)-m(:)).^2) / nx;


    % Output (modulator)

    if iter_m <= nim && (iter == Par.im(iter_m) || (E <= Etol && nim == 1 && ...
            Par.im(1) == Par.Ni))

        m_out(1+(iter_m-1)*ns:iter_m*ns) = m(abs(ix));

        iter_m = iter_m + 1;

    end


    % Output (infeasibility error)

    if iter_e <= nie && (iter == Par.ie(iter_e) || (E <= Etol && nie == 1 && ...
            Par.ie(1) == Par.Ni))

        e_out(iter_e) = E;

        iter_e = iter_e + 1;

    end

end



% [4] FINAL OUTPUT

% If only the final estimate of the modulator is requested

if isempty(Par.im)
        
    m_out(:) = m(abs(ix));
        
end


% Scaling back the modulator estimates

m_out = max_s * m_out;


if isempty(Par.ie)
    
    
    e_out = E;
    
end


% Scaling back the error estimates

e_out = max_s * sqrt(e_out);

