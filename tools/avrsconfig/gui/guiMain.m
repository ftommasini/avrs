function varargout = guiMain(varargin)
% GUIMAIN M-file for guiMain.fig
%      GUIMAIN, by itself, creates a new GUIMAIN or raises the existing
%      singleton*.
%
%      H = GUIMAIN returns the handle to a new GUIMAIN or the handle to
%      the existing singleton*.
%
%      GUIMAIN('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUIMAIN.M with the given input arguments.
%
%      GUIMAIN('Property','Value',...) creates a new GUIMAIN or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before guiMain_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to guiMain_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help guiMain

% Last Modified by GUIDE v2.5 13-Feb-2012 15:13:08

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @guiMain_OpeningFcn, ...
                   'gui_OutputFcn',  @guiMain_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before guiMain is made visible.
function guiMain_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to guiMain (see VARARGIN)

% Choose default command line output for guiMain
handles.output = hObject;

handles.figRecinto = 0;
handles.axRecinto = 0;
handles.rtoCargado = 0;
handles.fs = 0;
handles.ordenFI = 0;
handles.distMax = 0;
handles.conf = crear_conf;
handles.mats = MaterialesArq();
handles.mats = cargarMateriales(handles.mats);
set(handles.cboMat, 'String', getNombres(handles.mats));
%actualizarListaMat(hObject, handles);

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes guiMain wait for user response (see UIRESUME)
% uiwait(handles.guiMain);


% --- Outputs from this function are returned to the command line.
function varargout = guiMain_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes during object creation, after setting all properties.
function guiMain_CreateFcn(hObject, eventdata, handles)
% hObject    handle to guiMain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called



% --- Executes on selection change in lstSup.
function lstSup_Callback(hObject, eventdata, handles)
% hObject    handle to lstSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns lstSup contents as cell array
%        contents{get(hObject,'Value')} returns selected item from lstSup
ind = get(hObject, 'Value');

if ind == 0
    return;
end

planos = get(handles.rto, 'Planos');
p = planos{ind};
cargar_recinto(handles);
pintar(handles.axRecinto, p);
a = area3D(p);
set(handles.txtArea, 'String', ['Area: ' num2str(roundn(a,-3)) ' m2']);



% --- Executes during object creation, after setting all properties.
function lstSup_CreateFcn(hObject, eventdata, handles)
% hObject    handle to lstSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in cboMat.
function cboMat_Callback(hObject, eventdata, handles)
% hObject    handle to cboMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns cboMat contents as cell array
%        contents{get(hObject,'Value')} returns selected item from cboMat
ind = get(hObject, 'Value');
mats = get(handles.mats, 'Materiales');  
mat = mats{ind};  % obtengo el material
axes(handles.axMat);
% Coeficientes de absorci�n
coefAbs = get(mat, 'CoefAbsorcion');
sCoef = [num2str(coefAbs(1)) ', ' num2str(coefAbs(2)) ', ' num2str(coefAbs(3)) ', ' ...
    num2str(coefAbs(4)) ', ' num2str(coefAbs(5)) ', ' num2str(coefAbs(6))];
set(handles.txtCoef, 'String', sCoef);
xLabel = {'125', '250', '500', '1000', '2000', '4000'}; 
bar(coefAbs, 1);
set(handles.axMat, 'XTickLabel', xLabel);
ylim([0 1]);  % l�mites de ordenada entre 0 y 1
grid on;



% --- Executes during object creation, after setting all properties.
function cboMat_CreateFcn(hObject, eventdata, handles)
% hObject    handle to cboMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on button press in butAsignarSel.
function butAsignarSel_Callback(hObject, eventdata, handles)
% hObject    handle to butAsignarSel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if handles.rtoCargado == 0
    return;
end

indMat = get(handles.cboMat, 'Value');
matsarq = get(handles.mats, 'Materiales');
mat = matsarq{indMat};  % obtengo el material seleccionado
indPlano = get(handles.lstSup, 'Value');
planos = get(handles.rto, 'Planos');
planos{indPlano} = set(planos{indPlano}, 'MaterialArq', mat);  % asigno el material
handles.rto = set(handles.rto, 'Planos', planos);
guidata(hObject, handles);
cargar_recinto(handles);


% --- Executes on button press in butAsignarTodos.
function butAsignarTodos_Callback(hObject, eventdata, handles)
% hObject    handle to butAsignarTodos (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if handles.rtoCargado == 0
    return;
end

indMat = get(handles.cboMat, 'Value');
matsarq = get(handles.mats, 'Materiales');
mat = matsarq{indMat};  % obtengo el material seleccionado
planos = get(handles.rto, 'Planos');
nPlanos = length(planos);

for i = 1:nPlanos
    planos{i} = set(planos{i}, 'MaterialArq', mat);
end

handles.rto = set(handles.rto, 'Planos', planos);
guidata(hObject, handles);
cargar_recinto(handles);


% --------------------------------------------------------------------
function mnuAbrir_Callback(hObject, eventdata, handles)
% hObject    handle to mnuAbrir (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile(['conf' filesep '*.conf'], 'Load configuration file');

if ~isempty(nombre) && ischar(nombre)
    nombreDir = [nombre(1:end-5) '_conf'];
    nombreMat = [nombre(1:end-5) '.mat'];    
    load('-mat', [ruta filesep nombreDir filesep nombreMat], 'rto', 'c');  % carga el recinto desde el archivo
    handles.rto = rto;  % se asigna el recinto al handles
    handles.rtoCargado = 1;  % indica que el recinto ya está cargado
    handles.conf = c;
    guidata(hObject, handles);
    
    if handles.figRecinto == 0  % no hay recinto cargado
        handles.figRecinto = figure('Name', 'Room');
        %set(handles.figRecinto,'closer', '')
        handles.axRecinto = axes;
    end

    %movegui('center');
    grid on;
    view(3);  % vista tridimensional de los ejes
    axis equal;    
    cargar_recinto(handles);
    guidata(hObject, handles);  % guarda las modificaciones al handles 
    
    set(handles.txtDXF, 'String', [ruta c.room_dxf_file]);
    set(handles.txtVol, 'String', num2str(c.room_volume));
    set(handles.txtDist, 'String', num2str(c.ism_max_distance));
    set(handles.txtOrdenFI, 'String', num2str(c.ism_max_order));
    set(handles.txtFileSup, 'String', [ruta c.room_filter_surfaces_file]);
    set(handles.txtXF, 'String', num2str(c.sound_source_position(1)));
    set(handles.txtYF, 'String', num2str(c.sound_source_position(2)));
    set(handles.txtZF, 'String', num2str(c.sound_source_position(3)));
    set(handles.txtAzF, 'String', num2str(c.sound_source_orientation(1)));
    set(handles.txtElF, 'String', num2str(c.sound_source_orientation(2)));
    set(handles.txtDirect, 'String', [ruta c.sound_source_directivity_file]);
    set(handles.txtXR, 'String', num2str(c.listener_position(1)));
    set(handles.txtYR, 'String', num2str(c.listener_position(2)));
    set(handles.txtZR, 'String', num2str(c.listener_position(3)));
    set(handles.txtAzR, 'String', num2str(c.listener_orientation(1)));
    set(handles.txtElR, 'String', num2str(c.listener_orientation(2)));
    set(handles.txtHRTFSp, 'String', [ruta c.listener_hrtf_file]);
    set(handles.txtHRTFFilt, 'String', [ruta c.listener_filter_hrtf_file]);
    set(handles.txtInput, 'String', [ruta c.anechoic_file]);
    set(handles.txtMasterGain, 'String', num2str(c.master_gain_db));
    set(handles.txtTemp, 'String', num2str(c.temperature));
    set(handles.txtAngle, 'String', num2str(c.angle_threshold));
    set(handles.txtBIRLen, 'String', num2str(c.bir_length));
    
    % activa elementos
    set(handles.butLimpiar, 'Enable', 'on');
end


% --------------------------------------------------------------------
function mnuGuardar_Callback(hObject, eventdata, handles)
% hObject    handle to mnuGuardar (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uiputfile(['conf' filesep '*.conf'], 'Save configuration');
len = size(nombre, 2);

if isempty(nombre) || ~ischar(nombre)
    return;
end

% controla la extensión de archivo
if ~strcmp(nombre(len - 4:len), '.conf')
    nombre = [nombre '.conf'];
    nombreDir = [nombre '_conf'];
else
    nombreDir = [nombre(1:end-5) '_conf'];
end

if ~exist(['conf' filesep nombreDir], 'dir')
    mkdir('conf', nombreDir);
end

c = crear_conf;

% DXF file
fdxf = get(handles.txtDXF, 'String');

if isempty(fdxf) || ~exist(fdxf, 'file')
    errordlg('Error in DXF file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fdxf), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fdxf, ['conf' filesep nombreDir]);
end

c.room_dxf_file = [nombreDir filesep extraer_nombre_archivo(fdxf)];

% Room volume
vol = str2double(get(handles.txtVol, 'String'));

if isnan(vol) || vol <= 0
    errordlg('Error in Room volume', 'AVRSConfig');
    return;
end

c.room_volume = vol;

% Surfaces
planos = get(handles.rto, 'Planos');
nPlanos = length(planos);

if nPlanos <= 0
    errordlg('Error in Surfaces', 'AVRSConfig');
    return;
end

c.room_n_surfaces = nPlanos;

% Filter of surfaces
fsup = get(handles.txtFileSup, 'String');

if isempty(fsup) || ~exist(fsup, 'file')
    errordlg('Error in Surfaces filter file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fsup), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fsup, ['conf' filesep nombreDir]);
end

c.room_filter_surfaces_file = [nombreDir filesep extraer_nombre_archivo(fsup)];

% ISM
maxDist = str2double(get(handles.txtDist, 'String'));
maxOrden = str2num(get(handles.txtOrdenFI, 'String'));

if isnan(maxDist) || maxDist <= 0 || isnan(maxOrden) || maxOrden <= 0
    errordlg('Error in ISM parameters', 'AVRSConfig');
    return;
end

c.ism_max_order = maxOrden;
c.ism_max_distance = maxDist;

% Sound source position and orientation
f = get(handles.rto, 'Fuente');
posF = get(f, 'Posicion');
azF = get(f, 'Azimut');
elF = get(f, 'Elevacion');

if ~isempty(find(isnan(posF))) || isnan(azF) || isnan(elF)
    errordlg('Error in Sound source', 'AVRSConfig');
    return;
end

c.sound_source_position = posF;
c.sound_source_orientation = [azF elF];

% Sound source directivity
fdir = get(handles.txtDirect, 'String');

if isempty(fdir) || ~exist(fdir, 'file')
    errordlg('Error in Sound source directivity file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fdir), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fdir, ['conf' filesep nombreDir]);
end

c.sound_source_ir_file = [nombreDir filesep extraer_nombre_archivo(fdir)];
c.sound_source_directivity_file = [nombreDir filesep extraer_nombre_archivo(fdir)];

% Listener position and orientation
rec = get(handles.rto, 'Receptor');
posR = get(rec, 'Posicion');
azR = get(rec, 'Azimut');
elR = get(rec, 'Elevacion');

if ~isempty(find(isnan(posR))) || isnan(azR) || isnan(elR)
    errordlg('Error in Listener', 'AVRSConfig');
    return;
end

c.listener_position = posR;
c.listener_orientation = [azR elR];

% HRTF files
fhsp = get(handles.txtHRTFSp, 'String');

if isempty(fhsp) || ~exist(fhsp, 'file')
    errordlg('Error in HRTF spectrums file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fhsp), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fhsp, ['conf' filesep nombreDir]);
end

c.listener_hrtf_file = [nombreDir filesep extraer_nombre_archivo(fhsp)];

fhf = get(handles.txtHRTFFilt, 'String');

if isempty(fhf) || ~exist(fhf, 'file')
    errordlg('Error in HRTF filters file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fhf), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fhf, ['conf' filesep nombreDir]);
end

c.listener_filter_hrtf_file = [nombreDir filesep extraer_nombre_archivo(fhf)];

% Input
fin = get(handles.txtInput, 'String');

if isempty(fin) || ~exist(fin, 'file')
    errordlg('Error in Input file', 'AVRSConfig');
    return;
end

% verifica que no trate de copiar el archivo en sí mismo
if ~strcmp(extraer_ruta_archivo(fin), [pwd filesep 'conf' filesep nombreDir])
    copyfile(fin, ['conf' filesep nombreDir]);
end

c.anechoic_file = [nombreDir filesep extraer_nombre_archivo(fin)];

% Output gain
gain = str2double(get(handles.txtMasterGain, 'String'));

if isnan(gain)
    errordlg('Error in Output gain', 'AVRSConfig');
    return;
end

c.master_gain_db = gain;

% General
temp = str2double(get(handles.txtTemp, 'String'));
angle = str2double(get(handles.txtAngle, 'String'));
birl = str2double(get(handles.txtBIRLen, 'String'));

if isnan(temp) || isnan(angle) || angle <= 0 || isnan(birl) || birl <= 0
    errordlg('Error in General parameters', 'AVRSConfig');
    return;
end

c.temperature = temp;
c.angle_threshold = angle;
c.bir_length = birl;

handles.conf = c;
guidata(hObject, handles);

% Save config
ok = save_conf(c, [ruta nombre]);

% Save mat file
nombreMat = [nombre(1:end-5) '.mat'];
rto = handles.rto;  % graba el recinto en una variable
save(['conf' filesep nombreDir filesep nombreMat], 'rto', 'c');  % graba el archivo (-mat por defecto)

if ok
    msgbox('Configuration saved', 'AVRSConfig');
end



% function ok = saveConfig(filename, handles)
% 
% fid = fopen(filename, 'w+', 'n');
% 
% p = get(handles.rto, 'Planos');
% 
% for i = 1:length(p)
%     mat = get(p{i}, 'MaterialArq');  % material del plano dado
%     nomMat = get(mat, 'Nombre');
%     
%     if ~isempty(nomMat)
%         [b a] = calcularFiltroMat(mat, 4, 44100, 1024);
%         bstring = num2str(b,'%1.15f, ');  % double-precision
%         bstring = bstring(1:end-2);
%         astring = num2str(a,'%1.15f, ');  % double-precision
%         astring = astring(1:end-2);
%     else
%         bstring = '';
%         astring = '';
%     end
%     
%     fprintf(fid, 'SURFACE_%d_FILTER_B = %s\n', i, bstring);
%     fprintf(fid, 'SURFACE_%d_FILTER_A = %s\n', i, astring); 
% end
% 
% fprintf(fid, 'ISM_MAX_ORDER = %s\n', num2str(handles.ordenFI));
% fprintf(fid, 'ISM_MAX_DISTANCE = %s\n', num2str(handles.distMax));
% 
% fclose(fid);
% ok = 1;


function txtXF_Callback(hObject, eventdata, handles)
% hObject    handle to txtXF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtXF as text
%        str2double(get(hObject,'String')) returns contents of txtXF as a double



% --- Executes during object creation, after setting all properties.
function txtXF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtXF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtYF_Callback(hObject, eventdata, handles)
% hObject    handle to txtYF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtYF as text
%        str2double(get(hObject,'String')) returns contents of txtYF as a double


% --- Executes during object creation, after setting all properties.
function txtYF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtYF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtZF_Callback(hObject, eventdata, handles)
% hObject    handle to txtZF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtZF as text
%        str2double(get(hObject,'String')) returns contents of txtZF as a double


% --- Executes during object creation, after setting all properties.
function txtZF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtZF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function txtXR_Callback(hObject, eventdata, handles)
% hObject    handle to txtXR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtXR as text
%        str2double(get(hObject,'String')) returns contents of txtXR as a double


% --- Executes during object creation, after setting all properties.
function txtXR_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtXR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function txtYR_Callback(hObject, eventdata, handles)
% hObject    handle to txtYR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtYR as text
%        str2double(get(hObject,'String')) returns contents of txtYR as a double


% --- Executes during object creation, after setting all properties.
function txtYR_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtYR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtZR_Callback(hObject, eventdata, handles)
% hObject    handle to txtZR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtZR as text
%        str2double(get(hObject,'String')) returns contents of txtZR as a double


% --- Executes during object creation, after setting all properties.
function txtZR_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtZR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butAplicarF.
function butAplicarF_Callback(hObject, eventdata, handles)
% hObject    handle to butAplicarF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
x = str2double(get(handles.txtXF, 'String'));
y = str2double(get(handles.txtYF, 'String'));
z = str2double(get(handles.txtZF, 'String'));
az = str2double(get(handles.txtAzF, 'String'));
el = str2double(get(handles.txtElF, 'String'));

if isnan(x) || isnan(y) || isnan(z) || isnan(az) || isnan(el)
    errordlg('Error', 'AVRSConfig');
    return;
end

f = Fuente([x y z], [az el]);  % crea la fuente
handles.rto = set(handles.rto, 'Fuente', f);
guidata(hObject, handles);
cargar_recinto(handles);


% --- Executes on button press in butAplicarR.
function butAplicarR_Callback(hObject, eventdata, handles)
% hObject    handle to butAplicarR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
x = str2double(get(handles.txtXR, 'String'));
y = str2double(get(handles.txtYR, 'String'));
z = str2double(get(handles.txtZR, 'String'));
az = str2double(get(handles.txtAzR, 'String'));
el = str2double(get(handles.txtElR, 'String'));

if isnan(x) || isnan(y) || isnan(z) || isnan(az) || isnan(el)
    errordlg('Error', 'AVRSConfig');
    return;
end

rec = Receptor([x y z], [az, el]);  % crea el receptor
handles.rto = set(handles.rto, 'Receptor', rec);
guidata(hObject, handles);
cargar_recinto(handles);


% --- Executes on button press in butLimpiar.
function butLimpiar_Callback(hObject, eventdata, handles)
% hObject    handle to butLimpiar (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
cargar_recinto(handles);
set(handles.txtArea, 'String', '');


% --- Executes when user attempts to close guiMain.
function guiMain_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to guiMain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure
% if handles.figRecinto ~= 0
%     delete(handles.figRecinto);
% end

delete(hObject);


% function actualizarListaMat(hObject, handles)
% handles.mats = MaterialesArq();
% handles.mats = cargarMateriales(handles.mats);
% guidata(hObject, handles);
% set(handles.cboMat, 'String', getNombres(handles.mats));


function edit23_Callback(hObject, eventdata, handles)
% hObject    handle to txtFrecMuestreo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtFrecMuestreo as text
%        str2double(get(hObject,'String')) returns contents of txtFrecMuestreo as a double


% --- Executes during object creation, after setting all properties.
function edit23_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtFrecMuestreo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



% --- Executes on button press in butAreaTotal.
function butAreaTotal_Callback(hObject, eventdata, handles)
% hObject    handle to butAreaTotal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if handles.rtoCargado
    a = areaTotal(handles.rto);
    msgbox(['Total area: ' num2str(roundn(a, -3)) ' m^2'], 'AVRSConfig');
end


function edit26_Callback(hObject, eventdata, handles)
% hObject    handle to edit26 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit26 as text
%        str2double(get(hObject,'String')) returns contents of edit26 as a double


% --- Executes during object creation, after setting all properties.
function edit26_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit26 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtVol_Callback(hObject, eventdata, handles)
% hObject    handle to txtVol (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtVol as text
%        str2double(get(hObject,'String')) returns contents of txtVol as a double


% --- Executes during object creation, after setting all properties.
function txtVol_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtVol (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butVol.
function butVol_Callback(hObject, eventdata, handles)
% hObject    handle to butVol (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
rto = handles.rto;
V = str2double(get(handles.txtVol, 'String'));
rto = set(rto, 'Volumen', V);
handles.rto = rto;
guidata(hObject, handles);


function txtDist_Callback(hObject, eventdata, handles)
% hObject    handle to txtDist (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtDist as text
%        str2double(get(hObject,'String')) returns contents of txtDist as a double


% --- Executes during object creation, after setting all properties.
function txtDist_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtDist (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in chkDist.
function chkDist_Callback(hObject, eventdata, handles)
% hObject    handle to chkDist (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of chkDist
val = get(hObject, 'Value');

if val == 0
    set(handles.txtDist,'Enable','off');
elseif val == 1
    set(handles.txtDist,'Enable','on');
end



% --- Executes on button press in checkbox4.
function checkbox4_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox4



function txtOrdenSup_Callback(hObject, eventdata, handles)
% hObject    handle to txtOrdenSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtOrdenSup as text
%        str2double(get(hObject,'String')) returns contents of txtOrdenSup as a double


% --- Executes during object creation, after setting all properties.
function txtOrdenSup_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtOrdenSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function txtResSup_Callback(hObject, eventdata, handles)
% hObject    handle to txtResSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtResSup as text
%        str2double(get(hObject,'String')) returns contents of txtResSup as a double


% --- Executes during object creation, after setting all properties.
function txtResSup_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtResSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butBorrarMat.
function butBorrarMat_Callback(hObject, eventdata, handles)
% hObject    handle to butBorrarMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
indMat = get(handles.cboMat, 'Value');
borrarMaterial(handles.mats, indMat);
guardarMateriales(handles.mats);
actualizarListaMat(hObject, handles);


function txtAzR_Callback(hObject, eventdata, handles)
% hObject    handle to txtAzR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtAzR as text
%        str2double(get(hObject,'String')) returns contents of txtAzR as a double


% --- Executes during object creation, after setting all properties.
function txtAzR_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtAzR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtElR_Callback(hObject, eventdata, handles)
% hObject    handle to txtElR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtElR as text
%        str2double(get(hObject,'String')) returns contents of txtElR as a double


% --- Executes during object creation, after setting all properties.
function txtElR_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtElR (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



% --- Executes on button press in butOri2F.
function butOri2F_Callback(hObject, eventdata, handles)
% hObject    handle to butOri2F (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
rto = handles.rto;
f = get(rto, 'Fuente');
rec = get(rto, 'Receptor');
[az el] = apuntarFuente(rec, f);
rec = set(rec, 'Azimut', az);
rec = set(rec, 'Elevacion', el);
handles.rto = set(handles.rto, 'Receptor', rec);
guidata(hObject, handles);
set(handles.txtAzR, 'String', num2str(az));
set(handles.txtElR, 'String', num2str(el));
cargar_recinto(handles);


function txtDXF_Callback(hObject, eventdata, handles)
% hObject    handle to txtDXF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtDXF as text
%        str2double(get(hObject,'String')) returns contents of txtDXF as a double


% --- Executes during object creation, after setting all properties.
function txtDXF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtDXF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit35_Callback(hObject, eventdata, handles)
% hObject    handle to txtVol (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtVol as text
%        str2double(get(hObject,'String')) returns contents of txtVol as a double



% --- Executes during object creation, after setting all properties.
function edit35_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtVol (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function txtOrdenFI_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtOrdenFI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function txtDirect_Callback(hObject, eventdata, handles)
% hObject    handle to txtDirect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtDirect as text
%        str2double(get(hObject,'String')) returns contents of txtDirect as a double


% --- Executes during object creation, after setting all properties.
function txtDirect_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtDirect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butDirect.
function butDirect_Callback(hObject, eventdata, handles)
% hObject    handle to butDirect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile('*.*', 'Directivity file');

try
    if ~isempty(nombre) && ischar(nombre)
        set(handles.txtDirect, 'String', [ruta nombre]);
    end
catch
    errordlg('Load error...', 'Error');
end


% --- Executes on button press in butDXF.
function butDXF_Callback(hObject, eventdata, handles)
% hObject    handle to butDXF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile(['dxf' filesep '*.dxf'], 'DXF file');

try
    if ~isempty(nombre) && ischar(nombre)
        handles.rto = Recinto([ruta nombre], 'dxf');
        handles.rtoCargado = 1;  % indica que el recinto ya está cargado    
        
        if handles.figRecinto == 0  % no hay recinto cargado
            handles.figRecinto = figure('Name', 'Room');
            %set(handles.figRecinto,'closer', '')
            handles.axRecinto = axes;
        end

        %movegui('center');
        grid on;
        view(3);  % vista tridimensional de los ejes
        axis equal;
        guidata(hObject, handles);  % guarda las modificaciones al handles     
        cargar_recinto(handles);
        set(handles.txtDXF, 'String', [ruta nombre]);
        
        % activa elementos
        set(handles.butLimpiar, 'Enable', 'on');
    end
catch
    errordlg('Load error...', 'Error');
end



function txtAzF_Callback(hObject, eventdata, handles)
% hObject    handle to txtAzF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtAzF as text
%        str2double(get(hObject,'String')) returns contents of txtAzF as a double


% --- Executes during object creation, after setting all properties.
function txtAzF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtAzF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function txtElF_Callback(hObject, eventdata, handles)
% hObject    handle to txtElF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtElF as text
%        str2double(get(hObject,'String')) returns contents of txtElF as a double


% --- Executes during object creation, after setting all properties.
function txtElF_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtElF (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butOri2R.
function butOri2R_Callback(hObject, eventdata, handles)
% hObject    handle to butOri2R (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
rto = handles.rto;
f = get(rto, 'Fuente');
rec = get(rto, 'Receptor');
[az el] = apuntarReceptor(f, rec);
f = set(f, 'Azimut', az);
f = set(f, 'Elevacion', el);
handles.rto = set(handles.rto, 'Fuente', f);
guidata(hObject, handles);
set(handles.txtAzF, 'String', num2str(az));
set(handles.txtElF, 'String', num2str(el));
cargar_recinto(handles);



function txtHRTFSp_Callback(hObject, eventdata, handles)
% hObject    handle to txtHRTFSp (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtHRTFSp as text
%        str2double(get(hObject,'String')) returns contents of txtHRTFSp as a double


% --- Executes during object creation, after setting all properties.
function txtHRTFSp_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtHRTFSp (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butHRTFSp.
function butHRTFSp_Callback(hObject, eventdata, handles)
% hObject    handle to butHRTFSp (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile('*.*', 'HRTF spectrums file');

if ~isempty(nombre) && ischar(nombre)
    set(handles.txtHRTFSp, 'String', [ruta nombre]);
end


function txtHRTFFilt_Callback(hObject, eventdata, handles)
% hObject    handle to txtHRTFFilt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtHRTFFilt as text
%        str2double(get(hObject,'String')) returns contents of txtHRTFFilt as a double



% --- Executes during object creation, after setting all properties.
function txtHRTFFilt_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtHRTFFilt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butHRTFFilt.
function butHRTFFilt_Callback(hObject, eventdata, handles)
% hObject    handle to butHRTFFilt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile('*.*', 'HRTF filters file');

if ~isempty(nombre) && ischar(nombre)
    set(handles.txtHRTFFilt, 'String', [ruta nombre]);
end


% --- Executes on button press in butFiltrosSup.
function butFiltrosSup_Callback(hObject, eventdata, handles)
% hObject    handle to butFiltrosSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
filename = get(handles.txtFileSup, 'String');

if handles.rtoCargado == 0 || isempty(filename)
    return;
end

planos = get(handles.rto, 'Planos');
nPlanos = length(planos);
orden = str2num(get(handles.txtOrdenSup, 'String'));
n = str2num(get(handles.txtResSup, 'String'));
bb = zeros(nPlanos, orden + 1);
aa = zeros(nPlanos, orden + 1);

for i = 1:nPlanos
    mat = get(planos{i}, 'MaterialArq');
    [b a] = calcularFiltroMat(mat, orden, 44100, n);
    bb(i,:) = b;
    aa(i,:) = a;
end

ok = save_filtro_sup(bb, aa, nPlanos, orden, filename);

if ok == 1
    msgbox('Creating... OK', 'AVRSConfig');
end


function txtFileSup_Callback(hObject, eventdata, handles)
% hObject    handle to txtFileSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtFileSup as text
%        str2double(get(hObject,'String')) returns contents of txtFileSup as a double


% --- Executes during object creation, after setting all properties.
function txtFileSup_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtFileSup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtInput_Callback(hObject, eventdata, handles)
% hObject    handle to txtInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtInput as text
%        str2double(get(hObject,'String')) returns contents of txtInput as a double


% --- Executes during object creation, after setting all properties.
function txtInput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butInput.
function butInput_Callback(hObject, eventdata, handles)
% hObject    handle to butInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[nombre ruta] = uigetfile('*.wav', 'Anechoic WAVE file');

if ~isempty(nombre) && ischar(nombre)
    set(handles.txtInput, 'String', [ruta nombre]);
end



function txtMasterGain_Callback(hObject, eventdata, handles)
% hObject    handle to txtMasterGain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtMasterGain as text
%        str2double(get(hObject,'String')) returns contents of txtMasterGain as a double


% --- Executes during object creation, after setting all properties.
function txtMasterGain_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtMasterGain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtTemp_Callback(hObject, eventdata, handles)
% hObject    handle to txtTemp (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtTemp as text
%        str2double(get(hObject,'String')) returns contents of txtTemp as a double


% --- Executes during object creation, after setting all properties.
function txtTemp_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtTemp (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtAngle_Callback(hObject, eventdata, handles)
% hObject    handle to txtAngle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtAngle as text
%        str2double(get(hObject,'String')) returns contents of txtAngle as a double


% --- Executes during object creation, after setting all properties.
function txtAngle_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtAngle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txtBIRLen_Callback(hObject, eventdata, handles)
% hObject    handle to txtBIRLen (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtBIRLen as text
%        str2double(get(hObject,'String')) returns contents of txtBIRLen as a double


% --- Executes during object creation, after setting all properties.
function txtBIRLen_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtBIRLen (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butAdminMat.
function butAdminMat_Callback(hObject, eventdata, handles)
% hObject    handle to butAdminMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
guiMaterial('WindowStyle', 'modal');
uiwait;
handles.mats = borrarMateriales(handles.mats);
handles.mats = cargarMateriales(handles.mats);
set(handles.cboMat, 'String', getNombres(handles.mats));
