
%                       C O P Y R I G H T   N O T I C E
%
% Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
% All Rights Reserved. The underlying technology is protected by PCT Patent
% Application No. PCT/EP2021/054650.
%
% This file is part of the AP demodulation library, which is free software: you can
% redistribute it and/or modify it under the terms of the GNU General Public License
% as published by the Free Software Foundation in version 2.
%
% This program is distributed in the hope that it will be useful, but WITHOUT ANY
% WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
% PARTICULAR PURPOSE. See the GNU General Public License v2 for more details. You
% should have received a copy of the GNU General Public License v2 along with this
% program. If not, see https://www.gnu.org/licenses/.
%
% Contact the Technology Transfer Office, IST Austria, Am Campus 1,
% A-3400 Klosterneuburg, Austria, +43-(0)2243 9000, twist@ist.ac.at, for commercial
% licensing opportunities.
%
% All other inquiries should be directed to the author, Mantas Gabrielaitis,
% mgabriel@ist.ac.at



% EXAMPLE 3
%
% In this example, a nonuniformly sampled 1D amplitude-modulated signal built from a
% white-noise carrier and an LP-random modulator is generated and demodulated by
% using the AP-Projected algorithm. The inferred modulator is then compared with the
% predefined one. This example illustrates application of the functions
% 'f_ap_demodulation' and 'f_ap_demodulation_mex' for demodulating nonuniformly
% sampled signals.


close all

clear all



% Function type (m or mex) selected by the user

fType = 'mex'; % select between 'm' and 'mex'



% Paths to the m and mex functions that perform AP-demodulation

addpath ../library_m ../library_mex



% Number of sample points

n = 2^10;



% Time vector

rng(1);

t = [0; cumsum(0.5+2.0*rand(n,1))];

t = 10 * t(1:end-1) / t(end);



% Modulator (LP-random)

m = zeros(n,1);

w = [1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787, -0.0108, ...
   -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, 0.7651, -0.9884, -1.1668, ...
    0.6584, -1.3693, -0.4143, 0.6981, 0.9181, 2.9477, -0.3757, 0.8218, -0.5699, ...
   -0.5495, -0.0108, 0.0854, 1.0189, 1.7904, 0.7577, -0.8434, -0.7300, -0.8675, ...
   -0.6159, -0.3319, 1.4449, 0.6741];

for i=0:19
    
    m = m + w(1+2*i)*cos(2*pi*i*t/10+w(2*(i+1)));
        
end

m = m - min(m(:)) + 0.001;

m = m / max(m(:));



% Carrier (sinusoidal)

c = sin(2*pi*5*t);



% Signal

s = c .* m;



% Demodulation

if strcmpi(fType, 'm')

    fDemod = @(x,y,z) f_ap_demodulation (x, y, [], z);
    
elseif strcmpi(fType, 'mex')
    
    fDemod = @(x,y,z) f_ap_demodulation_mex (x, y, [], z);
    
end


Par.Al = 'P';

Par.Fs = 1 / mean(diff(t));

Par.Fc = 25 * Par.Fs / (n*8);

Par.Et = 10^-4;

Par.Ni = 3*10^4;

Par.Nr = n * 8;


[m_, e, Niter, tcpu] = fDemod (s, Par, t);



% Visualization: signal and modulator

figure('Name', 'Signal and Modulator')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 2 35 7]);


plot(t,abs(s),'o-','LineWidth',0.75,'MarkerSize',2)

hold on

plot(t,m,'k','LineWidth',2)

plot(t,m_,'r','LineWidth',1)

hold off


xlabel('t')

ylabel('x')

axis([2 4 0 1])

box('off')


legend({'$\bf{|s|}$','$\bf{m}$','$\hat{\bf{m}}$'}, 'Interpreter','latex', ...
    'Location',[0.40, 0.75, 0.1, 0.1], 'FontSize',11)

