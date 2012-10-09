function cargar_recinto(handles)

% Dibuja el recinto
axes(handles.axRecinto);
cla;
dibujar(handles.axRecinto, handles.rto);   

% Obtiene los nombres de las superficies y sus materiales
p = get(handles.rto, 'Planos');
datasup = cell(length(p), 1);  

for i = 1:length(p)
    txt = get(p{i}, 'Nombre');  % nombre del plano    
    mat = get(p{i}, 'MaterialArq');  % material del plano dado
    nomMat = get(mat, 'Nombre');
    
    if isempty(nomMat)
        nomMat = '[unassigned]';
    end
    
    txt = [txt ' - ' nomMat];    
    datasup{i} = txt;
end

set(handles.lstSup, 'String', datasup);

% Carga los datos de fuente y receptor
rec = get(handles.rto, 'Receptor');

if isempty(rec)
    posR = [0 0 0];
else
    posR = get(rec, 'Posicion');
end

set(handles.txtXR, 'String', num2str(posR(1)));
set(handles.txtYR, 'String', num2str(posR(2)));
set(handles.txtZR, 'String', num2str(posR(3)));

f = get(handles.rto, 'Fuente');

if isempty(f)
    posF = [0 0 0];
else
    posF = get(f, 'Posicion');
end

set(handles.txtXF, 'String', num2str(posF(1)));
set(handles.txtYF, 'String', num2str(posF(2)));
set(handles.txtZF, 'String', num2str(posF(3)));

end