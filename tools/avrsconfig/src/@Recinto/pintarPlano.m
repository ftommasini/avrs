function pintarPlano(ax, r, pl, limpiar)
% PINTARPLANO Pinta un plano determinado del recinto
% Argumentos de entrada:
% ax: Ejes donde se diburá el plano
% r: Recinto del cual se pintará el plano
% pl: Número de plano que se pintará
% limpiar: 0 si no se borran los demás planos pintados, 1 para borrarlos

if ~isnumeric(pl)
    error('No se ha identificado correctamente el plano');
end

axes(ax);

if limpiar == 1
    cla;  % limpia el eje
    dibujar(ax, r);
end

plano = r.Planos{pl};
v = get(plano, 'Vertices');
fill3(v(:,1), v(:,2), v(:,3), 'k');