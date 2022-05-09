
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
 * This is the header file for the AP Demodulation library. It:
 * 
 * (1) Includes headers of all needed external libraries.
 * 
 * (2) Declares the input parameter structure for the f_apd_demodulation and other
 *     functions of this library.
 * 
 * (3) Defines constant Pi (if not defined).
 * 
 * (4) Declares prototypes of the frontend functions, i.e., functions directly
 *     accessible to the user, of this library.
 * 
 * (5) Defines the operating-system-dependent macro APD_ERR_FILE, which is used for
 *     error message formatting.
 *
 * (6) Defines macros with operating-system-dependent new-line control characters,
 *     which are used for error message formatting.
 * 
 * (7) Defines a macro with the error message prefix.
 * 
 * (8) Defines macros for error ids.
 */



#ifndef APD_HEADER

    #define APD_HEADER



    /* (1) EXTERNAL LIBRARIES */

    #include <stdlib.h>

    #include <stdio.h>

    #include <math.h>

    #include <float.h>

    #include <string.h>


    #include "mkl.h"

    #include "mkl_dfti.h"




    /* (2) INPUT PARAMETER STRUCTURE */

    struct strAPD_Par {

                        char         Al;

                        int          D;

                        double       Fs[3];

                        double       Fc[3];

                        double       Et;

                        long         Ni;

                        long         Ns[3];
                        
                        long         ns;

                        long         Nr[3];

                        long*        Nx;

                        double       Cp;

                        int          Br;

                        long*        im;

                        long*        ie;

                      };
                      
                      
                      

    /* (3) CONSTANT PI */

    #ifndef M_PI

        #define M_PI 3.14159265358979323846

    #endif




    /* (4) FRONTEND FUNCTIONS OF THE AP DEMODULATION LIBRARY  */
    
    #ifdef __cplusplus
    extern "C" {
    #endif

        int f_apd_demodulation (double*, struct strAPD_Par*, double*, double*, \
                                double*, double*, long*);

        void f_apd_set_errexit (int);

        void f_apd_get_error (int*, long*, char*, char*);

        void f_apd_print_error (int);

    #ifdef __cplusplus
    }
    #endif
    
    
    
    
    /* (5) APD_ERR_FILE macro */

    #ifdef _WIN32

        #define APD_ERR_FILE strrchr("\\" __FILE__, '\\') + 1

    #else

        #define APD_ERR_FILE strrchr("/" __FILE__, '/') + 1

    #endif




    /* (6) NEW-LINE CONTROL CHARACTERS */
    
    #ifdef _WIN32
    
        #define APD_ERR_MSG_NL "\r"
    
        #define APD_ERR_MSG_NL2 "\r\r"
    
    #else
    
        #define APD_ERR_MSG_NL "\n"
    
        #define APD_ERR_MSG_NL2 "\n\n"
    
    #endif




    /* (7) PREFIX OF THE ERROR MESSAGE */

    #define APD_ERR_MSG_PRE "(!) ERROR in AP Demodulation"



    
    /* (8) ERROR IDs */

    /* Macros of numeric codes of the error messages */

    #define APD_ERR_N 24     // the largest error id in use


    #define APD_ERR_ID_NON 0

    #define APD_ERR_ID_MEM 1

    #define APD_ERR_ID_FT1 2

    #define APD_ERR_ID_FT2 3

    #define APD_ERR_ID_FT3 4

    #define APD_ERR_ID_FT4 5

    #define APD_ERR_ID_AL 6
            
    #define APD_ERR_ID_D 7
                                   
    #define APD_ERR_ID_FS 8
                                   
    #define APD_ERR_ID_FC 9
                                   
    #define APD_ERR_ID_FC2 10
                                   
    #define APD_ERR_ID_ET 11
                                   
    #define APD_ERR_ID_NI 12
                                  
    #define APD_ERR_ID_NS 13
                                  
    #define APD_ERR_ID_NR 14
                                  
    #define APD_ERR_ID_CP 15
                                  
    #define APD_ERR_ID_BR 16
                                  
    #define APD_ERR_ID_IE 17
                                  
    #define APD_ERR_ID_IE2 18
                                  
    #define APD_ERR_ID_IM 19
                                  
    #define APD_ERR_ID_IM2 20
                                  
    #define APD_ERR_ID_S 21
                                  
    #define APD_ERR_ID_UB 22
                                  
    #define APD_ERR_ID_T 23

    #define APD_ERR_ID_NUL 24
                                  
                 
#endif

            
