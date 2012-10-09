function vNariz = calcularNariz(recept, vNorte)
% CALCULARNARIZ Calcula las coordenadas de la punta del vector receptor (nariz)
% con respecto al vector norte
% Argumentos de entrada:
% recept: objeto receptor
% vNorte: vector que representa el norte o referencia.
% Argumentos de salida:
% vNariz: coordenadas de la punta del vector receptor
%
% Creado por: Daniel Céspedes - Víctor Jasa (2004)
% Modificado por: Fabián Tommasini (10/2006)

azrad = get(recept, 'AzimutRad');
elrad = get(recept, 'ElevacionRad');
posR = get(recept, 'Posicion');

% azimut del vector norte con respecto al origen.
if vNorte ~= 0
    div = vNorte(2) / vNorte(1);
else
    div = 1500000;
end

azrad = azrad + atan(div);
tip = 0.25 * [sin(pi / 2 - elrad) * cos(azrad), sin(pi / 2 - elrad) * sin(azrad), cos(pi / 2 - elrad)];
vNariz = tip + posR;
