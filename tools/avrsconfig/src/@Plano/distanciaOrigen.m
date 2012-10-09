function dist = distanciaOrigen(p)
% DISTANCIAORIGEN Calcula la distancia plano-origen
% Argumentos de entrada:
% p: objeto plano
% Argumentos de salida:
% dist: escalar que indica la distancia del plano al origen
%
% Creado por: Daniel Céspedes - Víctor Jasa (10/2004)
% Modificado por: Fabián Tommasini (10/2006)

% cálculo de la distancia del plano al origen
coef = coeficientes(p);
A = coef(1);
B = coef(2);
C = coef(3);
D = coef(4);
% como la distancia es al origen: x = 0, y = 0, z = 0
dist = abs(D) / sqrt(A^2 + B^2 + C^2);
