function val = get(a, propName)
% GET Obtiene las propiedades del objeto especificado y
% devuelve el valor
% Propiedades:
%   Planos
%   Fuente
%   Receptor
%   Nombre
%   RespuestaImpulsiva
%   Volumen

switch propName
    case 'Planos'
        val = a.Planos;
    case 'Fuente'
        val = a.Fuente;
    case 'Receptor'
        val = a.Receptor;
    case 'Nombre'
        val = a.Nombre;
    case 'RespuestaImpulsiva'
        val = a.RespuestaImpulsiva;
    case 'Volumen'
        val = a.Volumen;
    otherwise
        error([propName,' no es una propiedad válida']);
end