
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



/* EXAMPLE 4
 *
 * In this example, a synthetic 1D amplitude-modulated signal built of a
 * random-spikes carrier and an LP-random modulator is generated and demodulated by
 * using the AP-Accelerated algorithm. Sample points of the the predefined and
 * inferred modulators and carriers are then written into a text file for further
 * analysis. Moreover, the intermediate estimates of the infeasibility and
 * demodulation errors are exported to a separate text file. This example illustrates
 * how to set the upper bound constraints on the modulator estimates and obtain
 * intermediate modulator estimates and infeasibility errors by using the function
 * 'f_apd_demodulation'. The results obtained also illustrate that imposing an upper
 * bound may reduce the rate of convergence of the AP algorithm in terms of
 * infeasibility error. However, that has no practical consequences on the
 * convergence in terms of the demodulation error.
 * 
 * Compile this program by using Option 2 described in the documentation.
 */



#include <stdlib.h>

#include <stdio.h>

#include <math.h>

#include "h_apd.h"



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

    long n = 1024;


    
    /* Time step */

    double dt = 10/(double)(n-1);


    
    /* Modulator (a low-pass-random signal) */

    double w[] = {1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787,\
                  -0.0108, -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, \
                  0.7651, -0.9884, -1.1668, 0.6584, -1.3693};

    double *m = (double*) calloc(n, sizeof(double));

    for (i=0; i<n; i++)
    {
        for (j=0; j<10; j++)

            m[i] = m[i] + w[2*j] * cos(2*M_PI*j*i/(double)n+w[1+2*j]); 

        m[i] = m[i] + 2.131185657756246;
        
        m[i] = m[i] / 7.926671964919291;
    }



    /* Carrier (a random-spikes signal) */

    long ci[] = {0, 38, 46, 27, 35, 30, 29, 32, 36, 37, 41, 38, 45, 32, 50, 27, 45, \
                 38, 42, 30, 32, 48, 53, 35, 45, 50, 51};

    double *c = (double*) calloc(n, sizeof(double));

    i = 0;
    
    for (j=0; j<27; j++)
    {
        i = i + ci[j];
        
        c[i] = 1;
    }


    
    /* Signal */

    double *s = (double*) malloc(n*sizeof(double));

    for (i=0; i<n; i++)

        s[i] = m[i] * c[i];



    /* Demodulation parameters */

    struct strAPD_Par Par;

    Par.Al = 'A';               // algorithm ('A'-> AP-Accelerated)
    
    Par.D = 1;                  // dimension of the signal
    
    Par.Fs[0] = 1/dt;           // sampling frequency
    
    Par.Fc[0] = 10*Par.Fs[0]/n; // cutoff frequency

    Par.Et = -1;                // infeasibility error tolerance (-1 -> completes all
                                // Par.Ni iterations)

    Par.Ni = 1e+3;              // maximum number of iterations
    
    Par.Ns[0] = n;              // number of sample points
    
    Par.Cp = 1;                 // compression parameter (=1->no compression)
    
    Par.Br = 1;                 // termination of AP-A upon numerical instability


    Par.im = (long*) malloc((Par.Ni+1)*sizeof(long)); // iterations to save modulator
                                                      // estimates at; here, we ask
    Par.im[0] = Par.Ni;                               // for estimates at every
                                                      // iteration
    for (i=1; i<=Par.im[0]; i++)
    
        Par.im[i] = i;


    Par.ie = (long*) malloc((Par.Ni+1)*sizeof(long)); // iterations to save
                                                      // infeasibility error
    Par.ie[0] = Par.Ni;                               // estimates at; here, we ask
                                                      // for estimates at every
    for (i=1; i<=Par.ie[0]; i++)                      // iteration
    
        Par.ie[i] = i;



    /* Upper bound on the modulator */

    double *Ub = NULL; // we assume no upper bound on the modulator



    /* Sampling coordinates of the input signal */
    
    double *t = NULL; // we use no interpolation



    /* Output arrays */

    double *out_m_wo_ub = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    double *out_e_wo_ub = (double*) malloc(Par.ie[0]*sizeof(double));
    
    double *out_E_wo_ub = (double*) calloc(Par.ie[0], sizeof(double));
    
    double *out_m_w_ub = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    double *out_e_w_ub = (double*) malloc(Par.ie[0]*sizeof(double));
    
    double *out_E_w_ub = (double*) calloc(Par.ie[0], sizeof(double));



    /* Demodulation (without the upper bound constraint) */

    exitflag = f_apd_demodulation (s, &Par, Ub, t, out_m_wo_ub, out_e_wo_ub, &iter);
    
    if (exitflag != 0)
        
        goto failed;
    
    else
    {
        printf(STR_NL "Demodulation without the upper bound constraint completed." \
                STR_NL);

        printf(STR_NL "The infeasibility error is %e. " STR_NL, \
                out_e_wo_ub[Par.ie[0]-1]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL STR_NL, iter);
    }
    
    
    
    /* Demodulation error (without the upper bound constraint) */
    
    double norm = 0;
        
    for (i=0; i<n; i++)

        norm = norm + m[i]*m[i];
    
    norm = sqrt(norm);
    
    
    for (j=0; j<Par.im[0]; j++)
    {
        
        for (i=0; i<n; i++)

            out_E_wo_ub[j] = out_E_wo_ub[j] + \
                    (m[i]-out_m_wo_ub[i+n*j]) * (m[i]-out_m_wo_ub[i+n*j]);
        
        out_E_wo_ub[j] = sqrt(out_E_wo_ub[j]) / norm;
    }
    
    
    
    /* Updated array of the modulator upper bound */

    Ub = (double*) malloc(n*sizeof(double));
    
    for (i=0; i<120; i++)

        Ub[i] = 0.59;
    
    for (i=120; i<160; i++)

        Ub[i] = 0.27;
    
    for (i=160; i<300; i++)

        Ub[i] = 0.59;
    
    for (i=300; i<400; i++)

        Ub[i] = 1;
    
    for (i=400; i<1023; i++)

        Ub[i] = 0.71;
    
    Ub[1023] = 0.311;
    
    
    
    /* Demodulation (with the upper bound constraint) */

    exitflag = f_apd_demodulation (s, &Par, Ub, t, out_m_w_ub, out_e_w_ub, &iter);
    
    
    if (exitflag != 0)
        
        goto failed;
    
    else
    {
        printf(STR_NL "Demodulation with the upper bound constraint completed." \
                STR_NL);

        printf(STR_NL "The infeasibility error is %e. " STR_NL, \
                out_e_w_ub[Par.ie[0]-1]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL, iter);
    }
    
    
    
    /* Demodulation error (with the upper bound constraint) */
    
    for (j=0; j<Par.im[0]; j++)
    {
        
        for (i=0; i<n; i++)

            out_E_w_ub[j] = out_E_w_ub[j] + \
                    (m[i]-out_m_w_ub[i+n*j]) * (m[i]-out_m_w_ub[i+n*j]);
        
        out_E_w_ub[j] = sqrt(out_E_w_ub[j]) / norm;
    }
    
    

    /* Output. Values of the signal as well as predefined and estimated modulators
     * are written to a text file as tab-delimited columns. */
    
    fid = fopen("./out_ex4_1.txt", "w");
    
    if (fid == NULL)
    {
        exitflag = -1;
        
        goto failed;           
    }
    
    else
    {
    
        fprintf(fid, "Sig.\t Mod.\t Mod. est. wo Ub\t Mod. est. w Ub\t Ub" STR_NL);

        for (i=0; i<n; i++)

            fprintf(fid, "%e\t%e\t%e\t%e\t%e" STR_NL, s[i], m[i], \
                    out_m_wo_ub[i+n*(Par.im[0]-1)], out_m_w_ub[i+n*(Par.im[0]-1)], \
                    Ub[i]);

        fclose(fid);
        
        if (exitflag != 0)
        {
            exitflag = -2;
            
            goto failed;
        }
    }
    
    
    fid = fopen("./out_ex4_2.txt", "w");
    
    if (fid == NULL)
    {
        exitflag = -1;
        
        goto failed;
    }
    
    else
    {
        fprintf(fid, "e wo Ub\t E wo Ub\t e w Ub\t E w Ub" STR_NL);

        for (i=0; i<Par.im[0]; i++)

            fprintf(fid, "%e\t%e\t%e\t%e" STR_NL, out_e_wo_ub[i], out_E_wo_ub[i], \
                    out_e_w_ub[i], out_E_w_ub[i]);

        fclose(fid);
        
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

        free(Ub);

        free(out_m_wo_ub);

        free(out_e_wo_ub);

        free(out_E_wo_ub);

        free(out_m_w_ub);

        free(out_e_w_ub);

        free(out_E_w_ub);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            fprintf (stderr, STR_NL "Error in example4.c: out_ex4_x.txt could not"\
                             " be opened for writing!" STR_NL STR_NL);
        
        else if (exitflag == -2)
            
            fprintf (stderr, STR_NL "Error in example4.c: stream to out_ex4_x.txt"\
                             " could not be closed!" STR_NL STR_NL);

        else
        
            f_apd_print_error(exitflag);
        
        goto finish;
    
}
