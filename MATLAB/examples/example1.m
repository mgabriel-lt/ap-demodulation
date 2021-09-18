
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



% EXAMPLE 1
%
% In this example, a synthetic 1D amplitude-modulated signal built from a harmonic
% carrier and a sinusoidal modulator is generated and demodulated by using the
% AP-Basic algorithm. The inferred modulator and carriers are then compared with the
% predefined ones. This example illustrates application of the functions
% 'f_ap_demodulation' and 'f_ap_demodulation_mex' in its simplest format, when the
% signal is sampled uniformly, and when only the final estimates of the modulator and
% infeasibility error are required.


close all

clear all



% Function type (m or mex) selected by the user

fType = 'm'; % select between 'm' and 'mex'



% Paths to the m and mex functions that perform AP-demodulation

addpath ../library_m ../library_mex



% Number of sample points

n = 2^14;



% Time vector

T = linspace(0,25,n)'; 



% Modulator (nonnegative sinusoidal)

m = (1.01+cos(2*pi*T)) / 2.01; 



% Carrier (harmonic)

c = zeros(n,1);

w = [0.4170, 0.7203, 0.0001, 0.3023, 0.1468, 0.0923, 0.1863, 0.3456, 0.3968, ...
    0.5388, 0.4192, 0.6852, 0.2045, 0.8781, 0.0274, 0.6705, 0.4173, 0.5587, ...
    0.1404, 0.1981, 0.8007, 0.9683, 0.3134, 0.6923, 0.8764, 0.8946, 0.0850, ...
    0.0391, 0.1698, 0.8781, 0.0983, 0.4211, 0.9579, 0.5332, 0.6919, 0.3155, ...
    0.6865, 0.8346, 0.0183, 0.7501];

for i=1:10
    
    c = c + w(1+4*(i-1)) * cos(2*pi* (256*i*(0:n-1)'/n + w(2+4*(i-1))));
    
    c = c + 0.01*w(3+4*(i-1)) * cos(2*pi* ((256*i+128)*(0:n-1)'/n + w(4*i)));
    
end

c = c / max(abs(c));



% Signal

s = m .* c;



% Demodulation

if strcmpi(fType, 'm')

    fDemod = @(x,y) f_ap_demodulation (x, y);
    
elseif strcmpi(fType, 'mex')
    
    fDemod = @(x,y) f_ap_demodulation_mex (x, y);
    
end


Par.Al = 'B';

Par.Fs = 1/(T(2)-T(1));

Par.Fc = 1.5;

Par.Et = 10^-5;

Par.Ni = 10^3;


[m_, e, iter, tcpu] = fDemod (s, Par);



% Visualization: signal and modulator

figure('Name', 'Signal and Modulator')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 10 35 7]);


plot(T,abs(s),'LineWidth',0.75)

hold on

plot(T,m,'k','LineWidth',2)

plot(T,m_,'r','LineWidth',1)

hold off


xlabel('t')

ylabel('x')

axis([3.5 8.5 0 1])

box('off')


legend({'$\bf{|s|}$','$\bf{m}$','$\hat{\bf{m}}$'}, 'Interpreter','latex', ...
    'Location',[0.545, 0.75, 0.1, 0.1], 'FontSize',11)



% Visualization: carrier

figure('Name', 'Carrier')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 1 35 7]);


plot(T,c,'k','LineWidth',2)

hold on

plot(T,s./m_,'Color',[0 0.7 0],'LineWidth',0.75)

hold off


xlabel('t')

ylabel('c')

axis([3.5 4.0 -1 1])

box('off')


legend({'$\bf{c}$','$\hat{\bf{c}}$'}, 'Interpreter','latex', ...
    'Location',[0.55, 0.80, 0.1, 0.1], 'FontSize',11)

