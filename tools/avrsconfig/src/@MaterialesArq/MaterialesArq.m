function ms = MaterialesArq(ruta)
% MATERIALESARQ Constructor de la clase MaterialesArq
%   ms = MaterialesArq() crea un objeto de la clase MaterialesArq (los
%   datos se encuentran en Data\materiales.mat
%   ms = MaterialesArq(ruta) crea un objeto de la clase MaterialesArq, donde
%   los datos se encuentran en la ruta que se pasa por par�metro
%
% Autor: Fabi�n Tommasini
% Creado: 03/2007
% Modificado: 04/2007

if nargin == 1
    ms.Ruta = ruta;
    ms.Materiales = cell(0);
    ms.Indice = 0;
    ms = class(ms, 'MaterialesArq');
elseif nargin == 0
    ms.Ruta = fullfile(pwd, 'materiales_db.mat');
    ms.Materiales = cell(0);
    ms.Indice = 0;
    ms = class(ms, 'MaterialesArq');
else
    error('N�mero de argumentos de entrada inv�lido');
    return;
end
