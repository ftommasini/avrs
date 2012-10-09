function pintar(ax, pl)
% PINTARPLANO Pinta un plano determinado del recinto
% Argumentos de entrada:
% ax: Ejes donde se diburá el plano
% pl: Plano que se pintará

axes(ax);
v = get(pl, 'Vertices');
fill3(v(:,1), v(:,2), v(:,3), 'c');