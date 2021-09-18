
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



% EXAMPLE 2
%
% In this example, a synthetic 2D amplitude-modulated signal built of a random-peaks
% carrier and an LP-random-field modulator is generated and demodulated by using the
% AP-Accelerated algorithm. The inferred modulator is then compared with the
% predefined one. This example illustrates an application of the functions
% 'f_ap_demodulation' and 'f_ap_demodulation_mex' for demodulating 2D signals.


close all

clear all



% Function type (m or mex) selected by the user

fType = 'mex'; % select between 'm' and 'mex'



% Paths to the m and mex functions that perform AP-demodulation

addpath ../library_m ../library_mex



% Sampling grid

[X, Y] = meshgrid(0:0.005:1); % grid used for demodulation purposes

n = size(X,1);

[X2, Y2] = meshgrid(0:0.025:1); % grid used for visualization purposes



% Modulator (LP-random-field)

m = zeros(size(X));

w = [0.5173, 0.9470, 0.7655, 0.2824, 0.2210, 0.6862, 0.1671, 0.3924, 0.6181, ...
    0.4119, 0.0025, 0.8840];

j = 0;

for i1=0:3
    
    for i2=0:2
        
        j = j + 1;
        
        m = m + w(j)*cos(2*pi*(i1*(0:n-1)/n+i2*(0:n-1)'/n)+w(j));
        
    end
    
end

m = m - min(m(:));

m = m / max(m(:));



% Subsampled modulator

m2 = interp2(X,Y,m,X2,Y2);



% Carrier (random peaks)

cc = [0.2220, 0.2067, 0.4884, 0.7659, 0.2968, 0.0807, 0.4413, 0.8799, 0.4142, ...
0.6288, 0.5999, 0.2847, 0.3276, 0.1656, 0.9602, 0.0243, 0.6998, 0.0229, ...
0.0016, 0.6398, 0.2591, 0.8705, 0.0022, 0.9815, 0.8137, 0.0291, 0.1115, ...
0.9649, 0.6354, 0.9267, 0.8248, 0.3610, 0.5464, 0.3655, 0.7951, 0.6389, ...
0.5835, 0.9435, 0.8436, 0.1008, 0.5104, 0.4783, 0.5147, 0.8005, 0.5726, ...
0.9851, 0.4524, 0.3320, 0.4077, 0.3303, 0.5267, 0.8930, 0.7699, 0.7100, ...
0.7673, 0.2396, 0.3636, 0.0601, 0.8132, 0.0634, 0.0851, 0.1703, 0.8146, ...
0.0598, 0.5710, 0.8257, 0.5809, 0.6523, 0.4593, 0.4967, 0.6040, 0.2185, ...
0.3653, 0.5421, 0.1344, 0.6792, 0.9972, 0.7268, 0.5885, 0.2456, 0.1656, ...
0.2782, 0.6248, 0.8938, 0.7703, 0.1198, 0.1547, 0.0004, 0.1611, 0.3383, ...
0.6337, 0.7140, 0.1907, 0.9386, 0.2925, 0.5712, 0.5882, 0.6689, 0.2065, ...
0.8112, 0.4473, 0.0477, 0.7143, 0.9132, 0.5831, 0.2310, 0.5443, 0.0688, ...
0.2748, 0.4697, 0.9382, 0.7194, 0.6532, 0.4846, 0.2203, 0.7646, 0.3682, ...
0.2952, 0.3210, 0.8742, 0.3407, 0.3807, 0.8874, 0.0466, 0.4245, 0.6895, ...
0.8714, 0.1656, 0.2115, 0.3212, 0.0040, 0.5278, 0.2544, 0.0021, 0.7132, ...
0.2993, 0.1577, 0.0797, 0.4320, 0.7166, 0.6996, 0.2169, 0.4249, 0.3518, ...
0.9538, 0.0420, 0.2370, 0.2778, 0.1176, 0.9295, 0.9049, 0.9100, 0.0045, ...
0.4728, 0.2697, 0.8605, 0.3467, 0.1765, 0.1566, 0.3896, 0.3706, 0.0925, ...
0.2739, 0.5697, 0.1154, 0.5252, 0.1085, 0.6850, 0.5321, 0.2710, 0.4101, ...
0.4250, 0.6209, 0.4344, 0.0603, 0.6348, 0.8840, 0.1613, 0.2017, 0.9767, ...
0.4458, 0.0140, 0.5626, 0.0921, 0.3696, 0.1786, 0.5415, 0.3791, 0.4891, ...
0.9199, 0.7476, 0.0325, 0.8198, 0.6506, 0.4079, 0.5986, 0.1102, 0.9745, ...
0.2820, 0.5881, 0.6075, 0.0562, 0.1923, 0.9325, 0.5379, 0.8779, 0.8283, ...
0.9809, 0.7651, 0.7090, 0.9867, 0.8185, 0.5057, 0.9999, 0.2444, 0.0221, ...
0.4203, 0.9045, 0.7554, 0.7669, 0.2311, 0.5881, 0.3348, 0.6998, 0.6015, ...
0.8316, 0.9592, 0.6635, 0.7464, 0.5475, 0.4010, 0.8478, 0.7038, 0.6468, ...
0.2885, 0.3513, 0.8803, 0.4530, 0.8347, 0.8748, 0.1201, 0.4327, 0.1652, ...
0.9487, 0.1985, 0.1338, 0.5014, 0.0863, 0.9117, 0.2444, 0.0224, 0.7622, ...
0.1201, 0.5540, 0.4990, 0.0593, 0.4552, 0.8290, 0.7764, 0.3111, 0.0139, ...
0.4933, 0.9800, 0.9281, 0.5271, 0.6275, 0.8195, 0.2567, 0.6348, 0.9209; ...
...
0.8707, 0.9186, 0.6117, 0.5184, 0.1877, 0.7384, 0.1583, 0.2741, 0.2961, ...
0.5798, 0.2658, 0.2536, 0.1442, 0.9639, 0.1884, 0.2046, 0.7795, 0.5777, ...
0.5155, 0.9856, 0.8025, 0.9227, 0.4695, 0.3989, 0.5465, 0.0865, 0.2512, ...
0.6318, 0.8119, 0.9126, 0.0942, 0.0355, 0.7961, 0.2443, 0.3521, 0.4934, ...
0.9393, 0.1117, 0.3460, 0.3834, 0.9611, 0.8500, 0.4466, 0.0204, 0.4114, ...
0.8014, 0.7029, 0.3600, 0.8986, 0.0827, 0.6608, 0.9652, 0.7591, 0.7016, ...
0.9743, 0.2215, 0.8103, 0.4497, 0.2642, 0.2421, 0.8078, 0.1953, 0.8103, ...
0.9650, 0.3025, 0.6594, 0.4728, 0.2419, 0.4187, 0.2111, 0.7486, 0.4256, ...
0.4779, 0.2652, 0.3019, 0.6013, 0.5609, 0.6158, 0.6050, 0.0713, 0.0604, ...
0.3483, 0.1597, 0.7306, 0.1815, 0.9303, 0.4071, 0.8455, 0.7393, 0.7428, ...
0.8889, 0.2651, 0.6342, 0.2582, 0.4256, 0.9901, 0.8879, 0.3714, 0.3307, ...
0.4458, 0.0275, 0.8636, 0.8304, 0.4334, 0.3488, 0.0260, 0.0195, 0.5254, ...
0.9706, 0.2852, 0.9667, 0.1369, 0.7670, 0.0974, 0.7489, 0.6573, 0.6528, ...
0.7640, 0.5043, 0.4854, 0.2949, 0.5953, 0.0584, 0.3418, 0.8476, 0.4459, ...
0.1661, 0.5777, 0.9752, 0.0141, 0.7735, 0.7273, 0.9084, 0.6823, 0.1977, ...
0.8807, 0.8074, 0.0984, 0.5922, 0.5683, 0.3182, 0.4868, 0.0836, 0.6991, ...
0.0567, 0.6351, 0.7000, 0.6762, 0.1547, 0.6705, 0.6147, 0.3651, 0.2509, ...
0.9189, 0.4966, 0.5290, 0.8573, 0.8691, 0.2597, 0.7464, 0.4243, 0.0416, ...
0.1331, 0.8380, 0.6698, 0.3229, 0.8539, 0.9468, 0.9107, 0.5435, 0.5347, ...
0.9536, 0.4167, 0.4788, 0.1567, 0.6897, 0.5744, 0.5319, 0.2763, 0.7413, ...
0.2140, 0.8967, 0.1410, 0.5868, 0.1654, 0.1175, 0.0989, 0.1180, 0.5080, ...
0.8384, 0.3963, 0.0145, 0.9733, 0.5382, 0.3626, 0.5241, 0.4665, 0.8944, ...
0.0441, 0.0947, 0.2165, 0.3988, 0.6922, 0.5553, 0.1900, 0.6840, 0.6099, ...
0.2820, 0.8589, 0.5188, 0.6836, 0.1999, 0.7759, 0.1299, 0.6388, 0.3019, ...
0.6396, 0.0041, 0.0733, 0.2318, 0.5796, 0.0011, 0.9319, 0.8927, 0.6560, ...
0.7422, 0.3567, 0.0286, 0.0101, 0.5152, 0.6869, 0.0101, 0.0947, 0.3179, ...
0.6055, 0.9869, 0.8656, 0.3513, 0.8832, 0.8192, 0.9975, 0.7987, 0.4752, ...
0.4678, 0.3848, 0.1052, 0.5652, 0.3117, 0.5121, 0.1686, 0.7248, 0.9140, ...
0.7688, 0.5681, 0.3609, 0.6887, 0.9962, 0.4963, 0.6052, 0.5642, 0.1326, ...
0.1580, 0.0161, 0.7922, 0.4014, 0.0568, 0.3983, 0.3021, 0.1127, 0.3214]';

c = zeros(size(X));

for i=1:size(cc,1)
    
    c = c + exp(-((X-cc(i,1)).^2+(Y-cc(i,2)).^2)*8000);
    
end



% Signal

s = c .* m;



% Demodulation

if strcmpi(fType, 'm')

    fDemod = @(x,y) f_ap_demodulation (x, y);
    
elseif strcmpi(fType, 'mex')
    
    fDemod = @(x,y) f_ap_demodulation_mex (x, y);
    
end


Par.Al = 'A';

Par.Fs = [n n];

Par.Fc = [4 4];

Par.Et = 10^-6;

Par.Ni = 10^2;


[m_, e, iter, tcpu] = fDemod (s, Par);



% Visualization: signal and modulator

figure('Name', 'Signal and Modulator')

set(gcf, 'Units', 'centimeters');

set(gcf, 'Position', [5 2 20 15]);


surf(X,Y,s,'FaceColor','none', 'EdgeColor', [0 0 0], 'EdgeAlpha',0.4, ...
    'LineWidth',0.2)

hold on

colormap summer

surf(X,Y,m_, 'EdgeColor','none', 'FaceColor', 'flat', 'FaceAlpha',0.6)

surf(X2,Y2,m2, 'FaceColor','none', 'EdgeColor', [1 1 1]*0.85, 'EdgeAlpha',0.6, ...
    'LineWidth',1.25)

hold off


xlabel('t_1')

ylabel('t_2')

zlabel('x')


view(-198,59)

grid('on')

box('on')


legend({'$\bf{|s|}$','$\bf{m}$','$\hat{\bf{m}}$'}, 'Interpreter','latex', ...
    'Location',[0.59, 0.80, 0.1, 0.1], 'FontSize',11)

