function ms = agregarMaterial(ms, m)
% AGREGARMATERIAL Agrega un material a la base de datos
% Argumentos de entrada:
% ms: objeto de la clase MaterialesArq
% m: objeto de la clase Material que se va a agregar
% Argumentos de salida:
% ms: objeto de la clase MaterialesArq actualizado con el nuevo material
%
% Autor: Fabián Tommasini
% Creado: 03/2007
% Modificado: 06/2011

ms.Indice = ms.Indice + 1;
ms.Materiales{ms.Indice} = m;

