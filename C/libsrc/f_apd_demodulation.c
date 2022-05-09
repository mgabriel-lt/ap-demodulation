
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
 * See https://github.com/mgabriel-lt/ap-demodulation for the latest version of the
 * code and user-friendly explanations on the working principle, domains of
 * application, and advice on the usage of different AP Demodulation algorithms in
 * practice.
 */


/* C O N T E N T S
 * 
 * A function that performs amplitude demodulation by using a selected alternating
 * projection algorithm:
 *
 * (1) f_apd_demodulation.
 */


#ifndef APD_SOURCE

#define APD_SOURCE



#include "h_apd.h"


#include "l_apd_error_handling.c"

#include "l_apd_auxiliary.c"

#include "l_apd_algorithms.c"



int f_apd_demodulation ( double* s, \

                         struct strAPD_Par* Par, \
                        
                         double* Ub, \

                         double* t, \
                        
                         double* out_m, \
                        
                         double* out_e, \
                        
                         long* iter )
{
/* P U R P O S E
 *
 * Performs demodulation of the input signal by using the AP Demodulation approach
 * formulated and developed in
 *
 *   M. Gabrielaitis. "Fast and Accurate Amplitude Demodulation of Wideband Signals,"
 *   IEEE Transactions on Signal Processing, vol. 69, pp. 4039-4054, 2021. DOI:
 *   10.1109/TSP.2021.3087899.
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
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_apd_compression, (2) f_apd_interpolation, (3) f_apd_s_Ub_init,
 * 
 * (4) f_apd_mkl_dft_init, (5) f_apd_input_validation, (6) DftiFreeDescriptor,
 * 
 * (7) f_apd_basic, (8) f_apd_accelerated, (9) f_apd_projected.
 */
    

    
/***********************************************************************************/
/************************** DEFINITIONS & INITIALIZATIONS **************************/
/***********************************************************************************/
    
    
    int exitflag = 0;

    f_apd_set_error (exitflag, __LINE__, APD_ERR_FILE);
    
    
    
    /* Basic variables */
    
    long i;
    
    long nx = 1;
    
    long nx_2;
    
    long *ix_map = NULL;
    
    
    DFTI_DESCRIPTOR_HANDLE dft_handle = 0;
    
    
    double *s_local = NULL;
    
    double *Ub_local = NULL;
    
    double *s_local2 = NULL;
    
    double *Ub_local2 = NULL;
    
    double *s_local3 = NULL;
    
    double *Ub_local3 = NULL;
    
    double *pr_s = s;
    
    double *pr_Ub = Ub;
    


    /* Validation of the input data */

    exitflag = f_apd_input_validation (s, Par, Ub, t);
    
    if (exitflag != APD_ERR_ID_NON) goto finish;
    
    
    
    /* Dimensions of the actual signal to be demodulated */
    
    if (t != NULL)
        
        Par->Nx = Par->Nr;
    
    else
        
        Par->Nx = Par->Ns;

    
    
    /* Numbers of sample points */
    
    if (t != NULL)
    {
        Par->ns = Par->Ns[0]; /* original signal */

        for (i=0; i<(Par->D); i++)
            
            nx = nx * Par->Nx[i]; /* actual signal */
    }
    
    else
    {
        Par->ns = 1;
        
        for (i=0; i<(Par->D); i++)
        {
            Par->ns = Par->ns * Par->Ns[i]; /* original signal */

            nx = nx * Par->Nx[i]; /* actual signal */
        }
    }
    
    nx_2 = (nx / Par->Nx[Par->D-1]) * (Par->Nx[Par->D-1]+2-(Par->Nx[Par->D-1]%2));
    
    
    
/***********************************************************************************/
/********************************* CALCULATION *************************************/
/***********************************************************************************/

    
    /* Compression */
    
    if (Par->Cp > 1)
    {
       
        s_local = (double*) malloc(Par->ns*sizeof(double));

        if (s_local==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}

        memcpy(s_local, pr_s, Par->ns*sizeof(double));
        
        
        f_apd_compression (s_local, Par->ns, 1/(Par->Cp));


        if (Ub != NULL)
        {
            Ub_local = (double*) malloc(Par->ns*sizeof(double));

            if (Ub_local==NULL)
            {
                f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}

            memcpy(Ub_local, pr_Ub, Par->ns*sizeof(double));
            
            
            f_apd_compression (Ub_local, Par->ns, 1/(Par->Cp));
        }

        else

            Ub_local = NULL;
        
        
        pr_s = s_local;
    
        pr_Ub = Ub_local;
    }
    
    
    
    /* Interpolation */
    
    ix_map = (long*) malloc(Par->ns*sizeof(long));
        
    if (ix_map==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}

    
    if (t != NULL)
    {
        s_local2 = (double*) malloc(nx*sizeof(double));
        
        if (s_local2==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        if (Ub != NULL)
        {
            Ub_local2 = (double*) malloc(nx*sizeof(double));

            if (Ub_local2==NULL)
            {
                f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
        }
        
        else

            Ub_local2 = NULL;
       

        exitflag = f_apd_interpolation (pr_s, Par, pr_Ub, t, s_local2, Ub_local2, \
                ix_map);
        
        if (exitflag != APD_ERR_ID_NON) goto finish;

        
        pr_s = s_local2;
        
        pr_Ub = Ub_local2;
        
    }
    
    else
    {
        for (i=0; i<(Par->ns); i++)
            
            ix_map[i] = i;
    }

    
    
    /* Local copies of the signal and upper bound arrays with the element placement
     * replaced from the standard to the Intel MKL DFT's. The last dimension of the
     * arrays is complemented by one or two elements as required by the MKL DFT
     * routine. The mapping between the two conventions is provided as well. */
       
    s_local3 = (double*) malloc(nx_2*sizeof(double));
        
    if (s_local3==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    if (Ub != NULL)
    {
        Ub_local3 = (double*) malloc(nx_2*sizeof(double));

        if (Ub_local3==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    }
    else
        
        Ub_local3 = NULL;


    exitflag = f_apd_s_Ub_init (pr_s, pr_Ub, ix_map, Par->D, Par->Nx, Par->ns, \
            s_local3, Ub_local3);
    
    if (exitflag != APD_ERR_ID_NON) goto finish;

    
    pr_s = s_local3;
    
    pr_Ub = Ub_local3;
    


    /* Intel MKL DFT's descriptor */

    exitflag = f_apd_mkl_dft_init (Par->D, Par->Nx, &dft_handle);
    
    if (exitflag != APD_ERR_ID_NON) goto finish;

    
    
    /* Demodulation */

    if (Par->Al == 'B')
    
        exitflag = f_apd_basic (pr_s, Par, pr_Ub, ix_map, &dft_handle, out_m, out_e, \
                iter);
    
    else if (Par->Al == 'A')
        
        exitflag = f_apd_accelerated (pr_s, Par, pr_Ub, ix_map, &dft_handle, out_m, \
                out_e, iter);
    
    else if (Par->Al == 'P')
        
        exitflag = f_apd_projected (pr_s, Par, pr_Ub, ix_map, &dft_handle, out_m, \
                out_e, iter);
    
    if (exitflag != APD_ERR_ID_NON) goto finish;

    

    /* Decompression */
    
    if (Par->Cp > 1)
    
        f_apd_compression (out_m, Par->ns*(Par->im[0]), Par->Cp);
    
    
    
/***********************************************************************************/
/************************** OUTPUT & MEMORY DEALLOCATION ***************************/
/***********************************************************************************/
    
    
    finish:
        
        free(s_local);
    
        free(Ub_local);

        free(ix_map);

        free(s_local2);

        free(Ub_local2);

        free(s_local3);

        free(Ub_local3);

        DftiFreeDescriptor (&dft_handle);
        
        return exitflag;

    failed:

        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;
        
}


#endif

