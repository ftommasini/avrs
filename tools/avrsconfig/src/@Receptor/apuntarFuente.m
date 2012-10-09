function [az el] = apuntarFuente(rec, f)
%APUNTARFUENTE Summary of this function goes here
%   Detailed explanation goes here

posRec = get(rec, 'Posicion');
posFuente = get(f, 'Posicion');

lo = (posFuente - posRec) / norm(posFuente - posRec);  % vector unitario orientación del listener
[az el] = vector2angulos(lo(1), lo(2), lo(3));
end

