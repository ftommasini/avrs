function ok = visibilidad1(plano, vs, pRec)
% VISIBILIDAD1 Calcula si la fuente es visible o no para el receptor
% Realiza el primer test
% Argumentos de entrada:
% plano: plano sobre el cual se calculará la visibilidad de la fuente virtual
% vs: coordenadas de la fuente virtual
% pRec: coordenadas del receptor
% Argumentos de salida:
% ok: indica si la fuente es visible
%   2: la fuente corta un borde (a determinar el comportamiento)
%   1: la fuente es visible
%   0: la fuente no es visible
%
% Autor: Fabián Tommasini 
% Creado: 10/2006
% Modificado: 12/2006

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
    ok = 0;
    return;
end

t = -(A * pRec(1) + B * pRec(2) + C * pRec(3) + D) / denom;
pCorte = pRec + (vs - pRec) * t;  % posición de corte de la recta que une la fuente virtual y al receptor
ok = estaDentro(plano, pCorte);  % verifica si el punto de corte está dentro del plano dado
