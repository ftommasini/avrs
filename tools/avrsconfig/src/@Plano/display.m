function display(p)
% DISPLAY Muestra el objeto plano
% Argumentos de entrada:
% p: objeto plano

disp('Objeto Plano');
disp(' ');
nom = get(p, 'Nombre');

if isempty(nom)
    nom = '(Sin nombre)';
end

disp(['Nombre: ' nom]);
disp(' ');
disp('Vértices');
vert = get(p, 'Vertices');
disp(vert);