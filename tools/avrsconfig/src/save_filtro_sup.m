function retvalue = save_filtro_sup(bb, aa, nSup, orden, filename)
%SAVE_CONF Summary of this function goes here
%   Detailed explanation goes here

fid = fopen(filename, 'w+', 'n', 'UTF-8');

if fid < 0
    retvalue = 0;
    return;
end

% Header
fwrite(fid, nSup, 'uint32');
fwrite(fid, orden, 'uint32');

for i = 1:nSup
    fwrite(fid, bb(i,:), 'double');
    fwrite(fid, aa(i,:), 'double');
end

fclose(fid);
retvalue = 1;

end

