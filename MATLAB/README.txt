

C O N T E N T S   O F   T H E   C U R R E N T   D I R E C T O R Y

[./library_m] - folder with the MATLAB m-file code of the AP Demodulation library:

    (i) f_ap_demodulation.m - defines the f_ap_demodulation function (the user's
                              interface to the AP Demodulation algorithms).

    (ii) f_ap_basic.m - defines the f_ap_basic function, which estimates the
                        modulator of a signal by using the AP-Basic algorithm.

    (iii) f_ap_accelerated.m - defines the f_ap_accelerated function, which
                               estimates the modulator of a signal by using the
                               AP-Accelerated algorithm.

    (iv) f_ap_projected.m - defines the f_ap_accelerated function, which estimates
                            the modulator of a signal by using the AP-Projected
                            algorithm.


[./library_mex] - folder with the code of the MATLAB MEX function interface of the
                  AP Demodulation library implemented in C:

    (i) f_ap_demodulation_mex.c - A MEX gateway function for f_ap_demodulation
                                  defined in ../C/library/.

    (ii) compilation.m - MATLAB script for compiling f_ap_demodulation_mex.c to
                         produce the corresponding MEX function executable.


[./examples] - folder with five examples (example[1-5].m) of signal demodulation,
               demonstrating various usage cases of f_ap_demodulation and
               f_ap_demodulation_mex.



U S A G E

The AP Demodulation approach was formulated and developed in

   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
   10.1109/TSP.2021.3087899.

We refer the user to this publication for any questions related to the working
principle of the new method.

All demodulation capabilities of the current library are accessible solely via the
f_ap_demodulation (MATLAB m-file function) or f_ap_demodulation_mex (MATLAB MEX
function executable). Sufficient information for the correct use of these functions
can be found in the comments section of, respectively, f_ap_demodulation.m and
f_ap_demodulation_mex.c. Specifically, the requirements for and properties of the
input and output arguments are described there. To become more confident in using
f_ap_demodulation(_mex), consider analyzing the five examples of its applications
provided in ./examples.



E X T E R N A L   L I B R A R I E S   (M E X   F U N C T I O N)

Besides the standard C library, generating the f_ap_demodulation_mex executable
requires Intel's oneAPI Math Kernel Library (MKL), specifically, its Fast Fourier
Transform routines. Thus, the latter must be installed on your system. MKL is
available for all major OS types free of charge (see https://software.intel.com/
content/www/us/en/develop/tools/oneapi/base-toolkit/download.html for the
installation files).

After installing MKL, it is necessary to modify the environment variable carrying the
load path for this library on your system, as explained next.

[Linux] If using the bash shell, add the following line to either the .bashrc or
        .bash_profile files located in your home directory:

        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib/intel64/

	    If using the csh shell, add the following line to either the .cshrc or
        .profile files located in your home directory:

        setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib/intel64/

	    Here, we assumed that MKL is installed on /opt/intel/oneapi. If that is not
        the case, modify the '/opt/intel/oneapi' part as needed.

[Mac] If using the bash shell, add the following line to either the .bashrc or
        .bash_profile files located in your home directory:

        export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib/intel64/

	    If using the tcsh shell, add the following line to either the .tcshrc or
        .profile files located in your home directory:

        setenv DYLD_LIBRARY_PATH {$DYLD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib/intel64/

	    Here, we assumed that MKL is installed on /opt/intel/oneapi. If that is not
        the case, modify the '/opt/intel/oneapi' part as needed.

[Windows] In the File Explorer, right-click on 'This PC' and select the 'Properties'
          field. Then, go to 'Advanced System Settings' and press on 'Environment
          Variables' on the 'Advanced' tab. Here, you will need to modify the system
          environment variable 'Path.' To do this, select this variable on the list,
          press the 'Edit' button, and then press the 'Edit text' button in the
          appeared panel. There, add a semicolon symbol at the very end of the
          current text line, followed by

          C:\Program Files (x86)\Intel\oneAPI\mkl\latest\redist\intel64\

          Here, we assumed that MKL is installed on C:\Program Files (x86)\Intel
          \oneAPI. If that is not the case, modify the 'C:\Program Files (x86)\Intel
          \oneAPI' part as needed.



C O M P I L I N G,  L I N K I N G,   &   B U I L D I N G   M E X   F U N C T I O N

f_ap_demodulation_mex.c is supposed to be compiled with GCC. This is the state-of-
the-art compiler available free of charge for all major OS types. It is preinstalled
on Linux systems as a rule. A GCC installation guide for Windows and Mac can be found
following this link: https://www.guru99.com/c-gcc-install.html.

f_ap_demodulation_mex executabale is generated by using MATLAB's built-in function
mex. The latter must be configured to use GCC (or its Windows OS alternative MinGW)
for C language compilation (see MATLAB help for the instructions). On Linux
platforms, this setup is trivial: type 'mex -setup' in MATLAB command prompt and
select the needed compiler. On Windows OS, the environment variable MW_MINGW64_LOC
must be set additionally for MATLAB to recognize the compiler. You can find how to do
this here: https://www.mathworks.com/matlabcentral/answers/313298-i-already-have-
mingw-on-my-computer-how-do-i-configure-it-to-work-with-matlab. Note that MATLAB
would not properly parse white space characters in the MinGW installation pathname.
Thus, the latter must not contain white spaces.

The f_ap_demodulation_mex executable is built by running the compilation.m script.
The user, however, must modify some paths corresponding to the locations of relevant
AP Demodulation and MKL library files on the system explicitly in the mentioned
script. This script is well commented on, making the required modification
straightforward.

Finally, note that the MKL library is dynamically linked to f_ap_demodulation_mex.
That is, the f_ap_demodulation_mex executable cannot run successfully without the
relevant MKL library files present on the system during runtime.
    
