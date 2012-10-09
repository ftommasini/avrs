function a = set(a, varargin)
% SET Setea las propiedades y devuelve el objeto actualizado
% Propiedades:
%   Nombre
%   Vertices
%   MaterialArq

propertyArgIn = varargin;

while length(propertyArgIn) >= 2,
    prop = propertyArgIn{1};
    val = propertyArgIn{2};
    propertyArgIn = propertyArgIn(3:end);
    
    switch prop
        case 'Vertices'
            if verticesCorrectos(val)
                p.Vertices = val;        
            else
                p.Vertices = [];
            end
        case 'Nombre'
            a.Nombre = val;
        case 'MaterialArq'
            a.MaterialArq = val;
        otherwise
            error([prop ' no es una propiedad válida']);
    end
end