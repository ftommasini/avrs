function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Nombre
%   Vertices
%   MaterialArq

switch propName
    case 'Vertices'
        val = a.Vertices;
    case 'Nombre'
        val = a.Nombre;
    case 'MaterialArq'
        val = a.MaterialArq;
    otherwise
        error([propName,' no es una propiedad válida']);
end