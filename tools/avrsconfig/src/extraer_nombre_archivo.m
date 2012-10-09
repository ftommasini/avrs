function filename = extraer_nombre_archivo(full_path)

remain = full_path;

while ~isempty(remain)
   [str, remain] = strtok(remain, filesep);
end

filename = str;

end