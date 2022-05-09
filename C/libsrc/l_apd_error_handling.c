
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
 * Static global variables and functions for error handling and error message
 * formating and printing + a function for checking the validity of input parameters
 * for f_apd_demodulation:
 *
 * (1) sgAPD_ERR_ID, sgAPD_ERR_EXIT, sgAPD_ERR_LINE, sgAPD_ERR_FILE, and
 *     sgAPD_ERR_MSG.
 * 
 * (2) f_apd_set_errexit, f_apd_set_error, f_apd_get_error, and f_apd_print_error.
 * 
 * (3) f_apd_input_validation.
 */


    
#include "h_apd.h"



/* (1) STATIC GLOBAL VARIABLES FOR ERROR HANDLING */

/* Variable with the numeric id of the error status */

static int sgAPD_ERR_ID = 0;


/* Variable that indicates the behavior of the program upon error */

static int sgAPD_ERR_EXIT = 1;


/* Variable that holds the line no. of the file where the error occurs */

static long sgAPD_ERR_LINE = 0;


/* Variable that holds the name of the code file where the error occurs */

static char sgAPD_ERR_FILE[200] = "";


/* Variable that holds error messages (see h_apd.h for definitions of the
   error message id macros) */

const char *sgAPD_ERR_MSG[] = {

    /* No errors */
    "AP Demodulation algorithm completed without errors!",                 //[0]
                                                                           //
    /* Out of memory */
    "Out of memory!",                                                      //[1]
                                                                           //
    /* MKL DFT routines */
    "Error while creating an MKL DFT descriptor!",                         //[2]
                                                                           //
    "Error while setting the MKL DFT descriptor parameters!",              //[3]
                                                                           //
    "Error while committing the MKL DFT descriptor!",                      //[4]
                                                                           //
    "Error while computing the DFT!",                                      //[5]
                                                                           //
    /* Input validation */
    "AP algorithm, set by Par.Al, must be either \'B\' (Basic), "          //[6]
    "\'A\' (Accelerated), or \'P\' (Projected)!",                          //
                                                                           //
    "The number of signal dimensions, set by Par.D, must be equal to "     //[7]
    "1, 2, or 3!",                                                         //
                                                                           //
    "Sampling frequencies, set by Par.Fs, must be positive real "          //[8]
    "numbers!",                                                            //
                                                                           //
    "Cutoff frequencies, set by Par.Fc, must be non-negative real "        //[9]
    "numbers!",                                                            //
                                                                           //
    "Cutoff frequencies, set by Par.Fc, cannot be higher "                 //[10]
    "than half of the sampling frequencies set by Par.Fs!",                //
                                                                           //
    "Error tolerance, set by Par.Et, must be a real number!",              //[11]
                                                                           //
    "Iteration number, set by Par.Niter, must be a positive number!",      //[12]
                                                                           //
    "Numbers of elements of the signal array in each dimension, "          //[13]
    "set by Par.Ns, must be integers higher than 1!",                      //
                                                                           //
    "Numbers of elements of the interpolated signal array in each "        //[14]
    "dimension, set by Par.Nr, must be integers higher than 1!",           //
                                                                           //
    "Compression parameter, set by Par.Cp, must be a real "                //[15]
    "number not smaller than 1!",                                          //
                                                                           //
    "Indicator of premature termination of the AP-A "                      //[16]
    "algorithm, set by Par.Br, must be either 0 or 1!",                    //
                                                                           //
    "The number of error-sampling iterations, set by "                     //[17]
    "Par.ie[0], must be positive!",                                        //
                                                                           //
    "Error-sampling iterations, set by Par.ie, must be "                   //[18]
    "nonnegative strictly increasing numbers!",                            //
                                                                           //
    "The number of modulator-sampling iterations, set by "                 //[19]
    "Par.im[0], must be positive!",                                        //
                                                                           //
    "Modulator-sampling iterations, set by Par.im, must be "               //[20]
    "nonnegative strictly increasing numbers!",                            //
                                                                           //
    "Array with the signal values, set by the input argument "             //[21]
    "s, must consist of real numbers!",                                    //
                                                                           //
    "Array with the upper bound values for modulator, set by "             //[22]
    "the input argument Ub, must consist of real numbers not "             //
    "smaller than entries of the absolute value signal!",                  //
                                                                           //
    "Array with the sampling coordinates, set by the input "               //[23]
    "argument t, must consist of real numbers!",                           //
                                                                           //
    /* For C++ version */
    "The second input argument has to be a NULL pointer!"                  //[24]
                                                                           //
    /* Invalid error id */
    "Invalid error id provided to f_apd_print_error!"                       //[25]
    };




/* (2) FUNCTIONS FOR ERROR HANDLING */

void f_apd_set_errexit (int err_exit)
{
/* P U R P O S E
 *
 * Stores the indicator of the behavior of f_apd_demodulation upon an error.
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

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * None.
 */
    
    sgAPD_ERR_EXIT = err_exit;
}




void f_apd_set_error ( const int err_id, \

                       const long err_line, \

                       const char* err_file )
{
/* P U R P O S E
 *
 * Stores the line number and the filename associated to the error.
 */

/* I N P U T   A R G U M E N T S
 *
 * [err_id] - error id (see the definition of sgAPD_ERR_MSG for relevant error
 *            messages and their ids).
 * 
 * [err_line] - line number of the code file associated with the error.
 * 
 * [err_file] - name of the code file associated with the error.
 */

/* O U T P U T   A R G U M E N T S
 *
 * None.
 */

/* R E T U R N   V A L U E
 *
 * None.
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * None.
 */
    
    sgAPD_ERR_ID = err_id;

    sgAPD_ERR_LINE = err_line;
    
    strcpy(sgAPD_ERR_FILE, err_file);


    if (sgAPD_ERR_EXIT != 0 && err_id != APD_ERR_ID_NON)
    {
        f_apd_print_error (err_id);

        exit(err_id);
    }
}




void f_apd_get_error ( int* err_id, \

                       long* err_line, \

                       char* err_file, \

                       char* err_msg )
{
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
 *            variable). See the definition of sgAPD_ERR_MSG for relevant error
 *            messages and their ids. If err_id == NULL, no value is assigned.
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

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * None.
 */
    
    if (err_id != NULL)

        (*err_id) = sgAPD_ERR_ID;

    if (err_line != NULL)

        (*err_line) = sgAPD_ERR_LINE;

    if (err_file != NULL)
    
        strcpy(err_file, sgAPD_ERR_FILE);

    if (err_msg != NULL)

        strcpy(err_msg, sgAPD_ERR_MSG[sgAPD_ERR_ID]);
}




void f_apd_print_error (int err_id)
{
/* P U R P O S E
 *
 * Prints an error message to stderr (standard executable) or MATLAB command window
 * (MEX executable).
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

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * (1) mexErrMsgIdAndTxt (relevant only for MEX compilation).
 */
    
    if (err_id < 0 || err_id > APD_ERR_N)
    {
        err_id = APD_ERR_N + 1;
    }

    
    #ifdef APD_MEX_COMPILATION
        
        mexErrMsgIdAndTxt("AP_Demodulation:IV", APD_ERR_MSG_NL APD_ERR_MSG_PRE \
                          " (line %ld in %s): " APD_ERR_MSG_NL2 "%s", sgAPD_ERR_LINE, \
                          sgAPD_ERR_FILE, sgAPD_ERR_MSG[err_id]);

    #else

        fprintf (stderr, APD_ERR_MSG_NL APD_ERR_MSG_PRE " (line %ld in %s): " \
                 APD_ERR_MSG_NL2 "%s" APD_ERR_MSG_NL2, sgAPD_ERR_LINE, \
                 sgAPD_ERR_FILE, sgAPD_ERR_MSG[err_id]);

    #endif

}




/* (3) FUNCTION FOR INPUT VALIDATION */

int f_apd_input_validation ( const double* s, \

                             const struct strAPD_Par* Par, \

                             const double* Ub, \
                           
                             const double* t )
{
/* SHORT DESCRIPTION
 *
 * Checks the validity of input parameters for f_apd_demodulation.
 */
    
/* INPUT ARGUMENTS
 *
 * [s] - input signal.
 *
 * [Par] - pointer to the structure with demodulation parameters:
 *
 *         .Al - demodulation algorithm. Possible options are: 'B' - AP-Basic,
 *              'A' - AP-Accelerated, 'P' - AP-Projected. {Type: char}
 *
 *         .D - number of signal dimensions. Currently, 0 < D < 4 are available.
 *              {Type: int}
 *
 *         .Fs - sampling frequencies. This is an array of D elements with sampling
 *               frequency for each dimension of the signal. {Type: double}
 *
 *         .Fc - cutoff frequencies of the modulator. This is an array of D elements
 *               with cutoff frequencies for each dimension of the signal.
 *              {Type: double}
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
 *         .Ns - number of elements in every dimension of the original input signal
 *               (before any possible interpolation). This is an array with the
 *               number of elements equal to the number of signal dimensions. It does
 *               not assume the additional two elements in the last dimension of s.
 *               {Type: long}
 *
 *         .Nr - number of sample points on the refined uniform grid used for
 *               interpolating nonuniformly sampled signals. This is an array with
 *               the number of elements equal to the number of signal dimensions.
 *               This field must be defined only if the third input argument t is
 *               provided (see below). If t is not provided, .Nr is ignored.
 *               {Type: long}
 *
 *         .Cp - compression parameter. If .Cp > 1, demodulation is performed by
 *               using signal compression (see equations (12)-(13) in the paper cited
 *               in the header of this file. This field is optional (the default is
 *               .Cp=1). {Type: double}
 *
 *         .Br - indicator of premature termination of the 'AP-Accelerated' algorithm
 *               when the value of the λ factor drops below one. If .Br=1, premature
 *               termination is assumed (default). Otherwise, the AP-A is not stopped
 *               premature even when λ decreases below 1. This field is required only
 *               if .Al='A'. It is optional (the default is .Br=1). {Type: int}
 *
 *         .im - array with the iteration numbers at which the modulator estimates 
 *               have to be saved for the output. the first element is the length of
 *               the array (excluding the first element itself). {Type: long}
 *
 *         .ie - array with the iteration numbers at which the infeasibility error,
 *               ϵ, values have to be saved for the output. The first element of .ie
 *               is the length of the array (excluding the first element itself).
 *               {Type: long}
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
 * None.
 */

/* R E T U R N   V A L U E
 *
 * [exitflag] - exit flag. Any positive value indicates an error (for numerical and
 *              textual definitions of the exit status, see l_ap_error_handling.c).
 */

/* N O N S T A N D A R D   F U N C T I O N S   U S E D
 *
 * None.
 */
    
    
    
    /* Definition and initialization of the local variables */
    
    int exitflag = 0;
    
    long i;
    
    long Ns = 1;
    
    long Ns3;
    
    
    
    
    /* Input validation */
    
    if (Par->Al != 'B' && Par->Al != 'A' && Par->Al != 'P')
    {
        f_apd_set_error(APD_ERR_ID_AL,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->D<1 || Par->D>3)
    {
        f_apd_set_error(APD_ERR_ID_D,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if ( (Par->D>0 && (Par->Fs[0] <= 0 || !isfinite(Par->Fs[0]))) || \
              (Par->D>1 && (Par->Fs[1] <= 0 || !isfinite(Par->Fs[1]))) || \
              (Par->D>2 && (Par->Fs[2] <= 0 || !isfinite(Par->Fs[2]))) )
    {
        f_apd_set_error(APD_ERR_ID_FS,__LINE__,APD_ERR_FILE); goto failed;}

    else if ( (Par->D>0 && (Par->Fc[0] <= 0 || !isfinite(Par->Fc[0]))) || \
              (Par->D>1 && (Par->Fc[1] <= 0 || !isfinite(Par->Fc[1]))) || \
              (Par->D>2 && (Par->Fc[2] <= 0 || !isfinite(Par->Fc[2]))) )
    {
        f_apd_set_error(APD_ERR_ID_FC,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if ( (Par->D>0 && Par->Fc[0] / Par->Fs[0] > 0.5) || \
              (Par->D>1 && Par->Fc[1] / Par->Fs[1] > 0.5) || \
              (Par->D>2 && Par->Fc[2] / Par->Fs[2] > 0.5) )
    {
        f_apd_set_error(APD_ERR_ID_FC2,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (!isfinite(Par->Et))
    {
        f_apd_set_error(APD_ERR_ID_ET,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->Ni <= 0)
    {
        f_apd_set_error(APD_ERR_ID_NI,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if ( (t == NULL && \
             ((Par->D>0 && Par->Ns[0] <= 1) || \
              (Par->D>1 && Par->Ns[1] <= 1) || \
              (Par->D>2 && Par->Ns[2] <= 1))) || (t != NULL && Par->Ns[0] <= 1) )
    {
        f_apd_set_error(APD_ERR_ID_NS,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if ( t != NULL && \
              ((Par->D>0 && Par->Nr[0] <= 1) || \
               (Par->D>1 && Par->Nr[1] <= 1) || \
               (Par->D>2 && Par->Nr[2] <= 1)) )
    {
       f_apd_set_error(APD_ERR_ID_NR,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->Cp<1 || !isfinite(Par->Cp))
    {
        f_apd_set_error(APD_ERR_ID_CP,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->Al == 'A' && Par->Br != 0 && Par->Br != 1)
    {
        f_apd_set_error(APD_ERR_ID_BR,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->ie == NULL || Par->ie[0] <= 0)
    {
        f_apd_set_error(APD_ERR_ID_IE,__LINE__,APD_ERR_FILE); goto failed;}
    
    else if (Par->im == NULL || Par->im[0] <= 0)
    {
        f_apd_set_error(APD_ERR_ID_IM,__LINE__,APD_ERR_FILE); goto failed;}
    
    else
    {
        for (i = 1; i <= Par->ie[0]; i++)
        {
            if (Par->ie[i] < 0 || (i>1 && Par->ie[i-1] >= Par->ie[i]))
            {
                f_apd_set_error(APD_ERR_ID_IE2,__LINE__,APD_ERR_FILE); goto failed;}
        }
        
        
        for (i = 1; i <= Par->im[0]; i++)
        {
            if (Par->im[i] < 0 || (i > 1 && Par->im[i-1] >= Par->im[i]))
            {
                f_apd_set_error(APD_ERR_ID_IM2,__LINE__,APD_ERR_FILE); goto failed;}
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
                f_apd_set_error(APD_ERR_ID_S,__LINE__,APD_ERR_FILE); goto failed;}
        }
    }
    
    
    
    if (exitflag == 0 && Ub != NULL)
    {
        
        for (i=0; i < Ns; i++)
        {
            if ( !isfinite(Ub[i]) || Ub[i] < fabs(s[i]))
            {
                f_apd_set_error(APD_ERR_ID_UB,__LINE__,APD_ERR_FILE); goto failed;}
        }
    }
    
    
    
    if (exitflag == 0 && t != NULL)
    {
        
        Ns3 = Ns * Par->D;
        
        for (i=0; i < Ns3; i++)
        {
            if ( !isfinite(t[i]) )
            {
                f_apd_set_error(APD_ERR_ID_T,__LINE__,APD_ERR_FILE); goto failed;}
        }
    }
    
    
    
    /* Output */

    finish:
    
        return exitflag;

    failed:

        f_apd_get_error (&exitflag, NULL, NULL, NULL);

        goto finish;
    
}
