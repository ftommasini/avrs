function a = set(a, varargin)
% SET Setea las propiedades y devuelve el objeto actualizado
% Propiedades:
%   Planos
%   NombresPlanos
%   Fuente
%   Receptor
%   Nombre
%   Volumen
%   RespuestaImpulsiva

propertyArgIn = varargin;

while length(propertyArgIn) >= 2,
    prop = propertyArgIn{1};
    val = propertyArgIn{2};
    propertyArgIn = propertyArgIn(3:end);
    
    switch prop
        case 'Planos'
            a.Planos = val;
        case 'Fuente'
            a.Fuente = val;
        case 'Receptor'
            a.Receptor = val;
        case 'Nombre'
            a.Nombre = val;
        case 'RespuestaImpulsiva'
            a.RespuestaImpulsiva = val;
        case 'Volumen'
            a.Volumen = val;
        otherwise
            error([prop ' no es una propiedad válida']);
    end
end