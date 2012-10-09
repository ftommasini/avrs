function fis = calcularFI2(fis, dist, grabar)
% CALCULARFI2 Calcula fuentes-imágenes (hasta determinada distancia)
% Argumentos de entrada:
% fis: objeto FuentesImagenes
% Argumentos de salida:
% fis: objeto FuentesImagenes actualizado
% dist: distancia máxima para el cálculo de fuentes-imágenes
% grabar: indica si se graba el resultado en el archivo fis.mat
% (subdirectorio Data)
%   0: no se graba ningún archivo
%   1: se graba el archivo (opción por defecto)
%
% Autor: Fabián Tommasini
% Creado: 10/2006
% Última modificación: 08/2007

global MAX_ORDEN;
global MAX_DIST;
global TFI;

if nargin == 2
    grabar = 1;
end

if fis.Calculado == 1  % verifica si ya se ha calculado (para no calcular nuevamente)
    return;
end

r = fis.Recinto;
f = get(r, 'Fuente');

if isempty(f) || isempty(r)
    error('No se definió la fuente o el receptor');
end

planos = get(r, 'Planos');  % obtiene los planos del recinto
TFI = fis.Arbol;
MAX_ORDEN = fis.Orden;
MAX_DIST = dist;
nodo = NodoArbol(f, -1);  % se agrega el nodo raíz (fuente real)
TFI = agregarNodo(TFI, nodo);
padre = getIdNodo(TFI, nodo);
propagar(r, f, padre, planos);  % comienza la propagación de las fuentes-imágenes
fis.Calculado = 1;  % ya se calcularon las FI
fis.Arbol = TFI;  % valor a devolver

if grabar 
    save([pwd '\Data\fis.mat'], 'fis');
end



% Función recursiva que propaga las fuentes-imágenes
function propagar(r, f, padre, planos, orden)

global MAX_ORDEN;
global MAX_DIST;
global TFI;

if nargin == 4
    % se llama a la función para comenzar con la propagación de las
    % fuentes-imágenes, por lo que se comienza con el primer orden
    orden = 1;  
end

if orden > MAX_ORDEN  % condición de corte
    return;
end

n = length(planos);  % cantidad de planos
posFuente = get(f, 'Posicion');  % obtiene la posición de la fuente (ya sea real o virtual)
posRec = get(get(r, 'Receptor'), 'Posicion');  % obtiene la posición del receptor 

for i = 1:n
    % realiza la reflexión
    normalp = normal(planos{i});
    dpf = distanciaOrigen(planos{i}) - dot(posFuente, normalp);  % distancia del plano a la fuente    
    
    if dpf > 0  % verifica si la fuente es válida (si no lo es, se descarta)     
        pvs = posFuente + 2 * dpf * normalp;  % posición de la "progeny vs"
        
%         if roundn(pvs, -3) == [-40, 0, 18.5]
%             disp('aca');
%         end
        
        % se puede utilizar aquí el test de proximidad
        % (en este caso se realiza por orden de la fuente)
        
        % en caso de verificar obstrucciones, se debe realizar aquí           
              
        distR = norm(pvs - posRec);  % distancia de la fuente-imágen al receptor  
        
        if distR <= MAX_DIST  % verifica que esté dentro de la distancia indicada        
            vis1 = visibilidad1(planos{i}, pvs, posRec);  % primer test de visibilidad
        
            % Crea el objeto fuente-imagen y setea parámetros
            fi = FuenteImagen(pvs, orden);
            fi = set(fi, 'Visible', vis1);
            fi = set(fi, 'IdPlano', i);  % graba el plano que creo la fuente-imagen
            fi = set(fi, 'DistReceptor', distR);

            nodo = NodoArbol(fi, padre);
            TFI = agregarNodo(TFI, nodo);  % recuerda el nodo (lo agrega al árbol)
            idNodo = getIdNodo(TFI, nodo);  % obtiene el índice del nuevo padre
            vis2 = 0;

            % Para fuentes-imágenes de orden superior y que son visibles (no se
            % tiene en cuenta las que cortan los bordes [vis1 = 2])
            if orden > 1 && vis1 == 1
                vis2 = visibilidad2(TFI, idNodo, planos, posRec);  % segundo test de visibilidad
                TFI = setVisibilidad(TFI, idNodo, vis2);
            end

            propagar(r, fi, idNodo, planos, orden + 1);
        end         
    end
end


