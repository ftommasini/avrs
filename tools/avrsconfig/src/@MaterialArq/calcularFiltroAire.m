function [B A] = calcularFiltroAire(distancia, fs, n)
% CALCULARFILTROAIRE Calcula los coeficientes B y A del filtro para simular
% el aire (se considera 20% de humedad y 20ºC de temperatura)
% Argumentos de entrada:
% distancia: distancia a recorrer para la cual se quiere calcular el filtro
% fs: frecuencia de sampleo
% n: puntos de resolución
% Argumentos de salida:
% B: coeficientes b del filtro 
% A: coeficientes a del filtro 
%
% Autor: Juan Gorgas
% Modificaciones: Fabián Tommasini - Oscar Ramos
% Creado: 2005
% Modificado: 04/2007

deltaF = fs / n;

% Función de Absorción de Aire
% ----------------------------
f = 0:deltaF:fs/2;
hr = 0.2;  % humedad (en porcentaje)
T = 273.15 + 20; % temperatura en grados Kelvin (a 20ºC)
To = 293.15;  % temperatura de referencia en grados Kelvin
Pa = 101325;  % presión atmosférica
Pr = 101325;  % presión atmosférica de referencia
h = 0.4615; % concentración molar del vapor de agua

frO = (Pa / Pr) * (24 + 4.04 * 10^4 * h * ((0.02 + h) / (0.391 + h)));
frN = (Pa / Pr)*(T / To)^(-1/2) * (9 + 280 * h * exp(-4.170 * ((T / To)^(-1/3)-1)));

z = 0.1068 * exp(-3352/T) * (frN + ((f.^2) / frN)).^-1;
y = (T / To)^(-5/2) * (0.01275 * exp(-2239.1 / T) * (frO + ((f.^2) / frO)).^-1 + z);
a = 8.686 * (f.^2) .* ((1.84 * 10^-11 * (Pa / Pr)^-1 * (T / To)^(1/2)) + y);  % [dB/m]
as = -a * distancia + 20 * log10(1 / distancia);  % [dB] (incluida 1/r)

as = 10 .^ (as / 20);  % transformar los dB de atenuación
as2 = [as as(end-1:-1:2)];  % agrega la parte simétrica

% Reconstrucción del sistema de fase mínima
% -----------------------------------------
y = real(ifft(log(as2))); % analisis cepstrum
w = [1; 2*ones(n/2-1, 1); ones(1 - rem(n,2), 1); zeros(n/2-1, 1)]; % ventana para obtención de un sistema de fase mínima
ri = ifft(exp(fft(w' .* y)));  % obtención de la respuesta impulsiva (síntesis cepstrum)
rif = fft(ri, n);  % obtención de la respuesta en frecuencias compleja a partir de la respueta impulsiva
rif = rif(1:n/2+1);  %utilización de la primera mitad de los valores hasta la frecuencia de nyquist

% Función de ponderación (ERB)
% ----------------------------
% fr = (0:deltaF:fs/2);
% % fórmula de la ERB para la frecuencia en Hz
% % (no confundir con la fórmula en la que la frecuencia está en KHz)
% erb = 0.108 * fr + 24.7; % función ERB
% erbinv = (1./erb);
% wt = erbinv ./ max(erbinv);  % pesos normalizados entre los valores 0 y 1
wt = funcionERB(fs, n);

% Obtención de los coeficientes B y A del filtro IIR
% --------------------------------------------------
fsrad = 2 * pi * (0:deltaF:fs/2) / fs;  % frecuencias en radianes linealmente espaciadas

[B,A] = invfreqz(rif, fsrad, 1, 2, wt, 30); % obtencion de coeficientes B y A
