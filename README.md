
<p align="center">

  <img src="svg/Fig0_logo.svg#gh-light-mode-only"  />

  <img src="svg/Fig0_logo_dark.svg#gh-dark-mode-only"  />
  
</p>



&nbsp;
# Table of Contents

[About](#SecAbout)

[Organization & Usage of the Library](#SecOrganization)

- [|1|&nbsp; C Implementation](#SecImplC)

    - [|1.1|&nbsp; Contents of ./C](#SecContC)
   
    - [|1.2|&nbsp; Frontend Functions](#SecFrntFcC)
   
    - [|1.3|&nbsp; Reserved Names](#SecResNam)
   
    - [|1.4|&nbsp; Error Handling](#SecErrHnd)
   
    - [|1.5|&nbsp; External Libraries](#SecExtLibC)
   
    - [|1.6|&nbsp; Compilation](#SecCmpC)
   
    - [|1.7|&nbsp; Access from C++](#SecAccCpp)
   
- [|2|&nbsp; MATLAB Implementation](#SecImplM)

    - [|2.1|&nbsp; Contents of ./MATLAB](#SecContM)
   
    - [|2.2|&nbsp; Frontend Functions](#SecFrntFcM)
    
    - [|2.3|&nbsp; External Libraries](#SecExtLibM)
    
    - [|2.4|&nbsp; Compilation](#SecCmpM)
    
- [|3|&nbsp; Examples](#SecExmpl)

- [|4|&nbsp; Usage Tips](#SecUseTip)

[Theoretical Background](#SecTheory)

[Acknowledgments](#SecAcknw)



&nbsp;
<a name="SecAbout"></a>
# About

**AP&nbsp;Demodulation** is a mini-library of routines implementing the new approach to amplitude demodulation of arbitrary-bandwidth signals introduced in<sup>[1](#footnote1),</sup><sup>[2](#footnote2)</sup>

> [M.&nbsp;Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband signals," IEEE Transactions on Signal Processing, vol.&nbsp;69, pp.&nbsp;4039-4054, 2021. DOI:&nbsp;10.1109/TSP.2021.3087899.](https://ieeexplore.ieee.org/document/9449954)

This approach has many desirable computational properties that make it the preferred choice in many situations:

- In the context of narrowband signals, the new approach outperforms classical demodulation algorithms in terms of robustness to data distortions and compatibility with nonuniform sampling.

- When considering demodulation of wideband signals, it surpasses the current state-of-the-art techniques in terms of computational efficiency by up to many orders of magnitude.

Such performance enables practical applications of amplitude demodulation in previously inaccessible settings. Specifically, online and large-scale offline demodulation of wideband signals, signals in higher dimensions, and poorly-sampled signals become practically feasible.

**Download**

- To download the repository via a web browser, click the "Clone" button above the list of files (on the main page) and select "Download ZIP."
- To clone the repository via the command line, type `git clone https://github.com/mgabriel-lt/ap-demodulation.git`

**Converting to PDF**

To convert the current README.md to a PDF file while retaining the GitHub formatting,

1. Install the Google Chrome extension [Github Markdown Printer](https://chrome.google.com/webstore/detail/github-markdown-printer/fehpdlpmcegfpbkgcnaleindodeegapk?hl=en).

2. Right-click anywhere on the page you want to print and select `Print GitHub Markdown`.

3. Select the `Save as PDF` option in the *Destination* field in the *Print* tab of the opened window and click `Save`. You may also want to adjust the scale parameter in the *Settings* for a proper alignment of the cited code sections.

___
<a name="footnote1">**1**</a>: Please cite this paper in your published works that use the *AP&nbsp;Demodulation* library.

<a name="footnote2">**2**</a>: A freely available preprint of this paper is on [arXiv](https://arxiv.org/abs/2102.04832).
___
&nbsp;



<a name="SecOrganization"></a>
# Organization & Usage of the Library

Currently, the *AP&nbsp;Demodulation* library includes **C** and **MATLAB** implementations, organized into separate folders in the repository root. The MATLAB implementation is divided into **m-file** and **MEX** function sections. In all cases, code with different signal demodulation examples is included to illustrate applications of the library in practice. Below, we describe the contents of each available version of the library, their compilation, and usage.


<a name="SecImplC"></a>
## |1|&nbsp; C Implementation

<a name="SecContC"></a>
### |1.1|&nbsp; Contents of ./C
- \[**./C/libsrc**\] &#8211; folder with the C code of the *AP&nbsp;Demodulation* library.

    - ***f_apd_demodulation.c*** defines the `f_apd_demodulation` function (the user's interface to the *AP&nbsp;Demodulation* algorithms).
    
    - ***l_apd_algorithms.c*** defines functions implementing different versions of the actual AP algorithms.
    
    - ***l_apd_error_handling.c*** defines functions and (static global) variables used to validate input arguments for `f_apd_demodulation` and error handling for the whole library. Three of these functions, `f_apd_set_errexit`, `f_apd_get_error`, and `f_apd_print_error`, are explicitly accessible to the user (see next section for their description).
    
    - ***l_apd_auxiliary.c*** defines various auxiliary functions for the *AP&nbsp;Demodulation* approach.
    
    - ***h_apd.h*** is the main header file of the *AP&nbsp;Demodulation* library. Together with definitions of all the macros, it declares the input parameter structure `strAPD_Par` and prototypes of the four functions of this library, namely, `f_apd_demodulation`, `f_apd_set_errexit`, `f_apd_get_error`, and `f_apd_print_error`, that are directly accessible to the user.

- \[**./C/examples**\] &#8211; folder with five examples (*example\[1-5\].c*) of signal demodulation, demonstrating various usage cases of `f_apd_demodulation`.

- \[**./C/libbin**\] &#8211; (initially) empty folder where *shared* or *dynamic-link* binary files of the library may be kept by the user if it is chosen to generate them (see [Compilation](#SecCompC)).


<a name="SecFrntFcC"></a>
### |1.2|&nbsp; Frontend Functions

The user's interface to the C version of *AP&nbsp;Demodulation* library consists of four functions: `f_apd_demodulation`, `f_apd_set_errexit`, `f_apd_get_error`, and `f_apd_print_error`. We describe each of them below.

**`f_apd_demodulation`** is the user’s gateway to the *AP&nbsp;Demodulation* computing algorithms.

<details><summary>FULL DESCRIPTION (click here)</summary>
<p>

```c
int f_apd_demodulation (double* s, struct strAPD_Par* Par, double* Ub, double* t, \
                        double* out_m, double* out_e, long* iter)
					   
/* P U R P O S E
 *
 * Performs demodulation of the input signal by using the AP Demodulation approach.
 */
 
 /* I N P U T   A R G U M E N T S
 *
 * [s] - input signal.
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .Al - demodulation algorithm. Possible options are: 'B' - AP-Basic,
 *               'A' - AP-Accelerated, 'P' - AP-Projected. {Type: char}
 *
 *         .D - number of signal dimensions. Currently, 0 < D < 4 are available.
 *              {Type: int}
 *
 *         .Fs - sampling frequencies for each dimension of the signal. This is an
 *               array of D elements (memory preallocated on stack). {Type: double}
 *
 *         .Fc - cutoff frequencies of the modulator for each dimension of the
 *               signal. This is an array of D elements (memory preallocated on
 *               stack). {Type: double}
 *
 *         .Et - infeasibility error tolerance used to control the termination of the
 *               demodulation algorithm. The iterative process is stopped when the
 *               infeasibility error, ϵ, drops to the level of .Et or below. If
 *               .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
 *               below), is completed. {Type: double}
 *
 *         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
 *               chosen AP algorithm is iterated not more than .Ni times independent
 *               of whether ϵ drops at or below .Et. {Type: long}
 *
 *         .Ns - numbers of sample points of the original input signal in every
 *               dimension (if signal is uniformly sampled) or the total number of
 *               sample points (if the signal is nonuniformly sampled). This is an
 *               array with the number of elements equal to the number of signal
 *               dimensions (memory preallocated on stack). If the signal is sampled
 *               nonuniformly, the total number of sample points must be assigned to
 *               Par.Nr[0]. {Type: long}
 *
 *         .Nr - numbers of sample points on the refined uniform interpolation grid
 *               in every dimension. This is an array of D elements (memory
 *               preallocated on stack). This field must be defined only if the
 *               fourth input argument, t, is provided, i.e, not equal to NULL (see
 *               below). Otherwise, .Nr is ignored. {Type: long}
 *
 *         .Cp - compression parameter. If .Cp > 1, demodulation is performed by
 *               using signal compression (see equations (12)-(13) in the paper cited
 *               in the header of this file. {Type: double}
 *
 *         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
 *               when the λ factor drops below one. If .Br=1 (recommended), premature
 *               termination is assumed. Otherwise, if .Br=0, the AP-A is not stopped
 *               prematurely even when λ decreases below 1. This field is required
 *               only if .Al='A'. {Type: int}
 *
 *         .im - array with the iteration numbers at which the modulator estimates 
 *               have to be saved for the output. The first element is the length of
 *               the array (excluding the first element itself). At least one
 *               iteration has to be assigned to .im. {Type: long}
 *
 *         .ie - array with the iteration numbers at which the infeasibility error,
 *               ϵ, values have to be saved for the output. The first element of .ie
 *               is the length of the array (excluding the first element itself).
 *               At least one iteration has to be assigned to .ie. {Type: long}
 *
 *         Two additional fields, .ns (number of sample points of the original
 *         signal) and .Nx (dimensions of the actual, possibly interpolated signal)
 *         are assigned values in this function. No other fields of Par or other
 *         input arguments of this function are modified inplace.
 *
 * [Ub] - upper bound on the modulator. This array must have the same number of
 *        elements as the input signal or must be set to NULL (if no upper bound on
 *        the modulator is assumed).
 *
 * [t] - sampling coordinates of the input signal. This is a 2D array with the number
 *       of columns equal to the dimension of the input signal and the number of rows
 *       equal to the number of signal sample points. This input argument is optional
 *       and has to be used only if the input signal is sampled nonuniformly.
 *       Otherwise, t must be set to NULL. If t is not NULL, i.e., signal is
 *       nonuniformly sampled, the input argument Par has to contain the field .Nr
 *       (see above).
 */

/* O U T P U T   A R G U M E N T S
 *
 * [out_m] - array with modulator estimates at algorithm iterations indicated by
 *           Par.im (memory allocated  externally). The modulator estimates are
 *           arranged columnwise. out_m has to point to a memory block sufficient to
 *          hold at least one instance of the modulator estimate.
 *
 * [out_e] - array with error estimates at algorithm iterations indicated by Par.ie
 *           (memory allocated  externally). out_e has to point to a memory block
 *           sufficient to hold at least one instance of the error estimate.
 *
 * [iter] - number of AP iterations (this is the address of an externally defined
 *          scalar variable).
 */

/* R E T U R N   V A L U E
 *
 * [exitflag] - exit flag. Any positive value indicates an error (for numerical and
 *              textual definitions of the exit status, see l_ap_error_handling.c).
 * 
 *              Upon an error, all memory dynamically allocated in this function or
 *              functions called by this function is freed.
 */
```

</p>
</details>


**`f_apd_set_errexit`** allows the user to set the behavior of the program when an error occurs while running `f_apd_demodulation`.

<details><summary>FULL DESCRIPTION (click here)</summary>
<p>

```c
void f_apd_set_errexit (int err_exit)

/* P U R P O S E
 *
 * Stores the indicator of the behavior of the program upon error.
 */

/* I N P U T   A R G U M E N T S
 *
 * [err_exit] - indicator of the behavior of f_apd_demodulation upon an error.
 *              0 sets f_apd_demodulation to return control to the calling function.
 *              Any nonzero value sets f_apd_demodulation to print the error message
 *              to stderr and exit the whole program.
 */

/* O U T P U T   A R G U M E N T S
 *
 * None.
 */

/* R E T U R N   V A L U E
 *
 * None.
 */
```

</p>
</details>

**`f_apd_get_error`** provides access to the numeric error id, the reason for the error, the name of the source file, and the line number of that file where the error occurred while running `f_apd_demodulation`.

<details><summary>FULL DESCRIPTION (click here)</summary>
<p>

```c
void f_apd_get_error (int err_id, long* err_line, char* err_file, char* err_msg)

/* P U R P O S E
 *
 * Outputs the line number, the filename, and the error message associated with the
 * provided error id.
 */

/* I N P U T   A R G U M E N T S
 *
 * None.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [err_id] - error id (this is the address of an externally defined scalar
 *            variable). If err_id == NULL, no value is assigned.
 * 
 * [err_line] - line number of the code file associated with the provided error id
 *              (this is the address of an externally defined scalar variable). If
 *              err_line == NULL, no value is assigned.
 * 
 * [err_file] - name of the code file associated with the provided error id (this a
 *              pointer to an externally defined array with at least 200 elements).
 *              If err_file == NULL, no value is assigned.
 * 
 * [err_msg] -  error message associated with the provided error id (this a pointer
 *              to an externally defined array with at least 200 elements). If
 *              err_msg == NULL, no value is assigned.
 */
 
 /* R E T U R N   V A L U E
 *
 * None.
 */
```

</p>
</details>

**`f_apd_print_error`** prints a fully formatted error message to `stderr` that includes all information accessible via `f_apd_get_error`.

<details><summary>FULL DESCRIPTION (click here)</summary>
<p>

```c
void f_apd_print_error (int err_id)

/* P U R P O S E
 *
 * Prints an error message to stderr.
 */

/* I N P U T   A R G U M E N T S
 *
 * [err_id] - error id.
 */

/* O U T P U T   A R G U M E N T S
 *
 * None.
 */

/* R E T U R N   V A L U E
 *
 * None.
 */
```

</p>
</details>


<a name="SecResNam"></a>
### |1.3|&nbsp; Reserved Names

The following must be taken into account by the users when naming objects in their programs:

- All functions defined in *AP&nbsp;Demodulation* are global and start with the prefix `f_apd_`.

- The list of macros defined in *AP&nbsp;Demodulation* is

  - `APD_ERR_*`,
  - `APD_HEADER`,
  - `APD_SOURCE`,
  - `APD_DEMODULATION_MEX`,
  - `M_PI` (defined only if absent in the included external libraries).

- One structure variable type, `strAPD_Par` is defined in *AP&nbsp;Demodulation*.

- No global variables are declared or used in *AP&nbsp;Demodulation*. 

The users' responsibility is to make sure that names identical to those of the global objects of *AP&nbsp;Demodulation* are not defined elsewhere in their programs.


<a name="SecErrHnd"></a>
### |1.4|&nbsp; Error Handling

The user can control the behavior of `f_apd_demodulation` upon an error by using `f_apd_set_errexit` described [above](#SecFrntFcC). The default mode (no action needed) is to print an error message to `stderr` and exit the program. This is analogous to calling `f_apd_set_errexit` with its argument set to `1` before calling `f_apd_demodulation`. If `0` is chosen as the argument instead, `f_apd_demodulation` stops at the point of the error, frees all memory allocated in this function and functions called by it, and returns to the calling program with an appropriately set exit value. The exit regime set by `f_apd_set_errexit` remains valid throughout the program and can be updated only by calling this same function with a different argument.

Any positive return value of `f_apd_demodulation` indicates an error. If the user chooses to overtake the control from `f_apd_demodulation` upon an error, it is his/her responsibility to check the return value of this function and foresee steps to be taken in the program if it is positive.

The user can access diagnostic information about the error or print it to `stderr` by using, respectively, `f_apd_get_error` or `f_apd_print_error` described above. All possible error messages and their numeric codes are defined in *l_apd_error_handling.c*.


<a name="SecExtLibC"></a>
### |1.5|&nbsp; External Libraries

Besides the standard C library, `f_apd_demodulation` uses Intel's oneAPI Math Kernel Library (oneMKL), precisely, its Fast Fourier Transform routines. Thus, the latter must be available on your system. oneMKL versions for all major OS types can be obtained free of charge [here](https://software.intel.com/content/www/us/en/develop/tools/oneapi/base-toolkit/download.html) (look for the oneAPI Base Toolkit).

After installing oneMKL, it is advisable to modify the environment variable carrying the load path for this library on your system, as explained next.<sup>[3](#footnote3)</sup>

<a name="LnxComp"></a><details><summary>**LINUX** (click to expand)</summary>
<p>

- If using the *bash* or *zsh* shells, add the following line to either the *.bashrc*, *.zshrc*, or *.profile* files in your home directory:

    ```
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib/intel64
    ```

- If using the *csh* shell, add the following line to either the *.cshrc* or *.profile* files in your home directory:

    ```
    setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib/intel64
    ```

Here, we assumed that oneMKL is installed on */opt/intel/oneapi*. If that is not the case, modify the *'/opt/intel/oneapi'* part as needed.

</p>
</details>

<a name="MacComp"></a><details><summary>**MAC** (click to expand)</summary>
<p>

- If using the *bash* or *zsh* shells, add the following line to either the *.bashrc*, *.zshrc*, or *.profile* files in your home directory:

   ```
   export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/intel/oneapi/mkl/latest/lib
   ```

- If using the *tcsh* shell, add the following line to either the *.tcshrc* or *.profile* files in your home directory:
    
  ```
  setenv DYLD_LIBRARY_PATH {$DYLD_LIBRARY_PATH}:/opt/intel/oneapi/mkl/latest/lib
  ```

	Here, we assumed that oneMKL is installed on */opt/intel/oneapi*. If that is not the case, modify the '*/opt/intel/oneapi*' part as needed.
	
</p>
</details>


<a name="WinComp"></a><details><summary>**WINDOWS** (click to expand)</summary>
<p>

In the File Explorer, right-click on *'This PC'* and select the *'Properties'* field. Then, go to *'Advanced System Settings'* and press *'Environment Variables'* on the *'Advanced'* tab. Here, select the *'Path'* variable (system-level or specific-user-level), press the *'Edit'* button, and press *'New'* in the appeared panel. There, enter

```
C:\Program Files (x86)\Intel\oneAPI\mkl\latest\redist\intel64
```

and click *OK*. Here, we assumed that oneMKL is installed on *C:\Program Files (x86)\Intel\oneAPI*. If that is not the case, modify the '*C:\Program Files (x86)\Intel\oneAPI*' part as needed.

</p>
</details>

Note that oneMKL library is dynamically linked with *AP&nbsp;Demodulation*. Thus, when referring to `f_apd_demodulation`, your program cannot run successfully without the relevant oneMKL library files present on the system during runtime.

___
<a name="footnote1">**3**</a>: If oneMKL is properly installed on your system, this modification is not required. However, it is a must if the instalation cannot be completed due to any reason, e.g., the user does not have required permisions. In the latter case, it is sufficient to have separate library files available.
___


<a name="SecCmpC"></a>
### |1.6|&nbsp; Compilation

*AP&nbsp;Demodulation* library is compatible with the C99 and later standards. We recommend using GCC, a state-of-the-art compiler available free of charge for all major OS types. It is preinstalled on Linux systems as a rule. A GCC installation guide for Windows and Mac can be found following [this link](https://www.guru99.com/c-gcc-install.html).

>**For Windows Users:** If you plan to compile your code via the command line instead of a dedicated IDE, the `Path` environment variable has to be appended with the installation path of your compiler. One can do this in the same way as setting the load path for the oneMKL library explained [above](#WinComp). For example, if MinGW<sup>[4](#footnote4)</sup> is installed on `C:\CodeBlocks\MinGW`, append the `Path` variable by `C:\CodeBlocks\MinGW\bin`.

There are **two alternative ways** of accessing the functionality of the *AP&nbsp;Demodulation* library.

1. `#include`'ing the library's source files to your code and then compiling the whole set together.

2. Generating a *shared* (Linux & Mac) or *dynamic-link* (Windows) library, which can then be linked to the main program.

We describe both possibilities below.

**<details><summary>Option 1: Including the Source Code** (click to expand)</summary>
<p>

In this case, include *f_apd_demodulation.c* in the headers of your code files that refer to `f_apd_demodulation` via the preprocessor's `#include` directive (see *example\[1,2,5\].c* for an example). The `#include` directive with *f_apd_demodulation.c* can appear in different source files of the same program if needed &#8211; an `#ifndef ... #endif` type guard there prevents the multiple inclusion error.

The basic form of the compilation directive used to generate the executable file is

```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathMKLLib/libmkl_intel_lp64.so PathMKLLib/libmkl_sequential.so PathMKLLib/libmkl_core.so -lm -o myprogram
```

for **Linux**,

```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathMKLLib/libmkl_intel_lp64.dylib PathMKLLib/libmkl_sequential.dylib PathMKLLib/libmkl_core.dylib -lm -o myprogram
```

for **Mac**, and


```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathMKLLib\mkl_rt.lib -o myprogram
```

for **Windows**.<sup>[5](#footnote5)</sup>

Here,

- `PathAPDSrc` is the full path to the folder with the C code of the *AP&nbsp;Demodulation* library, e.g., */usr/local/ap-demodulation/C/libsrc* or *C:\Users\mgabriel\lib\ap-demodulation\C\libsrc*.

- `PathMKLInclude` is the full path to the folder with the header files of the oneMKL library. For example, if oneMKL is installed on */opt/intel/oneapi* or *C:\Program Files (x86)/Intel/oneAPI*, `PathMKLInclude` would be, respectively, */opt/intel/oneapi/mkl/latest/include* or *C:\Program Files (x86)/Intel/oneAPI/mkl/latest/include*. 

- `mycode.c` is the C source file that uses `f_apd_demodulation` and is to be compiled.

- `PathMKLLib` is the full path to the folder that contains needed oneMKL (shared) library files. For example, if oneMKL is installed on /opt/intel/oneapi or C:\Program Files (x86)/Intel/oneAPI, `PathMKLLib` would be, respectively, */opt/intel/oneapi/mkl/latest/lib/intel64* (for Linux), */opt/intel/oneapi/mkl/latest/lib* (for Mac), or *C:\Program Files (x86)/Intel/oneAPI/mkl/latest/lib/intel64* (for Windows).

- `-lm` indicates the standard C (shared) library that contains precompiled basic math routines.

- `myprogram` is the full path to the executable to be generated.

</p>
</details>


**<details><summary>Option 2: Generating & Linking a Shared Library** (click to expand)</summary>
<p>

To create the shared library, follow these steps.

1. Create the position independent object file by executing

    ```
    gcc -fPIC -c -Wall -I PathMKLInclude PathAPDSrc/f_apd_demodulation.c -o PathAPDBin/f_apd_demodulation.o
    ```

    on **Linux** & **Mac** and

    ```
    gcc -fPIC -c -Wall -I PathMKLInclude PathAPDSrc\f_apd_demodulation.c -o PathAPDBin\f_apd_demodulation.o
    ```

    on **Windows**.
    
2. Create the binary file of the library. To this end, use

    ```
    gcc -shared PathAPDBin/f_apd_demodulation.o -o PathAPDBin/libapd.so
    ```
    
    for **Linux**,

    ```
    gcc -dynamiclib PathAPDBin/f_apd_demodulation.o PathMKLLib/libmkl_intel_lp64.dylib PathMKLLib/libmkl_sequential.dylib PathMKLLib/libmkl_core.dylib -o PathAPDBin/libapd.dylib
    ```
    
    for **Mac**, and

    ```
    gcc -shared PathAPDBin\f_apd_demodulation.o PathMKLLib\mkl_rt.lib -o PathAPDBin\libapd.dll -Wl,--out-implib,PathAPDBin\libapd.dll.a
    ```
    
    for **Windows**.
    
Here, `PathAPDSrc`, `PathMKLInclude`, and `PathMKLLib` are the same as in the description of **Option 1**. `PathAPDBin` is the full path to the folder where you place the shared (dynamic-link) library files of *AP&nbsp;Demodulation*, e.g., *./C/libbin*.

To access the functionality of the *AP&nbsp;Demodulation* library in your program, include the header file *h_apd.h* in the source files of that program. The basic form of the compilation directive used to generate the executable file of your program is then<sup>[5](#footnote5)</sup>

```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathAPDBin/libapd.so PathMKLLib/libmkl_intel_lp64.so PathMKLLib/libmkl_sequential.so PathMKLLib/libmkl_core.so -lm -o myprogram
```

for **Linux**,

```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathAPDBin/libapd.dylib PathMKLLib/libmkl_intel_lp64.dylib PathMKLLib/libmkl_sequential.dylib PathMKLLib/libmkl_core.dylib -lm -o myprogram
```

for **Mac**, and

```
gcc -I PathAPDSrc -I PathMKLInclude mycode.c PathAPDBin\libapd.dll PathMKLLib\mkl_rt.lib -o myprogram
```

Finally, for the dynamic loader to recognize the `apd` library, you have to append `PathAPDBin` to the `LD_LIBRARY_PATH` (for Linux), `DYLD_LIBRARY_PATH` (for Mac), or `Path` (for Windows) environment variables. This can be done in the same way as setting the load path for the oneMKL library explained [above](#SecExtLibC).

</p>
</details>

___
<a name="footnote4">**4**</a>: MinGW provides access to GCC on Windows OS.

<a name="footnote5">**5**</a>: The above compilation statements are minimal &#8211; they do not assume additional source files, libraries, or compilation options. The latter can be added following the input argument syntax of the C compiler used if needed. If GCC is the compiler of your choice, please check *"An Introduction to GCC"* by Brian Gough, an excellent and concise text on the most relevant aspects of GCC usage in practice.
___


<a name="SecAccCpp"></a>
### |1.7|&nbsp; Access from C++

The library is "C++-aware", so that its frontend functions can be called from C++ programs directly. We are planning to include a C++ class wrapper of *AP&nbsp;Demodulation* in the next update.
&nbsp;



<a name="SecImplM"></a>
## |2|&nbsp; MATLAB Implementation

<a name="SecContM"></a>
### |2.1|&nbsp; Contents of ./MATLAB

- \[**./libm**\] &#8211; folder with the MATLAB m-file code of the *AP&nbsp;Demodulation* library:

    - *f_apd_demodulation.m* defines the `f_apd_demodulation` function (the user's interface to the *AP&nbsp;Demodulation* algorithms).

    - *f_apd_{basic, accelerated, projected}.m* defines the `f_apd_{basic,accelerated,projected}` functions, which estimate the modulator of a signal by using the AP-{Basic, Accelerated, Projected} algorithms.
    
    - *f_apd_compresion.m* defines the `f_apd_compression` function, which performs signal compression.
    
    - *f_apd_interpolation.m* defines the `f_apd_interpolation` function, which performs signal interpolation on a refined uniform grid.
    
    - *f_apd_input_validation.m* defines the `f_apd_input_validation` function, which checks the validity of input arguments for `f_apd_demodulation`.

- \[**./libmex**\] &#8211; folder with the code of the MATLAB MEX function interface of the *AP&nbsp;Demodulation* library implemented in C:

    - *f_apd_demodulation_mex.c* defines the MEX gateway function for `f_apd_demodulation` defined in *../C/libsrc/*.

    - *compilation.m* is a MATLAB script for compiling *f_apd_demodulation_mex.c* to produce the corresponding MEX function executable.

\[**./examples**\] &#8211; folder with five examples (example\[1-5\].m) of signal demodulation, demonstrating various usage cases of `f_apd_demodulation` and `f_apd_demodulation_mex`.


<a name="SecFrntFcM"></a>
### |2.2|&nbsp; Frontend Functions

The user's interface to the MATLAB *m-file* and *MEX* versions of the *AP&nbsp;Demodulation* library consists of, respectively, functions, `f_apd_demodulation` and `f_apd_demodulation_mex`.<sup>[6](#footnote6)</sup> Both of them have identical input and output content. So, we describe only the first of them here.

**`f_apd_demodulation`** is the user’s gateway to the *AP&nbsp;Demodulation* computing algorithms.

<details><summary>FULL DESCRIPTION (click here)</summary>
<p>

```
function [m_out, e_out, iter, tcpu] = f_apd_demodulation (s, Par, Ub, t)
					   
% P U R P O S E
%
% Performs demodulation of the input signal in the offline mode by using the
% AP Demodulation approach.
%
%
% I N P U T   A R G U M E N T S
%
% [s] - input signal. This is a real array of a chosen dimension D <= 3. If the input
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
%               infeasibility error, Ïµ, drops to the level of .Et or below. If
%               .Et â‰¤ 0, then the maximum allowed number of iterations, .Ni (see
%               below), is completed.
%
%         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
%               chosen AP algorithm is iterated not more than .Ni times independent
%               of whether Ïµ drops at or below .Et.
%
%         .Nr - numbers of sample points on the refined uniform interpolation grid in
%               every dimension. This is an array of D elements. This field must be
%               defined only if the fourth input argument, t, is provided and is
%               nonempy (see below). Otherwise, .Nr is ignored.
%
%         .Cp - compression parameter. If .Cp > 1, demodulation is performed by using
%               signal compression (see equations (12)-(13) in the paper cited in the
%               header of this file. This field is optional (the default is .Cp==1).
%
%         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
%               when the value of the lambda factor drops below one. If .Br==1,
%               premature termination is assumed (default). Otherwise, the AP-A is
%               not stopped premature even when lambda decreases below 1. This field
%               is required only if .Al=='A'. It is optional (the default is .Br==1).
%
%         .im - array with the iteration numbers at which the modulator estimates 
%               have to be saved for the output. If .im is empty, only the final
%               modulator estimate is saved. The same is true when numel(i.im)==1 and
%               .im(1)==.Ni. This field is optional (.im==[] is assumed by default).
%
%         .ie - array with the iteration numbers at which the infeasibility error,
%               epsilon, values have to be saved for the output. If .ie is empty,
%               only the final error is saved. The same is true when numel(i.im)==1
%               and .im(1)==.Ni. This field is optional (.ie==[] is assumed by
%               default).
%
% [Ub] - array with the upper bound constraints on the modulator. Two options are
%        possible. If Ub is an empty array, no upper bound constraint is assumed.
%        Otherwise, Ub must be an array with the same dimensions as s. This input
%        argument is optional (Ub==[] is assumed by default).
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
```

</p>
</details>

___
<a name="footnote6">**6**</a>: The MEX implementation features shorter computing times than its m-file counterpart. The difference between the two versions is the most substantial for short 1D signals, when it may reach &#x223C;100 times. The MEX version advantage saturates at &#x223C;4 times for very long signals. Both implementations show similar performance for signals defined in higher dimensions.
___


<a name="SecExtLibM"></a>
### |2.3|&nbsp; External Libraries

The *m-file* version of the library relies solely on MATLAB kernel and works straight away without any additional actions. The *MEX* version follows the same requirements as the C implementation. In particular, compilation and usage of `f_apd_demodulation_mex` require Intel's oneAPI Math Kernel Library to be present on your system (see [above](#SecExtLibC) for further information).


<a name="SecCmpM"></a>
### |2.4|&nbsp; Compilation

The `f_apd_demodulation_mex` executable is built by simply running the *compilation.m* script. For it to succeed, some contribution is needed from the user:

- The paths corresponding to the locations of relevant *AP&nbsp;Demodulation* and MKL library files on the system need be modified in the mentioned script. This script is well commented on, making the required modification straightforward.

- A C compiler supporting C99 or later standards has to be available on the system. We recommend using GCC, a state-of-the-art compiler available free of charge for all major OS types. It is preinstalled on Linux systems as a rule. A GCC installation guide for Windows and Mac can be found following [this link](https://www.guru99.com/c-gcc-install.html).

- The MATLAB's built-in MEX builder function `mex` has to be configured to use the compiler of your choice (e.g., GCC or its Windows OS alternative MinGW). This can be achieved by simply typing `mex -setup` in MATLAB's command prompt and then selecting the needed compiler).

  >**For Windows Users:** An environment variable with the compiler's installation path may be needed to set additionally for MATLAB to recognize it. For example, in the case of MinGW, MW_MINGW64_LOC with the full path must be set. You can find how to do this [here](https://www.mathworks.com/matlabcentral/answers/313298-i-already-have-mingw-on-my-computer-how-do-i-configure-it-to-work-with-matlab). Note that MATLAB would not correctly parse white space characters in the installation pathname. Thus, the latter must not contain white spaces.
&nbsp;



<a name="SecExmpl"></a>
## |3|&nbsp; Examples

- **Example&nbsp;1** ([C](C/examples/example1.c), [MATLAB](MATLAB/examples/example1.m)) illustrates the usage of *AP&nbsp;Demodulation* in its simplest form, when the signal is sampled uniformly and when only the final estimates of the modulator and infeasibility error are required.

- **Example&nbsp;2** ([C](C/examples/example2.c), [MATLAB](MATLAB/examples/example2.m)) illustrates application of *AP&nbsp;Demodulation* for demodulating 2D signals.

- **Example&nbsp;3** ([C](C/examples/example3.c), [MATLAB](MATLAB/examples/example3.m)) illustrates application of *AP&nbsp;Demodulation* for demodulating nonuniformly sampled signals.
- **Example&nbsp;4** ([C](C/examples/example4.c), [MATLAB](MATLAB/examples/example4.m)) illustrates how to set the upper bound constraints on the modulator estimates and obtain intermediate modulator estimates and infeasibility errors by using *AP&nbsp;Demodulation*. The results obtained also reveal that imposing an upper bound may reduce the rate of convergence of the AP algorithm in terms of infeasibility error. However, that may have no practical impact on the convergence in terms of demodulation error.

- **Example&nbsp;5** ([C](C/examples/example5.c), [MATLAB](MATLAB/examples/example5.m)) illustrates how to infer the upper and lower envelopes of a signal by using *AP&nbsp;Demodulation*.
&nbsp;



<a name="SecUseTip"></a>
## |4|&nbsp; Usage Tips

Below, we give some practical tips for using *AP&nbsp;Demodulation* effectively. We recommend reading the [Theoretical Background](#SecTheory) section (or the original paper cited in the [About](#SecAbout) section) beforehand to get the most of the guidance provided here.

**<details><summary>Choice of the Algorithm**</summary>
<p>

*AP&nbsp;Demodulation* defines three algorithms that feature a different balance between the scope of recovery guarantees and speed: *AP-Basic (AP-B)*, *AP-Accelerated (AP-A)*, and *AP-Projected (AP-P)*. As mentioned in [Theoretical Background](#SecNewAppImp), *AP-A* stands out concerning the convergence rate, *AP-P* features the widest exact-recovery guarantees, and *AP-Basic* balances these two aspects. Nevertheless, for many signals met in practice, the modulator recovery precision shown by all three methods is rather similar.

Based on these findings, the following rules of thumb can be used when selecting the algorithm:

- *AP-A* must always be tried first.

- The demodulation accuracy superiority of *AP-P* offsets its speed disadvantage compared with *AP-A* mainly for signals of highly irregular spike-train nature, e.g., signals built of spike-train carriers, signals with many missing sample points, or highly nonuniformly sampled signals.

- When applied to signals of irregular spike-train nature, *AP-B* often provides accuracy sufficient for practical purposes while being considerably faster than *AP-P*.

The algorithm selection is passed to `f_apd_demodulation(_mex)` via `Par.Al`.

</p>
</details>


**<details><summary>Choice of the Cutoff Frequency**</summary>
<p>

One of the pivotal parameters in determining the outcome of *AP&nbsp;Demodulation* algorithms that the user must set is the cutoff frequency <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only">. As explained in [Theoretical Background](#SecNewAppImp), exact modulator recovery is possible only if <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> is not smaller than the modulator's cutoff frequency <!-- $\omega$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/omega.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/omega_dark.svg#gh-dark-mode-only">. On the other hand, how high <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> can be is constrained by the fundamental recovery condition. Of course, in practice, modulators are not strictly low-pass. However, as long as the cumulative energy of the modulator above <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> is vanishing, so is the demodulation error.

No rule would allow finding the interval of eligible <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> values without additional information. Two possibilities exist in general:

- Inferring appropriate <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> based on prior knowledge about the partial but sufficient properties of modulators and carriers that the signal under consideration consists of.

- Determining appropriate <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> in a supervised learning setup when signal examples and the corresponding modulators from the same family as the signal under consideration are available.

The value of <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> is passed to `f_apd_demodulation(_mex)` via `Par.Fc`. For consistency reasons, it must have the same physical dimension as the sampling frequency set by `Par.Fs`.

</p>
</details>


**<details><summary>Termination Conditions**</summary>
<p>

*AP&nbsp;Demodulation* algorithms are iterative in nature, and they converge to the limit point monotonically. Hence, when the full recovery conditions are satisfied, a more and more accurate modulator estimate is obtained with each iteration.

Ideally, we would like to have access to the convergence error at each iteration to decide when the accuracy sufficient for our purposes is reached. This is, however, impossible in practice because the true modulator is not known in advance. As a proxy of the convergence error, *AP&nbsp;Demodulation* exploits the infeasibility error, &#1013;, which is the relative distance from the current point to any of the constraint sets implied onto the modulator<sup>[7](#footnote7)</sup>. By the construction of *AP&nbsp;Demodulation*, &#1013; is bound to follow the convergence error tightly, which justifies its choice for this purpose.

The termination of *AP&nbsp;Demodulation* algorithms is controlled by two input parameters to `f_apd_demodulation(_mex)`:

- The infeasibility error tolerance, `Par.Et`,

- The maximum allowed number of iterations, `Par.Ni`.

`f_apd_demodulation(_mex)` finishes when either &#1013; drops below `Par.Et`, the iteration count exceeds `Par.Ni` or both. To terminate the algorithm after exactly `Par.Ni` iterations independent of &#1013;, assign a nonpositive value to `Par.Et`.

The number of algorithm iterations typically sufficient in practice is

- ~5&middot;10<sup>0</sup> for AP-Accelerated,
- ~5&middot;10<sup>2</sup> for AP-Basic,
- ~5&middot;10<sup>3</sup> for AP-Projected.

These numbers are however only for guidance &#8211; each particular situation needs to be considered separately.

___
<a name="footnote7">**7**</a>: Specifically, &#1013; is the Euclidean distance from the current estimate to the most distant constraint set divided by the square root of the total number of sample points.
___

</p>
</details>


**<details><summary>Obtaining the Final & Intermediate Estimates**</summary>
<p>

`f_apd_demodulation(_mex)` returns the final estimates of the modulator and the corresponding infeasibility error at iterations kept respectively in `Par.im` and `Par.ie` arrays. The data arrangement in these arrays slightly differs between C and MATLAB implementations.

- In **C**, the first elements of `Par.im` and `Par.ie` store the total numbers of iterations at which the modulator and error estimates are to be saved. The remaining entries hold the iteration numbers put as strictly ascending numbers.

- In **MATLAB**, all entries of `Par.im` and `Par.ie`  hold the iterations at which the modulator and error estimates are to be saved put in strictly ascending numbers.

Typically, only the final estimate is of interest. It can be obtained as follows.

- In **C**, set the first element of `Par.im` and/or `Par.ie` to 1 and the second element to `Par.Ni`.

- In **MATLAB**, set `Par.im` and/or `Par.ie` to `1` or simply ignore these fields, i.e., set them to be empty or not define them at all.

In this case, the final iteration will be saved independently of whether this iteration coincides with `Par.Ni` or not. This does not apply when more than one iteration is indicated by `Par.im` and/or `Par.ie`. Then, all entries of `Par.im` and `Par.ie` that exceed the iteration at which the algorithm terminates will be ignored.

</p>
</details>


**<details><summary>Compression**</summary>
<p>

In some cases, the cumulative energy of the modulator residing in the spectral region above any reasonable <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/varpi_dark.svg#gh-dark-mode-only"> may be unacceptably high. For example, this happens when an otherwise low-pass modulator features abrupt transitions to/from prolonged intervals of low-signal amplitude (e.g., see Fig.&nbsp;7 in the paper cited in the [About](#About) section). In such cases, the demodulation error can be reduced considerably by using the compression trick. Specifically, we replace a signal, <!-- $\mathbf{s}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/s_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/s_bold_dark.svg#gh-dark-mode-only">, by its compressed version
<!-- $$
\mathbf{\underline{s}} = \mathrm{sgn}(\mathbf{s}) \circ |\mathbf{s}|^{1/p}.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/s_ubar_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/s_ubar_def_dark.svg#gh-dark-mode-only"></div>

Here <!-- $\mathrm{sgn}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/sgn.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/sgn_dark.svg#gh-dark-mode-only"> is the elementwise sign function of vector elements, <!-- $\circ$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/circ.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/circ_dark.svg#gh-dark-mode-only"> denotes the elementwise product of two vectors, and <!-- $p \in (1,\infty)$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/p_range.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/p_range_dark.svg#gh-dark-mode-only"> is the compression parameter. The modulator estimate of the original signal is then obtained by decompressing the modulator estimate of <!-- $\mathbf{\underline{s}}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/s_ubar.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/s_ubar_dark.svg#gh-dark-mode-only">:
<!-- $$
\mathbf{m}^* = \mathbf{\underline{m}}^p.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/m_star_bold.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/m_star_bold_dark.svg#gh-dark-mode-only"></div>

The optimal value of <!-- $p$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/p.svg#gh-light-mode-only"> <img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/p_dark.svg#gh-dark-mode-only"> depends on the signal class and can be found empirically if not known in advance. We refer the user to Section&nbsp;VIII.B in the paper cited in the [About](#About) section for more information on using the compression trick.

In `f_apd_demodulation(_mex)`, the compression parameter $p$ is represented by `Par.p`. If `Par.p<=1`, no compression is assumed. 

</p>
</details>


**<details><summary>Nonuniform Sampling**</summary>
<p>

In its default mode, *AP&nbsp;Demodulation* assumes that sample points of the input signal are uniformly spaced. Nonuniformly sampled signals can be handled by exploiting a special form of interpolation on a refined uniform grid (see Section&nbsp;IX.B in the paper cited in the [About](#About) section for the mathematical definition of this procedure).

In practice, the interpolation is achieved by passing two additional input arguments to `f_apd_demodulation(_mex)`:

- The coordinates of the sample points as the fourth input argument `t`.

- The number of sample points along each dimension of the refined uniform grid as `Par.Nr`.

[Example&nbsp;3](SecExmpl) illustrates how to use `f_apd_demodulation(_mex)` in this setting.

</p>
</details>


**<details><summary>Bounding Modulators from Above**</summary>
<p>

In its original formulation (see the paper cited in the [About](#About) section),  *AP&nbsp;Demodulation* algorithms assume no upper bound on the modulator. If the information about the upper bound on the modulator is known in advance, it may be exploited to increase inference accuracy. This is especially relevant in situations when the exact recovery condition (see [Theoretical Background](#SecNewAppImp)) is not satisfied by a large margin.

`f_apd_demodulation(_mex)` accepts the upper bound on the modulator as its third input argument, `Ub`, and exploits this information if provided. There are a few points to take into account if using the upper bound constraint:

- Despite the fact that all *AP&nbsp;Demodulation* algorithms converge in theory when assuming the upper bound constraint, the convergence rate is compromised. We found that this has noticeable consequences only when very small recovery errors are targeted. In practice, such low recovery errors are rarely achievable in principle because the full recovery conditions are not met precisely. Hence, in many cases, using the upper bound constraint is rational (see [Example&nbsp;4](SecExmpl) for an illustration).

- In the case of the *AP-Accelerated* algorithm, setting the upper bound constraint may cause self-propelling numerical inaccuracies. The breakdown point is reached when the relaxation parameter &#955; drops below 1 (in theory, &#955;&#8805;1). *AP-Accelerated* can be terminated automatically upon such event by setting `Par.Br` to `1`. We highly recommend doing this because, otherwise, the obtained modulator estimate may be erroneous.

</p>
</details>


**<details><summary>Boundary effects**</summary>
<p>

We consider finite segments of signals in *AP&nbsp;Demodulation*. Hence, the use of Fourier Transform in the corresponding algorithms necessarily implies an implicit assumption of the periodic boundary conditions. Real-world signals do not typically satisfy this assumption. In these cases, the transition between the last and the first sample points induces spurious high-frequency components in the Fourier Transform of the signal. This leads to inaccuracies in the modulator estimates at the beginning and the end of the sampled interval.

Ideally, these distortions can be avoided by collecting additional sample points at the beginning and the end of the signal segment of interest. Typically, the spurious effects are noticeable only in the intervals of the first and the last <!-- $3 \cdot n \cdot (\varpi/f_s)$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/transit_window.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/transit_window_dark.svg#gh-dark-mode-only"> sample points (here, <!-- $n$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/n.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/n_dark.svg#gh-dark-mode-only"> is the total number of sample points).

If collecting additional sample points is not possible, the boundary effects can sometimes be reduced by using signal windowing. Specifically, instead of demodulating the original signal, we demodulate its windowed version,
<!-- $$
\mathbf{s}_\mathrm{w} = \mathbf{w} \circ \mathbf{s}.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/sw_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/sw_def_dark.svg#gh-dark-mode-only"></div>

Then, the modulator of the original signal is calculated by reversing the windowing effect on the modulator estimate of the windowed signal,
<!-- $$
\mathbf{m} = \mathbf{w^{-1}} \circ \mathbf{m}_\mathrm{w}.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/inv_window.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/inv_window_dark.svg#gh-dark-mode-only"></div>

Here, the window must smoothly scale the signal towards 0 at the boundaries, with no effect at the midst. A modified Hann window is a good candidate in many cases:
<!-- $$
\mathrm{w}_i =\begin{cases} \sin^2\Big(\frac{\pi \cdot (i+1/2)}{2 \cdot \tau}\Big), & \quad 1 \leq i < \tau\\ 1, & \quad \tau \leq i \leq n - \tau \\
\cos^2\Big(\frac{\pi \cdot (i+\tau-n-1/2)}{2 \cdot \tau}\Big), & \quad n-\tau < i \leq n \end{cases}.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/hann_window.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/hann_window_dark.svg#gh-dark-mode-only"></div>

Here, <!-- $\tau$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/tau.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/tau_dark.svg#gh-dark-mode-only"> is the length of the transition window where the signal points are scaled. Typically, <!-- $\tau=3 \cdot n \cdot (\varpi/f_s)$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/tau_eq_transition_window.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/tau_eq_transition_window_dark.svg#gh-dark-mode-only"> is sufficient.

Signal windowing transformations are not implemented in `f_apd_demodulation(_mex)` and must be managed directly by the user.

</p>
</details>


**<details><summary>Generalized Envelope Detection**</summary>
<p>

In the amplitude demodulation setting, the modulator is assumed to be slowly modulating a carrier that fluctuates in a bounded interval around zero without a long-term trend. The modulated signal has clearly pronounced lower and upper envelopes, which are both equal to the modulator in absolute value. However, there exist signals to which these assumptions do not apply, yet, they have (possibly different) identifiable lower and upper envelopes.

*AP&nbsp;Demodulation* is well suited to infer signal envelopes of such signals too. However, a simple pre- and postprocessing of the signals is needed. Specifically, an upper envelope can be obtained by following these steps:

1. Offset the original signal by subtracting the value of its smallest sample point:
   <!-- $$
   \mathbf{s}_\mathrm{o} = \mathbf{s} - \mathrm{min}[\mathbf{s}].
   $$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/so_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/so_def_dark.svg#gh-dark-mode-only"></div>

2. Calculate the modulator, <!-- $\mathbf{m}_\mathrm{o}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/mo.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/mo_dark.svg#gh-dark-mode-only">, of <!-- $\mathbf{s}_\mathrm{o}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/so.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/so_dark.svg#gh-dark-mode-only"> by using a chosen *AP&nbsp;Demodulation* algorithm.
3. The upper envelope of <!-- $\mathbf{s}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/s_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/s_bold_dark.svg#gh-dark-mode-only"> is then given by
   <!-- $$
   \mathbf{e}_\mathrm{u} = \mathbf{m}_\mathrm{o} + \mathrm{min}[\mathbf{s}].
   $$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/eu_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/eu_def_dark.svg#gh-dark-mode-only"></div>

The lower envelope is just the negative upper envelope of <!-- $-\mathbf{s}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/minus_s_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/minus_s_bold_dark.svg#gh-dark-mode-only">.

The signal pre- and postprocessing steps required for generalized envelope detection are not implemented in `f_apd_demodulation(_mex)` and must be programmed directly by the user. [Example 5](#SecExmpl) illustrates using *AP&nbsp;Demodulation* in this setting.
&nbsp;

</p>
</details>



<a name="SecTheory"></a>
# Theoretical Background

Here, we provide a minimal theoretical background behind the new approach, which we believe is necessary for an efficient and fruitful application of the underlying numerical algorithms in practice. We refer the user to the paper cited in the [About](#About) section for the full mathematical theory underlying the *AP&nbsp;Demodulation* approach and analysis of its properties.

<a name="SecAD"></a>
### Amplitude Demodulation

Amplitude demodulation refers to the factorization of a signal into a slow-varying modulator and fast varying carrier.

<p align="center">

  <img src="svg/Fig1_what_is_AD.svg#gh-light-mode-only" />

  <img src="svg/Fig1_what_is_AD_dark.svg#gh-dark-mode-only" />
  
</p>

<a name="SecClsSet"></a>
### Classical Setting

Originally, amplitude demodulation was intended for use with signals built of nonnegative low-pass modulators and locally sinusoidal carriers, such that the frequency of the carrier is above the frequency band of the modulator.

<p align="center">

  <img src="svg/Fig2_classical_setting.svg#gh-light-mode-only" />

  <img src="svg/Fig2_classical_setting_dark.svg#gh-dark-mode-only" />
  
</p>


<a name="SecBeClsSet"></a>
### Beyond the Classical Setting

The classical setting has been fruitfully exploited. However, many relevant problems require demodulating signals built from wideband carriers. Three major types of these carriers have been identified, namely, harmonic/quasi-harmonic, random/quasi-random, and regular or irregular spike trains.

<p align="center">

  <img src="svg/Fig3_wideband_signals_1.svg#gh-light-mode-only" />

  <img src="svg/Fig3_wideband_signals_1_dark.svg#gh-dark-mode-only" />
  
</p>

When applied to process signals made of such carriers, classical approaches fail by mixing the carrier and modulator information.

<p align="center">

  <img src="svg/Fig3_wideband_signals_2.svg#gh-light-mode-only" />

  <img src="svg/Fig3_wideband_signals_2_dark.svg#gh-dark-mode-only" />
  
</p>

<a name="SecNewApp"></a>
### New Approach

*AP&nbsp;Demodulation* frames demodulation as a recovery problem of a signal from an unlabelled mix of its true and corrupted sample points. It achieves the recovery via tailor-made alternating projection algorithms.

<a name="SecNewAppSet"></a>
#### Setting

- We consider real signals in their sampled representation so that each signal is an element in an *n*-dimensional Euclidean space:
  <!-- $$
  \mathbf{s} \equiv (s_1,s_2,\ldots,s_n)^\mathrm{T} \in \mathbb{R}^n.
  $$ --> 

  <div align="center"><img style="background: none;#gh-light-mode-only" src="svg/s_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/s_def_dark.svg#gh-dark-mode-only"></div>

- Modulators are assumed to be elements of a set of nonnegative low-pass signals, parameterized by the cutoff frequency <!-- $\omega$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/omega.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/omega_dark.svg#gh-dark-mode-only">:
  <!-- $$
  \mathbf{m} \in \mathcal{M}_{\omega} = \underbrace{\{ \mathbf{x} \in \mathbb{R}^n: x_i \geq 0\}}_{non\text{-}negative} \cap \underbrace{\{ \mathbf{x} \in \mathbb{R}^n: (\mathbf{F}\mathbf{x})_i=0, i >\omega\}}_{low\text{-}pass}.
  $$ --> 

  <div align="center"><img style="background: none;#gh-light-mode-only" src="svg/Mw_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/Mw_def_dark.svg#gh-dark-mode-only"></div>

   This is exactly the definition of modulators considered in the classical setting.

- Carriers are assumed to be signals bounded between -1 and +1 such that the maximum gap between the neighboring points with the absolute value equal to 1 is not bigger than some integer <!-- $d$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/d_italic.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/d_italic_dark.svg#gh-dark-mode-only">:
  <!-- $$
  \mathcal{C}_d = \underbrace{\{ \mathbf{x} \in \mathbb{R}^n: |x_i| \leq 1\}}_{bounded~~btw~-1~and~1} \cap \underbrace{\{\mathbf{x} \in \mathbb{R}^n: (\forall i) \textstyle \sum_{j=i}^{i+d-1}I_{\{1\}}(|x_j|) \geq 1\}}_{|x_i|=1~points~~d\text{-}densely~spread}.
  $$ --> 

  <div align="center"><img style="background: none;#gh-light-mode-only" src="svg/Cd_def.svg#gh-light-mode-only"><img style="background: none;#gh-dark-mode-only" src="svg/Cd_def_dark.svg#gh-dark-mode-only"></div>
  This definition goes well beyond sine waves and covers all practically relevant carrier types, both narrowband and wideband.

  <p align="center">
  
    <img src="svg/Fig4_new_approach_setting.svg#gh-light-mode-only" />
    
    <img src="svg/Fig4_new_approach_setting_dark.svg#gh-dark-mode-only" />
  
  </p>

<a name="SecNewAppPri"></a>
#### Principle

- Sample points with <!-- $|c_i|=1$ --> <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/abs_ci_eq_1.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/abs_ci_eq_1_dark.svg#gh-dark-mode-only"> correspond to <!-- $|s_i| = m_i$ --> <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/si_eq_mi.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/si_eq_mi_dark.svg#gh-dark-mode-only">.

- If the positions of these sample points were known, we could recover <!-- $\mathbf{m}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/m_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/m_bold_dark.svg#gh-dark-mode-only"> from <!-- $\mathbf{s}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/s_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/s_bold_dark.svg#gh-dark-mode-only"> by using the classical sampling theorem.

- The positions of <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/abs_ci_eq_1.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/abs_ci_eq_1_dark.svg#gh-dark-mode-only"> are, however, not known. Hence, amplitude demodulation can be seen as the task of modulator recovery from a mix of its true (<!-- $|c_i|=1$ --> <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/abs_ci_eq_1.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/abs_ci_eq_1_dark.svg#gh-dark-mode-only">) and corrupted (<!-- $|c_i|\neq1$ --> <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/ci_neq_1.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/ci_neq_1_dark.svg#gh-dark-mode-only">) sample points when the labels of sample points are not known.

- If we manage to find <!-- $\mathbf{m}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/m_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/m_bold_dark.svg#gh-dark-mode-only">, <!-- $\mathbf{c}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/c_bold.svg#gh-light-mode-only"><img style="transform: translateY(0.1em); background: none;#gh-dark-mode-only" src="svg/c_bold_dark.svg#gh-dark-mode-only"> follows trivially from <!-- $c_i=s_i/m_i$ --> <img style="transform: translateY(0.3em); background: none;#gh-light-mode-only" src="svg/ci_eq_si_r_mi.svg#gh-light-mode-only"><img style="transform: translateY(0.3em); background: none;#gh-dark-mode-only" src="svg/ci_eq_si_r_mi_dark.svg#gh-dark-mode-only">.<sup>[8](#footnote8)</sup>


<a name="SecNewAppImp"></a>
#### Implementation

Remarkably, the modulator can be recovered from an unlabeled mix of its true and corrupted sample points via a constrained &#8467;<sup>2</sup> norm minimization as long as the true sample points are spread densely enough. Specifically, with an overwhelming probability,
<!-- $$
\displaystyle \mathbf{m} = \underset{\mathbf{x} \in \mathcal{S}_s \cap \mathcal{S}_\varpi}{\arg\min} \|\mathbf{x}\|_2, \quad\textrm{if}~\lceil n / d \rceil \geq 2\varpi - 1.
$$ --> 

<div align="center"><img style="background: none;#gh-light-mode-only" src="svg/m_eq_argmin.svg#gh-light-mode-only"></div>
Here,

- <!-- $\mathcal{S}_{s} = \{\mathbf{x} \in \mathbb{R}^n: x_i \geq |\mathbf{s}|\}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/Ss_def.svg#gh-light-mode-only"> is a constraint set which implies that the modulator estimate must be elementwise not smaller than the absolute value of the signal.

- <!-- $\mathcal{S}_{\varpi} = \{\mathbf{x} \in \mathbb{R}^n: (\mathbf{F}\mathbf{x})_i=0, i >\varpi\ \geq \omega\}$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/Sw_def.svg#gh-light-mode-only"> is the constraint set which assures that the modulator estimate belongs to the set of low pass signals with the cutoff frequency parameter not lower than that of the true modulator.

- <!-- $\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/varpi.svg#gh-light-mode-only"> is the cutoff frequency that parameterizes the set <!-- $\mathcal{S}_\varpi$ --> <img style="transform: translateY(0.1em); background: none;#gh-light-mode-only" src="svg/Sw.svg#gh-light-mode-only">.

*AP&nbsp;Demodulation* defines three alternating projection algorithms that feature a different balance between the accuracy and speed to solve the above minimization problem in an iterative manner: **AP-Basic**, **AP-Accelerated**, and **AP-Projected**. *AP-Accelerated* stands out with respect to the convergence rate, *AP-Projected* features the widest exact-recovery guarantees, whereas *AP-Basic* balances these two aspects (see the [Usage Tips](#SecUseTip) section for advice how to select one of the available algorithms in practice).


<a name="SecNewAppCap"></a>
#### Capabilities

The new approach allows demodulating both narrowband and wideband signals accurately in many relevant situations:

<p align="center">

  <img src="svg/Fig5_new_approach_examples_1.svg#gh-light-mode-only"  />

  <img src="svg/Fig5_new_approach_examples_1_dark.svg#gh-dark-mode-only"  />
  
</p>

It also has other practically important qualities:

<p align="center">

  <img src="svg/Fig5_new_approach_examples_2.svg#gh-light-mode-only"  />

  <img src="svg/Fig5_new_approach_examples_2_dark.svg#gh-dark-mode-only"  />
  
</p>

___
<a name="footnote8">**8**</a>: This does not work for sample points with <!-- $m_i=0$ --> <img style="transform: translateY(0.2em); background: none;#gh-light-mode-only" src="svg/mi_eq_0.svg#gh-light-mode-only"><img style="transform: translateY(0.2em); background: none;#gh-dark-mode-only" src="svg/mi_eq_0_dark.svg#gh-dark-mode-only">. However, if present at all, the latter are sparse and can be interpolated from the neighboring points.
___



&nbsp;
<a name="SecAcknw"></a>
# Acknowledgments

Many thanks to [Edvinas](https://github.com/vebvinas) for helping with testing the library build settings in different OS environments.