function m = MaterialArq(nom, coefAbs)
% MATERIALARQ Constructor de la clase MaterialArq
%   m = MaterialArq(nom, coefAbs) crea un material con el nombre y los 
%   coeficientes de absorción (Sabine)
%
% Autor: Fabián Tommasini
% Creado: 03/2007
% Modificado: 04/2007

if nargin < 2
    error('Número de argumentos de entrada inválido');   
elseif length(coefAbs) ~= 6  
    % 6 coeficientes de absorción para 125, 250, 500, 1000, 2000, 4000 Hz
    error('Número de coeficientes de absorción inválido'); 
end

if nargin == 2
    m.Nombre = nom;
    m.CoefAbsorcion = coefAbs;  % coeficientes de absorción Sabine
    coefAbs2 = 1 - exp(-coefAbs);  % corrección de Norris-Eyring
    m.CoefReflexion = sqrt(1 - coefAbs2);  % coeficientes de reflectancia
    m = class(m, 'MaterialArq');
else
    error('Número de argumentos de entrada inválido'); 
    return;
end
