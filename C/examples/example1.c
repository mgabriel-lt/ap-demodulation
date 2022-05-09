
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



/* EXAMPLE 1
 *
 * In this example, a synthetic 1D amplitude-modulated signal built from a harmonic
 * carrier and a sinusoidal modulator is generated and demodulated by using the
 * AP-Basic algorithm. Sample points of the the predefined and inferred modulators
 * and carriers are then written into a text file for further analysis. This example
 * illustrates application of the function 'f_apd_demodulation' in its simplest form,
 * when the signal is sampled uniformly, and when only the final estimates of the
 * modulator and infeasibility error are required.
 *
 * Compile this program by using Option 1 described in the documentation.
 */


#include <stdlib.h>

#include <stdio.h>

#include <math.h>

#include "f_apd_demodulation.c"



#ifdef _WIN32
    
    #define STR_NL "\r"

#else

    #define STR_NL "\n"

#endif



int main(void)
{

    /* File pointer */

    FILE *fid;
    


    /* Exit flag */

    int exitflag = 0;



    /* Sets f_apd_demodulation to return control to the calling f-tion upon error */

    f_apd_set_errexit(0);

    

    /* Iteration variables */
    
    long i;
    
    long j;
    
    long iter;


    
    /* Number of sample points */

    long n = 16384;


    
    /* Time step for the time vector */

    double dt = (double)25/(n-1);


    
    /* Modulator (a nonnegative cosine) */

    double *m = (double*) malloc(n*sizeof(double));

    for (i=0; i<n; i++)

        m[i] = (1.01+cos(2*M_PI*i*dt)) / 2.01; 



    /* Carrier (a harmonic signal) */

    double w[] = {0.4170, 0.7203, 0.0001, 0.3023, 0.1468, 0.0923, 0.1863, 0.3456, \
                  0.3968, 0.5388, 0.4192, 0.6852, 0.2045, 0.8781, 0.0274, 0.6705, \
                  0.4173, 0.5587, 0.1404, 0.1981, 0.8007, 0.9683, 0.3134, 0.6923, \
                  0.8764, 0.8946, 0.0850, 0.0391, 0.1698, 0.8781, 0.0983, 0.4211, \
                  0.9579, 0.5332, 0.6919, 0.3155, 0.6865, 0.8346, 0.0183, 0.7501};

    double *c = (double*) calloc(n, sizeof(double));

    for (i=0; i<n; i++)
    {
        for (j=0; j<10; j++)
        {
            c[i] = c[i] + w[4*j] * cos(2*M_PI*(256*(j+1)*i/(double)n+w[1+4*j]));

            c[i] = c[i] + 0.01*w[2+4*j] * \
                    cos(2*M_PI*((256*(j+1)+128)*i/(double)n+w[3+4*j]));
        }

        c[i] = c[i] / 2.628456776936774;
    }


    
    /* Signal */

    double *s = (double*) malloc(n*sizeof(double));
    
    for (i=0; i<n; i++)

        s[i] = m[i] * c[i];



    /* Demodulation parameters */

    struct strAPD_Par Par;

    Par.Al = 'B';       // algorithm ('B'-> AP-Basic)
    
    Par.D = 1;          // dimension of the signal
    
    Par.Fs[0] = 1/dt;   // sampling frequency
    
    Par.Fc[0] = 1.5;    // cutoff frequency

    Par.Et = 1e-5;      // infeasibility error tolerance

    Par.Ni = 1e+3;      // maximum number of iterations

    Par.Ns[0] = n;      // number of sample points
    
    Par.Cp = 1;         // compression parameter (=1 -> no compression)


    Par.im = (long*) malloc(2*sizeof(long)); // iterations to save modulator
                                             // estimates at; here, we ask for
    Par.im[0] = 1;                           // estimate at the last iteration only

    Par.im[1] = Par.Ni;


    Par.ie = (long*) malloc(2*sizeof(long)); // iterations to save infeasibility
                                             // error estimates at; here, we ask for
    Par.ie[0] = 1;                           // estimate at the last iteration only
    
    Par.ie[1] = Par.Ni;



    /* Upper bound on the modulator */

    double *Ub = NULL; // we assume no upper bound on the modulator



    /* Sampling coordinates of the input signal */
    
    double *t = NULL; // we use no interpolation



    /* Output arrays */

    double *out_m = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    double *out_c = (double*) malloc(Par.im[0]*n*sizeof(double));
        
    double *out_e = (double*) malloc(Par.ie[0]*sizeof(double));



    /* Demodulation */

    exitflag = f_apd_demodulation (s, &Par, Ub, t, out_m, out_e, &iter);
    
    if (exitflag != 0)
        
        goto failed;
    
    else
    {
        printf(STR_NL "Demodulation completed." STR_NL);
    
        printf(STR_NL "The infeasibility error is %e." STR_NL, out_e[0]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL, iter);
    }
    
    
    
    /* Carrier estimate */
    
    for (i=0; i<n; i++)

        out_c[i] = s[i] / out_m[i];
    
    
    
    /* Output. Values of the predefined and estimated modulators and carriers are
     * written to a text file as tab-delimited columns. */

    fid = fopen("./out_ex1.txt", "w");
    
    if (fid == NULL)
    {
        exitflag = -1;
        
        goto failed;
    }
    
    else
    {
        fprintf(fid, "Mod.\t Carr.\t Mod. est.\t Carr. est." STR_NL);

        for (i=0; i<n; i++)

            fprintf(fid, "%e\t%e\t%e\t%e" STR_NL, m[i], c[i], out_m[i], out_c[i]);

        exitflag = fclose(fid);
        
        if (exitflag != 0)
        {
            exitflag = -2;
            
            goto failed;
        }
    }
    
    
    printf(STR_NL "Results saved." STR_NL STR_NL);
    
    
    
    /* Output & Memory deallocation */
    
    finish:
        
        free(m);
    
        free(c);

        free(s);

        free(Par.im);

        free(Par.ie);

        free(out_m);

        free(out_c);

        free(out_e);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            fprintf (stderr, STR_NL "Error in example1.c: out_ex1.txt could not "\
                             "be opened for writing!" STR_NL STR_NL);
        
        else if (exitflag == -2)
            
            fprintf (stderr, STR_NL "Error in example1.c: stream to out_ex1.txt "\
                             "could not be closed!" STR_NL STR_NL);

        else
        
            f_apd_print_error(exitflag);
        
        goto finish;
        
}
