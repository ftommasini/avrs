function [az el] = vector2angulos(xv, yv, zv)
%VECTOR2ANGULOS Summary of this function goes here
%   Detailed explanation goes here

% Creado: 06/2011

% azimuth
[theta, ro, z] = cart2pol(xv, yv, zv);
az = -(rad2deg(theta) - 90);
% elevation
r = sqrt(xv ^ 2 + yv ^ 2);
[theta2 ro2] = cart2pol(r, zv);
el = rad2deg(theta2);

end

