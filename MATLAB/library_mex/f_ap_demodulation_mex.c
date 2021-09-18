
/*                       C O P Y R I G H T   N O T I C E
 *
 * Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
 * All Rights Reserved. The underlying technology is protected by PCT Patent
 * Application No. PCT/EP2021/054650.
 *
 * This file is part of the AP Demodulation library, which is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation in version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License v2 for more details. You
 * should have received a copy of the GNU General Public License v2 along with this
 * program. If not, see https://www.gnu.org/licenses/.
 *
 * Contact the Technology Transfer Office, IST Austria, Am Campus 1,
 * A-3400 Klosterneuburg, Austria, +43-(0)2243 9000, twist@ist.ac.at, for commercial
 * licensing opportunities.
 *
 * All other inquiries should be directed to the author, Mantas Gabrielaitis,
 * mgabriel@ist.ac.at
 */


/* C O N T E N T S
 *
 * A MEX gateway routine for f_ap_demodulation:
 *
 * (1) mexFunction.
 */



#include <time.h>

#include "mex.h"

#include "f_ap_demodulation.c"



void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
/* S H O R T   D E S C R I P T I O N
 *
 * f_ap_demodulation_mex estimates the amplitude modulator of a given signal in the
 * offline mode by using the AP Demodulation approach formulated and developed in
 *
 *   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
 *   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
 *   10.1109/TSP.2021.3087899.
 *
 * Please cite the above publication when referring to the algorithms implemented in
 * this code or the code itself.
 *
 * See https://github.com/mgabriel-lt/ap-demodulation for the latest version of the
 * code and user-friendly explanations on the working principle, domains of
 * application, and advice on the usage of different AP Demodulation algorithms in
 * practice.
 */
    
/* I N P U T   A R G U M E N T S
 *
 * [1] - input signal. This is a real array with at least two elements and of a
 *       chosen dimension D ≤ 3. If the fourth input argument, t, is nonempty, i.e.,
 *       interpolation of the input signal is required, then s must be a 1D array.
 *
 * [2] - parameters characterizing the signal and demodulation procedure. This is a
 *       variable of the structure type. Its fields are as follows:
 *
 *       .Al - demodulation algorithm. Possible options are: 'B' - AP-Basic,
 *             'A' - AP-Accelerated, 'P' - AP-Projected.
 *
 *       .Fs - sampling frequencies for each dimension of the signal. This is an
 *             array of D elements.
 *
 *       .Fc - cutoff frequencies of the modulator for each dimension of the signal.
 *             This is an array of D elements.
 *
 *       .Et - infeasibility error tolerance used to control the termination of the
 *             demodulation algorithm. The iterative process is stopped when the
 *             infeasibility error, ϵ, drops to the level of .Et or below. If
 *             .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
 *             below), is completed.
 *
 *       .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
 *             chosen AP algorithm is iterated not more than .Ni times independent of
 *             whether ϵ drops at or below .Et.
 *
 *       .Nr - numbers of sample points on the refined uniform interpolation grid in
 *             every dimension. This is an array of D elements. This field must be
 *             defined only if the fourth input argument, t, is provided (see below).
 *             Otherwise, .Nr is ignored.
 *
 *       .Cp - compression parameter. If .Cp > 1, demodulation is performed by using
 *             signal compression (see equations (12)-(13) in the paper cited in the
 *             header of this file. This field is optional (the default is .Cp=1).
 *
 *       .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
 *             when the λ factor drops below one. If .Br=1 (recommended), premature
 *             termination is assumed (default). Otherwise, if .Br=0, the AP-A is not
 *             stopped prematurely even when λ decreases below 1. This field is
 *             required only if .Al='A'. It is optional (the default is .Br=1).
 *
 *       .im - array with the iteration numbers at which the modulator estimates 
 *             have to be saved for the output. If .im is empty, only the final
 *             modulator estimate is saved. This field is optional (.im=[] is assumed
 *             by default).
 *
 *       .ie - array with the iteration numbers at which the infeasibility error, ϵ,
 *             values have to be saved for the output. If .ie is empty, only the
 *             final error is saved. This field is optional (.ie=[] is assumed by
 *             default).
 *
 * [3] - array with the upper bound constraints on the modulator. Two options are
 *       possible. If Ub is an empty array, no upper bound constraint is assumed.
 *       Otherwise, Ub must be an array with the same dimensions as s. This input
 *       argument is optional (Ub=[] is assumed by default).
 *
 * [4] - sampling coordinates of the input signal. This is a real 2D array with the
 *       number of columns equal to the dimension of the input signal and the number
 *       of rows equal to the number of signal sample points. This input argument is
 *       optional and should be used only if the input signal is sampled
 *       nonuniformly. If this input argument is provided, then the second input
 *       argument must contain the field .Nr (see above).
 */

/* O U T P U T   A R G U M E N T S
 *
 * [1] - modulator.
 *
 * [2] - infeasibility error of the modulator, if .Online == 0, or CPU time for each
 *       demodulation window, if .Online == 1.
 *
 * [3] - number of AP iterations used (the average number is returned, in the
 *       case of window splitting or online demodulation).
 *
 * [4] - running time of the program in seconds.
 */
    
    
/***********************************************************************************/
/*********************** DECLARATIONS AND INITIALIZATIONS  *************************/
/***********************************************************************************/
    
    
    int exitflag = 0;
    
    mxChar *pr_c;
    
    double *pr_in;
    
    mxArray *pr_in2;
    
    mwSize D;
    
    const mwSize *Ns;
    
    const mwSize *sz_t;
    
    mwSize *Nm;
    
    long N;
    
    
    long i;
    
    long n = 1;
    
    long n_im;
    
    long n_ie;
    
    double *pr_s;
    
    double *pr_t = NULL;
    
    double *Ub = NULL;
    
    double *e = NULL;
    
    double *pr_m;
    
    double *pr_e;
    
    long iter;
    
    
    struct s_ParamsAP Par;
    
    
    clock_t start, end;
    
    double cpu_time_used;
    
    
    
    
    /* Start of measuring the running time */
    
    start = clock();
    
    
    
/***********************************************************************************/
/************************** READOUT OF INPUT ARGUMENTS  ****************************/
/***********************************************************************************/
     
    
    /* Checking if the number of input arguments is right */
    
    if( nrhs < 2 || nrhs > 4 )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Two to four input arguments "\
                                                    "are required!");
    
    
    /* Checking if the number of output arguments is right */
    
    if( nlhs < 1 || nlhs > 4 )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "One to four output arguments "\
                                                    "are required!");

                
                
    
    /* Making sure that the 1st input argument is a real array with right dimensions */
    
    N = mxGetNumberOfElements(prhs[0]);
    
    Ns = mxGetDimensions (prhs[0]);
    
    if ( nrhs < 4 || mxIsEmpty(prhs[3]) )
    {
        D = mxGetNumberOfDimensions (prhs[0]);
        
        if ( D == 2 && (Ns[0] == 1 || Ns[1] == 1) )
        
            D = D - 1;
    }
    else
    {
        sz_t = mxGetDimensions (prhs[3]);
        
        D = sz_t[1];
    }
        
    
    if ( !mxIsNumeric(prhs[0]) || N < 2 || D > 3 ) /*what if complex ??? */
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 1st input argument must be"\
                          " a signal with at least two sample points and no more"\
                          " than three dimensions!");
    
    
        
    
    /* Getting a pointer to the 1st input argument */
        
    pr_s = mxGetPr(prhs[0]);
    
    
    
    
    /* Checking if the 2nd input argument is a structure */
    
    if( !mxIsStruct(prhs[1]) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 2nd input argument must "\
                                                    "be a structure!");
    
                
    
    
    /* Creating a local input parameter structure Par */
                
    pr_in2 = mxGetField(prhs[1], 0, "Fs");
    
    if ( pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 2nd input argument must "\
                                                    "have the field Fs!");
    
    else if ( mxGetNumberOfElements(pr_in2) != D || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Fs' of the 2nd input "\
                          "argument must be a numeric array with the number of "\
                          "elements equal to the dimension of the signal!");
    
    else
        
        Par.Fs = mxGetPr(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Fc");
    
    if ( pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 2nd input argument must "\
                                                    "have the field Fc!");
    
    else if ( mxGetNumberOfElements(pr_in2) != D || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Fc' of the 2nd input "\
                          "argument must be a numeric array with the number of "\
                          "elements equal to the dimension of the signal!");
    
    else
        
        Par.Fc = mxGetPr(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Al");
    
    if ( pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal","The 2nd input argument must "\
                          "have the field 'Al'!");
    
    else if ( !mxIsScalar(pr_in2) || !mxIsChar(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Al' of the 2nd input "\
                          "argument must be a char scalar!");
    
    else
    {
        pr_c = mxGetChars(pr_in2);
        
        Par.Al = pr_c[0];
    }
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Et");
    
    if ( pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 2nd input argument must "\
                                                    "have the field Et!");
    
    else if ( !mxIsScalar(pr_in2) || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Et' of the 2nd input "\
                          "argument must be a numeric scalar!");
    
    else
        
        Par.Et = mxGetScalar(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Ni");
    
    if ( pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal","The 2nd input argument must "\
                          "have the field Ni!");
    
    else if ( mxGetNumberOfElements(pr_in2) != 1 || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Ni' of the 2nd input "\
                          "argument must be a numeric scalar!");
    
    else
        
        Par.Ni = (long) mxGetScalar(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Nr");
    
    if ( nrhs == 4 && !mxIsEmpty(prhs[3]) && pr_in2 == NULL )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 2nd input argument must "\
                                                    "have the field Nr!");
    
    else if ( nrhs == 4 && !mxIsEmpty(prhs[3]) && \
            (mxGetNumberOfElements(pr_in2) != D || !mxIsNumeric(pr_in2)) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Nr' of the 2nd input "\
                          "argument must be a numeric array with the number of "\
                          "elements equal to the dimension of the signal!");
    
    else if (nrhs == 4)
    {
        pr_in = mxGetPr(pr_in2);
        
        Par.Nr = (long*) mxMalloc(D*sizeof(long));
        
        if (Par.Nr == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal", \
                                              "Out of memory!");
        
        for (i=0; i<D; i++)
            
            Par.Nr[i] = (long) pr_in[i];
    }
    
    else
        
        Par.Nr = NULL;
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Cp");
    
    if ( pr_in2 == NULL || mxIsEmpty(pr_in2) )
        
        Par.Cp = 1;
    
    else if ( !mxIsScalar(pr_in2) || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Cp' of the 2nd input "\
                          "argument must be a numeric scalar!");
    
    else
        
        Par.Cp = mxGetScalar(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "Br");
    
    if ( pr_in2 == NULL || mxIsEmpty(pr_in2) )
        
        Par.Br = 1;
    
    else if ( !mxIsScalar(pr_in2) || !mxIsNumeric(pr_in2) )
        
        mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "Field 'Br' of the 2nd input "\
                          "argument must be a numeric scalar!");
    
    else
        
        Par.Br = (int) mxGetScalar(pr_in2);
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "im");
    
    if ( pr_in2 == NULL || mxIsEmpty(pr_in2) )
    {
        n_im = 1;
        
        
        Par.im = (long*) mxMalloc(2*sizeof(long));
        
        if (Par.im == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");
        
        
        Par.im[0] = n_im;
        
        Par.im[1] = Par.Ni;
    }
    
    else
    {
        pr_in = mxGetPr(pr_in2);
        
        n_im = (long) mxGetNumberOfElements(pr_in2);
        
        
        Par.im = (long*) mxMalloc((n_im+1)*sizeof(long));
        
        if (Par.im == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");
        
        
        Par.im[0] = n_im;
        
        for (i=0; i<n_im; i++)
            
            Par.im[i+1] = (long) pr_in[i];
    }
    
    
    
    
    pr_in2 = mxGetField(prhs[1], 0, "ie");
    
    if ( pr_in2 == NULL || mxIsEmpty(pr_in2) )
    {
        n_ie = 1;
        
        
        Par.ie = (long*) mxMalloc(2*sizeof(long));
        
        if (Par.ie == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");
        
        
        Par.ie[0] = n_ie;
        
        Par.ie[1] = Par.Ni;
    }
    
    else
    {
        pr_in = mxGetPr(pr_in2);
        
        n_ie = (long) mxGetNumberOfElements(pr_in2);
        
        
        Par.ie = (long*) mxMalloc((n_ie+1)*sizeof(long));
        
        if (Par.ie == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");
        
        
        Par.ie[0] = n_ie;
        
        for (i=0; i<n_ie; i++)
            
            Par.ie[i+1] = (long) pr_in[i];
    }
    
    
    
    
    Par.D = (int) D;
    
    
    
    
    if ( nrhs < 4 || mxIsEmpty(prhs[3]) )
    {
    
        Par.Ns = (long*) mxMalloc(D*sizeof(long));

        if (Par.Ns == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");

        if (D == 1 && Ns[0] == 1)

            Par.Ns[0] = (Ns[0] != 1) * Ns[0] + (Ns[1] != 1) * Ns[1];

        else

            for (i=0; i<D; i++)

                Par.Ns[i] = (long) Ns[i];
    }
    
    else
    {   
        Par.Ns = (long*) mxMalloc(1*sizeof(long));

        if (Par.Ns == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                              "Out of memory!");
        
        Par.Ns[0] = N;
    }
        
    
    
    

    /* Making sure that the 3rd input argument is a real array with the right number
     * of elements */
    
    if ( nrhs > 2 && !mxIsEmpty(prhs[2]) )
    {
    
        n = (long) mxGetNumberOfElements(prhs[2]);
        

        Ub = (double*) mxMalloc(N*sizeof(double));

        if (Ub == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal",\
                                          "Out of memory!");
        
        
        if ( !mxIsNumeric(prhs[2]) || n != N )
            
            mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The third input argument "\
                              "must be either empty or a numeric array with the "\
                              "same number of elements as the input signal!");
        
        else
        {

            pr_in = mxGetPr(prhs[2]);

            for (i=0; i<N; i++)

                Ub[i] = pr_in[i];
        }

            
    }
    
    
    

    /* Making sure that the 4th input argument is a real array with the right
     * dimensions */
    
    if ( nrhs > 3 && !mxIsEmpty(prhs[3]) )
    {
        
        sz_t = mxGetDimensions (prhs[3]);

        if ( !mxIsNumeric(prhs[3]) || mxGetNumberOfDimensions(prhs[3]) != 2 || \
            sz_t[0] != N || sz_t[1] != D )

            mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "The 4th input argument "\
                              "must be either empty or a 2D array with the number "\
                              "of columns equal to the dimension of the input "\
                              "signal and the number of rows equal to the number "\
                              "of signal sample points!");
        
        else
            
            pr_t = mxGetPr(prhs[3]);
        
    }
    
    
    
/***********************************************************************************/
/******************** MEMORY ALLOCATION FOR OUTPUT ARGUMENTS  **********************/
/***********************************************************************************/

    
    /* Allocating memory for the first two output arguments */
    
    if ( nrhs < 4 || mxIsEmpty(prhs[3]) )
    {
        Nm = (mwSize*) mxMalloc((D+1)*sizeof(mwSize));

        if (Nm == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal","Out of memory!");


        for (i=0; i<D; i++)

            Nm[i] = Ns[i];

        Nm[i] = n_im;

        plhs[0] = mxCreateNumericArray(D+1, Nm, mxDOUBLE_CLASS, mxREAL);
    }
    
    else
    {
        Nm = (mwSize*) mxMalloc(2*sizeof(mwSize));

        if (Nm == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal","Out of memory!");
        
        
        Nm[0] = N;
        
        Nm[1] = n_im;
        
        plhs[0] = mxCreateNumericArray(2, Nm, mxDOUBLE_CLASS, mxREAL);
    }
              
    
    if (nlhs > 1)
        
        plhs[1] = mxCreateDoubleMatrix(n_ie, 1, mxREAL);
    
    
    
    /* Getting pointers to the first two output arguments */

    pr_m = mxGetPr(plhs[0]);
    
    if (nlhs > 1)
        
        pr_e = mxGetPr(plhs[1]);
    
    else
    {
        e = (double*) mxMalloc(n_ie*sizeof(double));
        
        if (e == NULL) mexErrMsgIdAndTxt("AP_Demodulation:InpVal","Out of memory!");
        
        
        pr_e = e;
    }
    
    
    
/***********************************************************************************/
/********************************* CALCULATION *************************************/
/***********************************************************************************/


    /* Calling the computational routine */

    exitflag = f_ap_demodulation (pr_s, &Par, Ub, pr_t, pr_m, pr_e, &iter);
    
    
     
/***********************************************************************************/
/************************** DYNAMIC MEMORY DEALLOCATION ****************************/
/***********************************************************************************/
    
    
    mxFree(Par.Nr);
    
    mxFree(Par.im);
    
    mxFree(Par.ie);
    
    mxFree(Par.Ns);
    
    mxFree(Ub);
    
    mxFree(Nm);
    
    mxFree(e);
    
    
    
/***********************************************************************************/
/************************ READOUT OF THE COMPUTATION TIME **************************/
/***********************************************************************************/
    
    
    /* Assigning the third output argument if requested */
    
    if (nlhs > 2)
        
        plhs[2] = mxCreateDoubleScalar(iter);
    
    
    
    
    /* Stopping measuring the running time */
    
    end = clock();
   
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    
    
    
    /* Assigning the fourth output argument if requested */
    
    if (nlhs > 3)
        
        plhs[3] = mxCreateDoubleScalar(cpu_time_used);
    
    
    
    if (exitflag != 0) mexErrMsgIdAndTxt("AP_Demodulation:InpVal", "");
    
}

