function c = centro(p)
% CENTRO Calcula las coordenadas del centro geométrico del plano
% Argumentos de entrada:
% p: objeto plano
% Argumentos de salida:
% c: vector del punto central
%   c = [cx, cy, cz]
%
% Creado por: Daniel Céspedes - Víctor Jasa (2004)
% Modificado por: Fabián Tommasini (10/2006)

v = get(p, 'Vertices');
x = v(:,1);
y = v(:,2);
z = v(:,3);
% centros
cx = ((max(x) - min(x)) / 2) + min(x);
cy = ((max(y) - min(y)) / 2) + min(y);
cz = ((max(z) - min(z)) / 2) + min(z);
c = [cx cy cz];
