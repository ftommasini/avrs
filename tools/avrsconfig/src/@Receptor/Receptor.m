function rec = Receptor(pos, ori, nom)
% RECEPTOR Constructor de la clase Receptor
%   rec = Receptor(pos) crea un receptor en la posici�n pos, la posici�n debe
%   ser indicada como [x, y, z]
%   rec = Receptor(pos, nom) crea un receptor en la posici�n pos 
%   y con el nombre nom

% Modificado: 06/2011

if nargin == 0
    rec.Nombre = '';  % no tiene nombre
    rec.Posicion = [0 0 0];  % en la posici�n [0 0 0] por defecto
    rec.Azimut = 0;
    rec.Elevacion = 0;
    rec = class(rec, 'Receptor');
elseif nargin == 1
    rec.Nombre = '';  % no tiene nombre
    rec.Posicion = pos;
    rec.Azimut = 0;
    rec.Elevacion = 0;
    rec = class(rec, 'Receptor');
elseif nargin == 2
    rec.Nombre = '';  % no tiene nombre
    rec.Posicion = pos;
    rec.Azimut = ori(1);
    rec.Elevacion = ori(2);
    rec = class(rec, 'Receptor');    
elseif nargin == 3
    if ischar(nom)
        rec.Nombre = nom;
        rec.Posicion = pos;
        rec.Azimut = ori(1);
        rec.Elevacion = ori(2);
        rec = class(rec, 'Receptor');       
    end
end