
%                       C O P Y R I G H T   N O T I C E
%
% Copyright ©2021. Institute of Science and Technology Austria (IST Austria).
% All Rights Reserved. The underlying technology is protected by PCT Patent
% Application No. PCT/EP2021/054650.
%
% This file is part of the AP Demodulation library, which is free software: you can
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



function out = f_apd_compression (in, Cp)
% >
% P U R P O S E
%
% Compresses the dynamic range of the input array by using the power function model.
%
%
% I N P U T   A R G U M E N T S
%
% [in] - input signal.
%
% [Cp] - compression parameter.
%
%
% O U T P U T   A R G U M E N T S
%
% [out] - compressed signal.
%
%
% M A T L A B - E X T E R N A L   F U N C T I O N S   U S E D
%
% None.



out = sign(in) .* abs(in).^Cp;

