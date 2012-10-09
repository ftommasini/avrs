function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Posicion
%   Orden
%   Visible
%   IdPlano
%   DistReceptor

switch propName
    case 'Posicion'
        val = a.Posicion;
    case 'Orden'
        val = a.Orden;
    case 'Visible'
        val = a.Visible;
    case 'IdPlano'
        val = a.IdPlano;
    case 'DistReceptor'
        val = a.DistReceptor;
    otherwise
        error([propName,' no es una propiedad válida']);
end