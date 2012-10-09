function fi = FuenteImagen(coord, orden)
% FUENTEIMAGEN Constructor de la clase FuenteImagen
%   fi = FuenteImagen(c, o) crea una fuente-imagen en las coordenadas c
%   del orden o

if nargin == 2  % dos argumentos
    fi.Posicion = coord;
    fi.Visible = 1;
    fi.Orden = orden;
    fi.IdPlano = 0;
    fi.DistReceptor = 0;
    fi = class(fi, 'FuenteImagen');
else
    error('Parámetros insuficientes');
end