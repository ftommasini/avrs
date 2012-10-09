function guardarMateriales(ms)

materiales = ms.Materiales;
% 
% fid = fopen(ms.Ruta, 'w');
% 
% for i = 1:ms.Indice
%     nom = get(materiales{i}, 'Nombre');
%     coefAbs = get(materiales{i}, 'CoefAbsorcion');
%     fwrite(fid, nom, 'schar');  % graba el nombre del material
%     fprintf(fid, coefAbs, 'float32');  % graba los coeficientes de absorci�n
% end
% 
% fclose(fid);

nombres = cell(ms.Indice, 1);
coefAbs = zeros(ms.Indice, 6);

for i = 1:ms.Indice
    nombres{i} = get(materiales{i}, 'Nombre');
    coefAbs(i,:) = get(materiales{i}, 'CoefAbsorcion');
end

save(ms.Ruta, 'nombres', 'coefAbs');
