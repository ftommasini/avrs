function p = Plano(a, nom)
% PLANO Constructor de la clase Plano
%   p = Plano(v) crea un plano con los vértices indicados en v,
%   donde las filas son los diferentes vértices y las columnas
%   corresponden a x, y, z respectivamente
%   p = Plano(v, nom) crea un plano con los vértices v y con el nombre nom

if nargin == 0
    p.Nombre = '';  % no tiene nombre
    p.Vertices = []; 
    p.MaterialArq = [];
    p = class(p, 'Plano');
elseif nargin == 1
    if isa(a, 'Plano')
        p = a;
    else
        p.Nombre = '';
       
        if verticesCorrectos(a)
            p.Vertices = a;        
        else
            p.Vertices = [];
        end
        
        p.MaterialArq = [];
        p = class(p, 'Plano');
    end
elseif nargin == 2
    if ischar(nom)
        p.Nombre = nom;
        
        if verticesCorrectos(a)
            p.Vertices = a;        
        else
            p.Vertices = [];
        end   
        
        p.MaterialArq = [];
        p = class(p, 'Plano');
    end
end
