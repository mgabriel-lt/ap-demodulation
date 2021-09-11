

C O N T E N T S   O F   T H E   C U R R E N T   D I R E C T O R Y

[./library] - folder with the C code of the AP demodulation library:

    (i) f_ap_demodulation.c - defines the f_ap_demodulation function (the user's
                              interface to the AP demodulation algorithms).

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

The AP demodulation approach was formulated and developed in

   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
   10.1109/TSP.2021.3087899.

We refer the user to this publication for any questions related to the working
principle of the new method.

All demodulation capabilities of the current library are accessible solely via the
f_ap_demodulation function. The sufficient information for the correct use of this
function can be found in the comments section of f_ap_demodulation.c. Specifically,
the requirements for and properties of the input and output arguments are described
there. To consolidate the confidence in using f_ap_demodulation, consider analyzing
the five examples of its applications provided in ./examples.

To use f_ap_demodulation in an external C code, simply include f_ap_demodulation.c
in the headers of your code files referring to f_ap_demodulation via the
preprocessor's #include directive (see example[1-5].c for an example). The #include
directive with f_ap_demodulation.c can appear in different source files of the same
program if needed - the multiple inclusion error is prevented by an #ifndef...#endif
type guard. For simpler compilation (explained below), consider adding the directory
with the source files of the AP demodulation library to the include path of the
compiler using suitable environment variables.



E X T E R N A L   L I B R A R I E S

Besides the standard C library, f_ap_demodulation uses Intel's oneAPI Math Kernel
Library (oneMKL), specifically, its Fast Fourier Transform routines. Thus, the latter
must be installed on your system. oneMKL is available for all major OS types free of
charge (see https://software.intel.com/content/www/us/en/develop/tools/oneapi/
base-toolkit/download.html for the installation files). We highly recommend setting
the environment variables with the include and link paths of this library for simpler
compilation - see the installation guide of the MKL library for the instructions.



C O M P I L A T I O N

C code using the AP demodulation library is supposed to be compiled with GCC. This is
the state-of-the-art compiler available free of charge for all major OS types. It is
preinstalled on Linux systems as a rule. A GCC installation guide for Windows and Mac
can be found following this link: https://www.guru99.com/c-gcc-install.html.

The exact directive to the compiler depends on whether the OS environment variables
corresponding to the include, link, and load paths of the compiler are set or not:

(i) If no include path of the AP demodulation library and no include, link, and load
    paths of the oneMKL library are set via appropriate environment variables, the
    basic form of the compilation directive used to generate an executable file is

    gcc -I Path_to_AP_Demodulation -I Path_to_MKL_include mycode.c \
        Path_to_MKL_lib/libmkl_rt.so -lm -o myprogram

    Here,

        "Path_to_AP_Demodulation" is the full path of the folder with the C code of
        the AP demodulation library, e.g., /usr/local/ap-demodulation/C/library/

        "Path_to_MKL_include" is the full path of the folder with the header files of
        the MKL library. For example, if a 2019 release of the MKL was installed in
        /usr/local/intel on a Linux system, Path_to_MKL_include would be
        /usr/local/intel/compilers_and_libraries_2019/linux/mkl/include

        "mycode.c" is the C code that uses f_ap_demodulation to be compiled.

        "Path_to_MKL_lib/libmkl_rt.so" is the full path of the needed MKL (shared)
        library file that contains precompiled FFT routines. For example, if a 2019
        release of the MKL was installed in /usr/local/intel on a Linux system,
        Path_to_MKL_lib would be
        /usr/local/intel/compilers_and_libraries_2019/linux/mkl/lib/intel64

        "-lm" refers to the full path to the standard C (shared) library file that
        contains precompiled basic math routines.

        "myprogram" is the full path of the executable to be generated.

(ii) If the include and load paths of the AP demodulation and oneMKL libraries are
     set via the environment variables, the compiler directive for generating an
     executable file simplifies to

     gcc mycode.c -lmkl_rt -lm -o myprogram

Note that, in both cases, the MKL library is dynamically linked. That is, myprogram
cannot run successfully without relevant MKL library files present on the system
during runtime.
    
The above compilation statements are minimal in the sense that they do not assume
additional source files, libraries, or compilation options. The latter can be added
following the standard input argument syntax of GCC if needed. Please check
"An Introduction to GCC" by Brian Gough, an excellent and concise text on the most
relevant aspects of GCC usage in practice. In particular, Section 3.1 there explains
how to set the environment variables relevant for the compilation process on a Linux
system. In the case of another OS, google for analogs.

