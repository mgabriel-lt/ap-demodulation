
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



/* EXAMPLE 4
 *
 * In this example, a synthetic 1D amplitude-modulated signal built of a
 * random-spikes carrier and an LP-random modulator is generated and demodulated by
 * using the AP-Accelerated algorithm. Sample points of the the predefined and
 * inferred modulators and carriers are then written into a text file for further
 * analysis. Moreover, the intermediate estimates of the infeasibility and
 * demodulation errors are exported to a separate text file. This example illustrates
 * how to set the upper bound constraints on the modulator estimates and obtain
 * intermediate modulator estimates and infeasibility errors by using the functions
 * 'f_ap_demodulation'. The results obtained also illustrate that imposing an upper
 * bound may reduce the rate of convergence of the AP algorithm in terms of
 * infeasibility error. However, that has no practical consequences on the
 * convergence in terms of the demodulation error. */



#include <stdio.h>

#include <math.h>

#include "f_ap_demodulation.c"



#ifdef _WIN32
    
    #define STR_NL "\r"

#else

    #define STR_NL "\n"

#endif



int main(void)
{

    /* Declarations and initializations */
    
    int line;
    
    int exitflag = 0;
    
    
    long i, j, iter, n;
    
    double dt, norm;
    
    double *m=NULL, *c=NULL, *s=NULL, *Ub=NULL, *t=NULL;
    
    double *out_m_wo_ub=NULL, *out_e_wo_ub=NULL, *out_E_wo_ub=NULL, \
           *out_m_w_ub=NULL, *out_e_w_ub=NULL, *out_E_w_ub=NULL;
    
    double w[] = {1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787,\
                  -0.0108, -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, \
                  0.7651, -0.9884, -1.1668, 0.6584, -1.3693};
                  
    long ci[] = {0, 38, 46, 27, 35, 30, 29, 32, 36, 37, 41, 38, 45, 32, 50, 27, 45, \
                 38, 42, 30, 32, 48, 53, 35, 45, 50, 51};
                  
    struct s_ParamsAP Par;
    
    Par.Fs = NULL; Par.Fc = NULL; Par.Ns = NULL; Par.im = NULL; Par.ie = NULL;
    
    FILE *fid = NULL;


    
    /* Number of sample points */

    n = 1024;


    
    /* Time step */

    dt = 10/(double)(n-1);


    
    /* Modulator (LP-random) */

    m = (double*) calloc(n, sizeof(double));
    
    if (m == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)
    {
        for (j=0; j<10; j++)

            m[i] = m[i] + w[2*j] * cos(2*M_PI*j*i/(double)n+w[1+2*j]); 

        m[i] = m[i] + 2.131185657756246;
        
        m[i] = m[i] / 7.926671964919291;
    }



    /* Carrier (random-spikes) */

    c = (double*) calloc(n, sizeof(double));
    
    if (c == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    i = 0;
    
    for (j=0; j<27; j++)
    {
        i = i + ci[j];
        
        c[i] = 1;
    }


    
    /* Signal */

    s = (double*) malloc(n*sizeof(double));
    
    if (s == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    

    for (i=0; i<n; i++)

        s[i] = m[i] * c[i];



    /* Demodulation parameters */

    Par.Al = 'A';
    
    Par.D = 1;

    
    Par.Fs = (double*) malloc(1*sizeof(double));
    
    if (Par.Fs == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fs[0] = 1/dt;
    

    Par.Fc = (double*) malloc(1*sizeof(double));
    
    if (Par.Fc == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    Par.Fc[0] = 10 * Par.Fs[0] / n;


    Par.Et = -1;

    Par.Ni = 1e+3;
    
    
    Par.Ns = (long*) malloc(1*sizeof(long));
    
    if (Par.Ns == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.Ns[0] = n;
    

    Par.Cp = 1;
    
    
    Par.Br = 1;


    Par.im = (long*) malloc((Par.Ni+1)*sizeof(long));
    
    if (Par.im == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.im[0] = Par.Ni;
    
    for (i=1; i<=Par.im[0]; i++)
    
        Par.im[i] = i; /* we ask for m estimates at every iteration */


    Par.ie = (long*) malloc((Par.Ni+1)*sizeof(long));
    
    if (Par.ie == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}

    Par.ie[0] = Par.Ni;
    
    for (i=1; i<=Par.ie[0]; i++)
    
        Par.ie[i] = i; /* we ask for e estimates at every iteration */



    /* The arrays of modulator upper bound and signal sampling coordinates */

    Ub = NULL; /* we assume no upper bound on the modulator */
    
    t = NULL; /* we use no interpolation */



    /* Output arrays */

    out_m_wo_ub = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_m_wo_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_e_wo_ub = (double*) malloc(Par.ie[0]*sizeof(double));
    
    if (out_e_wo_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_E_wo_ub = (double*) calloc(Par.ie[0], sizeof(double));
    
    if (out_E_wo_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_m_w_ub = (double*) malloc(Par.im[0]*n*sizeof(double));
    
    if (out_m_w_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_e_w_ub = (double*) malloc(Par.ie[0]*sizeof(double));
    
    if (out_e_w_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
    out_E_w_ub = (double*) calloc(Par.ie[0], sizeof(double));
    
    if (out_E_w_ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}



    /* Demodulation (without the upper bound constraint) */

    exitflag = f_ap_demodulation (s, &Par, Ub, t, out_m_wo_ub, out_e_wo_ub, &iter);
    
    if (exitflag != 0)
        
        goto finish;
    
    else
    {
        printf(STR_NL "Demodulation without the upper bound constraint completed." \
                STR_NL);

        printf(STR_NL "The infeasibility error is %e. " STR_NL, \
                out_e_wo_ub[Par.ie[0]]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL STR_NL, iter);
    }
    
    
    
    /* Demodulation error (without the upper bound constraint) */
    
    norm = 0;
        
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
    
    if (Ub == NULL) {line=__LINE__-2; exitflag=-1; goto failed;}
    
    
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

    exitflag = f_ap_demodulation (s, &Par, Ub, t, out_m_w_ub, out_e_w_ub, &iter);
    
    
    if (exitflag != 0)
        
        goto finish;
    
    else
    {
        printf(STR_NL "Demodulation with the upper bound constraint completed." \
                STR_NL);

        printf(STR_NL "The infeasibility error is %e. " STR_NL, \
                out_e_w_ub[Par.ie[0]]);

        printf(STR_NL "The number of used iterations is %ld" STR_NL STR_NL, iter);
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
        line = __LINE__-4;
        
        exitflag = -3;
        
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
            line = __LINE__-4;
            
            exitflag = -4;
            
            goto failed;
        }
    }
    
    
    fid = fopen("./out_ex4_2.txt", "w");
    
    if (fid == NULL)
    {
        line = __LINE__-4;
        
        exitflag = -3;
        
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
            
            fprintf (stderr, STR_NL "Error (line %d in %s): Out of memmory!" STR_NL,\
                    line, __FUNCTION__);
        
        else if (exitflag == -3)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): out_ex4_x.txt could not"\
                             " be opened for writing!" STR_NL, line, __FUNCTION__);
        
        if (exitflag == -4)
            
            fprintf (stderr, STR_NL "Error (line %d in %s): stream to out_ex4_x.txt"\
                             " could not be closed!" STR_NL, line, __FUNCTION__);
        
        goto finish;
    
    
       
    
    
    return 0;
    
}

