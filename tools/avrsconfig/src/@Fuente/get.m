function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Nombre
%   Posicion
%   Orden (siempre igual a 0)
%   DistDirecta
%   DistReceptor

switch propName
    case 'Posicion'
        val = a.Posicion;
    case 'Nombre'
        val = a.Nombre;
    case 'Orden'
        val = a.Orden;
    case 'DistReceptor'
        val = a.DistReceptor;
    case 'Azimut'
        val = a.Azimut;
    case 'Elevacion'
        val = a.Elevacion;
    case 'AzimutRad'
        val = a.Azimut * pi / 180;  % trasnforma grados en radianes 
    case 'ElevacionRad'
        val = a.Elevacion * pi / 180;  % transforma grados en radianes.        
    otherwise
        error([propName,' no es una propiedad válida']);
end