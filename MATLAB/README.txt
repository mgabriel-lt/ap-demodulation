

C O N T E N T S   O F   T H E   C U R R E N T   D I R E C T O R Y

[./library_m] - folder with the MATLAB m-file code of the AP demodulation library:

    (i) f_ap_demodulation.m - defines the f_ap_demodulation function (the user's
                              interface to the AP demodulation algorithms).

    (ii) f_ap_basic.m - defines the f_ap_basic function, which estimates the
                        modulator of a signal by using the AP-Basic algorithm.

    (iii) f_ap_accelerated.m - defines the f_ap_accelerated function, which
                               estimates the modulator of a signal by using the
                               AP-Accelerated algorithm.

    (iv) f_ap_projected.m - defines the f_ap_accelerated function, which estimates
                            the modulator of a signal by using the AP-Projected
                            algorithm.


[./library_mex] - folder with the code of the MATLAB MEX function interface of the
                  AP demodulation library implemented in C:

    (i) f_ap_demodulation_mex.c - A MEX gateway function for f_ap_demodulation
                                  defined in ../C/library/.

    (ii) compilation.m - MATLAB script for compiling f_ap_demodulation_mex.c to
                         produce the corresponding MEX function executable.


[./examples] - folder with five examples (example[1-5].m) of signal demodulation,
               demonstrating various usage cases of f_ap_demodulation and
               f_ap_demodulation_mex.



U S A G E

The AP demodulation approach was formulated and developed in

   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
   10.1109/TSP.2021.3087899.

We refer the user to this publication for any questions related to the working
principle of the new method.

All demodulation capabilities of the current library are accessible solely via the
f_ap_demodulation (MATLAB m-file function) or f_ap_demodulation_mex (MATLAB MEX
function executable). The sufficient information for the correct use of these
functions can be found in the comments section of, respectively, f_ap_demodulation.m
and f_ap_demodulation_mex.c. Specifically, the requirements for and properties of the
input and output arguments are described there. To consolidate the confidence in
using f_ap_demodulation, consider analyzing the five examples of its applications
provided in ./examples.



E X T E R N A L   L I B R A R I E S   (M E X   F U N C T I O N)

Besides the standard C library, generating the f_ap_demodulation_mex executable
requires Intel's oneAPI Math Kernel Library (oneMKL), specifically, its Fast Fourier
Transform routines. Thus, the latter must be installed on your system. oneMKL is
available for all major OS types free of charge (see https://software.intel.com/
content/www/us/en/develop/tools/oneapi/base-toolkit/download.html for the
installation files).



C O M P I L A T I O N   A N D   L I N K I N G   F O R   M E X   F U N C T I O N

f_ap_demodulation_mex.c is supposed to be compiled with GCC. This is the state-of-
the-art compiler available free of charge for all major OS types. It is preinstalled
on Linux systems as a rule. A GCC installation guide for Windows and Mac can be found
following this link: https://www.guru99.com/c-gcc-install.html. f_ap_demodulation_mex
executbale is generated with the MATLAB's built-in function mex. The latter must be
configured to use GCC for C language compilation (see MATLAB help for the
instructions).

The compilation of f_ap_demodulation_mex.c is performed by running the compilation.m
script. The user, however, must set modify paths corresponding to the locations of
relevant AP demodulation and MKL library files on the system explicitly in the
mentioned script. This script is well commented on, making the required modification
straightforward.

Note that the MKL library is dynamically linked. That is, the f_ap_demodulation_mex
executable cannot run successfully without the relevant MKL library files present on
the system during runtime.
    
