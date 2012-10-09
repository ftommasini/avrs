function t = calcTiempo(f)
% CALCTIEMPO Calcula el tiempo en segundos que está la
% fuente pasada por parámetro
% Argumentos de entrada:
% f: fuente real
% Argumentos de salida:
% t: tiempo en segundos
%
% Autor: Fabián C. Tommasini
% Creado: 08/2007

vSonido = 343;  % en m/s
t = f.DistReceptor / vSonido;