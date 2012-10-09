function ok = verticesCorrectos(a)
% VERTICESCORRECTOS Verifica la consistencia de los vértices (función privada)
%
% Creado por: Fabián Tommasini (10/2006)

if size(a, 2) == 3
    ok = 1;
else
    ok = 0;
end