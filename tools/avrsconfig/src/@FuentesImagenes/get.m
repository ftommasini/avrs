function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Arbol
%   Recinto
%   Orden

switch propName
    case 'Arbol'
        val = a.Arbol;
    case 'Recinto'
        val = a.Recinto;
    case 'Orden'
        val = a.Orden;
    otherwise
        error([propName,' no es una propiedad válida']);
end