function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Nombre
%   CoefAbsorcion
%   CoefReflexion

switch propName
    case 'Nombre'
        val = a.Nombre;
    case 'CoefAbsorcion'
        val = a.CoefAbsorcion;
    case 'CoefReflexion'
        val = a.CoefReflexion;
    otherwise
        error([propName,' no es una propiedad válida']);
end