function a = set(a, varargin)
% SET Setea las propiedades y devuelve el objeto actualizado
% Propiedades:
%   Nombre
%   Posicion
%   DistDirecta
%   DistReceptor

propertyArgIn = varargin;

while length(propertyArgIn) >= 2,
    prop = propertyArgIn{1};
    val = propertyArgIn{2};
    propertyArgIn = propertyArgIn(3:end);
    
    switch prop
        case 'Posicion'
            a.Posicion = val;
        case 'Nombre'
            a.Nombre = val;
        case 'DistReceptor'
            a.DistReceptor = val;
        case 'Azimut'
            a.Azimut = val;
        case 'Elevacion'
            a.Elevacion = val;
        case 'AzimutRad'
            a.Azimut = val * 180 / pi;  % transforma radianes en grados
        case 'ElevacionRad'
            a.Elevacion = val * 180 / pi;  % transforma radianes en grados            
        otherwise
            error([prop ' no es una propiedad válida']);
    end
end