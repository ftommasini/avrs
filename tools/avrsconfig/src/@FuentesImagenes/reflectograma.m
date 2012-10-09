function [t val] = reflectograma(fis, fs, largoRI, splFuente, dibujar)
% REFLECTOGRAMA Obtiene el reflectograma de las fuentes-imágenes
% calculadas
% Argumentos de entrada:
% fis: objecto FuentesImagenes
% fs: frecuencia de muestreo
% largoRI: largo de la respuesta
% slpFuente: SPL de la fuente (en dB)
% dibujar: indica si se dibuja el reflectograma en pantalla
%   0: no se dibuja
%   1: se dibuja (valor por defecto)
% Argumentos de salida:
% t: vector de tiempo (en milisegundos)
% val: vector con las amplitudes del reflectograma (en dB)
%
% Autor: Fabián Tommasini
% Creado: 04/2007
% Modificado: 08/2007

if nargin == 4
    dibujar = 1;  % por defecto dibuja el reflectograma
end

nodos = get(get(fis, 'Arbol'), 'Nodos');
fr = get(nodos{1}, 'Info');
[fivis m asoc] = getVisibles(fis, 1);
seg = zeros(m, 1);  % segundos de arribo para cada fuente visible
val = zeros(largoRI * fs, 1);  % valores del reflectograma

for j = 1:m
    dist = get(fivis{j}, 'DistReceptor');
    seg(j) = calcTiempoDirecta(fivis{j}, fr);    
    sample = roundn(seg(j) * fs, 0) + 1;
         
    if val(sample) == 0
        val(sample) = 1 / dist;
    else
        val(sample) = val(sample) + (1 / dist);
    end
end

t = (0:length(val) - 1)' ./ (fs / 1000);  % intervalos de tiempo (en ms)
ind = find(val);  % se extraen los índices de los valores no nulos
    
% Cálculo de los valores en dB de las fuentes-imágenes
valdb = 20 * log10(val(ind)); 
val(ind) = valdb - splFuente; 
indCero = find(val == 0);
val(indCero) = -90;  % 90 dB de atenuación con respecto a la fuente

if dibujar     
    % Grafica el reflectograma
    figure('Name', 'Reflectograma');
    tDir = calcTiempo(fr) * 1000;
%     h = stem(t, val);
%     set(h, 'Marker', 'none');
    plot(t, val);
    porc = 0.02;
    lmin = (largoRI * 1000) * porc;
    lmax = ind(end) / (fs / 1000) + lmin;
    xlim([-lmin lmax]);
    title(['Tiempo directa: ' num2str(tDir) ' ms']);
    xlabel('Tiempo [ms]');
    ylabel('Amplitud [dB]');
    grid on;
end