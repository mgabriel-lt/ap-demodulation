

% [1] Choose between 'dynamic' and 'static' linking. Currently, only the former is
%     available

CompType = 'dynamic';



% [2] Compilation flags

CFlags = '''$CFLAGS -DAP_MEX_COMPILATION''';



% [3] Input and output filenames

InFileName = 'f_ap_demodulation_mex.c';

OutFileName = './f_ap_demodulation_mex';



% [4] Path to the main directory of the Intel MKL library
%
%     This path must be changed depending on where on your sustem the Intel MKL
%     library is installed.

IntelPath = '/usr/local/intel2019/compilers_and_libraries_2019/linux/mkl/';



% [5] Paths to the headers and executables of the AP demodulation and Intel MKL
% libraries

IncludeDirAP = '/home/mgabriel/Documents/simulations/auditory_demodulation/code_for_sharing/C/library/';

IncludeDirMKL = [IntelPath, 'include'];
            
LibraryDirMKL = [IntelPath, 'lib/intel64/'];



% [6] external libraries to include for compilation

Libraries = {[LibraryDirMKL 'libmkl_rt.so '], '-lm ','-ldl'};



% [7] Compilation

if strcmpi(CompType,'Dynamic')
    
    Final = [   'mex CFLAGS=', CFlags, ...
                ' ', InFileName, ...
                ' -I', IncludeDirAP, ...
                ' -I', IncludeDirMKL, ...
                ' ', Libraries{:}, ...
                ' -output ', OutFileName   ];
            
elseif strcmpi(CompType,'static')
    % TODO
end
        
    
eval(Final)
