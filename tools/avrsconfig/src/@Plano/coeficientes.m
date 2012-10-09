function cp = coeficientes(p)
% COEFICIENTES Calcula los coeficientes del plano
% Argumentos de entrada:
% p: objecto plano
% Argumentos de salida:
% cp: es un vector fila con los coeficientes geometricos del plano:
% cp = [A, B, C, D] donde A, B, C, D son los coeficientes del plano
% 
% Creado por: Daniel Céspedes - Víctor Jasá  (10/2004)
% Modificado por: Fabián Tommasini (10/2006)

% Cálculo de coeficientes
v = get(p, 'Vertices');
a = [v(2,2) - v(1,2), v(2,3) - v(1,3); v(3,2) - v(1,2), v(3,3) - v(1,3)];
b = [v(2,3) - v(1,3), v(2,1) - v(1,1); v(3,3) - v(1,3), v(3,1) - v(1,1)];
c = [v(2,1) - v(1,1), v(2,2) - v(1,2); v(3,1) - v(1,1), v(3,2) - v(1,2)];
A = det(a);
B = det(b);
C = det(c);
D = -1 * (A * v(1,1) + B * v(1,2) + C * v(1,3));
cp = [A B C D];