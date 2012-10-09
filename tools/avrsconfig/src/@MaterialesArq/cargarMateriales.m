function ms = cargarMateriales(ms)
% CARGARMATERIALES Carga la base de datos de materiales
% Argumentos de entrada:
% ms: objeto de la clase MaterialesArq
% Argumentos de salida
% ms: objeto de la clase MaterialesArq con los materiales cargados
%
% Autor: Fabián Tommasini
% Creado: 03/2007
% Modificado: 02/2012

ruta = ms.Ruta;

if ~exist(ruta, 'file')
    ms.Materiales = cell(0);
    return;
end

ms = borrarMateriales(ms);
load(ms.Ruta, 'nombres', 'coefAbs');
n = length(nombres);

[nombresSort ind] = sortcell(nombres);
coefAbsSort = coefAbs(ind, :);

for i = 1:n
    mate = MaterialArq(nombresSort{i}, coefAbsSort(i,:));
    ms = agregarMaterial(ms, mate);
end



