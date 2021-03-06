
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
 * Seven auxiliary functions for amplitude demodulation via alternating projections:
 *
 * (1) f_apd_minmax,
 *
 * (2) f_apd_abs_scaled_max_abs,
 *
 * (3) f_apd_compression,
 *
 * (4) f_apd_interpolation,
 *
 * (5) f_apd_s_Ub_init,
 *
 * (6) f_apd_mkl_dft_init,
 *
 * (7) f_apd_mkl_dft_PMw.
 */



#include "h_apd.h"



void f_apd_minmax ( const double* in, \

                    const long N, \
                   
                    double* out_min, \
                   
                    double* out_max )
{
/* P U R P O S E
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
 * [out_min] - the minimum value of the input array (this is the address of an
 *             externally defined scalar variable).
 *
 * [out_max] - the maximum value of the input array (this is the address of an
 *             externally defined scalar variable).
 */

/* R E T U R N   V A L U E
 *
 * None.
 */
    
    
    /* Definitions and initializations */
    
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




double f_apd_abs_scaled_max_abs ( const double* in, \

                                  const long N, \

                                  double* out)
{
/* P U R P O S E
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
 * [out] - normalized absolute-value signal (memory allocated externally);
 */

/* R E T U R N   V A L U E
 *
 * [maxval] - absolute maximum value of the input signal.
 */
    
    
    /* Definitions and initializations */
    
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




void f_apd_compression ( double* s, \

                         const long N, \

                         const double p )
{
/* P U R P O S E
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
 * [s] - compressed signal (memory allocated externally).
 */

/* R E T U R N   V A L U E
 *
 * None.
 */
    
    
    /* Definitions and initializations */
    
    long i;
    
    
    
    /* Calculation */
    
    for (i=0; i<N; i++)
        
        s[i] = ((s[i]>0)-(s[i]<0)) * pow(fabs(s[i]),p);
    
}




int f_apd_interpolation ( const double* s, \

                          const struct strAPD_Par* Par, \

                          const double* Ub, \
                         
                          const double* t, \
                         
                          double* s_out, \
                         
                          double* Ub_out, \
        
                          long* ix_out )
{
/* P U R P O S E
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
 *         .D - number of the input signal dimensions. {Type: int}
 *
 *         .ns - number of sample points of the original input signal. {Type: long}
 *
 *         .Nr - number of sample points on the refined uniform grid. This is an
 *               array of D elements. {Type: long}
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
 * [s_out] - interpolated signal (memory allocated externally). The number of
 *           dimensions of this array is given by Par.D.
 *
 * [Ub_out] - interpolated upper bounds on the modulator (memory allocated
 *            externally). The dimensions of this array are the same as of s_out.
 *
 * [ix_out] - linear indexes of the elements of s_out corresponding to every element
 *            of s (memory allocated externally).
 */

/* R E T U R N   V A L U E
 *
 * [exitflag] - exit flag. Any positive value indicates an error (for numerical and
 *              textual definitions of the exit status, see l_ap_error_handling.c).
 * 
 *              Upon an error, all memory dynamically allocated in this function is
 *              freed.
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_apd_print_error, (2) f_apd_minmax.
 */
    
    
    /* Definitions and initializations */
    
    int exitflag = 0;

    f_apd_set_error (exitflag, __LINE__, APD_ERR_FILE);
    
    
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

    if (cumnr==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    ix_aux = (long*) malloc((Par->D)*sizeof(long));
        
    if (ix_aux==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    tmin = (double*) malloc((Par->D)*sizeof(double));
    
    if (tmin==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    tmax = (double*) malloc((Par->D)*sizeof(double));
    
    if (tmax==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    dt = (double*) malloc((Par->D)*sizeof(double));
        
    if (dt==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    
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
        
    if (r2_all==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    
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
        f_apd_minmax (t+(i1*(Par->ns)), (Par->ns), tmin+i1, tmax+i1);
        
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
    
    finish:
    
        free(cumnr);
    
        free(ix_aux);

        free(tmin);

        free(tmax);

        free(dt);

        free(r2_all);

        return exitflag;

    failed:

        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;

}




int f_apd_s_Ub_init ( const double* s, \

                      const double* Ub, \
                     
                      long* ix, \
                     
                      const int D, \

                      const long* N, \
                     
                      const long ns, \
                     
                      double* out_s, \
                     
                      double* out_Ub )
{
/* P U R P O S E
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
 * [out_s] - output signal with rearranged placement and +2 elements in the last
 *           dimension to meet the MKL DFT requirements (memory allocated
 *           externally).
 *
 * [out_Ub] - output upper bound on the modulator with rearranged placement and +2
 *            elements in the last dimension to meet the MKL DFT requirements (memory
 *            allocated externally).
 *
 * [ix] - array with the mapping between the indexes of the original and (possibly)
 *        interpolated signal arrays compliant with the Intel's MKL DFT indexing
 *        convention (memory allocated externally).
 */

/* R E T U R N   V A L U E
 *
 * [exitflag] - exit flag. Any positive value indicates an error (for numerical and
 *              textual definitions of the exit status, see l_ap_error_handling.c).
 * 
 *              Upon an error, all memory dynamically allocated in this function is
 *              freed.
 */
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_apd_print_error.
 */
    
    
    /* Definitions and initializations */
    
    int exitflag = 0;

    f_apd_set_error (exitflag, __LINE__, APD_ERR_FILE);
    
    
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
        
        if (ilin0_to_ilin==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    }
    
    
    
    /* Remapping of the s and Ub arrays */
    
    if (D == 1)
    {
        memcpy(out_s, s, N[0]*sizeof(double));

        if (Ub != NULL)
               
            memcpy(out_Ub, Ub, N[0]*sizeof(double));
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
    
    finish:
    
        free(ilin0_to_ilin);

        return exitflag;

    failed:

        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;
    
}




int f_apd_mkl_dft_init ( const int D, \

                         const long* N, \

                         DFTI_DESCRIPTOR_HANDLE* dft_handle )
{
/* P U R P O S E
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
 * [dft_handle] - pointer to the initialized and comitted descriptor handle.
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
 * (1) DftiCreateDescriptor, (2) DftiSetValue, (3) DftiCommitDescriptor.
 */
    
    
    /* Definitions and initializations */
    
    int exitflag = 0;

    f_apd_set_error (exitflag, __LINE__, APD_ERR_FILE);
    
    
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
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT1, __LINE__,APD_ERR_FILE); goto failed;}
        
        n = N[0];
    }
    else
    {
        N_ = (MKL_LONG*) malloc(D*sizeof(MKL_LONG));
        
        if (N_==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        for (i=0; i < D; i++)
        {
            N_[i] = (MKL_LONG) N[i];
            
            n = n * N[i];
        }
        
        
        status = DftiCreateDescriptor ( dft_handle, DFTI_DOUBLE, DFTI_REAL, \
                (MKL_LONG) D, N_);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT1,__LINE__,APD_ERR_FILE); goto failed;}
    }



    /* DFT strides in real and conjugate domains */

    rs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));

    if (rs==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}


    cs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));

    if (cs==NULL)
    {
        f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}


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

    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}


    status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, cs);

    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}

    
    
    /* Setting the calculations to be done in-place */
    
    status = DftiSetValue (*dft_handle, DFTI_PLACEMENT, DFTI_INPLACE);
    
    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}

    
    
    /* Setting the data storage scheme in the Fourier domain */
    
    status = DftiSetValue (*dft_handle, DFTI_CONJUGATE_EVEN_STORAGE, \
            DFTI_COMPLEX_COMPLEX);
    
    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}
    
    
    status = DftiSetValue (*dft_handle, DFTI_PACKED_FORMAT, DFTI_CCE_FORMAT);
    
    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}

    
    
    /* Setting the backward transform as the inverse transform */
    
    status = DftiSetValue (*dft_handle, DFTI_BACKWARD_SCALE, 1.0/((double)n));
    
    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}

    
    
    /* Setting all computations to use one CPU thread */
    
    /*status = DftiSetValue (*dft_handle, DFTI_THREAD_LIMIT, 1);
    
    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}*/

    
    
    /* Commiting the DFT descriptor */

    status = DftiCommitDescriptor (*dft_handle);

    if (status != DFTI_NO_ERROR)
    {
        f_apd_set_error(APD_ERR_ID_FT3,__LINE__,APD_ERR_FILE); goto failed;}

    
    
    /* Output & Memory deallocation */
    
    finish:
    
        free(N_);

        free(rs);

        free(cs);
        
        return exitflag;
                
    failed:
        
        DftiFreeDescriptor(dft_handle);
        
        *dft_handle = 0;
        
        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;
        
}




int f_apd_mkl_dft_PMw ( double* s, \
                     
                        const int D, \
                     
                        const long* N, \
                     
                        const long* iL, \

                        const long* iR, \
                     
                        DFTI_DESCRIPTOR_HANDLE* dft_handle )
{
/* P U R P O S E
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
 * [s] - projected input signal (memory allocated externally).
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
 * (1) f_apd_print_error, (2) DftiSetValue, (3) DftiCommitDescriptor,
 * 
 * (4) DftiComputeForward, (5) DftiComputeBackward.
 */
    
    
    /* Definitions and initializations */
    
    int exitflag = 0;

    f_apd_set_error (exitflag, __LINE__, APD_ERR_FILE);
    
    
    long i1, i2, i3;
    
    MKL_LONG status;
    
    MKL_LONG *rs = NULL;
    
    MKL_LONG *cs = NULL;
    
    
    
    /* Calculation */
    
    if (D == 1)
    {
       /* Forward FFT */
        
        status = DftiComputeForward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT4,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        
        /* Projection onto Mw in the Fourier domain */
        
        for (i1 = 2*iL[0]; i1 < N[0]+2; i1++)
            
            s[i1] = 0;
        
        
        
        /* Backward FFT */
        
        status = DftiComputeBackward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT4,__LINE__,APD_ERR_FILE); goto failed;}
    }
    
    else
    {
        /* Calculation of strides in real and conjugate domains */
        
        rs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));
    
        if (rs==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
    
        
        cs = (MKL_LONG*) malloc((D+1)*sizeof(MKL_LONG));
    
        if (cs==NULL)
        {
            f_apd_set_error(APD_ERR_ID_MEM,__LINE__,APD_ERR_FILE); goto failed;}
        
    
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
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, cs);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}
        
        

        /* Commiting the descriptor */
    
        status = DftiCommitDescriptor (*dft_handle);

        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT3,__LINE__,APD_ERR_FILE); goto failed;}

        
        
        /* Forward FFT */
        
        status = DftiComputeForward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT4,__LINE__,APD_ERR_FILE); goto failed;}

        
        
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
            f_apd_set_error(APD_ERR_ID_D,__LINE__,APD_ERR_FILE); goto failed;
        }

        
        
        /* Update of strides of the FFT descriptor for the backward transform */
        
        status = DftiSetValue(*dft_handle, DFTI_INPUT_STRIDES, cs);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        status = DftiSetValue(*dft_handle, DFTI_OUTPUT_STRIDES, rs);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT2,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        
        /* Commiting the descriptor */
    
        status = DftiCommitDescriptor (*dft_handle);

        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT3,__LINE__,APD_ERR_FILE); goto failed;}
        
        
        
        /* Backward FFT */
        
        status = DftiComputeBackward (*dft_handle, s);
        
        if (status != DFTI_NO_ERROR)
        {
            f_apd_set_error(APD_ERR_ID_FT4,__LINE__,APD_ERR_FILE); goto failed;}
    }
    
    
    
    /* Output & Memory deallocation */
    
    finish:
        
        free(rs);
    
        free(cs);
        
        return exitflag;

    failed:
        
        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;
 
}

