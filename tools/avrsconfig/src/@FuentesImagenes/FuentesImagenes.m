function fis = FuentesImagenes(r, orden)
% FUENTESIMAGENES Constructor de la clase FuentesImagenes
%   fis = FuentesImagenes(r, n) crea un objeto que contiene las fuentes-imagenes de
%   orden n para el recinto r

if nargin == 2
    fis.Arbol = Arbol;
    f = get(r, 'Fuente');
    rec = get(r, 'Receptor');
    distR = norm(get(f, 'Posicion') - get(rec, 'Posicion'));
    f = set(f, 'DistReceptor', distR);
    r = set(r, 'Fuente', f);
    fis.Recinto = r; 
    fis.Orden = orden;
    fis.Calculado = 0;  % valor que indica si se han calculado las FI
    fis = class(fis, 'FuentesImagenes');
else
    error('No hay suficientes argumentos para crear el objeto');
end
