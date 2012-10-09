function dibujarVector(ax, origen, destino)
% DIBUJARVECTOR Dibuja un vector (función privada)
% Argumentos de entrada:
% ax: ejes donde se dibujará el vector
% origen: punto de origen del vector
% destino: punto de destino del vector
%
% Creado por: Daniel Céspedes - Víctor Jasa (2004)
% Modificado por: Fabián Tommasini (10/2006)

v = zeros(2,3);
v(1,:) = origen;
v(2,:) = destino;
axes(ax);
hold on;
plot3(v(:,1), v(:,2), v(:,3), 'k');


