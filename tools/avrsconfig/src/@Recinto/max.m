function [xMax yMax zMax] = max(r)
% MAX Calcula los puntos máximos de un recinto

xMax = 0;
yMax = 0;
zMax = 0;
planos = get(r, 'Planos');

for i = 1:length(planos)
    v = get(planos{i}, 'Vertices');
    xMaxT = max(v(:,1));
    yMaxT = max(v(:,2));
    zMaxT = max(v(:,3));
    
    if xMaxT > xMax
        xMax = xMaxT;
    end
    
    if yMaxT > yMax
        yMax = yMaxT;
    end
    
    if zMaxT > zMax
        zMax = zMaxT;
    end
end
