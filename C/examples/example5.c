
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


/* EXAMPLE 5
 *
 * In this example, a synthetic 1D amplitude-modulated signal built from a
 * regular-spikes carrier and two LP-random modulators is generated and demodulated
 * by using the AP-Basic algorithm. The two modulators are used to shape the lower
 * and upper envelopes of the signal. Sample points of the the predefined and
 * inferred modulators and carriers are then written into a text file for further
 * analysis. This example illustrates how to infer the upper and lower envelopes of a
 * signal by using the functions 'f_ap_demodulation'. */



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
    
    double dt, smin, smmin;
    
    double *m1=NULL, *m2=NULL, *c1=NULL, *c2=NULL, *s=NULL, *Ub=NULL, *t=NULL;
    
    double *out_m1=NULL, *out_m2=NULL, *out_e1=NULL, *out_e2=NULL;
    
    double w[] = {1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787,\
                  -0.0108, -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, \
                  0.7651, -0.9884, -1.1668, 0.6584, -1.3693, 0.7608, 0.7810, 0.9041,\
                  0.2338, 0.1767, 0.3911, 0.3206, 0.8155, 0.6135, 0.7600};
                  
    struct s_ParamsAP Par;
    
    Par.Fs = NULL; Par.Fc = NULL; Par.Ns = NULL; Par.im = NULL; Par.ie = NULL;
    
    FILE *fid = NULL;


    
    /* Number of sample points */

    n = 1024;


    
    /* Time step */

    dt = (double)10/(n-1);


    
    /* Modulators (LP-random) */

    m1 = (double*) calloc(n, sizeof(double));
    
    if (m1 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)
    {
        for (j=0; j<10; j++)

            m1[i] = m1[i] + w[2*j] * cos(2*M_PI*j*i/(double)n+w[1+2*j]); 

        m1[i] = m1[i] + 2.130185657756246;
        
        m1[i] = m1[i] / 7.925671964919291;
    }
    
    
    m2 = (double*) calloc(n, sizeof(double));
    
    if (m2 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)
    {
        for (j=14; j>=0; j--)

            m2[i] = m2[i] + w[2*j] * cos(2*M_PI*j*(n-i)/(double)n+w[1+2*j]); 

        m2[i] = m2[i] + 2.581418146550079;
        
        m2[i] = m2[i] / 8.690964954126397 / 2;
    }



    /* Carrier (regular spikes) */

    c1 = (double*) calloc(n, sizeof(double));
    
    if (c1 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    c2 = (double*) calloc(n, sizeof(double));
    
    if (c2 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=4-1; i<n; i+=32)
    {
        c1[i] = 1;
        
        c2[i-2] = -1;
        
        c2[i+2] = -1;
    }


    
    /* Signal */

    s = (double*) malloc(n*sizeof(double));
    
    if (s == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)

        s[i] = m1[i] * c1[i] + m2[i] * c2[i];
    
    
    
    /* Minimum elements of the signal and the negative signal */
    
    smin = s[0];
    
    smmin = -s[0];
    
    for (i=1; i<n; i++)
    {
        if (s[i] < smin)
            
            smin = s[i];
        
        else if (-s[i] < smmin)
            
            smmin = -s[i];
    }



    /* Demodulation parameters */

    Par.Al = 'B';
    
    Par.D = 1;

    
    Par.Fs = (double*) malloc(1*sizeof(double));
    
    if (Par.Fs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fs[0] = 1/dt;
    

    Par.Fc = (double*) malloc(1*sizeof(double));
    
    if (Par.Fc == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fc[0] = 15 * Par.Fs[0] / n;
    

    Par.Et = 1e-6;

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

    out_m1 = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_m1 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_m2 = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_m2 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_e1 = (double*) malloc(Par.ie[0]*sizeof(double));
    
    if (out_e1 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_e2 = (double*) malloc(Par.ie[0]*sizeof(double));
    
    if (out_e2 == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}



    /* Demodulation (lower envelope) */

    for (i=0; i<n; i++)
        
        s[i] = -s[i] - smmin;
    
    
    exitflag = f_ap_demodulation (s, &Par, Ub, t, out_m2, out_e2, &iter);
    
    if (exitflag != 0)
        
        goto finish;
    
    else
    {
        printf(STR_NL "Demodulation for estimating the lower envelope completed." \
                STR_NL);
    
        printf(STR_NL "The infeasibility error is %e. " STR_NL, out_e1[0]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL STR_NL, iter);
    }
    
    
    for (i=0; i<n; i++)
    {
        out_m2[i] = -out_m2[i] - smmin;
        
        s[i] = -s[i] - smmin - smin;
    }
    
    
    /* Demodulation (upper envelope) */
    
    exitflag = f_ap_demodulation (s, &Par, Ub, t, out_m1, out_e1, &iter);
    
    if (exitflag != 0)
        
        goto finish;
    
    else
    {
        printf(STR_NL "Demodulation for estimating the upper envelope completed." \
                STR_NL);
    
        printf(STR_NL "The infeasibility error is %e. " STR_NL, out_e2[0]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL STR_NL, iter);
    }
    
    
    for (i=0; i<n; i++)
    {
        out_m1[i] = out_m1[i] + smin;
        
        s[i] = s[i] + smin;
    }
    
    
    
    /* Output. Values of the signal as well as predefined and estimated envelopes
     * are written to a text file as tab-delimited columns. */
    
    fid = fopen("./out_ex5.txt", "w");
    
    if (fid == NULL)
    {
        line = __LINE__-4;
        
        exitflag = -3;
        
        goto failed;
    }
    
    else
    {
    
        fprintf(fid, "Sig.\t Mod. u.\t Mod. l.\t Mod. u. est.\t Mod. l. est." STR_NL);

        for (i=0; i<n; i++)

            fprintf(fid, "%e\t%e\t%e\t%e\t%e" STR_NL, s[i], m1[i], -m2[i], out_m1[i], \
                    out_m2[i]);

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
        
        free(m1);
    
        free(m2);

        free(c1);

        free(c2);

        free(s);

        free(Par.Fs);

        free(Par.Fc);

        free(Par.Ns);

        free(Par.im);

        free(Par.ie);

        free(out_m1);

        free(out_m2);

        free(out_e1);

        free(out_e2);
        
        return exitflag;
                
    failed:
        
        if (exitflag == -1)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): Out of memmory!" STR_NL,\
                    line, __FUNCTION__);
        
        else if (exitflag == -3)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): out_ex5.txt could not "\
                             "be opened for writing!" STR_NL, line, __FUNCTION__);
        
        if (exitflag == -4)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): stream to out_ex5.txt "\
                             "could not be closed!" STR_NL, line, __FUNCTION__);
        
        goto finish; 
    
        
}

