
%                       C O P Y R I G H T   N O T I C E
%
% Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
% All Rights Reserved. The underlying technology is protected by PCT Patent
% Application No. PCT/EP2021/054650.
%
% This file is part of the AP demodulation library, which is free software: you can
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



% C O N T E N T S
%
% f_ap_demodulation (main function),
%
% f_ap_input_validation (local function),
%
% f_ap_compression (local function),
%
% f_ap_interpolation (local function).



function [m_out, e_out, iter, tcpu] = f_ap_demodulation (s, Par, Ub, t)
% >
% S H O R T   D E S C R I P T I O N
%
% f_ap_demodulation performs demodulation of the input signal in the offline mode by
% using the AP demodulation approach formulated and developed in
%
%    M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband signals,"
%    IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
%    10.1109/TSP.2021.3087899
%
% Please cite the above publication when referring to the algorithms implemented in
% this code or the code itself.
%
% See https://github.com/mgabriel-lt/ap-demodulation for the latest version of the
% code and user-friendly explanations on the working principle, domains of
% application, and advice on the usage of different AP demodulation algorithms in
% practice.
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

f_ap_input_validation (s, Par, Ub, t)



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

    s = f_ap_compression(s, 1/Par.Cp);
    
    if ~isempty(Ub)
        
        Ub = f_ap_compression(Ub, 1/Par.Cp);
        
    end
    
end


% Interpolation of the signal (if applicable).

if nargin > 3 && ~isempty(t)
    
    [s, Ub, ix] = f_ap_interpolation (s, Par, Ub, t);
    
else
    
    ix = 1:numel(s);
    
end


% Demodulation

if Par.Al == 'B'
    
    [m_out, e_out, iter] = f_ap_basic (s, Par, Ub, ix);
    
elseif Par.Al == 'A'
    
    [m_out, e_out, iter] = f_ap_accelerated (s, Par, Ub, ix);
        
elseif Par.Al == 'P'
    
    [m_out, e_out, iter] = f_ap_projected (s, Par, Ub, ix);
        
end


% Decompression

if Par.Cp > 1

    m_out = f_ap_compression(m_out, Par.Cp); % decompression of the signal
    
end


% Computing time

tcpu = toc;

end




% LOCAL FUNCTIONS: f_ap_input_validation, f_ap_compression, f_ap_interpolation.


function f_ap_input_validation (s, Par, Ub, t)
% >
% S H O R T   D E S C R I P T I O N
%
% Checks the validity of input arguments for f_ap_demodulation.
%
%
% I N P U T   A R G U M E N T S
%
% [s] - input signal. This is a real array of a chosen dimension D ≤ 3. If the fourth
%       input argument, t, is nonempty, i.e., interpolation of the input signal is
%       required, then s must be a 1D array.
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
%               defined only if the fourth input argument t is provided and is
%               nonempy (see below). Otherwise, .Nr is ignored.
%
%         .Cp - compression parameter.
%
%         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
%               when the value of the λ factor drops below one.
%
%         .im - array with the iteration numbers at which the modulator estimates 
%               have to be saved for the output. If .im is empty, only the final
%               modulator estimate is saved.
%
%         .ie - array with the iteration numbers at which the infeasibility error, ϵ,
%               values have to be saved for the output. If .ie is empty, only the
%               final error is saved.
%
% [Ub] - array with the upper bound constraints on the modulator. This array must be
%        empty or have the same dimensions as the first input argument, s.
%
% [t] - sampling coordinates. This is a real 2D array, with the number of rows equal
%       to the number of signal sample points, and the number of columns equal to the
%       dimension of the input signal. This input argument must be empty if no signal
%       interpolation on a uniform grid is assumed.
%
%
% O U T P U T   A R G U M E N T S
%
% None.



% Dimensionality of the input signal

if isempty(t)
    
    D = sum(size(s)>1);
    
else
    
    D = size(t,2);
    
end



% Checking input argument s

if numel(s) <= 1 || ndims(s) > 3 || ~isreal(s)
    
    error(['The array with the signal samples, s, must be real, not empty, and ',...
        'have no more than 3 dimensions!'])
    
elseif ndims(s)==3 && any(size(s)==1)
    
    error(['If the array with the signal samples, s, is 3D, it must have no ',...
        'singleton dimensions!'])
    
elseif ~isempty(t) && D > 1
    
    error(['If the interpolation of the input signal, s, is assumed, s must be ',...
        'a 1D array!'])
    
end



% Checking fields of the input argument Par

if ~all(isfield(Par, {'Fs','Fc','Al','Et','Ni'}))
    
    error(['The second input argument, Par, must be a structure with fields',...
        '''Fs'', ''Fc'', ''Al'', ''Et'', and ''Ni'' at least!'])
    
elseif ~isempty(t) && ~isfield(Par, 'Nr')
    
    error(['The second input argument, Par, must contain a field ''Nr'' with ',...
        'the size of the interpolated signal array if the fourth input argument ',...
        'is provided!'])
    
end


% .Al
if Par.Al ~= 'B' && Par.Al ~= 'A' && Par.Al ~= 'P' && Par.Al ~= 'R'
    
    error(['The demodulation algorithm, Par.Al, must be one of ',...
        '''B'', ''A'', ''P'', and ''R''!']);
% .Fs
elseif ~isreal(Par.Fs) || any(Par.Fs(:) <= 0) || numel(Par.Fs)~=D
    
    error(['The sampling frequency array , Par.Fs, must consist of positive ', ...
        'real entries whose number is equal to the number of dimensions of the ', ...
        'input signal array, s!'])
% .Fc
elseif ~isreal(Par.Fc) || any(Par.Fc(:) <= 0) || numel(Par.Fc)~=numel(Par.Fs) || ...
        any(Par.Fc(:) > Par.Fs(:)*0.5)
    
    error(['The cutoff frequency array, Par.Fc, must consist of non-negative ', ...
        'real entries whose number is equal to the number of dimensions of the ', ...
        'input signal array, s. Moreover, Par.Fc must not exceed half of Par.Fs', ...
        ' for each entry!'])
% .Et
elseif ~isreal(Par.Et) || ~isscalar(Par.Et)
    
    error('The infeasibility error tolerance, Par.Et, must be a real scalar!')
% .Ni    
elseif Par.Ni < 0 || ~isreal(Par.Ni) || ~isscalar(Par.Ni)
    
    error(['The maximum number of allowed iterations, Par.Ni, must be a ', ...
        'non-negative real scalar!'])
% .Nr
elseif ~isempty(t) && (~isreal(Par.Nr) || any(Par.Nr <= 1) || ...
        any(Par.Nr~=floor(Par.Nr)) || numel(Par.Nr)~=numel(Par.Fs))
    
    error(['The array of numbers of sampling points, Par.Nr, must consist of ',...
        'integers exceeding 1 whose number is equal to the number of dimensions ',...
        'of the input signal array s!'])
% .Cp
elseif Par.Cp < 1 || ~isreal(Par.Cp) || ~isscalar(Par.Cp)
    
    error(['The compression parameter, Par.Cp, must be a real scalar not ', ...
        'smaller than 1!'])
% .Br
elseif ~isreal(Par.Br) || ~isscalar(Par.Br)
    
    error(['The indicator of the premature termination of the ''AP-Accelerated'','...
        ' algorithm, Par.Br, must be a real scalar!'])
% .im
elseif any(Par.im <= 0) || ( ~isempty(Par.im) && ...
        (~isreal(Par.im) || ~issorted(Par.im) || any(Par.im~=floor(Par.im)) || ...
        numel(Par.im)~=numel(unique(Par.im))) )
    
    error(['The array with the iteration numbers at which the modulator ', ...
        'estimates have to be saved for the output, Par.im, must be empty or ', ...
        'consist of unique nonnegative increasing integers!'])
% .ie
elseif any(Par.ie <= 0) || ( ~isempty(Par.ie) && ...
        (~isreal(Par.ie) || ~issorted(Par.ie) || any(Par.ie~=floor(Par.ie)) || ...
        numel(Par.ie)~=numel(unique(Par.ie))) )
    
    error(['The array with the iteration numbers at which the modulator ', ...
        'estimates have to be saved for the output, Par.ie, must be empty or ', ...
        'consist of unique nonnegative increasing integers!'])
    
end


% Checking input argument Ub

if ~isempty(Ub) && ( ~isreal(Ub) || numel(Ub)~=numel(s) || any(Ub(:)<abs(s(:))) )
    
    error(['The array with the upper bound constraints on the modulator, Ub, ',...
        'must be an empty array or a real non-negative array with the same ',...
        'number of elements as the input signal. Entries of Ub must not be ',...
        'smaller than those of the absolute value signal!'])
    
end


% Checking input argument t

if ~isempty(t) && ( ~ismatrix(t) || size(t,1)~=numel(s) || ~isreal(t) )
    
    error(['The array with sampling coordinates, t, must be a matrix with the ', ...
        'number of columns equal to the dimension of the input signal and the ', ...
        'number of rows equal to the number of signal sample points!'])
    
end


end





function out = f_ap_compression (in, Cp)
% >
% S H O R T   D E S C R I P T I O N
%
% Compresses the dynamic range of the input array by using the power function model.
%
%
% I N P U T   A R G U M E N T S
%
% [in] - input signal.
%
% [Cp] - compression parameter.
%
%
% O U T P U T   A R G U M E N T S
%
% [out] - compressed signal.



out = sign(in) .* abs(in).^Cp;


end




function [out_s, out_Ub, out_ix] = f_ap_interpolation (s, Par, Ub, t)
% >
% S H O R T   D E S C R I P T I O N
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


end

