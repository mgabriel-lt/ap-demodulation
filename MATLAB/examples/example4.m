
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



% EXAMPLE 4
%
% In this example, a synthetic 1D amplitude-modulated signal built of a random-spikes
% carrier and an LP-random modulator is generated and demodulated by using the
% AP-Accelerated algorithm. Demodulation is performed with and without (default)
% upper bound constraints. The inferred modulators are then compared with the
% predefined one. Moreover, the intermediate estimates of the modulators and
% infeasibility errors are saved and then visualized. This example illustrates how to
% set the upper bound constraints on the modulator estimates and obtain intermediate
% modulator estimates and infeasibility errors by using functions 'f_ap_demodulation'
% and 'f_ap_demodulation_mex'. The results obtained also illustrate that imposing an
% upper bound may reduce the rate of convergence of the AP algorithm in terms of
% infeasibility error. However, that has no practical consequences on the convergence
% in terms of the demodulation error.


close all

clear all



% Function type (m or mex) selected by the user

fType = 'm'; % select between 'm' and 'mex'



% Paths to the m and mex functions that perform AP-demodulation

addpath ../library_m ../library_mex


% Number of sample points

n = 2^10;


% Time vector

T = linspace(0,10,n)';



% Modulator (LP-random)

m = zeros(n,1);

w = [1.5648, 0.5312, 0.1413, 0.7588, -0.8616, -0.3586, 0.9106, -0.1787, -0.0108, ...
   -0.0989, -0.3559, -0.4015, 0.2917, -0.3458, -1.1990, 0.7651, -0.9884, -1.1668, ...
    0.6584, -1.3693];

for i=0:9
    
    m = m + w(1+2*i)*cos(2*pi*i*(0:n-1)'/n+w(2*(i+1)));
        
end

m = m - min(m(:)) + 0.001;

m = m / max(m(:));



% Carrier (random-spikes)

ci = [1, 38, 46, 27, 35, 30, 29, 32, 36, 37, 41, 38, 45, 32, 50, 27, 45, 38, 42, ...
    30, 32, 48, 53, 35, 45, 50, 51];

ci = cumsum(ci);

c = zeros(n,1);

c(ci) = 1;



% Signal

s = c .* m;



% Demodulation (without UB)

if strcmpi(fType, 'm')

    fDemod = @(x,y,z) f_ap_demodulation (x, y, z);
    
elseif strcmpi(fType, 'mex')
    
    fDemod = @(x,y,z) f_ap_demodulation_mex (x, y, z);
    
end


Par.Al = 'A';

Par.Fs = 1 / (T(2)-T(1));

Par.Fc = 10 * Par.Fs / n;

Par.Et = -1;

Par.Ni = 10^3;

Par.im = 1:10^3;

Par.ie = 1:10^3;


[m_1, e1] = fDemod (s, Par, []);



% Demodulation (with UB)

Ub = [0.59*ones(120,1); 0.27*ones(40,1); 0.59*ones(140,1); 1*ones(100,1); ...
    0.71*ones(623,1); 0.311];


[m_2, e2] = fDemod (s, Par, Ub);



% Demodulation errors

E1 = zeros(numel(Par.im),1);

E2 = zeros(numel(Par.im),1);

for i=1:numel(Par.im)
    
    E1(i) = norm(m_1(:,i)-m) / norm(m);
    
    E2(i) = norm(m_2(:,i)-m) / norm(m);
        
end



% Visualization: signal and modulator

figure('Name', 'Signal and Modulator')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 10 35 7]);


plot(T,abs(s),'o-','LineWidth',0.75,'MarkerSize',2)

hold on

plot(T,m, 'k','LineWidth',2)

plot(T,m_1(:,end), 'r','LineWidth',1.5)

plot(T,m_2(:,end), 'Color',[0.18, 0.74, 0.19],'LineWidth',1.5)

plot(T,Ub, '--','Color',0.6*[1 1 1],'LineWidth',1)

hold off


xlabel('t')

ylabel('x')

% axis([2 4 0 1])

box('off')


legend({'$\bf{|s|}$','$\bf{m}$','$\hat{\bf{m}}$ wo ub','$\hat{\bf{m}}$ w ub','ub'}, ...
    'Interpreter','latex', 'Location',[0.50, 0.75, 0.1, 0.1], 'FontSize',11)



% Visualization: errors

figure('Name', 'Infeasibility and Demodulation Errors')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [0 1 35 7]);


subplot(1,2,1)

loglog(e1, 'r','LineWidth',1.5)

hold on

loglog(e2, 'Color',[0.18, 0.74, 0.19],'LineWidth',1.5)

hold off


xlabel('N_{iter}')

ylabel('\epsilon')


legend({'$\epsilon$ wo ub','$\epsilon$ w ub'}, ...
    'Interpreter','latex', 'Location',[0.30, 0.55, 0.1, 0.1], 'FontSize',11)


subplot(1,2,2)

semilogx(E1, 'r','LineWidth',1.5)

hold on

semilogx(E2, 'Color',[0.18, 0.74, 0.19],'LineWidth',1.5)

hold off


xlabel('N_{iter}')

ylabel('E')


legend({'$E_m$ wo ub','$E_m$ w ub'}, ...
    'Interpreter','latex', 'Location',[0.75, 0.75, 0.1, 0.1], 'FontSize',11)

