function r = Recinto(a, t)
% RECINTO Constructor de la clase Recinto
%   r = Recinto(p) crea un recinto con los planos contenidos 
%   en el cell array p
%   r = Recinto(ruta, 'dxf') importa un recinto contenido en un archivo DXF
%   r = Recinto(r2) crea otro recinto a partir del recinto r2

if nargin == 0  % sin argumentos
    r.Planos = cell(0);  % cell array vacío
elseif nargin == 1  % un argumento de entrada
    if isa(a, 'Recinto')
        r = a;
        return;
    elseif iscell(a)   
        r.Planos = a;
    else
        error('El argumento de entrada no es un cell array de planos');
    end
elseif nargin == 2  % dos argumentos (ruta y tipo)
    if ischar(a) && ischar(t)
        if strcmp(t, 'dxf')  % archivo dxf
            r.Planos = leerDXF(a);
        end
    else
        error('Los argumentos de entrada no son válidos');
    end
end

r.Fuente = [];  % vacío
r.Receptor = [];  % vacío
r.Nombre = '';  % vacío
r.RespuestaImpulsiva = [];  % vacío
r.Volumen = 0;  % no se ha definido
r = class(r, 'Recinto');

%TODO: Falta verificar que el recinto sea cerrado