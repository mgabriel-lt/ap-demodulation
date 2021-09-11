
/*                       C O P Y R I G H T   N O T I C E
 *
 * Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
 * All Rights Reserved. The underlying technology is protected by PCT Patent
 * Application No. PCT/EP2021/054650.
 *
 * This file is part of the AP demodulation library, which is free software: you can
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
 * This is the header file for the f_ap_demodulation routine. It includes all needed
 * external libraries, declares the input parameter structure for the
 * f_ap_demodulation function, declares the prototype of the f_ap_demodulation
 * function itself, defines macros with error mesages, and operating-system-dependent
 * new-line control characters.
 */



#ifndef ap_demodulation_h

    #define ap_demodulation_h



    /* EXTERNAL LIBRARIES */

    #include <stdio.h>

    #include <math.h>

    #include <float.h>

    #include <string.h>


    #include "mkl.h"

    #include "mkl_dfti.h"




    /* INPUT PARAMETER STRUCTURE */

    struct s_ParamsAP {

                        double*      Fs;

                        double*      Fc;

                        char         Al;

                        double       Et;

                        long         Ni;

                        int          D;

                        long*        Ns;
                        
                        long         ns;

                        long*        Nr;

                        long*        Nx;

                        double       Cp;

                        int          Br;

                        long*        im;

                        long*        ie;

                      };
                      
                      
                      
                      
    /* AP DEMODULATION FUNCTION */
                      
    int f_ap_demodulation (double*, struct s_ParamsAP*, double*, const double*, \
            double*, double*, long*);
    
    
    
    
    /* ERROR MESSAGES (INVALID INPUT)*/
    
    #define AP_ERR_MSG_AL "AP algorithm, set by Par.Al, must be either \'B\' "\
                          "(Basic), \'A\' (Accelerated), or \'P\' (Projected)!"
            
    #define AP_ERR_MSG_D "The number of signal dimensions, set by Par.D, must be "\
                         "equal to 1, 2, or 3!"
                                   
    #define AP_ERR_MSG_FS "Sampling frequencies, set by Par.Fs, must be positive "\
                          "real numbers!"
                                   
    #define AP_ERR_MSG_FC "Cutoff frequencies, set by Par.Fc, must be "\
                          "non-negative real numbers!"
                                   
    #define AP_ERR_MSG_FC2 "Cutoff frequencies, set by Par.Fc, cannot be higher "\
                           "than half of the sampling frequencies set by Par.Fs!"
                                   
    #define AP_ERR_MSG_ET "Error tolerance, set by Par.Et, must be a real number!"
                                   
    #define AP_ERR_MSG_NI "Iteration number, set by Par.Niter, must be a positive "\
                          "number!"
                                  
    #define AP_ERR_MSG_NS "Numbers of elements of the signal array in each "\
                          "dimension, set by Par.Ns, must be integers higher than 1!"
                                  
    #define AP_ERR_MSG_NR "Numbers of elements of the interpolated signal array in "\
                          "each dimension, set by Par.Nr, must be integers higher "\
                          "than 1!"
                                  
    #define AP_ERR_MSG_CP "Compression parameter, set by Par.Cp, must be a real "\
                          "number not smaller than 1!"
                                  
    #define AP_ERR_MSG_BR "Indicator of premature termination of the AP-A "\
                          "algorithm, set by Par.Br, must be either 0 or 1!"
                                  
    #define AP_ERR_MSG_IE "The number of error-sampling iterations, set by "\
                          "Par.ie[0], must be positive!"
                                  
    #define AP_ERR_MSG_IE2 "Error-sampling iterations, set by Par.ie, must be "\
                           "nonnegative strictly increasing numbers!"
                                  
    #define AP_ERR_MSG_IM "The number of modulator-sampling iterations, set by "\
                          "Par.im[0], must be positive!"
                                  
    #define AP_ERR_MSG_IM2 "Modulator-sampling iterations, set by Par.im, must be "\
                           "nonnegative strictly increasing numbers!"
                                  
    #define AP_ERR_MSG_S "Array with the signal values, set by the input argument "\
                         "s, must consist of real numbers!"
                                  
    #define AP_ERR_MSG_UB "Array with the upper bound values for modulator, set by "\
                          "the input argument Ub, must consist of real numbers not "\
                          "smaller than entries of the absolute value signal!"
                                  
    #define AP_ERR_MSG_T "Array with the sampling coordinates, set by the input "\
                         "argument t, must consist of real numbers!"
                                  

                       
                                  
    /* ERROR MESSAGES (OTHER)*/
    
    #define AP_ERR_MSG_PRE "(!) ERROR in AP Demodulation"
                                  
    #define AP_ERR_MSG_MEM "Out of memmory!"
                                  
                                  
                                  
                               
                                  
    /* NEW-LINE CONTROL CHARACTERS */
    
    #ifdef _WIN32
    
        #define AP_STR_NL "\r\n"
    
        #define AP_STR_NL2 "\r\n\r\n"
    
    #else
    
        #define AP_STR_NL "\n"
    
        #define AP_STR_NL2 "\n\n"
    
    #endif
                                  
                  
#endif

            