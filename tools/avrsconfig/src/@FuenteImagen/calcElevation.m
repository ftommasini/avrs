function el = calcElevation(fi, rec)
% CALCELEVATION Obtiene el ángulo en grados de elevación que se encuentra la
% fuente-imagen con respecto al receptor
% Argumentos de entrada:
% fi: fuente-imagen de la cual se quiere saber la elevación
% rec: objeto receptor
% Argumentos de salida:
% el: elevación de la fuente-imagen con respecto al receptor
%
% Autor: Fabián Tommasini
% Creado: 12/2006
% Modificado: 08/2007

posRec = get(rec, 'Posicion');
posFi = get(fi, 'Posicion');
posFiRec = posRec - posFi;  % posición de la fuente con respecto al receptor
r = sqrt(posFiRec(1) ^ 2 + posFiRec(2) ^ 2);
[t2 r2] = cart2pol(r, posFiRec(3));
el = -t2 * 180 / pi;

