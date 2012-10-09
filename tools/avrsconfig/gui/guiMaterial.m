function varargout = guiMaterial(varargin)
% GUIMATERIAL M-file for guiMaterial.fig
%      GUIMATERIAL, by itself, creates a new GUIMATERIAL or raises the existing
%      singleton*.
%
%      H = GUIMATERIAL returns the handle to a new GUIMATERIAL or the handle to
%      the existing singleton*.
%
%      GUIMATERIAL('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUIMATERIAL.M with the given input arguments.
%
%      GUIMATERIAL('Property','Value',...) creates a new GUIMATERIAL or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before guiMaterial_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to guiMaterial_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help guiMaterial

% Last Modified by GUIDE v2.5 13-Feb-2012 16:37:50

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @guiMaterial_OpeningFcn, ...
                   'gui_OutputFcn',  @guiMaterial_OutputFcn, ...
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


% --- Executes just before guiMaterial is made visible.
function guiMaterial_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to guiMaterial (see VARARGIN)

% Choose default command line output for guiMaterial
handles.output = hObject;
handles.mats = MaterialesArq();
handles.mats = borrarMateriales(handles.mats);
handles.mats = cargarMateriales(handles.mats);
set(handles.lstMat, 'String', getNombres(handles.mats));
n = length(getNombres(handles.mats));
set(handles.texTotal, 'String', ['Total: ' num2str(n)]);
enable_new('off', handles);
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes guiMaterial wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = guiMaterial_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in butCerrar.
function butCerrar_Callback(hObject, eventdata, handles)
% hObject    handle to butCerrar (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
delete(handles.guiMaterial);


% --- Executes on button press in butNuevo.
function butNuevo_Callback(hObject, eventdata, handles)
% hObject    handle to butNuevo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
enable_new('on', handles);


function txtName_Callback(hObject, eventdata, handles)
% hObject    handle to txtName (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txtName as text
%        str2double(get(hObject,'String')) returns contents of txtName as a double


% --- Executes during object creation, after setting all properties.
function txtName_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txtName (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt125_Callback(hObject, eventdata, handles)
% hObject    handle to txt125 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt125 as text
%        str2double(get(hObject,'String')) returns contents of txt125 as a double


% --- Executes during object creation, after setting all properties.
function txt125_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt125 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt250_Callback(hObject, eventdata, handles)
% hObject    handle to txt250 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt250 as text
%        str2double(get(hObject,'String')) returns contents of txt250 as a double


% --- Executes during object creation, after setting all properties.
function txt250_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt250 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt500_Callback(hObject, eventdata, handles)
% hObject    handle to txt500 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt500 as text
%        str2double(get(hObject,'String')) returns contents of txt500 as a double


% --- Executes during object creation, after setting all properties.
function txt500_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt500 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt1000_Callback(hObject, eventdata, handles)
% hObject    handle to txt1000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt1000 as text
%        str2double(get(hObject,'String')) returns contents of txt1000 as a double


% --- Executes during object creation, after setting all properties.
function txt1000_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt1000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt2000_Callback(hObject, eventdata, handles)
% hObject    handle to txt2000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt2000 as text
%        str2double(get(hObject,'String')) returns contents of txt2000 as a double


% --- Executes during object creation, after setting all properties.
function txt2000_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt2000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function txt4000_Callback(hObject, eventdata, handles)
% hObject    handle to txt4000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of txt4000 as text
%        str2double(get(hObject,'String')) returns contents of txt4000 as a double


% --- Executes during object creation, after setting all properties.
function txt4000_CreateFcn(hObject, eventdata, handles)
% hObject    handle to txt4000 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



% --- Executes during object creation, after setting all properties.
function guiMaterial_CreateFcn(hObject, eventdata, handles)
% hObject    handle to guiMaterial (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
movegui('center');


% --- Executes on selection change in lstMat.
function lstMat_Callback(hObject, eventdata, handles)
% hObject    handle to lstMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns lstMat contents as cell array
%        contents{get(hObject,'Value')} returns selected item from lstMat
ind = get(handles.lstMat, 'Value');

if ind == 0
    return;
end

mats = get(handles.mats, 'Materiales');  
mat = mats{ind};  % obtengo el material
coefAbs = get(mat, 'CoefAbsorcion');
set(handles.texCoef, 'String', num2str(coefAbs));


% --- Executes during object creation, after setting all properties.
function lstMat_CreateFcn(hObject, eventdata, handles)
% hObject    handle to lstMat (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in butDelete.
function butDelete_Callback(hObject, eventdata, handles)
% hObject    handle to butDelete (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
ind = get(handles.lstMat, 'Value');

if ind == 0
    return;
end

names = get(handles.lstMat, 'String');
button = questdlg(['Delete ' names{ind} '?'], 'AVRSConfig', 'Yes', 'No', 'No');

if strcmp(button, 'Yes')
    handles.mats = borrarMaterial(handles.mats, ind);
    guidata(hObject, handles);
    set(handles.lstMat, 'Value', 1);
    set(handles.lstMat, 'String', getNombres(handles.mats));
    n = length(getNombres(handles.mats));
    set(handles.texTotal, 'String', ['Total: ' num2str(n)]);
end


% --- Executes on button press in butAdd.
function butAdd_Callback(hObject, eventdata, handles)
% hObject    handle to butAdd (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
nom = get(handles.txtName, 'String');

if isempty(nom)
     errordlg('Error in name', 'AVRSConfig');
    return;
end

c125 = str2double(get(handles.txt125, 'String'));
c250 = str2double(get(handles.txt250, 'String'));
c500 = str2double(get(handles.txt500, 'String'));
c1000 = str2double(get(handles.txt1000, 'String'));
c2000 = str2double(get(handles.txt2000, 'String'));
c4000 = str2double(get(handles.txt4000, 'String'));
coefAbs = [c125 c250 c500 c1000 c2000 c4000];

ind = find(isnan(coefAbs));  % obtengo los ídices de los valores correctos

if ~isempty(ind)
    errordlg('Error in coefficients', 'AVRSConfig');
    return;
end

m = MaterialArq(nom, coefAbs);
handles.mats = agregarMaterial(handles.mats, m);
guidata(hObject, handles);
set(handles.lstMat, 'String', getNombres(handles.mats));
n = length(getNombres(handles.mats));
set(handles.texTotal, 'String', ['Total: ' num2str(n)]);
% diable new section
set(handles.txtName, 'String', '');
set(handles.txt125, 'String', '');
set(handles.txt250, 'String', '');
set(handles.txt500, 'String', '');
set(handles.txt1000, 'String', '');
set(handles.txt2000, 'String', '');
set(handles.txt4000, 'String', '');
enable_new('off', handles);
msgbox('Material added', 'AVRSConfig');


% --- Executes on button press in butSave.
function butSave_Callback(hObject, eventdata, handles)
% hObject    handle to butSave (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
guardarMateriales(handles.mats);
msgbox('Materials DB saved', 'AVRSConfig');
delete(handles.guiMaterial);


function enable_new(activate, handles)
set(handles.texName, 'Enable', activate);
set(handles.txtName, 'Enable', activate);
set(handles.texAbs, 'Enable', activate);
set(handles.tex125, 'Enable', activate);
set(handles.txt125, 'Enable', activate);
set(handles.tex250, 'Enable', activate);
set(handles.txt250, 'Enable', activate);
set(handles.tex500, 'Enable', activate);
set(handles.txt500, 'Enable', activate);
set(handles.tex1000, 'Enable', activate);
set(handles.txt1000, 'Enable', activate);
set(handles.tex2000, 'Enable', activate);
set(handles.txt2000, 'Enable', activate);
set(handles.tex4000, 'Enable', activate);
set(handles.txt4000, 'Enable', activate);
set(handles.butAdd, 'Enable', activate);

