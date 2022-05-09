
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



% [1] Choose between 'dynamic' and 'static' linking. Currently, only the former is
%     available

CompType = 'dynamic';



% [2] Compilation flags

CFlags = '''$CFLAGS -DAPD_MEX_COMPILATION''';



% [3] Input and output filenames

InFileName = 'f_apd_demodulation_mex.c';

OutFileName = './f_apd_demodulation_mex';



% [4] Path to the main directory of the Intel MKL library
%
%     This path must be changed depending on where on your system the Intel MKL
%     library is installed.

IntelPath = '/opt/intel/oneapi/mkl/latest/';



% [5] Paths to the headers and executables of the AP Demodulation and Intel MKL
% libraries

IncludeDirAP = '../../C/libsrc/';

IncludeDirMKL = [IntelPath, 'include'];
            
LibraryDirMKL = [IntelPath, 'lib/intel64/'];



% [6] external libraries to include for compilation

if isunix || ismac

%     Libraries = {[LibraryDirMKL 'libmkl_intel_lp64.so '], ...
%                  [LibraryDirMKL 'libmkl_sequential.so '], ...
%                  [LibraryDirMKL 'libmkl_core.so '], '-lm '};
    Libraries = {[LibraryDirMKL 'libmkl_rt.so '], '-lm '};
    
elseif ispc
    
    Libraries = {[LibraryDirMKL 'mkl_rt.lib']};
    
end



% [7] Compilation

if strcmpi(CompType,'Dynamic')
    
    Final = [   'mex CFLAGS=', CFlags, ...
                ' -I', IncludeDirAP, ...
                ' -I', IncludeDirMKL, ...
                ' ', InFileName, ...
                ' ', Libraries{:}, ...
                ' -output ', OutFileName   ];
            
elseif strcmpi(CompType,'static')
    % TODO
end
        
    
eval(Final)
