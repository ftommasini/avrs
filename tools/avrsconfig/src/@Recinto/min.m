function [xMin, yMin, zMin] = min(r)
% MIN Calcula los puntos mínimos de un recinto

xMin = 1e20;
yMin = 1e20;
zMin = 1e20;
planos = get(r, 'Planos');

for i = 1:length(planos)
    v = get(planos{i}, 'Vertices');
    xMinT = min(v(:,1));
    yMinT = min(v(:,2));
    zMinT = min(v(:,3));
    
    if xMinT < xMin 
        xMin=xMinT;
    end
    
    if yMinT < yMin 
        yMin=yMinT;
    end
        
    if zMinT < zMin        
        zMin=zMinT;
    end
end