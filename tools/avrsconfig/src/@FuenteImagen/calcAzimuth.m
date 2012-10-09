function az = calcAzimuth(fi, rec)
% CALCAZIMUTH Obtiene el ángulo en grados de azimuth que se encuentra la
% fuente-imagen con respecto al receptor
% Argumentos de entrada:
% fi: fuente-imagen de la cual se quiere saber el azimuth
% rec: objeto receptor
% Argumentos de salida:
% az: azimuth de la fuente-imagen con respecto al receptor
%
% Autor: Fabián Tommasini
% Creado: 12/2006
% Modificado: 08/2007

posRec = get(rec, 'Posicion');
posFi = get(fi, 'Posicion');
posFiRec = posRec - posFi;  % posición de la fuente con respecto al receptor
[t r z] = cart2pol(posFiRec(1), posFiRec(2), posFiRec(3));
az = -t * 180 / pi;
