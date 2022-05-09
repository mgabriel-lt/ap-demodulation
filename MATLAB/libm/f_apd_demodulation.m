
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



function [m_out, e_out, iter, tcpu] = f_apd_demodulation (s, Par, Ub, t)
% >
% P U R P O S E
%
% Performs demodulation of the input signal in the offline mode by using the
% AP Demodulation approach formulated and developed in
%
%    M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband signals,"
%    IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
%    10.1109/TSP.2021.3087899.
%
%
% I N P U T   A R G U M E N T S
%
% [s] - input signal. This is a real array of a chosen dimension D ≤ 3. If the input
%       signal is nonuniformly sampled, and the coordinates of the sample points are
%       provided via the fourth input argument, t (see below), s must be a 1D array,
%       independent of the true dimensionality of the signal.
%
% [Par] - parameters characterizing the signal and demodulation procedure. This is a
%         variable of the structure type. Its fields are as follows:
%
%         .Al - demodulation algorithm. Possible options are: 'B' - AP-Basic,
%               'A' - AP-Accelerated, 'P' - AP-Projected.
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
%         .Nr - numbers of sample points on the refined uniform interpolation grid in
%               every dimension. This is an array of D elements. This field must be
%               defined only if the fourth input argument, t, is provided and is
%               nonempy (see below). Otherwise, .Nr is ignored.
%
%         .Cp - compression parameter. If .Cp > 1, demodulation is performed by using
%               signal compression (see equations (12)-(13) in the paper cited in the
%               header of this file. This field is optional (the default is .Cp=1).
%
%         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
%               when the value of the λ factor drops below one. If .Br=1, premature
%               termination is assumed (default). Otherwise, the AP-A is not stopped
%               premature even when λ decreases below 1. This field is required only
%               if .Al='A'. It is optional (the default is .Br=1).
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
% [Ub] - array with the upper bound constraints on the modulator. Two options are
%        possible. If Ub is an empty array, no upper bound constraint is assumed.
%        Otherwise, Ub must be an array with the same dimensions as s. This input
%        argument is optional (Ub=[] is assumed by default).
%
% [t] - sampling coordinates. This is a real 2D array, with the number of rows equal
%       to the number of signal sample points, and the number of columns equal to the
%       dimension of the input signal. This input argument is optional and should
%       be used only if the input signal is sampled nonuniformly. If t is provided,
%       then the input argument Par must contain the field .Nr, and the input signal
%       arrray, s, must be 1D, independent of the true dimensionality of the signal
%       (see above).
%
%
% O U T P U T   A R G U M E N T S
%
% [m_out] - modulator estimates at predefined interations (D+1 array).
%
% [e_out] - infeasibility error estimates at predefined iteratiHons (1D array).
%
% [iter] - number of iterations actually used (scalar).
%
% [tcpu] - running time of the function in seconds (scalar).
%
%
% M A T L A B - E X T E R N A L   F U N C T I O N S   U S E D
%
% (1) f_apd_input_validation, (2) f_apd_compression, (3) f_apd_interpolation,
%
% (4) f_apd_basic, (5) f_apd_accelerated, (6) f_apd_projected.


tic

% [1] VALIDATION OF INPUT ARGUMENTS AND DEFAULT VALUES FOR UNDEFINED INPUTS

if nargin < 3
    
    Ub = [];
    
end

if nargin < 4
    
    t = [];
    
end

if ~isfield(Par,'Cp') || isempty(Par.Cp)
    
    Par.Cp = 1;
    
end

if ~isfield(Par,'Br') || isempty(Par.Br)
    
    Par.Br = 1;
    
end

if ~isfield(Par,'im')
    
    Par.im = [];
    
end

if ~isfield(Par,'ie')
    
    Par.ie = [];
    
end

if nargin < 3
    
    Ub = [];
    
end

f_apd_input_validation (s, Par, Ub, t)



% [2] DEFINITION AND INITIALIZATION OF RELEVANT VARIABLES

% Lengths of the original and possibly interpolated signal in each dimension

if nargin > 3 && ~isempty(t)

    Par.Ns = numel(s);
    
    Par.Nx = Par.Nr;
    
else
    
    Par.Ns = size(s)';
    
    if sum(Par.Ns~=1) == 1

        Par.Ns = max(Par.Ns);

    end
    
    Par.Nx = Par.Ns;
    
end



% [3] CALCULATIONS

% Compression of the signal (if applicable)

if Par.Cp > 1

    s = f_apd_compression(s, 1/Par.Cp);
    
    if ~isempty(Ub)
        
        Ub = f_apd_compression(Ub, 1/Par.Cp);
        
    end
    
end


% Interpolation of the signal (if applicable).

if nargin > 3 && ~isempty(t)
    
    [s, Ub, ix] = f_apd_interpolation (s, Par, Ub, t);
    
else
    
    ix = 1:numel(s);
    
end


% Demodulation

if Par.Al == 'B'
    
    [m_out, e_out, iter] = f_apd_basic (s, Par, Ub, ix);
    
elseif Par.Al == 'A'
    
    [m_out, e_out, iter] = f_apd_accelerated (s, Par, Ub, ix);
        
elseif Par.Al == 'P'
    
    [m_out, e_out, iter] = f_apd_projected (s, Par, Ub, ix);
        
end


% Decompression

if Par.Cp > 1

    m_out = f_apd_compression(m_out, Par.Cp); % decompression of the signal
    
end


% Computing time

tcpu = toc;

