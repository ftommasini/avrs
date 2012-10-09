function ok = visibilidad2(fis, ind, planos, pRec)
% VISIBILIDAD2 Calcula si la fuente es visible o no para el receptor
% Realiza el segundo test
% Argumentos de entrada:
% fis: �rbol que contiene todas la fuentes-im�genes calculadas
% ind: �ndice del nodo donde se encuentra la fuente-imagen a verificar
% planos: cell array con los planos del recinto
% pRec: coordenadas del receptor
% Argumentos de salida:
% ok: indica si la fuente es visible
%   1: la fuente es visible
%   0: la fuente no es visible
%
% Autor: Fabi�n Tommasini 
% Creado: 02/2007

nodo = getNodo(fis, ind);  % recupero el nodo
fi = get(nodo, 'Info');  % obtengo la fuente-imagen grabada en el nodo
vs = get(fi, 'Posicion');  % posici�n de la fuente-imagen
plano = planos{get(fi, 'IdPlano')};  % plano que gener� la fuente-imagen
orden = get(fi, 'Orden');  % orden de la fuente-imagen

while orden >= 1  % mientras sea de orden superior se verifica la visibilidad
    % se comprueba el corte por la superficie
    coef = coeficientes(plano);  % coeficientes geom�tricos del plano
    % c�lculo de la posici�n del corte con el plano, para la recta que une a la
    % fuente virtual con el receptor
    A = coef(1);
    B = coef(2);
    C = coef(3);
    D = coef(4);
    ar = vs(1) - pRec(1);  % x(vs) - x(receptor)
    br = vs(2) - pRec(2);  % y(vs) - y(receptor)
    cr = vs(3) - pRec(3);  % z(vs) - z(receptor)
    % c�lculo del par�metro t, que indica el lugar de corte con el plano, de las
    % ecuaciones param�tricas de la recta
    denom = A * ar + B * br + C * cr;

    if denom == 0
        ok = 0;
        return;
    end

    t = -(A * pRec(1) + B * pRec(2) + C * pRec(3) + D) / denom;
    pCorte = pRec + (vs - pRec) * t;  % posici�n de corte de la recta que une la fuente virtual y receptor
    dentro = estaDentro(plano, pCorte);

    if dentro == 0 || dentro == 2 % no corta a la superficie o cae sobre un borde
        ok = 0;
        return;  % se termina, la fuente no es visible
    else
        ok = 1;  % se continua verificando
    end
    
    pRec = pCorte;  % el punto de corte es el "nuevo receptor"
    idPadre = get(nodo, 'Padre');  % se obtiene el id de la fuente padre
    
    if idPadre == 1
        break;  % se finaliza porque se lleg� a la ra�z
    end
    
    nodo = getNodo(fis, idPadre);
    fi = get(nodo, 'Info');  % obtengo la fuente-imagen grabada en el nodo
    vs = get(fi, 'Posicion');  % posici�n de la fuente-imagen
    plano = planos{get(fi, 'IdPlano')};  % plano que gener� la fuente-imagen
    orden = get(fi, 'Orden');  % orden de la fuente-imagen
end
