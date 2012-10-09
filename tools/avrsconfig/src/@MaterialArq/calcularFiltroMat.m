function [B A] =  calcularFiltroMat(m, orden, fs, n)
% CALCULARFILTROMAT Calcula los coeficientes B y A del filtro para simular
% los materiales
% Argumentos de entrada:
% m: objeto MaterialArq
% orden: orden del filtro
% fs: frecuencia de sampleo
% n: puntos de resoluci�n
% Argumentos de salida:
% B: coeficientes b del filtro 
% A: coeficientes a del filtro 
%
% Autor: Juan Gorgas
% Modificaciones: Fabi�n Tommasini - Oscar Ramos
% Creado: 2005
% Modificado: 05/2011

coefRef = m.CoefReflexion;
ndiv2 = n / 2;
% resoluci�n en frecuencias
deltaF = fs / n;
% se extiende el coef. de 125 Hz a 0 Hz y el de 4000 Hz a la frecuencia de
% fs/2
coefRef = [coefRef(1) coefRef 0.5*coefRef(end)];
% frecuencias de los coeficientes de abs en Hz
frechz = [0 125 250 500 1000 2000 4000 fs/2];  
% �dem anterior en radianes por muestra
% (esto es: w=2*pi*f/fs (radianes); cuando f=fs/2 resulta w=pi)
frecrad = frechz * (2 * pi / fs);  

% Interpolaci�n
% -------------
% division del espectro en puntos linealmente espaciados y convertido a
% radianes/muestras
frecint = 2*pi*(0:deltaF:fs/2)/fs;  
% interpolacion lineal del logaritmo de los coeficientes de reflexi�n
crint = interp1(frecrad, log(coefRef), frecint', 'linear')';
% ajusta valores m�nimos y m�ximos
% crint = max(realmin, crint);
% crint = min(1, crint);

% Agrega la parte sim�trica
% -------------------------
crint2 = [crint crint(end-1:-1:2)];

% Reconstrucci�n del sistema de fase m�nima
% -----------------------------------------
% (no hace falta el log de crint2 ya que el resultado de la interpolaci�n
% ya est� en logaritmos)
y = real(ifft((crint2)));  % cepstrum
w = [1; 2*ones(ndiv2-1, 1); ones(1 - rem(n,2), 1); zeros(ndiv2-1, 1)]; % ventana para obtenci�n de un sistema de fase m�nima
ri = ifft(exp(fft(w' .* y)));  % obtenci�n de la respuesta impulsiva
rif = fft(ri, n);  % obtenci�n de la respuesta en frecuencias compleja a partir de la respueta impulsiva
rif = rif(1:ndiv2+1);  % utilizaci�n de la primera mitad de los valores hasta la frecuencia de nyquist

% Funci�n de ponderaci�n (ERB)
% ----------------------------
% f�rmula de la ERB para la frecuencia en Hz
% (no confundir con la f�rmula en la que la frecuencia est� en KHz)
wt = funcionERB(fs, n);

% Obtenci�n de los coeficientes B y A del filtro IIR
% --------------------------------------------------
% Se le pasa como par�metro la respuesta en frecuencia de la respuesta impulsiva
% (rif), los puntos de frecuencia de "rif" (frecint -hasta la mitad-), el
% orden del numerador del filtro (ya guardado en la clase), el orden del
% denominador (el mismo que el anterior), los pesos de las frecuencias (wt -de
% la funci�n de ERB-), y n�mero de iteraciones a realizar para que converja el
% filtro (30 en este caso)
[B A] = invfreqz(rif, frecint, orden, orden, wt, 30); 
