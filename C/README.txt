

C O N T E N T S   O F   T H E   C U R R E N T   D I R E C T O R Y

[./library] - folder with the C code of the AP Demodulation library:

    (i) f_ap_demodulation.c - defines the f_ap_demodulation function (the user's
                              interface to the AP Demodulation algorithms).

    (ii) h_ap_demodulation.h - includes all needed external libraries; declares the
                               input parameter structure for the f_ap_demodulation
                               function, declares the prototype of the
                               f_ap_demodulation function itself, defines macros with
                               error messages and operating-system-dependent new-line 
                               control characters.

    (iii) l_ap_input_validation.c - defines the f_ap_input_validation function, which
                                    is used to check the validity of input parameters
                                    for f_ap_demodulation.

    (iv) l_ap_algorithms.c - defines three functions, namely f_ap_basic,
                             f_ap_accelerated, and f_ap_projected, which implement
                             respective AP algorithms.

    (v) l_ap_auxiliary.c - defines the following auxiliary functions used directly
                           and indirectly by f_ap_demodulation: f_ap_print_error,
                           f_ap_minmax, f_ap_abs_scaled_max_abs, f_ap_compression,
                           f_ap_interpolation, f_ap_s_Ub_init, f_ap_mkl_dft_init,
                           f_ap_mkl_dft_PMw.

[./examples] - folder with five examples (example[1-5].c) of signal demodulation,
               demonstrating various usage cases of f_ap_demodulation.



U S A G E

The AP Demodulation approach was formulated and developed in

   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
   10.1109/TSP.2021.3087899.

We refer the user to this publication for any questions related to the working
principle of the new method.

All demodulation capabilities of the current library are accessible solely via the
f_ap_demodulation function. Sufficient information for the correct use of this
function can be found in the comments section of f_ap_demodulation.c. Specifically,
the requirements for and properties of the input and output arguments are described
there. To become more confident in using f_ap_demodulation, consider analyzing the
five examples of its applications provided in ./examples.

To use f_ap_demodulation in an external C code, include f_ap_demodulation.c in the
headers of your code files that refer to f_ap_demodulation via the preprocessor's
#include directive (see example[1-5].c for an example). The #include directive with
f_ap_demodulation.c can appear in different source files of the same program if
needed - an #ifndef ... #endif type guard there prevents the multiple inclusion
error. For simpler compilation (explained below), consider adding the directory with
the source files of the AP Demodulation library to the include path of the compiler
using suitable environment variables.



E X T E R N A L   L I B R A R I E S

Besides the standard C library, f_ap_demodulation uses Intel's oneAPI Math Kernel
Library (MKL), specifically, its Fast Fourier Transform routines. Thus, the latter
must be installed on your system. MKL is available for all major OS types free of
charge (see https://software.intel.com/content/www/us/en/develop/tools/oneapi/
base-toolkit/download.html for the installation files).

After installing MKL, it is necessary to modify the environment variable carrying the
load path for this library on your system, as explained next.

[Linux] If using the bash shell, add the following line to either the .bashrc or
        .bash_profile files located in your home directory:

        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib/intel64

	    If using the csh shell, add the following line to either the .cshrc or
        .profile files located in your home directory:

        setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib/intel64

	    Here, we assumed that MKL is installed on /opt/intel/oneapi. If that is not
        the case, modify the '/opt/intel/oneapi' part as needed.

[Mac] If using the bash shell, add the following line to either the .bashrc or
        .bash_profile files located in your home directory:

        export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib/intel64

	    If using the tcsh shell, add the following line to either the .tcshrc or
        .profile files located in your home directory:

        setenv DYLD_LIBRARY_PATH {$DYLD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib/intel64

	    Here, we assumed that MKL is installed on /opt/intel/oneapi. If that is not
        the case, modify the '/opt/intel/oneapi' part as needed.

[Windows] In the File Explorer, right-click on 'This PC' and select the 'Properties'
          field. Then, go to 'Advanced System Settings' and press on 'Environment
          Variables' on the 'Advanced' tab. Here, you will need to modify the system
          environment variable 'Path.' To do this, select this variable on the list,
          press the 'Edit' button, and then press the 'Edit text' button in the
          appeared panel. There, add a semicolon symbol at the very end of the
          current text line, followed by

          C:\Program Files (x86)\Intel\oneAPI\mkl\latest\redist\intel64

          Here, we assumed that MKL is installed on C:\Program Files (x86)\Intel
          \oneAPI. If that is not the case, modify the 'C:\Program Files (x86)\Intel
          \oneAPI' part as needed.



C O M P I L A T I O N

C code using the AP Demodulation library is supposed to be compiled with GCC. This is
the state-of-the-art compiler available free of charge for all major OS types. It is
preinstalled on Linux systems as a rule. A GCC installation guide for Windows and Mac
can be found following this link: https://www.guru99.com/c-gcc-install.html. On
Windows, if you plan to compile your code via command line instead of a dedicated
IDE, the system Path environment variable has to be appended with the installation
path of MinGW (MinGW provides access to GCC's functionality on Windows OS). One can
do this in the same way as setting the load path of the MKL library explained above.
For example, if the MinGW is installed on C:\CodeBlocks\MinGW, append the Path
variable by C:\CodeBlocks\MinGW\bin

The basic form of the compilation directive used to generate the executable file is

gcc -I PathAPLib -I PathMKLInclude mycode.c PathMKLLib/libmkl_rt.so -lm -o myprogram

for Linux and Mac OSs and

gcc -I PathAPLib -I PathMKLInclude mycode.c PathMKLLib/mkl_rt.lib -o myprogram

for Windows OS.

Here,

    'PathAPLib' is the full path of the folder with the C code of the AP Demodulation
    library, e.g., /usr/local/ap-demodulation/C/library or
    C:\Users\mgabriel\lib\ap-demodulation\C\library

    'PathMKLInclude' is the full path of the folder with the header files of the MKL
    library. For example, if MKL is installed on /opt/intel/oneapi or
    C:\Program Files (x86)/Intel/oneAPI, 'PathMKLInclude' would be, respectively,
    /opt/intel/oneapi/mkl/latest/include or
    C:\Program Files (x86)/Intel/oneAPI/mkl/latest/include. 

    'mycode.c' is the C code that uses f_ap_demodulation to be compiled.

    'PathMKLLib/libmkl_rt.so' is the full path of the needed MKL (shared)
    library file that contains precompiled FFT routines. For example, if MKL is
    installed on /opt/intel/oneapi or C:\Program Files (x86)/Intel/oneAPI,
    'PathMKLLib' would be, respectively, /opt/intel/oneapi/mkl/latest/lib/intel64 or
    C:\Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64

    '-lm' indicates the standard C (shared) library file that contains precompiled
    basic math routines.

    'myprogram' is the full path of the executable to be generated.

Note that the MKL library is dynamically linked. That is, the f_ap_demodulation_mex
executable cannot run successfully without the relevant MKL library files present on
the system during runtime.
    
The above compilation statements are minimal -- they do not assume additional source
files, libraries, or compilation options. The latter can be added following the
input argument syntax of GCC if needed. Please check "An Introduction to GCC" by
Brian Gough, an excellent and concise text on the most relevant aspects of GCC usage
in practice.

