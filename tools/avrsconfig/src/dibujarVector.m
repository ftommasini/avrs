function dibujarVector(origen, destino, color)
% DIBUJARVECTOR Grafica un vector 
% Argumentos de entrada:
% origen: coordenadas origen del vector
% destino: coordenadas de destino del vector
% color: color del vector, donde los valores posibles son los que admite la
% función plot (por defecto se grafica en negro)
%
% Autor: Daniel Céspedes - Victor Jasá
% Creado: 2004
% Modificaciones por: Fabián Tommasini
% Modificado: 12/2006

if nargin == 2
    color = 'k';
end

v = zeros(2,3);
v(1,:) = origen;
v(2,:) = destino;
hold on;
plot3(v(:,1), v(:,2), v(:,3), color);