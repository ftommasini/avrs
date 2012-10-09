function [az el] = apuntarReceptor(f, rec)
%APUNTARRECEPTOR Summary of this function goes here
%   Detailed explanation goes here

posFuente = get(f, 'Posicion');
posRec = get(rec, 'Posicion');

lo = (posRec - posFuente) / norm(posRec - posFuente);  % vector unitario orientación del listener
[az el] = vector2angulos(lo(1), lo(2), lo(3));

end


