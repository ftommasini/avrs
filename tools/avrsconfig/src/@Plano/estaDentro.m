function ok = estaDentro(plano, pCorte)
% ESTADENTRO Verifica si el punto dado est� dentro de los l�mites del plano dado
% Argumentos de entrada:
% plano: plano sobre el cual se quiere verificar la pertenencia del punto
% pCorte: coordenadas del punto de corte
% Argumentos de salida:
% ok: indica si el punto corta al plano
%   2: el punto de corte est� sobre un borde el plano
%   1: el punto de corte est� dentro del plano
%   0: el punto de corte no est� dentro del plano
%
% Autor: Fabi�n Tommasini
% Creado: 02/2007

% Based on:
% Extension of the image model to arbitrary polyhedra
% Jeffrey Borish, J. Acoust. Soc. Am. 75, 1827 (1984), DOI:10.1121/1.390983

vert = get(plano, 'Vertices');

if sobreBorde(vert, pCorte) == 1
    ok = 2;
    return;
end

% test para verificar si el punto de corte est� dentro de los l�mites
v1 = vert(4,:) - pCorte;
v2 = vert(1,:) - pCorte;
c = cross(v1, v2);

if norm(c) == 0
    ok = 0;
    return;
end

cosDir1 = roundn(c / norm(c), -5);  % cosenos directores para el producto cruz

for i = 1:3  % para el resto de los v�rtices
    v1 = vert(i,:) - pCorte;
    v2 = vert(i + 1,:) - pCorte;
    c = cross(v1, v2);     
    
    if norm(c) == 0
        ok = 0;
        return;
    end
    
    cosDir2 = roundn(c / norm(c), -5);  % cosenos directores para el producto cruz
    
    if isequal(cosDir1, cosDir2)
        ok = 1;
    else
        ok = 0;
        return;
    end   
end


function ok = sobreBorde(vert, pCorte)
% Verifica si el punto de corte cae sobre un borde de la superficie
v1 = roundn(vert(4,:), -5);
v2 = roundn(vert(1,:), -5);

if v2(1) - v1(1) == 0
    ok = 0;
    return;
end

tp = (pCorte(1) - v1(1))  ./ (v2(1) - v1(1));
coord = roundn(v1 + (v2 - v1) .* tp, -5);
pc = roundn(pCorte, -5);

if pc(1) == coord(1) && pc(2) == coord(2) && pc(3) == coord(3)
    ok = 1;
    return;
end

for i = 1:3  % para el resto de los v�rtices
    v1 = vert(i,:);
    v2 = vert(i + 1,:);
    
    if v2(1) - v1(1) == 0
        ok = 0;
        return;
    end

    tp = (pCorte(1) - v1(1))  ./ (v2(1) - v1(1));
    coord = roundn(v1 + (v2 - v1) .* tp, -5);
    pc = roundn(pCorte, -5);

    if pc(1) == coord(1) && pc(2) == coord(2) && pc(3) == coord(3)
        ok = 1;
        return;
    end
end

ok = 0;

