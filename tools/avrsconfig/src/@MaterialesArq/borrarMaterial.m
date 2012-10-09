function ms = borrarMaterial(ms, index)
% BORRARMATERIAL Elimina un material a la base de datos
% Argumentos de entrada:
% ms: objeto de la clase MaterialesArq
% index: índice del material que se va a eliminar
% Argumentos de salida:
% ms: objeto de la clase MaterialesArq actualizado con el nuevo material
%
% Autor: Fabián Tommasini
% Creado: 06/2011

ms.Indice = ms.Indice - 1;
ms.Materiales(index) = [];

