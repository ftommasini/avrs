function wt = funcionERB(fs, n)
% FUNCIONERB Función de ponderación (ERB)
% Argumentos de entrada:
% fs: frecuencia de sampleo
% n: puntos de resolución
% Argumentos de salida:
% wt: pesos normalizados de la función ERB
%
% Autor: Fabián Tommasini
% Creado: 04/2007

% resolución en frecuencias
deltaF = fs / n;
fr = (0:deltaF:fs/2);
% fórmula de la ERB para la frecuencia en Hz
% (no confundir con la fórmula en la que la frecuencia está en KHz)
erb = 0.108 * fr + 24.7; % función ERB
erbinv = (1./erb);
wt = erbinv ./ max(erbinv);  % pesos normalizados entre los valores 0 y 1