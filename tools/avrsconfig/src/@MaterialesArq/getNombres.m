function noms = getNombres(ms)
% GETNOMBRES Obtiene los nombres de los materiales
% Argumentos de entrada:
% ms: objeto de la clase MaterialesArq
% Argumentos de salida:
% noms: cell array con los nombres de los materiales
%
% Autor: Fabián Tommasini
% Creado: 03/2007
% Modificado: 04/2007

noms = cell(ms.Indice, 1);

for i = 1:ms.Indice
    mate = ms.Materiales{i}; 
    nom = get(mate, 'Nombre');
    noms{i} = nom;
end