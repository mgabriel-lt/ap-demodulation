
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
% See https://github.com/mgabriel-lt/ap-demodulation for the latest version of the
% code and user-friendly explanations on the working principle, domains of
% application, and advice on the usage of different AP Demodulation algorithms in
% practice.



% EXAMPLE 5
%
% In this example, a synthetic 1D amplitude-modulated signal built from a
% regular-spikes carrier and two LP-random modulators is generated and demodulated by
% using the AP-Basic algorithm. The two modulators are used to shape the lower and
% upper envelopes of the signal. The inferred envelopes are compared with the
% predefined ones. This example illustrates how to infer the upper and lower
% envelopes of a signal by using the functions 'f_apd_demodulation' and
% 'f_apd_demodulation_mex'.


close all

clear all %#ok<CLALL>



% Function type (m or mex) selected by the user

fType = 'm'; % select between 'm' and 'mex'



% Paths to the m and mex functions that perform AP-demodulation

addpath ../libm ../libmex



% Number of sample points

n = 2^10;



% Time vector

T = linspace(0,10,n)';



% Modulators corresponding to upper & lower envelopes (low-pass-random signals)

m1 = zeros(n,1);

w = [1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787, -0.0108, ...
   -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, 0.7651, -0.9884, -1.1668, ...
    0.6584, -1.3693, 0.7608, 0.7810, 0.9041, 0.2338, 0.1767, 0.3911, 0.3206, ...
    0.8155, 0.6135, 0.7600];

for i=0:9
    
    m1 = m1 + w(1+2*i)*cos(2*pi*i*(0:n-1)'/n+w(2*(i+1)));
        
end

m1 = m1 - min(m1(:));

m1 = m1 / max(m1(:));


m2 = zeros(n,1);

for i=14:-1:0
    
    m2 = m2 + w(1+2*i)*cos(2*pi*i*(0:n-1)'/n+w(2*(i+1)));
        
end

m2 = m2(end:-1:1) - min(m2(:));

m2 = m2 / max(m2(:)) / 2;



% Carrier (a regular spike-train signal)

ci = 4:32:n;

c1 = zeros(n,1);

c2 = zeros(n,1);

c1(ci) = 1;

c2(ci-2) = -1;

c2(ci+2) = -1;

c = c1 + c2;



% Signal

s = c1 .* m1 + c2 .* m2;



% Handle to the chosen demodulation function (m-file or mex)

if strcmpi(fType, 'm')

    fDemod = @(x,y) f_apd_demodulation (x, y);
    
elseif strcmpi(fType, 'mex')
    
    fDemod = @(x,y) f_apd_demodulation_mex (x, y);
    
end



% Demodulation control parameters

Par.Al = 'B'; % algorithm

Par.Fs = 1 / (T(2)-T(1)); % sampling frequency

Par.Fc = 15 * Par.Fs / n; % cutoff frequency

Par.Et = 10^-6; % infeasibility error tolerance

Par.Ni = 10^3; % maximum iteration number



% Estimating the upper envelope

smin = min(s);

[m1_, e1] = fDemod (s-smin, Par);

m1_ = m1_ + smin;



% Estimating the lower envelope

smmin = min(-s);

[m2_, e2] = fDemod (-s-smmin, Par);

m2_ = -m2_ - smmin;



% Visualization: signal and envelopes

figure('Name', 'Signal and Envelopes')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 5 35 7]);


pl(1) = plot(T,s,'LineWidth',0.75,'MarkerSize',2);

hold on

pl(2) = plot(T,m1,'k','LineWidth',2);

pl(3) = plot(T,-m2,'k','LineWidth',2);

pl(4) = plot(T,m1_,'r--','LineWidth',2);

pl(5) = plot(T,m2_,'--','Color',[0.18, 0.74, 0.19],'LineWidth',1);

hold off


xlabel('t')

ylabel('x')

axis([0 10 -0.5 1])

box('off')


legend([pl(1:2) pl(4:5)], {'$\bf{|s|}$','$\bf{m}$ upper and lower',...
    '$\hat{\bf{m}}$ upper','$\hat{\bf{m}}$ lower'}, 'Interpreter','latex', ...
    'Location',[0.53, 0.80, 0.1, 0.1], 'FontSize',11)

