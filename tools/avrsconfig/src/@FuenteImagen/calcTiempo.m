function t = calcTiempo(fi)
% CALCTIEMPO Calcula el tiempo en segundos que está la
% fuente-imágenes pasada por parámetro
% Argumentos de entrada:
% fi: fuente-imágen de la cual se quiere calcular el tiempo
% Argumentos de salida:
% t: tiempo en segundos
%
% Autor: Fabián C. Tommasini
% Creado: 08/2007

vSonido = 343;  % en m/s
t = fi.DistReceptor / vSonido;