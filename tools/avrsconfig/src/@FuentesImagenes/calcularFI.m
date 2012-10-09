function fis = calcularFI(fis, grabar)
% CALCULARFI Calcula fuentes-im�genes
% Argumentos de entrada:
% fis: objeto FuentesImagenes
% Argumentos de salida:
% fis: objeto FuentesImagenes actualizado
% grabar: indica si se graba el resultado en el archivo fis.mat
% (subdirectorio Data)
%   0: no se graba ning�n archivo
%   1: se graba el archivo (opci�n por defecto)
%
% Autor: Fabi�n Tommasini
% Creado: 10/2006
% �ltima modificaci�n: 08/2007

global MAX_ORDEN;
global TFI;

if nargin == 1
    grabar = 1;
end

if fis.Calculado == 1  % verifica si ya se ha calculado (para no calcular nuevamente)
    return;
end

r = fis.Recinto;
f = get(r, 'Fuente');

if isempty(f) || isempty(r)
    error('No se defini� la fuente o el receptor');
end

planos = get(r, 'Planos');  % obtiene los planos del recinto
TFI = fis.Arbol;
MAX_ORDEN = fis.Orden;
nodo = NodoArbol(f, -1);  % se agrega el nodo ra�z (fuente real)
TFI = agregarNodo(TFI, nodo);
padre = getIdNodo(TFI, nodo);
propagar(r, f, padre, planos);  % comienza la propagaci�n de las fuentes-im�genes
fis.Calculado = 1;  % ya se calcularon las FI
fis.Arbol = TFI;  % valor a devolver

if grabar 
    save([pwd '\Data\fis.mat'], 'fis');
end



% Funci�n recursiva que propaga las fuentes-im�genes
function propagar(r, f, padre, planos, orden)

global MAX_ORDEN;
global TFI;

if nargin == 4
    % se llama a la funci�n para comenzar con la propagaci�n de las
    % fuentes-im�genes, por lo que se comienza con el primer orden
    orden = 1;  
end

if orden > MAX_ORDEN  % condici�n de corte
    return;
end

n = length(planos);  % cantidad de planos
posFuente = get(f, 'Posicion');  % obtiene la posici�n de la fuente (ya sea real o virtual)
posRec = get(get(r, 'Receptor'), 'Posicion');  % obtiene la posici�n del receptor 

for i = 1:n
    % realiza la reflexi�n
    normalp = roundn(normal(planos{i}), -3);
    dpf = distanciaOrigen(planos{i}) - dot(posFuente, normalp);  % distancia del plano a la fuente    
    
    if dpf > 0  % verifica si la fuente es v�lida (si no lo es, se descarta)     
        pvs = roundn(posFuente + 2 * dpf * normalp, -3);  % posici�n de la "progeny vs"
        
       
        % se puede utilizar aqu� el test de proximidad
        % (en este caso se realiza por orden de la fuente)
        
        % en caso de verificar obstrucciones, se debe realizar aqu�           
              
        distR = norm(pvs - posRec);  % distancia de la fuente-im�gen al receptor        
        
        vis1 = visibilidad1(planos{i}, pvs, posRec);  % primer test de visibilidad
        
        % Crea el objeto fuente-imagen y setea par�metros
        fi = FuenteImagen(pvs, orden);
        fi = set(fi, 'Visible', vis1);  
        fi = set(fi, 'IdPlano', i);  % graba el plano que creo la fuente-imagen
        fi = set(fi, 'DistReceptor', distR);
        
        nodo = NodoArbol(fi, padre);
        TFI = agregarNodo(TFI, nodo);  % recuerda el nodo (lo agrega al �rbol)
        idNodo = getIdNodo(TFI, nodo);  % obtiene el �ndice del nuevo padre 
        vis2 = 0;
        
        % Para fuentes-im�genes de orden superior y que son visibles (no se
        % tiene en cuenta las que cortan los bordes [vis1 = 2])
        if orden > 1 && vis1 == 1        
            vis2 = visibilidad2(TFI, idNodo, planos, posRec);  % segundo test de visibilidad
            TFI = setVisibilidad(TFI, idNodo, vis2);           
        end            
          
        propagar(r, fi, idNodo, planos, orden + 1);   
    end
end


