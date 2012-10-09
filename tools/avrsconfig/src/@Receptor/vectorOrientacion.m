function [xo yo zo] = vectorOrientacion(rec)
%VECTORORIENTACION Summary of this function goes here
%   Detailed explanation goes here

% Creado: 06/2011

az = get(rec, 'Azimut');
el = get(rec, 'Elevacion');
theta = deg2rad(-az+90);
ro = deg2rad(el);
[xo yo zo] = sph2cart(theta, ro, 1);  % vector orientación unitario
end

