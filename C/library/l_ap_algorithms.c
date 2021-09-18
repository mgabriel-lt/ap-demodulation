
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
 * Three functions implementing different alternating projection algorithms of
 * amplitude demodulation:
 *
 * (1) f_ap_basic,
 *
 * (2) f_ap_accelerated,
 *
 * (3) f_ap_projected.
 */



int f_ap_basic (double* s, const struct s_ParamsAP* Par, const double* Ub, \
        const long* ix_map, DFTI_DESCRIPTOR_HANDLE* dft_handle, double* m_out, \
        double* e_out, long* iter)
{
/* S H O R T   D E S C R I P T I O N
 *
 * Calculates the modulator of a signal by using the AP-Basic algorithm. Signals
 * defined in up to 3 dimensions are allowed.
 */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal + 2 additional array elements along the last dimension. This
 *       input argument is modified in-place!
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .D - number of signal dimensions.
 *
 *         .Fs - sampling frequencies for each dimension of the signal. This is an
 *               array of D elements.
 *
 *         .Fc - cutoff frequencies of the modulator for each dimension of the
 *               signal. This is an array of D elements.
 *
 *         .Et - infeasibility error tolerance used to control the termination of the
 *               demodulation algorithm. The iterative process is stopped when the
 *               infeasibility error, ϵ, drops to the level of .Et or below. If
 *               .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
 *               below), is completed.
 *
 *         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
 *               chosen AP algorithm is iterated not more than .Ni times independent
 *               of whether ϵ drops at or below .Et.
 *
 *         .ns - number of sample points of the original input signal.
 *
 *         .Nx - number of elements of the provided input signal (possibly
 *               interpolated) in every dimension. This is an array with the number
 *               of elements equal to the number of signal dimensions. It does not
 *               assume the additional two elements in the last dimension of s.
 *
 *         .im - array with the iteration numbers at which the modulator estimates 
 *               have to be saved for the output. the first element is the length of
 *               the array (excluding the first element itself).
 *
 *         .ie - array with the iteration numbers at which the infeasibility error,
 *               ϵ, values have to be saved for the output. The first element of .ie
 *               is the length of the array (excluding the first element itself).
 *
 * [Ub] - upper bound on the modulator. This array must have the same number of
 *        elements as the input signal (does not include the additional two elements
 *        in the last dimension of s).
 *
 * [ix_map] - indexes of the moddulator elements to be saved for the output. This
 *            array is either NULL or consists of the same number of elements as the
 *            original input signal (before any possible interpolation).
 *
 * [dft_handle] - address of the comitted descriptor handle of the Intel MKL DFT.
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
 * [m_out#] - array with modulator estimates (memory allocated  externally).
 *
 * [e_out#] - array with error estimates (memory allocated  externally).
 *
 * [iter#] - the actual number of iterations used.
 */
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_abs_scaled_max_abs, (2) f_ap_mkl_dft_PMw.
 */
 
    
/***********************************************************************************/
/*********************** DECLARATIONS AND INITIALIZATIONS  *************************/
/***********************************************************************************/
    
    int line;
    
    int exitflag = 0;
    
    
    long i;
    
    long i_aux;
    
    long iter_m = 1;
    
    long iter_e = 1;
    
    long nx = 1;
    
    long nx_2;
    
    long *iL = NULL;
    
    long *iR = NULL;
    
    
    double E;
    
    double Etol;

    double max_s_abs;
    
    double s_old;
    
    double aux;
    
    
    double *s_abs = NULL;
    
    

    /* Number of sample points of the provided signal */
    
    for (i=0; i<(Par->D); i++)
    
        nx = nx * Par->Nx[i];
    
    nx_2 = (nx / Par->Nx[Par->D-1]) * (Par->Nx[Par->D-1]+2-(Par->Nx[Par->D-1]%2));
    
    

    /* Indexes of the left and right cutoff frequencies */
    
    iL = (long*) malloc((Par->D)*sizeof(long));
    
    if (iL == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    iR = (long*) malloc((Par->D)*sizeof(long));
    
    if (iR == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    for (i=0; i<(Par->D); i++)
    {
        iL[i] = 1 + (long) ceil(Par->Fc[i] / (Par->Fs[i] / Par->Nx[i]));
 
        iR[i] = Par->Nx[i] - iL[i];
    }
    
    
    
    /* Normalized absolute-value version of the signal */
    
    s_abs = (double*) malloc(nx_2*sizeof(double));
    
    if (s_abs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    max_s_abs = f_ap_abs_scaled_max_abs (s, nx_2, s_abs);
    
    
    
    /* Initialization of the error tolerance variable */
    
    if (Par->Et > 0)
        
        Etol = (Par->Et / max_s_abs) * (Par->Et / max_s_abs) * nx;
    
    else
            
        Etol = Par->Et;
    
    

    /* Initialization of the modulator and infeasibility error variables */

    E = 0;
    
    for (i=0; i<nx_2; i++)
    {
        /* Initial estimate of the modulator */
        
        s[i] = s_abs[i];
        
        
        /* Infeasibility error of the initial estimate of the modulator */
         
        E = E + s_abs[i] * s_abs[i];
    }
    
    
    
    /* Readout of the initial estimate of the modulator */

    if (Par->im[iter_m] == 0)
    {
        for (i=0; i<(Par->ns); i++)

            m_out[i] = s_abs[ix_map[i]] * max_s_abs;

        iter_m = iter_m + 1;
    }
    
    
    
    /* Readout of the infeasibility error */
        
    if (Par->ie[iter_e] == 0)
    {
        e_out[0] = sqrt(E / nx);

        iter_e = iter_e + 1;
    }

    

/***********************************************************************************/
/********************************* CALCULATION *************************************/
/***********************************************************************************/


    /* Alternating projections */
    
    *iter = 0;
    
    while (E > Etol && Par->Ni > *iter)
    {
        *iter = *iter + 1;
        
        
        
        /* Projection onto the set Mw */

        exitflag = f_ap_mkl_dft_PMw (s, Par->D, Par->Nx, iL, iR, dft_handle);
        
        if (exitflag != 0) goto finish;
        
        
                
        /* Projection onto the set Cd; error estimate */
        
        E = 0;
        
        for (i=0; i<nx_2; i++)
        {
            /* Copy of the current version of the modulator */
            
            s_old = s[i];
            
            
            
            /* Projection onto Cd */
            
            if (s[i] < s_abs[i])
                
                s[i] = s_abs[i];
            
            else if (Ub != NULL && s[i] > Ub[i])
                
                s[i] = Ub[i];
            
            
            
            /* Infeasibility error */
            
            aux = (s[i]-s_old);

            E = E + aux * aux;
            
        }
     
        
        
        /* Output (modulator) */
        
        if ( iter_m <= Par->im[0] && (*iter == Par->im[iter_m] || \
                (E <= Etol && Par->im[0] == 1 && Par->im[1] == Par->Ni)) )
        {
            i_aux = (iter_m-1)*(Par->ns);
            
            for (i=0; i<(Par->ns); i++)
                
                m_out[i+i_aux] = s[ix_map[i]] * max_s_abs;
            
            iter_m = iter_m + 1;
        }
        
        
        
        /* Output (infeasibility error) */
        
        if ( iter_e <= Par->ie[0] && (*iter == Par->ie[iter_e] || \
                (E <= Etol && Par->ie[0] == 1 && Par->ie[1] == Par->Ni)) )
        {
            e_out[iter_e-1] = sqrt(E / nx);
            
            iter_e = iter_e + 1;
        }
        
    }
    
    
    
/***********************************************************************************/
/************************** OUTPUT & MEMORY DEALLOCATION ***************************/
/***********************************************************************************/
    
    
    goto finish;
    
    finish:
        
        free(iL);
    
        free(iR);

        free(s_abs);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        goto finish;
    
    
}




int f_ap_accelerated (double* s, const struct s_ParamsAP* Par, const double* Ub, \
        const long* ix_map, DFTI_DESCRIPTOR_HANDLE* dft_handle, double* m_out, \
        double* e_out, long* iter)
{   
/* S H O R T   D E S C R I P T I O N
 *
 * Calculates the modulator of a signal by using the AP-Accelerated algorithm.
 * Signals defined in up to 3 dimensions are allowed.
 */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal + 2 additional array elements along the last dimension. This
 *       input argument is modified in-place!
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .Fs - sampling frequencies. This is an array of D elements with sampling
 *               frequency for each dimension of the signal.
 *
 *         .Fc - cutoff frequencies of the modulator. This is an array of D elements
 *               with cutoff frequencies for each dimension of the signal.
 *
 *         .Et - infeasibility error tolerance used to control the termination of the
 *               demodulation algorithm. The iterative process is stopped when the
 *               infeasibility error, ϵ, drops to the level of .Et or below. If
 *               .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
 *               below), is completed.
 *
 *         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
 *               chosen AP algorithm is iterated not more than .Ni times independent
 *               of whether ϵ drops at or below .Et.
 *
 *         .D - number of signal dimensions.
 *
 *         .ns - number of sample points of the original input signal.
 *
 *         .Nx - number of elements of the provided input signal (possibly
 *               interpolated) in every dimension. This is an array with the number
 *               of elements equal to the number of signal dimensions. It does not
 *               assume the additional two elements in the last dimension of s.
 *
 *         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
 *               when the λ factor drops below one. If .Br=1 (recommended), premature
 *               termination is assumed. Otherwise, if .Br=0, the AP-A is not stopped
 *               prematurely even when λ decreases below 1.
 *
 *         .im - array with the iteration numbers at which the modulator estimates 
 *               have to be saved for the output. the first element is the length of
 *               the array (excluding the first element itself).
 *
 *         .ie - array with the iteration numbers at which the infeasibility error,
 *               ϵ, values have to be saved for the output. The first element of .ie
 *               is the length of the array (excluding the first element itself).
 *
 * [Ub] - upper bound on the modulator. This array must have the same number of
 *        elements as the input signal (does not include the additional two elements
 *        in the last dimension of s).
 *
 * [ix_map] - indexes of the moddulator elements to be saved for the output. This
 *            array is either NULL or consists of the same number of elements as the
 *            original input signal (before any possible interpolation).
 *
 * [dft_handle] - address of the comitted descriptor handle of the Intel MKL DFT.
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
 * [m_out#] - array with modulator estimates (memory allocated  externally).
 *
 * [e_out#] - array with error estimates (memory allocated  externally).
 *
 * [iter#] - the actual number of iterations used.
 */
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_abs_scaled_max_abs, (2) f_ap_mkl_dft_PMw.
 */
    
    
/***********************************************************************************/
/*********************** DECLARATIONS AND INITIALIZATIONS  *************************/
/***********************************************************************************/
    
    
    int line;
    
    int exitflag = 0;
    
    
    long i;
    
    long i_aux;
    
    long iter_m = 1;
    
    long iter_e = 1;
    
    long nx = 1;
    
    long nx_2;
    
    long *iL = NULL;
    
    long *iR = NULL;
    
    
    double E;
    
    double Etol;

    double max_s_abs;
    
    double lambda;
    
    double nom;
    
    double denom;
    
    
    double *a = NULL;
    
    double *b = NULL;
    
    double *s_abs = NULL;
    
    
    
    /* Number of sample points of the provided signal */
    
    for (i=0; i<(Par->D); i++)
    
        nx = nx * Par->Nx[i];
    
    nx_2 = (nx / Par->Nx[Par->D-1]) * (Par->Nx[Par->D-1]+2-(Par->Nx[Par->D-1]%2));
    
    
    
    /* Indexes of the left and right cutoff frequencies */
    
    iL = (long*) malloc((Par->D)*sizeof(long));
    
    if (iL == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    iR = (long*) malloc((Par->D)*sizeof(long));
    
    if (iR == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    for (i=0; i<(Par->D); i++)
    {
        iL[i] = 1 + (long) ceil(Par->Fc[i] / (Par->Fs[i] / Par->Nx[i]));
        
        iR[i] = Par->Nx[i] - iL[i];
    }
    
    
    
    /* Normalized absolute-value version of the signal */
    
    s_abs = (double*) malloc(nx_2*sizeof(double));
    
    if (s_abs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    max_s_abs = f_ap_abs_scaled_max_abs (s, nx_2, s_abs);
    
    
    
    /* Initialization of the error tolerance variable */
    
    if (Par->Et > 0)
        
        Etol = (Par->Et / max_s_abs) * (Par->Et / max_s_abs) * nx;
    
    else
            
        Etol = Par->Et;
    
    
    
    /* Initialization of the modulator-related and infeasibility error variables */
    
    a = (double*) calloc(nx_2,sizeof(double));
    
    if (a == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    b = (double*) malloc(nx_2*sizeof(double));
    
    if (b == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    nom = 0;
    
    E = 0;
    
    for (i=0; i<nx_2; i++)
    {
        /* Initial estimates of the variable b and the nominator of lambda */
        
        b[i] = s_abs[i];
        
        nom = nom + b[i] * b[i];
        
        
        /* Infeasibility error of the initial estimate of the modulator */
         
        E = E + s_abs[i] * s_abs[i];
    }
    
    
    
    /* Readout of the initial estimate of the modulator */

    if (Par->im[iter_m] == 0)
    {
        for (i=0; i<(Par->ns); i++)

            m_out[i] = s_abs[ix_map[i]] * max_s_abs;

        iter_m = iter_m + 1;
    }
    
    
    
    /* Readout of the infeasibility error */
        
    if (Par->ie[iter_e] == 0)
    {
        e_out[0] = sqrt(E / nx);

        iter_e = iter_e + 1;
    }
    
    
    
/***********************************************************************************/
/********************************* CALCULATION *************************************/
/***********************************************************************************/
    
    
    /* Alternating projections */
    
    *iter = 0;
    
    while (E > Etol && Par->Ni > *iter)
    {
        *iter = *iter + 1;
        
        
        
        /* Projection onto the set Mw */
        
        exitflag = f_ap_mkl_dft_PMw (b, Par->D, Par->Nx, iL, iR, dft_handle);
        
        if (exitflag != 0) goto finish;
        
        
        
        /* Factor lambda */
        
        denom = 0;
        
        for (i=0; i<nx_2; i++)
            
            denom = denom + b[i] * b[i];
        
        
        if (denom != 0)
            
            lambda = nom / denom;
        
        else
            
            lambda = 1;
        
        if (lambda < 1 && Par->Br != 0)
            
            break;
        
        
        
        /* Projection onto the set Cd; a, b, nom, and error estimates */
        
        nom = 0;
        
        E = 0;
        
        for (i=0; i<nx_2; i++)
        {            
            /* a */
            
            a[i] = a[i] + lambda * b[i];
            
            
            
            /* Projection onto Cd */
            
            s[i] = a[i];
            
            if (s[i] < s_abs[i])
                
                s[i] = s_abs[i];
            
            else if (Ub != NULL && s[i] > Ub[i])
                
                s[i] = Ub[i];
            
            
            
            /* Nominator of the factor lambda for the next iteration */
            
            b[i] = s[i] - a[i];
            
            nom = nom + b[i] * b[i];
            
            
            
            /* Infeasibility error */
            
            E = nom;
        }
        
        
        
        /* Output (modulator) */
        
        if ( iter_m <= Par->im[0] && (*iter == Par->im[iter_m] || \
                (E <= Etol && Par->im[0] == 1 && Par->im[1] == Par->Ni)) )
        {
            i_aux = (iter_m-1)*(Par->ns);
            
            for (i=0; i<(Par->ns); i++)
                
                m_out[i+i_aux] = s[ix_map[i]] * max_s_abs;
            
            iter_m = iter_m + 1;
        }
        
        
        
        /* Output (infeasibility error) */
        
        if ( iter_e <= Par->ie[0] && (*iter == Par->ie[iter_e] || \
                (E <= Etol && Par->ie[0] == 1 && Par->ie[1] == Par->Ni)) )
        {
            e_out[iter_e-1] = sqrt(E / nx);
            
            iter_e = iter_e + 1;
        }
    }
    
    
    
/***********************************************************************************/
/************************** OUTPUT & MEMORY DEALLOCATION ***************************/
/***********************************************************************************/
    
    
    goto finish;
    
    finish:
        
        free(iL);
    
        free(iR);

        free(s_abs);
        
        free(a);
    
        free(b);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        goto finish;
    
    
}




int f_ap_projected (double* s, const struct s_ParamsAP* Par, const double* Ub, \
        const long* ix_map, DFTI_DESCRIPTOR_HANDLE* dft_handle, double* m_out, \
        double* e_out, long* iter)
{   
/* S H O R T   D E S C R I P T I O N
 *
 * Calculates the modulator of a signal by using the AP-Basic algorithm. Signals
 * defined in up to 3 dimensions are allowed.
 */

/* I N P U T   A R G U M E N T S
 *
 * [s] - input signal + 2 additional array elements along the last dimension. This
 *       input argument is modified in-place!
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .Fs - sampling frequencies. This is an array of D elements with sampling
 *               frequency for each dimension of the signal.
 *
 *         .Fc - cutoff frequencies of the modulator. This is an array of D elements
 *               with cutoff frequencies for each dimension of the signal.
 *
 *         .Et - infeasibility error tolerance used to control the termination of the
 *               demodulation algorithm. The iterative process is stopped when the
 *               infeasibility error, ϵ, drops to the level of .Et or below. If
 *               .Et ≤ 0, then the maximum allowed number of iterations, .Ni (see
 *               below), is completed.
 *
 *         .Ni - maximum number of allowed iterations of the chosen AP algorithm. The
 *               chosen AP algorithm is iterated not more than .Ni times independent
 *               of whether ϵ drops at or below .Et.
 *
 *         .D - number of signal dimensions.
 *
 *         .ns - number of sample points of the original input signal.
 *
 *         .Nx - number of elements of the provided input signal (possibly
 *               interpolated) in every dimension. This is an array with the number
 *               of elements equal to the number of signal dimensions. It does not
 *               assume the additional two elements in the last dimension of s.
 *
 *         .im - array with the iteration numbers at which the modulator estimates 
 *               have to be saved for the output. the first element is the length of
 *               the array (excluding the first element itself).
 *
 *         .ie - array with the iteration numbers at which the infeasibility error,
 *               ϵ, values have to be saved for the output. The first element of .ie
 *               is the length of the array (excluding the first element itself).
 *
 * [Ub] - upper bound on the modulator. This array must have the same number of
 *        elements as the input signal (does not include the additional two elements
 *        in the last dimension of s).
 *
 * [ix_map] - indexes of the moddulator elements to be saved for the output. This
 *            array is either NULL or consists of the same number of elements as the
 *            original input signal (before any possible interpolation).
 *
 * [dft_handle] - address of the comitted descriptor handle of the Intel MKL DFT.
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
 * [m_out#] - array with modulator estimates (memory allocated  externally).
 *
 * [e_out#] - array with error estimates (memory allocated  externally).
 *
 * [iter#] - the actual number of iterations used.
 */
    
/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) f_ap_abs_scaled_max_abs, (2) f_ap_mkl_dft_PMw.
 */
    
    
/***********************************************************************************/
/*********************** DECLARATIONS AND INITIALIZATIONS  *************************/
/***********************************************************************************/
    
    
    int line;
    
    int exitflag = 0;
    
    
    long i;
    
    long i_aux;
    
    long iter_m = 1;
    
    long iter_e = 1;
    
    long nx = 1;
    
    long nx_2;
    
    long *iL = NULL;
    
    long *iR = NULL;
    
    
    double E;
    
    double Etol;

    double max_s_abs;
    
    double aux;
    
    double aux2;
    
    
    double *s_abs = NULL;
    
    double *a = NULL;
    
    double *c = NULL;
    
    
    
    /* Number of sample points of the provided signal */
    
    for (i=0; i<(Par->D); i++)
    
        nx = nx * Par->Nx[i];
    
    nx_2 = (nx / Par->Nx[Par->D-1]) * (Par->Nx[Par->D-1]+2-(Par->Nx[Par->D-1]%2));
    
    
    
    /* Indexes of the left and right cutoff frequencies */
    
    iL = (long*) malloc((Par->D)*sizeof(long));
    
    if (iL == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    iR = (long*) malloc((Par->D)*sizeof(long));
    
    if (iR == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    for (i=0; i<(Par->D); i++)
    {
        iL[i] = 1 + (long) ceil(Par->Fc[i] / (Par->Fs[i] / Par->Nx[i]));
        
        iR[i] = Par->Nx[i] - iL[i];
    }

    
    
    /* Normalized absolute-value version of the signal */
    
    s_abs = (double*) malloc(nx_2*sizeof(double));
    
    if (s_abs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    max_s_abs = f_ap_abs_scaled_max_abs (s, nx_2, s_abs);

    
    
    /* Initialization of the error tolerance variable */
    
    if (Par->Et > 0)
        
        Etol = (Par->Et / max_s_abs) * (Par->Et / max_s_abs) * nx;
    
    else
            
        Etol = Par->Et;

    

    /* Initialization of the infeasibility error and modulator-related variables */
    
    a = (double*) malloc(nx_2*sizeof(double));
    
    if (a == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    
    c = (double*) malloc(nx_2*sizeof(double));
    
    if (c == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    
    E = 0;
    
    for (i=0; i<nx_2; i++)
    {
        /* Initial estimate of the modulator and variables a and c */
        
        s[i] = s_abs[i];
        
        a[i] = s[i];
        
        c[i] = s[i];
        
        
        /* Infeasibility error of the initial estimate of the modulator */
         
        E = E + s_abs[i] * s_abs[i];
    }

    

    /* Readout of the initial estimate of the modulator */

    if (Par->im[iter_m] == 0)
    {
        for (i=0; i<(Par->ns); i++)

            m_out[i] = s_abs[ix_map[i]] * max_s_abs;

        iter_m = iter_m + 1;
    }
    
    
    
    /* Readout of the infeasibility error */
        
    if (Par->ie[iter_e] == 0)
    {
        e_out[0] = sqrt(E / nx);

        iter_e = iter_e + 1;
    }
    
    
    
/***********************************************************************************/
/********************************* CALCULATION *************************************/
/***********************************************************************************/

    
    /* Alternating projections */

    *iter = 0;
    
    while (E > Etol && Par->Ni > *iter)
    {
        *iter = *iter + 1;
        
        
        /* Projection onto the set Mw */
        
        exitflag = f_ap_mkl_dft_PMw (a, Par->D, Par->Nx, iL, iR, dft_handle);
        
        if (exitflag != 0) goto finish;
        
        
        
        /* Projection onto the set Cd; error estimate */
        
        E = 0;
        
        for (i=0; i<nx_2; i++)
        {
            /* Projection onto Cd*/
            
            aux = s[i] - a[i];
            
            s[i] = (a[i] - c[i]);
            
            if (s[i] < s_abs[i])
                
                s[i] = s_abs[i];
            
            else if (Ub != NULL && s[i] > Ub[i])
                
                s[i] = Ub[i];
            
            
            
            /* c and a */
            
            aux2 = s[i] - a[i];
            
            c[i] = c[i] + aux2;
            
            a[i] = s[i];
            
            
            
            /* Infeasibility error */
            
            E = E + aux * aux + aux2 * aux2;
                
        }
        
        
        
        /* Output (modulator) */
        
        if ( iter_m <= Par->im[0] && (*iter == Par->im[iter_m] || \
                (E <= Etol && Par->im[0] == 1 && Par->im[1] == Par->Ni)) )
        {
            i_aux = (iter_m-1)*(Par->ns);
   
            for (i=0; i<(Par->ns); i++)
                
                m_out[i+i_aux] = s[ix_map[i]] * max_s_abs;
    
            iter_m = iter_m + 1;
        }
        
        
        
        /* Output (infeasibility error) */
        
        if ( iter_e <= Par->ie[0] && (*iter == Par->ie[iter_e] || \
                (E <= Etol && Par->ie[0] == 1 && Par->ie[1] == Par->Ni)) )
        {
            e_out[iter_e-1] = sqrt(E / nx / 2);
            
            iter_e = iter_e + 1;
        }
        
    }
    
    
    
/***********************************************************************************/
/************************** OUTPUT & MEMORY DEALLOCATION ***************************/
/***********************************************************************************/
    
    
    goto finish;
    
    finish:
        
        free(iL);
    
        free(iR);

        free(s_abs);
        
        free(a);
    
        free(c);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            f_ap_print_error (__FUNCTION__, line, AP_ERR_MSG_MEM);
        
        goto finish;
    
   
}

