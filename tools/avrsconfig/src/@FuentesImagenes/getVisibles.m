function [fivis cant ind] = getVisibles(fis, ord)
% GETVISIBLES Obtiene las fuentes-imágenes que son visibles
% Argumentos de entrada:
% fis: objeto FuentesImagenes
% ord: indica si el listado se devuelve ordenado por tiempo
%   0: sin ordenar (valor por defecto)
%   1: ordenado en forma creciente
% Argumentos de salida:
% fivis: cell array que contiene objetos FuenteImagen con las fuentes-imagenes
% visibles (vacío si no se ha calculado)
% cant: cantidad de fuentes-imágenes visibles
% ind: índices asociados al árbol de fuentes-imágenes
%
% Autor: Fabián Tommasini
% Creado: 02/2007
% Modificado: 08/2007

if fis.Calculado == 0
    fivis = [];
    error('Primero se deben calcular las fuentes-imágenes');
    return;
end

if nargin == 1
    ord = 0;  % desordenado por defecto
end

t = fis.Arbol;
nodos = get(t, 'Nodos');
n = get(t, 'Indice');
fivis = cell(0);
k = 0;
ind = zeros(1, 1);

for i = 1:n
    fi = get(nodos{i}, 'Info');
    
    if i ~= 1
        vis = get(fi, 'Visible');
    else
        vis = 1;
    end
    
    if vis == 1
        k = k + 1;
        fivis{k} = fi;
        ind(k) = i;  % guarda el índice asociado
    end
end

cant = length(fivis);

if ord == 1
    [fivis ind] = ordenar(fivis, cant, ind);
end
% --


function [fivis ind] = ordenar(fivis, m, ind)
% Ordenamiento por burbuja (ineficiente, pero fácil para empezar)

% posfr = get(fivis{1}, 'Posicion');  % relativo a la fuente real

for i = 2:m
    for j = 1:m-1
        dist1 = get(fivis{j}, 'DistReceptor');
        dist2 = get(fivis{j+1}, 'DistReceptor');
        
        if dist1 > dist2
            tmp1 = fivis{j};
            tmp2 = ind(j);
            fivis{j} = fivis{j+1};
            ind(j) = ind(j+1);
            fivis{j+1} = tmp1;
            ind(j+1) = tmp2;
        end
    end
end
