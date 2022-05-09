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



function f_apd_input_validation (s, Par, Ub, t)
% >
% P U R P O S E
%
% Checks the validity of input arguments for f_apd_demodulation.
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

