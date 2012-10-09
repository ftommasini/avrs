function a = set(a, varargin)
% SET Setea las propiedades y devuelve el objeto actualizado
% Propiedades:
%   Coordenadas
%   Orden
%   Visible
%   IdPlano
%   DistReceptor

propertyArgIn = varargin;

while length(propertyArgIn) >= 2,
    prop = propertyArgIn{1};
    val = propertyArgIn{2};
    propertyArgIn = propertyArgIn(3:end);
    
    switch prop
        case 'Posicion'
            a.Coordenadas = val;
        case 'Orden'
            a.Orden = val;
        case 'Visible'
            a.Visible = val;
        case 'IdPlano'
            a.IdPlano = val;
        case 'DistReceptor'
            a.DistReceptor = val;
        otherwise
            error([prop ' no es una propiedad válida']);
    end
end