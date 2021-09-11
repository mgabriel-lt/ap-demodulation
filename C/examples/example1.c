
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


/* EXAMPLE 1
 *
 * In this example, a synthetic 1D amplitude-modulated signal built from a harmonic
 * carrier and a sinusoidal modulator is generated and demodulated by using the
 * AP-Basic algorithm. Sample points of the the predefined and inferred modulators
 * and carriers are then written into a text file for further analysis. This example
 * illustrates application of the function 'f_ap_demodulation' in its simplest form,
 * when the signal is sampled uniformly, and when only the final estimates of the
 * modulator and infeasibility error are required. */



#include <stdio.h>

#include <math.h>

#include "f_ap_demodulation.c"



#ifdef _WIN32
    
    #define STR_NL "\r\n"

#else

    #define STR_NL "\n"

#endif



int main(void)
{

    /* Declarations and initializations */
    
    int line;
    
    int exitflag = 0;
    
    
    long i, j, iter, n;
    
    double dt;
    
    double *m=NULL, *c=NULL, *s=NULL, *Ub=NULL, *t=NULL;
    
    double *out_m=NULL, *out_c=NULL, *out_e=NULL;
    
    double w[] = {0.4170, 0.7203, 0.0001, 0.3023, 0.1468, 0.0923, 0.1863, 0.3456, \
                  0.3968, 0.5388, 0.4192, 0.6852, 0.2045, 0.8781, 0.0274, 0.6705, \
                  0.4173, 0.5587, 0.1404, 0.1981, 0.8007, 0.9683, 0.3134, 0.6923, \
                  0.8764, 0.8946, 0.0850, 0.0391, 0.1698, 0.8781, 0.0983, 0.4211, \
                  0.9579, 0.5332, 0.6919, 0.3155, 0.6865, 0.8346, 0.0183, 0.7501};
                  
    struct s_ParamsAP Par;
    
    Par.Fs = NULL; Par.Fc = NULL; Par.Ns = NULL; Par.im = NULL; Par.ie = NULL;
    
    FILE *fid = NULL;


    
    /* Number of sample points */

    n = 16384;


    
    /* Time step */

    dt = (double)25/(n-1);


    
    /* Modulator (nonnegative sinusoidal) */

    m = (double*) malloc(n*sizeof(double));
    
    if (m == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)

        m[i] = (1.01+cos(2*M_PI*i*dt)) / 2.01; 



    /* Carrier (harmonic) */

    c = (double*) calloc(n, sizeof(double));
    
    if (c == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

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

    s = (double*) malloc(n*sizeof(double));
    
    if (s == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    for (i=0; i<n; i++)

        s[i] = m[i] * c[i];



    /* Demodulation parameters */

    Par.Al = 'B';
    
    Par.D = 1;

    
    Par.Fs = (double*) malloc(1*sizeof(double));
    
    if (Par.Fs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fs[0] = 1/dt;
    

    Par.Fc = (double*) malloc(1*sizeof(double));
    
    if (Par.Fc == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fc[0] = 1.5;
    

    Par.Et = 1e-5;

    Par.Ni = 1e+3;
    
    
    Par.Ns = (long*) malloc(1*sizeof(long));
    
    if (Par.Ns == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.Ns[0] = n;
    

    Par.Cp = 1;


    Par.im = (long*) malloc(2*sizeof(long));
    
    if (Par.im == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.im[0] = 1;
    
    Par.im[1] = Par.Ni; /* we ask for m estimate at the last iteration */


    Par.ie = (long*) malloc(2*sizeof(long));
    
    if (Par.ie == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.ie[0] = 1;
    
    Par.ie[1] = Par.Ni; /* we ask for e estimate at the last iteration */



    /* The arrays of modulator upper bound and signal sampling coordinates */

    Ub = NULL; /* we assume no upper bound on the modulator */

    t = NULL; /* we use no interpolation */



    /* Output arrays */

    out_m = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_m == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_c = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_c == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_e = (double*) malloc(Par.ie[0]*sizeof(double));
    
    if (out_e == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}



    /* Demodulation */

    exitflag = f_ap_demodulation (s, &Par, Ub, t, out_m, out_e, &iter);
    
    if (exitflag != 0)
        
        goto finish;
    
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
        line = __LINE__-4;
        
        exitflag = -3;
        
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
            line = __LINE__-4;
            
            exitflag = -4;
            
            goto failed;
        }
    }
    
    
    printf(STR_NL "Results saved." STR_NL);
    
    
    
    /* Output & Memory deallocation */
    
    goto finish;
    
    finish:
        
        free(m);
    
        free(c);

        free(s);

        free(Par.Fs);

        free(Par.Fc);

        free(Par.Ns);

        free(Par.im);

        free(Par.ie);

        free(out_m);

        free(out_c);

        free(out_e);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): Out of memmory!" STR_NL,\
                    line, __FUNCTION__);
        
        else if (exitflag == -3)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): out_ex1.txt could not "\
                             "be opened for writing!" STR_NL, line, __FUNCTION__);
        
        if (exitflag == -4)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): stream to out_ex1.txt "\
                             "could not be closed!" STR_NL, line, __FUNCTION__);
        
        goto finish;
        
    
}

