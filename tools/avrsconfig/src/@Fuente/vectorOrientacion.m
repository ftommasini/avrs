function [xo yo zo] = vectorOrientacion(f)
%VECTORORIENTACION Summary of this function goes here
%   Detailed explanation goes here

% Creado: 02/2012

az = get(f, 'Azimut');
el = get(f, 'Elevacion');
theta = deg2rad(-az+90);
ro = deg2rad(el);
[xo yo zo] = sph2cart(theta, ro, 1);  % vector orientación unitario
end
