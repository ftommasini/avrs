function ruta = extraer_ruta_archivo(full_path)

ind = strfind(full_path, filesep);

if ~isempty(ind)
    ruta = full_path(1:ind(end) - 1);
else
    ruta = full_path;
end

end