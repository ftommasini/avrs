function f = Fuente(pos, ori, nom)
% FUENTE Constructor de la clase Fuente (fuente omnidireccional)
%   f = Fuente(pos) crea una fuente en la posici�n pos, la posici�n debe
%   ser indicada como [x, y, z]
%   f = Fuente(pos, nom) crea una fuente en la posici�n pos 
%   y con el nombre nom

% Modificado 02/2012

if nargin == 0
    f.Nombre = '';  % no tiene nombre
    f.Posicion = [0 0 0];  % en la posici�n [0 0 0] por defecto
    f.Azimut = 0;
    f.Elevacion = 0;
    f.Orden = 0;  % Orden 0
    f.DistReceptor = 0;
    f = class(f, 'Fuente');
elseif nargin == 1
    f.Nombre = '';  % no tiene nombre
    f.Posicion = pos;
    f.Azimut = 0;
    f.Elevacion = 0;
    f.Orden = 0;
    f.DistReceptor = 0;
    f = class(f, 'Fuente');
elseif nargin == 2
    f.Nombre = '';  % no tiene nombre
    f.Posicion = pos;
    f.Azimut = ori(1);
    f.Elevacion = ori(2);
    f.Orden = 0;
    f.DistReceptor = 0;
    f = class(f, 'Fuente');
elseif nargin == 3
    if ischar(nom)
        f.Nombre = nom;
        f.Posicion = pos;
        f.Azimut = ori(1);
        f.Elevacion = ori(2);
        f.Orden = 0;
        f.DistReceptor = 0;
        f = class(f, 'Fuente');
    end
end