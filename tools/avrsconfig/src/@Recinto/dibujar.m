function dibujar(ax, r, conNormal)
% DIBUJAR Dibuja un recinto
% Argumentos de entrada:
% ax: ejes donde se dibujar� el plano
% r: recinto que se dibuja
% conNormal: indica si se dibuja la normal al plano
%   0: no dibuja la normal (valor por defecto)
%   1: dibuja la normal
%
% Autor: Fabi�n Tommasini
% Creado: 12/2006
% Modificado: 02/2012

if nargin == 2
    conNormal = 0;
end

% Recinto
%[xMax yMax zMax] = max(r);
%set(ax, 'CameraPosition', [xMax yMax (zMax * 1.1)]);  % posiciona la c�mara
%view(3);
planos = get(r, 'Planos');

for i = 1:length(planos)
    dibujar(ax, planos{i}, conNormal);  % dibuja cada uno de los planos
    hold on;
end

% dibuja el origen
dibujarVector([0 0 0], [1 0 0], 'c');
dibujarVector([0 0 0], [0 1 0], 'c');
dibujarVector([0 0 0], [0 0 1], 'c');

xlabel('x'), ylabel('y'), zlabel('z');
axis equal;

% Fuente
f = get(r, 'Fuente');

if ~isempty(f)
    posFuente = get(f, 'Posicion');
    plot3(posFuente(1), posFuente(2), posFuente(3), 'r*');
    % dibuja vector de orientación del receptor
    [xo yo zo] = vectorOrientacion(f);
    lo = [xo yo zo];
    v = zeros(2,3);
    v(1,:) = posFuente;
    v(2,:) = lo + posFuente;
    hold on;
    plot3(v(:,1),v(:,2),v(:,3),'k');
end

% Receptor
rec = get(r, 'Receptor');

if ~isempty(rec)
    posRec = get(rec, 'Posicion');
    plot3(posRec(1), posRec(2), posRec(3), 'b^');
    
    % dibuja vector de orientación del receptor
    [xo yo zo] = vectorOrientacion(rec);
    lo = [xo yo zo];
    v = zeros(2,3);
    v(1,:) = posRec;
    v(2,:) = lo + posRec;
    hold on;
    plot3(v(:,1),v(:,2),v(:,3),'k');
end

end




