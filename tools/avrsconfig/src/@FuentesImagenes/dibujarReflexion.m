function dibujarReflexion(fis, indice)
% DIBUJARREFLEXION Dibuja la reflexión del sonido para una determinada
% fuente-imagen
% Argumentos de entrada:
% fis: objeto de la clase FuenteImagen
% indice: índice de la fuente imagen a dibujar
%
% Autor: Fabián Tommasini
% Creado: 02/2007
% Modificado: 04/2007


r = get(fis, 'Recinto');
f = get(r, 'Fuente');
rec = get(r, 'Receptor');
n = get(get(fis, 'Arbol'), 'Indice');

if indice <= 0 || indice > n
    error('El índice no es válido');
    return;
end

nodo = getNodo(get(fis, 'Arbol'), indice);  % recupero el nodo
fi = get(nodo, 'Info');  % obtengo la fuente-imagen grabada en el nodo
vs = get(fi, 'Posicion');  % posición de la fuente-imagen
planos = get(r, 'Planos');
plano = planos{get(fi, 'IdPlano')};  % plano que generó la fuente-imagen
orden = get(fi, 'Orden');  % orden de la fuente-imagen
pRec = get(rec, 'Posicion');
ax = axes;
dibujar(ax, r);
grid on;
hold on;
plot3(vs(1), vs(2), vs(3), 'ro');

while orden >= 1  % mientras sea de orden superior se verifica la visibilidad
    % se comprueba el corte por la superficie
    vert = get(plano, 'Vertices');
    coef = coeficientes(plano);  % coeficientes geométricos del plano
    % cálculo de la posición del corte con el plano, para la recta que une a la
    % fuente virtual con el receptor
    A = coef(1);
    B = coef(2);
    C = coef(3);
    D = coef(4);
    ar = vs(1) - pRec(1);  % x(vs) - x(receptor)
    br = vs(2) - pRec(2);  % y(vs) - y(receptor)
    cr = vs(3) - pRec(3);  % z(vs) - z(receptor)
    % cálculo del parámetro t, que indica el lugar de corte con el plano, de las
    % ecuaciones paramétricas de la recta
    denom = A * ar + B * br + C * cr;

    if denom == 0
        error('La recta no corta al plano');
        return;
    end

    t = -(A * pRec(1) + B * pRec(2) + C * pRec(3) + D) / denom;
    pCorte = pRec + (vs - pRec) * t;  % posición de corte de la recta que une la fuente virtual y receptor
    c = [pCorte; pRec];
    line(c(:,1), c(:,2), c(:,3), 'Color', 'k');
    pRec = pCorte;
    idPadre = get(nodo, 'Padre');  % se obtiene el id de la fuente padre

    if idPadre == 1
        break;  % se finaliza porque se llegó a la raíz
    end

    nodo = getNodo(get(fis, 'Arbol'), idPadre);
    fi = get(nodo, 'Info');  % obtengo la fuente-imagen grabada en el nodo
    vs = get(fi, 'Posicion');  % posición de la fuente-imagen
    plano = planos{get(fi, 'IdPlano')};  % plano que generó la fuente-imagen
    orden = get(fi, 'Orden');  % orden de la fuente-imagen
end

pRec = get(f, 'Posicion');
c = [pCorte; pRec];
line(c(:,1), c(:,2), c(:,3), 'Color', 'k');
axis equal;
