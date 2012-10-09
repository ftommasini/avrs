function coef = coefNE(m)
% COEFNE Función que devuelve los coeficientes de absorción corregido por
% la fórmula de Norris-Eyring
% Argumentos de entrada:
% m: objeto MaterialArq
% Argumentos de salida:
% coef: coeficientes corregidos por Norris-Eyring
%
% Autor: Fabián Tommasini
% Creado: 10/2007

coefS = get(m, 'CoefAbosorcion');
coef = 1 - exp(-coefS);