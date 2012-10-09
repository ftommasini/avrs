function dibujar(ax, p, conNormal)
% DIBUJAR Dibuja el plano p
% Argumentos de entrada:
% ax: ejes donde se dibujará el plano
% p: objeto plano a dibujar
% conNormal: indica si se dibuja la normal al plano
%   0: no dibuja la normal (valor por defecto)
%   1: dibuja la normal
%
% Fabián Tommasini (12/2006)

if nargin == 2
    conNormal = 0;
else
   % conNormal = 1;
end

if ~isempty(p.Vertices)
    axes(ax);  % hace que la figura de ejes ax, sea sobre la cual se dibujará el plano    
    line([p.Vertices(:,1); p.Vertices(1,1)], ...
        [p.Vertices(:,2); p.Vertices(1,2)], ...
        [p.Vertices(:,3); p.Vertices(1,3)]);
    
    if conNormal
        hold on;
        dibujarNormal(ax, p);
    end
else
    error('No hay vértices del plano');
end
