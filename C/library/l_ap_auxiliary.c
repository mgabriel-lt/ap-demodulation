
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
 * Eight auxiliary functions for amplitude demodulation via alternating projections:
 *
 * (1) f_ap_print_error,
 *
 * (2) f_ap_minmax,
 *
 * (3) f_ap_abs_scaled_max_abs,
 *
 * (4) f_ap_compression,
 *
 * (5) f_ap_interpolation,
 *
 * (6) f_ap_s_Ub_init,
 *
 * (7) f_ap_mkl_dft_init,
 *
 * (8) f_ap_mkl_dft_PMw.
 */



void f_ap_print_error (const char* function, const int line, const char* message)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Prints error message to stderr or MATLAB command window.
 */

/* I N P U T   A R G U M E N T S
 *
 * [function] - name of the function where the error is detected.
 *
 * [line] - number of line in the code of the dunction where error is detected.
 *
 * [message] - error message.
 */

/* O U T P U T   A R G U M E N T S
 *
 * None.
 */
    
    
    #ifdef AP_MEX_COMPILATION
        
        mexPrintf (AP_STR_NL AP_ERR_MSG_PRE " (line %d in %s): " AP_STR_NL2 \
              "%s" AP_STR_NL2, line, function, message);

    #else

        fprintf (stderr, AP_STR_NL AP_ERR_MSG_PRE " (line %d in %s): " AP_STR_NL2 \
              "%s" AP_STR_NL2, line, function, message);

    #endif

}




void f_ap_minmax (const double* in, const long N, double* out_min, double* out_max)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Finds the minimum and maximum values of the input array.
 */

/* I N P U T   A R G U M E N T S
 *
 * [in] - input array.
 *
 * [N] - number of elements in the array of the input signal.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [out_min#] - the minimum value of the input array (this is the address of an
 *              externally defined scalar variable.
 *
 * [out_max#] - the maximum value of the input array (this is the address of an
 *              externally defined scalar variable.
 */
    
    
    /* Initializations and declarations */
    
    long i;
    

    out_min[0] = in[0];
    
    out_max[0] = in[0];

    
    
    /* Calculation */
    
    for (i=1; i<N; i++)
    {

        if (out_min[0] > in[i])

            out_min[0] = in[i];
        
        else if (out_max[0] < in[i])
            
            out_max[0] = in[i];
    }

}




double f_ap_abs_scaled_max_abs (const double* in, const long N, double* out)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Calculates the absolute value of the input signal normzlized by its maximum.
 */

/* I N P U T   A R G U M E N T S
 *
 * [in] - input signal.
 *
 * [N] - number of elements in the array of the input signal.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [out#] - normalized absolute-value signal (memory allocated externally);
 *
 * [maxval] - absolute maximum value of the input signal.
 */
    
    
    /* Initializations and declarations */
    
    long i;
    
    double maxval = 0;
    
    
    
    /* The maximum absolute value of the signal */
    
    for (i=0; i<N; i++)
    {
        out[i] = fabs(in[i]);
        
        if (out[i] > maxval)
            
            maxval = out[i];
    }
    
    
    
    /* Scaled signal */
    
    for (i=0; i<N; i++)
        
        out[i] = out[i] / maxval;
    
    
    
    return (maxval);
 
}




void f_ap_compression (double* s, const long N, const double p)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Compresses the signal in-place by using the power function model.
 */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal.
 *
 * [N] - number of elements in the array of the input signal.
 *
 * [p] - compression parameter.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [s#] - compressed signal.
 */
    
    
    /* Initializations and declarations */
    
    long i;
    
    
    
    /* Calculation */
    
    for (i=0; i<N; i++)
        
        s[i] = ((s[i]>0)-(s[i]<0)) * pow(fabs(s[i]),p);
    
}




int f_ap_interpolation (const double* s, const struct s_ParamsAP* Par, \
        const double* Ub, const double* t, double* s_out, double* Ub_out, \
        long* ix_out)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Interpolates the input signal on a refined uniform grid following Eq. 23 in
 * M. Gabrielaitis IEEE Trans. Signal Process., vol. 69, pp. 4039-4054, 2021.
 */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal.
 *
 * [Par] - pointer to the structure with interpolation parameters:
 *
 *         .D - number of the input signal dimensions.
 *
 *         .ns - number of sample points of the original input signal.
 *
 *         .Nr - number of sample points on the refined uniform grid. This is an
 *               array of D elements.
 *
 * [Ub] - array with the upper bound values on the modulator. This is an array of the
 *        same size as the input signal s or Ub == NULL. In the latter case, no upper
 *        bound is assumed.
 *
 * [t] - sampling coordinates of the input signal. This is a 2D array with the number
 *       of columns equal to the dimension of the input signal (Par.D) and the number
 *       of rows equal to the total number of signal sample points, Par.Ns[0].
 */

/* O U T P U T   A R G U M E N T S
 *
 * [exitflag] - exit flag: exitflag=0 - no error, exitflag=-1 - out of memory error.
 *
 * [s_out#] - interpolated signal. The number of dimensions of this array is given by
 *            Par.D.
 *
 * [Ub_out#] - interpolated upper bounds on the modulator. The dimensions of this
 *             array are the same as of s_out.
 *
 * [ix_out#] - linear indexes of the elements of s_out corresponding to every'
 *             element of s.
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_print_error, (2) f_ap_minmax.
 */
    
    
    /* Initializations and declarations */
    
    int line;
    
    int exitflag = 0;
    
    
    int log_aux;
    
    long i1, i2, i3;
    
    long nr = 1;
    
    long *cumnr = NULL;
    
    long ix;
    
    long *ix_aux = NULL;
    
    
    double *tmin = NULL;
    
    double *tmax = NULL;
    
    double *dt = NULL;
    
    double r2;
    
    double *r2_all = NULL;
    
 
    
    
    /* Memory allocation for some of the internal arrays */
    
    cumnr = (long*) malloc((Par->D)*sizeof(long));

    if (cumnr == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    ix_aux = (long*) malloc((Par->D)*sizeof(long));
        
    if (ix_aux == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    tmin = (double*) malloc((Par->D)*sizeof(double));
    
    if (tmin == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    tmax = (double*) malloc((Par->D)*sizeof(double));
    
    if (tmax == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    dt = (double*) malloc((Par->D)*sizeof(double));
        
    if (dt == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    
    /* Number of sample points in the original and interpolated arrays */
    
    for (i1=0; i1<(Par->D); i1++)
    {
       nr = nr * Par->Nr[i1];
        
        if (i1==0)
            
            cumnr[i1] = Par->Nr[i1];
        
        else
            
            cumnr[i1] = Par->Nr[i1] * cumnr[i1-1];
    }
    
    
    
    /* Memory allocation for r2_all arrays */
    
    r2_all = (double*) malloc((Par->ns)*sizeof(double));
        
    if (r2_all == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    
    /* Preparation of the signal and upper bound output arrays */
    
        for (i1=0; i1<nr; i1++)
        {
            s_out[i1] = 0;
    
            if (Ub != NULL)
            
                Ub_out[i1] = INFINITY;
        }
    
    
    
    /* Step sizes of the time coordinate grid of the interpolated signal */
    
    for (i1=0; i1<(Par->D); i1++)
    {
        f_ap_minmax (t+(i1*(Par->ns)), (Par->ns), tmin+i1, tmax+i1);
        
        dt[i1] = (tmax[i1] - tmin[i1]) / (Par->Nr[i1] - 1);
    }



    /* Interpolation */
    
    for (i1=0; i1<(Par->ns); i1++)
    {
        /* The closest point on the new grid and the corresponding distance */
        
        r2 = 0;
        
        for (i2=0; i2<(Par->D); i2++)
        {
            i3 = i1 + i2*(Par->ns);

            ix_aux[i2] = lround((t[i3]-tmin[i2]) / dt[i2]);
            
            r2 = r2 + (t[i3]-tmin[i2]-ix_aux[i2]*dt[i2]) * \
                      (t[i3]-tmin[i2]-ix_aux[i2]*dt[i2]);
        }
        
        ix = ix_aux[0];
 
        for (i2=1; i2<(Par->D); i2++)

            ix = ix + ix_aux[i2] * cumnr[i2-1];

        
        /* Checking whether the closest point on the new grid has already been used*/
        
        log_aux = 0;
        
        for (i2=0; i2<i1; i2++)
        {
            if(ix_out[i2] == ix)
            {
                log_aux = 1;
                
                break;
            }
        }
        
        
        if (log_aux == 0 || r2 < r2_all[i2])
        {
            s_out[ix] = s[i1];
       
            if (Ub != NULL)
                
                Ub_out[ix] = Ub[i1];
            
            ix_out[i1] = ix;
            
            r2_all[i1] = r2;
        }
        
        if (log_aux == 1 && r2 < r2_all[i2])
        
            ix_out[i2] = -ix_out[i2];
        
        else if (log_aux == 1)
            
            ix_out[i1] = -ix_out[i1];
          
    }
    

    
    /* Absolute value of ix_out */
    
    for (i1=0; i1<(Par->ns); i1++)
        
        ix_out[i1] = labs(ix_out[i1]);
    
    
    
    /* Output & Memory deallocation */
    
    goto finish;
    
    finish:
    
        free(cumnr);
    
        free(ix_aux);

        free(tmin);

        free(tmax);

        free(dt);

        free(r2_all);

        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        goto finish;

}




int f_ap_s_Ub_init (const double* s, const double* Ub, long* ix, const int D, \
        const long* N, const long ns, double* out_s, double* out_Ub)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Remaps elements of the signal, modulator upper bound, and interpolation index
 * arrays to comply with the Intel's MKL DFT indexing convention. */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal.
 *
 * [Ub] - upper bound on the modulator. This array must have the same number of
 *        elements as the input signal.
 *
 * [ix] - array with the mapping between the indexes of the original and (possibly)
 *        interpolated signal arrays. This array contains ns number of elements.
 *
 * [D] - number of dimensions of the signal array.
 *
 * [N] - numbers of elements of the signal array in every dimension.
 *
 * [ns] - number of elements in the array ix.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [exitflag] - exit flag: exitflag=0 - no error, exitflag=-1 - out of memory error.
 *
 *              Upon an error, all memory dynamically allocated in this function is
 *              freed. The corresponding error message is sent to sderr.
 *
 * [out_s#] - output signal with rearranged placement and +2 elements in the last
 *            dimension to meet the MKL DFT requirements.
 *
 * [out_Ub#] - output upper bound on the modulator with rearranged placement and +2
 *             elements in the last dimension to meet the MKL DFT requirements.
 *
 * [ix#] - array with the mapping between the indexes of the original and (possibly)
 *         interpolated signal arrays compliant with the Intel's MKL DFT indexing
 *         convention.
 */
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_print_error.
 */
    
    
    /* Declarations and initializations */
    
    int line;
    
    int exitflag = 0;
    
    
    long i0, i1, i2;
    
    long n = 1, n_2;
    
    long i_lin0, i_lin;
    
    long strd0[3], strd[3];
    
    long *ilin0_to_ilin = NULL;
    
    
    
    /* Number of elements in the s and Ub arrays */
    
    for (i0=0; i0<D; i0++)
        
        n = n * N[i0];
        
    n_2 = (n / N[D-1]) * (N[D-1]+2-(N[D-1]%2));
    
    
    
    /* Initialization of some of the arrays */
    
    for (i0=0; i0<n_2; i0++)
    {
        out_s[i0] = 0;

        if (Ub != NULL)

            out_Ub[i0] = INFINITY;
    }
    
    
    if (D > 1)
    {
        ilin0_to_ilin = (long*) malloc(n*sizeof(long));
        
        if (ilin0_to_ilin == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    }
    
    
    
    /* Remapping of the s and Ub arrays */
    
    if (D == 1)
    {
       for (i0=0; i0<N[0]; i0++)
       {
           out_s[i0] = s[i0];
       
           if (Ub != NULL)
               
               out_Ub[i0] = Ub[i0];
       }
    }
    
    else if (D == 2)
    {
        strd0[0] = 1;
        
        strd0[1] = N[0];
        
        
        strd[0] = (N[1]/2+1)*2;
        
        strd[1] = 1;
        
    
        for (i0=0; i0<N[0]; i0++)
            
            for (i1=0; i1<N[1]; i1++)
            {
                i_lin0 = i0*strd0[0] + i1*strd0[1];
                
                i_lin = i0*strd[0] + i1*strd[1];
                
                
                ilin0_to_ilin[i_lin0] = i_lin;
                
                out_s[i_lin] = s[i_lin0];
                
                if (Ub != NULL)
                    
                    out_Ub[i_lin] = Ub[i_lin0];
            }
    }
    
    else if (D == 3)
    {
        strd0[0] = 1;
        
        strd0[1] = N[0];
        
        strd0[2] = N[0]*N[1];
        
        
        strd[0] = N[1]*(N[2]/2+1)*2;
        
        strd[1] = (N[2]/2+1)*2;
        
        strd[2] = 1;
        
    
        for (i0=0; i0<N[0]; i0++)
            
            for (i1=0; i1<N[1]; i1++)
            
                for (i2=0; i2<N[2]; i2++)
                {
                    i_lin0 = i0*strd0[0] + i1*strd0[1] + i2*strd0[2];

                    i_lin = i0*strd[0] + i1*strd[1] + i2*strd[2];

                    
                    ilin0_to_ilin[i_lin0] = i_lin;
                    
                    out_s[i_lin] = s[i_lin0];

                    if (Ub != NULL)
                        
                        out_Ub[i_lin] = Ub[i_lin0];
                }
        
    }
    
    
    
    /* Remapping of the ix array */
    
    if (D > 1)
    
        for (i0=0; i0<ns; i0++)

            ix[i0] = ilin0_to_ilin[ix[i0]];
    
    
    
    /* Output & Memory deallocation */
    
    goto finish;
    
    finish:
    
        free(ilin0_to_ilin);

        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        goto finish;
    
}




int f_ap_mkl_dft_init (const int D, const long* N, \
        DFTI_DESCRIPTOR_HANDLE* dft_handle)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Initializes Intel's Mkl DFT routine for AP algorithms. */

/* I N P U T   A R G U M E N T S
 *
 * [D] - number of DFT dimensions.
 *
 * [N] - numbers of elements of the DFT array in every dimension.
 *
 * [dft_handle] - address of an empty variable for the comitted descriptor handle.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [exitflag] - exit flag: exitflag=0 - no error, exitflag=-1 - out of memory error,
 *                         exitflag=-2 - error with allocating the MKL DFT's
 *                         descriptor.
 *
 *              Upon an error, all memory dynamically allocated in this function or
 *              functions called by this function is freed. The corresponding error
 *              message is sent to sderr.
 *
 * [dft_handle#] - pointer to the initialized and comitted descriptor handle.
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) DftiCreateDescriptor, (2) DftiSetValue, (3) DftiCommitDescriptor.
 */
    
    
    /* Declarations and initializations */
    
    int line;
    
    int exitflag = 0;
    
    
    int i;
    
    long n = 1;
    
    MKL_LONG status;
    
    MKL_LONG *N_ = NULL;
    
    MKL_LONG *rs = NULL;

    MKL_LONG *cs = NULL;
    
    
    
    /* DFT descriptor */
    
    if (D == 1)
    {
        status = DftiCreateDescriptor ( dft_handle, DFTI_DOUBLE, DFTI_REAL, \
                (MKL_LONG) D, (MKL_LONG) N[0]);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-3; exitflag=-2; goto failed;}
        
        n = N[0];
    }
    else
    {
        N_ = (MKL_LONG*) malloc(D*sizeof(MKL_LONG));
        
        if (N_ == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
        
        
        for (i=0; i < D; i++)
        {
            N_[i] = (MKL_LONG) N[i];
            
            n = n * N[i];
        }
        
        
        status = DftiCreateDescriptor ( dft_handle, DFTI_DOUBLE, DFTI_REAL, \
                (MKL_LONG) D, N_);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-3; exitflag=-2; goto failed;}
    }



    /* DFT strides in real and conjugate domains */

    rs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));

    if (rs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}


    cs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));

    if (cs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}


    cs[D] = 1;

    rs[D] = 1;

    cs[D-1] = (N[D-1]/2+1);

    rs[D-1] = cs[D-1]*2;

    for (i=D-2; i>0; i--)
    {
        cs[i] = cs[i+1] * N[i];

        rs[i] = rs[i+1] * N[i];
    }

    cs[0] = 0;

    rs[0] = 0;


    
    /* Setting strides of the DFT descriptor for the forward transform */

    status = DftiSetValue(*dft_handle, DFTI_INPUT_STRIDES, rs);

    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}


    status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, cs);

    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Setting the calculations to be done in-place */
    
    status = DftiSetValue (*dft_handle, DFTI_PLACEMENT, DFTI_INPLACE);
    
    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Setting the data storage scheme in the Fourier domain */
    
    status = DftiSetValue (*dft_handle, DFTI_CONJUGATE_EVEN_STORAGE, \
            DFTI_COMPLEX_COMPLEX);
    
    if (status != DFTI_NO_ERROR) {line=__LINE__-3; exitflag=-2; goto failed;}
    
    
    status = DftiSetValue (*dft_handle, DFTI_PACKED_FORMAT, DFTI_CCE_FORMAT);
    
    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Setting the backward transform as the inverse transform */
    
    status = DftiSetValue (*dft_handle, DFTI_BACKWARD_SCALE, 1.0/((double)n));
    
    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Setting all computations to use one CPU thread */
    
    status = DftiSetValue (*dft_handle, DFTI_THREAD_LIMIT, 1);
    
    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Commitof the DFT descriptor */

    status = DftiCommitDescriptor (*dft_handle);

    if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

    
    
    /* Output & Memory deallocation */
    
    goto finish;
    
    finish:
    
        free(N_);

        free(rs);

        free(cs);
        
        return exitflag;
                
    failed:
        
        DftiFreeDescriptor(dft_handle);
        
        *dft_handle = 0;
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        else if (exitflag == -2)
        
            f_ap_print_error (__FUNCTION__, line, DftiErrorMessage(status));
        
        goto finish;
        
}




int f_ap_mkl_dft_PMw (double* s, const int D, const long* N, const long* iL, \
        const long* iR, DFTI_DESCRIPTOR_HANDLE* dft_handle)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Implements the projection onto the set Mw by using the Intel's Mkl DFT routine. */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal + 2 additional array elements along the last dimension.
 *
 * [D] - number of dimensions of the signal array.
 *
 * [N] - numbers of elements of the signal array in every dimension. The additional
 *        two elements in the last dimension of s are not counted here.
 *
 * [iL] - indexes of the left cutoff frequencies.
 *
 * [iR] - indexes of the right cutoff frequencies.
 *
 * [dft_handle] - address of an empty variable for the comitted descriptor handle.
 */

/* O U T P U T   A R G U M E N T S
 *
 * [exitflag] - exit flag: exitflag=0 - no error, exitflag=-1 - out of memory error,
 *                         exitflag=-2 - error with allocating the MKL DFT's
 *                         descriptor.
 *
 *              Upon an error, all memory dynamically allocated in this function or
 *              functions called by this function is freed. The corresponding error
 *              message is sent to sderr.
 *
 * [s#] - pointer to the updated and comitted descriptor handle.
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_print_error, (2) DftiSetValue, (3) DftiCommitDescriptor,
 * 
 * (4) DftiComputeForward, (5) DftiComputeBackward.
 */
    
    
    /* Declarations and initializations */
    
    int line = 0;
    
    int exitflag = 0;
    
    
    long i1, i2, i3;
    
    MKL_LONG status;
    
    MKL_LONG *rs = NULL;
    
    MKL_LONG *cs = NULL;
    
    
    
    /* Calculation */
    
    if (D == 1)
    {
       /* Forward FFT */
        
        status = DftiComputeForward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        
        
        /* Projection onto Mw in the Fourier domain */
        
        for (i1 = 2*iL[0]; i1 < N[0]+2; i1++)
            
            s[i1] = 0;
        
        
        
        /* Backward FFT */
        
        status = DftiComputeBackward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
    }
    
    else
    {
        /* Calculation of strides in real and conjugate domains */
        
        rs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));
    
        if (rs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
        
        cs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));
    
        if (cs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
        
    
        cs[D] = 1;

        rs[D] = 1;

        cs[D-1] = (N[D-1]/2+1);

        rs[D-1] = cs[D-1]*2;

        for (i1=D-2; i1>0; i1--)
        {
            cs[i1] = cs[i1+1] * N[i1];

            rs[i1] = rs[i1+1] * N[i1];
        }
        
        cs[0] = 0;
        
        rs[0] = 0;
        
        

        /* Update of strides of the FFT descriptor for the forward transform */
        
        status = DftiSetValue(*dft_handle, DFTI_INPUT_STRIDES, rs);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        
        status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, cs);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        

        /* Commit of the descriptor */
    
        status = DftiCommitDescriptor (*dft_handle);

        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

        
        
        /* Forward FFT */
        
        status = DftiComputeForward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}

        
        
        /* Projection onto Mw in the Fourier domain */
        
        if (D == 2)
        {
            for (i2 = 2*iL[1]; i2 < N[1]+2-(N[0]%2); i2++)
                
                for (i1 = 0; i1 < N[0]; i1++)
                    
                    s[rs[1]*i1+rs[2]*i2] = 0;
            
            for (i2 = 0; i2 < 2*iL[1]; i2++)
                
                for (i1 = iL[0]; i1 <= iR[0]; i1++)
                    
                    s[rs[1]*i1+rs[2]*i2] = 0;
        }
        
        else if (D == 3)
        {
            for (i3 = 2*iL[2]; i3 < N[2]+2-(N[0]%2); i3++)
                
                for (i2 = 0; i2 < N[1]; i2++)
                
                    for (i1 = 0; i1 < N[0]; i1++)

                        s[rs[1]*i1+rs[2]*i2+rs[3]*i3] = 0;
            
            for (i3 = 0; i3 < 2*iL[2]; i3++)
                
                for (i2 = iL[1]; i2 <= iR[1]; i2++)
                
                    for (i1 = 0; i1 < N[0]; i1++)

                        s[rs[1]*i1+rs[2]*i2+rs[3]*i3] = 0;
            
            for (i3 = 0; i3 < 2*iL[2]; i3++)
                
                for (i1 = iL[0]; i1 <= iR[0]; i1++)
                {
                    for (i2 = 0; i2 < iL[1]; i2++)

                        s[rs[1]*i1+rs[2]*i2+rs[3]*i3] = 0;
                    
                    for (i2 = iR[1]+1; i2 < N[1]; i2++)

                        s[rs[1]*i1+rs[2]*i2+rs[3]*i3] = 0;
                }
        }
            
        else
        {
            exitflag = 1;
        
            goto failed;
        }

        
        
        /* Update of strides of the FFT descriptor for the backward transform */
        
        status = DftiSetValue(*dft_handle, DFTI_INPUT_STRIDES, cs);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        
        status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, rs);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        
        
        /* Commit of the descriptor */
    
        status = DftiCommitDescriptor (*dft_handle);

        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
        
        
        
        /* Backward FFT */
        
        status = DftiComputeBackward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR) {line=__LINE__-2; exitflag=-2; goto failed;}
    }
    
    
    
    /* Output & Memory deallocation */
    
    goto finish;
    
    finish:
        
        free(rs);
    
        free(cs);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        else if (exitflag == 1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_D);
        
        else if (exitflag == -2)
        
            f_ap_print_error (__FUNCTION__, line, DftiErrorMessage(status));
        
        goto finish;
 
}

