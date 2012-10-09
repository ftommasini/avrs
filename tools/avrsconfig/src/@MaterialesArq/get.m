function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Ruta
%   Materiales
%   Indice

switch propName
    case 'Ruta'
        val = a.Ruta;
    case 'Materiales'
        val = a.Materiales;
    case 'Indice'
        val = a.Indice;
    otherwise
        error([propName,' no es una propiedad válida']);
end