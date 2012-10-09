function dibujar(ax, f, conNom)
% DIBUJAR Dibuja la fuente en los ejes especificados
% Argumentos de entrada:
% ax: ejes donde se dibujará la fuente
% f: fuente a dibujar
% conNom: indica si se muestra el nombre de la fuente
%   0: no se muestra el nombre
%   1: se muestra el nombre

axes(ax);
pos = get(f, 'Posicion');
plot3(pos(1), pos(2), pos(3), 'r*');

if conNom == 1
    nom = get(f, 'Nombre');
    text(pos(1), pos(2), ['Fuente: ' nom], 'FontSize', 12);
end
