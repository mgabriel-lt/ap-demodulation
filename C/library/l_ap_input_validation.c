
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
 * A function for checking the validity of input parameters for f_ap_demodulation:
 *
 * (1) l_ap_input_validation.
 */


    
int f_ap_input_validation (const double* s, const struct s_ParamsAP* Par, \
        const double* Ub, const double* t)
{
/* SHORT DESCRIPTION
 *
 * Checks the validity of input parameters for f_ap_demodulation.
 */
    
/* INPUT ARGUMENTS
 *
 * [s] - input signal.
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .Al - demodulation algorithm. Possible options are: 'B' -- AP-Basic,
 *              'A' -- AP-Accelerated, 'P' -- AP-Projected.
 *
 *         .D - number of signal dimensions.
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
 *         .Ns - number of elements in every dimension of the original input signal
 *               (before any possible interpolation). This is an array with the
 *               number of elements equal to the number of signal dimensions. It does
 *               not assume the additional two elements in the last dimension of s.
 *
 *         .Nr - number of sample points on the refined uniform grid used for
 *               interpolating nonuniformly sampled signals. This is an array with
 *               the number of elements equal to the number of signal dimensions.
 *               This field must be defined only if the third input argument t is
 *               provided (see below). If t is not provided, .Nr is ignored.
 *
 *         .Cp - compression parameter. If .Cp > 1, demodulation is performed by
 *               using signal compression (see equations (12)-(13) in the paper cited
 *               in the header of this file. This field is optional (the default is
 *               .Cp=1).
 *
 *         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
 *               when the value of the λ factor drops below one. If .Br=1, premature
 *               termination is assumed (default). Otherwise, the AP-A is not stopped
 *               premature even when λ decreases below 1. This field is required only
 *               if .Al='A'. It is optional (the default is .Br=1).
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
 *        elements as the input signal or must be set to NULL (if no upper bound on
 *        the modulator is assumed).
 *
 * [t] - sampling coordinates of the input signal. This is a 2D array with the number
 *       of columns equal to the dimension of the input signal and the number of rows
 *       equal to the number of signal sample points. This input argument is optional
 *       and should be used only if the input signal is sampled nonuniformly.
 *       Otherwise, t must be set to NULL. If t is not NULL, then the input argument
 *       Par must contain the field .Nr (see above).
 */

/* O U T P U T   A R G U M E N T S
 *
 * [exitflag] - exit flag: exitflag=0 - no error, exitflag=1 - error with input
 *              arguments.
 *
 *              Upon an error, the corresponding error message is sent to sderr.
 */
    
    
    
    /* Declaration and initialization of the local variables */
    
    int exitflag = 0;
    
    long i;
    
    long Ns = 1;
    
    long Ns3;
    
    
    
    
    /* Input validation */
    
    if (Par->Al != 'B' && Par->Al != 'A' && Par->Al != 'P')
    {
        
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_AL);
        
        exitflag = 1;
    }
    
    else if (Par->D<1 || Par->D>3)
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_D);
        
        exitflag = 1;
    }
    
    else if ( (Par->D>0 && (Par->Fs[0] <= 0 || !isfinite(Par->Fs[0]))) || \
              (Par->D>1 && (Par->Fs[1] <= 0 || !isfinite(Par->Fs[1]))) || \
              (Par->D>2 && (Par->Fs[2] <= 0 || !isfinite(Par->Fs[2]))) )
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_FS);
        
        exitflag = 1;
    }
    
    else if ( (Par->D>0 && (Par->Fc[0] <= 0 || !isfinite(Par->Fc[0]))) || \
              (Par->D>1 && (Par->Fc[1] <= 0 || !isfinite(Par->Fc[1]))) || \
              (Par->D>2 && (Par->Fc[2] <= 0 || !isfinite(Par->Fc[2]))) )
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_FC);
        
        exitflag = 1;
    }
    
    else if ( (Par->D>0 && Par->Fc[0] / Par->Fs[0] > 0.5) || \
              (Par->D>1 && Par->Fc[1] / Par->Fs[1] > 0.5) || \
              (Par->D>2 && Par->Fc[2] / Par->Fs[2] > 0.5) )
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_FC2);
                
        exitflag = 1;
    }
    
    else if (!isfinite(Par->Et))
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_ET);
        
        exitflag = 1;
    }
    
    else if (Par->Ni <= 0)
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_NI);
        
        exitflag = 1;
    }
    
    else if ( (t == NULL && \
             ((Par->D>0 && Par->Ns[0] <= 1) || \
              (Par->D>1 && Par->Ns[1] <= 1) || \
              (Par->D>2 && Par->Ns[2] <= 1))) || (t != NULL && Par->Ns[0] <= 1) )
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_NS);
        
        exitflag = 1;
    }
    
    else if ( t != NULL && \
              ((Par->D>0 && Par->Nr[0] <= 1) || \
               (Par->D>1 && Par->Nr[1] <= 1) || \
               (Par->D>2 && Par->Nr[2] <= 1)) )
    {
       f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_NR);
        
        exitflag = 1;
    }
    
    else if (Par->Cp<1 || !isfinite(Par->Cp))
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_CP);
        
        exitflag = 1;
    }
    
    else if (Par->Al == 'A' && Par->Br != 0 && Par->Br != 1)
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_BR);
                
        exitflag = 1;
    }
    
    else if (Par->ie[0] <= 0)
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_IE);
                
        exitflag = 1;
    }
    
    else if (Par->im[0] <= 0)
    {
        f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_IM);
                
        exitflag = 1;
    }
    
    else
    {
        for (i = 1; i <= Par->ie[0]; i++)
        {
            if (Par->ie[i] < 0 || (i>1 && Par->ie[i-1] >= Par->ie[i]))
            {
                f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_IE2);
                
                exitflag = 1;
                
                break;
            }
        }
        
        
        for (i = 1; i <= Par->im[0]; i++)
        {
            if (Par->im[i] < 0 || (i > 1 && Par->im[i-1] >= Par->im[i]))
            {
                f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_IM2);
                
                exitflag = 1;
                
                break;
            }
        }
    }
    
    
    
    if (t == NULL)
    {
        for (i=0; i<(Par->D); i++)

            Ns = Ns * Par->Ns[i]; /* original signal */
    }
    
    else
    
        Ns = Par->Ns[0];
    
    
    
    if (exitflag == 0)
    {
        for (i=0; i < Ns; i++)
        {
            if ( !isfinite(s[i]) )
            {
                f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_S);
                
                exitflag = 1;
            }
        }
    }
    
    
    
    if (exitflag == 0 && Ub != NULL)
    {
        
        for (i=0; i < Ns; i++)
        {
            if ( !isfinite(Ub[i]) || Ub[i] < fabs(s[i]))
            {
                f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_UB);
                
                exitflag = 1;
            }
        }
    }
    
    
    
    if (exitflag == 0 && t != NULL)
    {
        
        Ns3 = Ns * Par->D;
        
        for (i=0; i < Ns3; i++)
        {
            if ( !isfinite(t[i]) )
            {
                f_ap_print_error (__FUNCTION__, __LINE__, AP_ERR_MSG_T);
                
                exitflag = 1;
            }
        }
    }
    
    
    
    return exitflag;
    
}
