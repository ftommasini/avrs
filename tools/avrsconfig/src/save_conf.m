function retvalue = save_conf(conf, filename)
%SAVE_CONF Summary of this function goes here
%   Detailed explanation goes here

if ~isstruct(conf)
    retvalue = 0;
    return;
end

fid = fopen(filename, 'w+', 'n', 'UTF-8');

if fid < 0
    retvalue = 0;
    return;
end

% Header
fprintf(fid, '# AVRS simulation configuration file\n');
fprintf(fid, '#\n');
fprintf(fid, '# Automatically created by AVRSConfig\n');
fprintf(fid, '# %s\n', datestr(now));
fprintf(fid, '# -----------------------------------\n');
fprintf(fid, '\n');

% Room
fprintf(fid, '########\n');
fprintf(fid, '# Room #\n');
fprintf(fid, '########\n');
fprintf(fid, '\n');
fprintf(fid, 'ROOM_DXF_FILE = %s\n', conf.room_dxf_file);
fprintf(fid, '# in cubic meters\n');
fprintf(fid, 'ROOM_VOLUME = %f\n', conf.room_volume);
fprintf(fid, 'ROOM_N_SURFACES = %d\n', conf.room_n_surfaces);
fprintf(fid, 'ROOM_FILTER_SURFACES_FILE = %s\n', conf.room_filter_surfaces_file);
fprintf(fid, 'ISM_MAX_ORDER = %d\n', conf.ism_max_order);
fprintf(fid, 'ISM_MAX_DISTANCE = %f\n', conf.ism_max_distance);
fprintf(fid, '\n');

% Sound Source
fprintf(fid, '################\n');
fprintf(fid, '# Sound Source #\n');
fprintf(fid, '################\n');
fprintf(fid, '\n');
fprintf(fid, 'SOUND_SOURCE_IR_FILE = %s\n', conf.sound_source_ir_file);
fprintf(fid, 'SOUND_SOURCE_DIRECTIVITY_FILE = %s\n', conf.sound_source_directivity_file);
fprintf(fid, '# x, y and z in meters\n');
fprintf(fid, '# referenced to the room coordinates\n');
fprintf(fid, 'SOUND_SOURCE_POSITION = %f, %f, %f\n', ...
    conf.sound_source_position(1), ...
    conf.sound_source_position(2), ...
    conf.sound_source_position(3));
fprintf(fid, '# azimuth and elevation angles in degrees\n');
fprintf(fid, '# referenced to the room coordinates, according to interaural-polar coordinate system\n');
fprintf(fid, 'SOUND_SOURCE_ORIENTATION = %f, %f\n', ...
    conf.sound_source_orientation(1), ...
    conf.sound_source_orientation(2));
fprintf(fid, '\n');

% Listener
fprintf(fid, '############\n');
fprintf(fid, '# Listener #\n');
fprintf(fid, '############\n');
fprintf(fid, '\n');
fprintf(fid, '# x, y and z in meters\n');
fprintf(fid, '# referenced to the room coordinates\n');
fprintf(fid, 'LISTENER_POSITION = %f, %f, %f\n', ...
    conf.listener_position(1), ...
    conf.listener_position(2), ...
    conf.listener_position(3));
fprintf(fid, '# azimuth and elevation angles in degrees\n');
fprintf(fid, '# referenced to the room coordinates, according to interaural-polar coordinate system\n');
fprintf(fid, 'LISTENER_ORIENTATION = %f, %f\n', ...
    conf.listener_orientation(1), ...
    conf.listener_orientation(2));
fprintf(fid, '# DB in AVRS format\n');
fprintf(fid, 'LISTENER_HRTF_FILE = %s\n', conf.listener_hrtf_file);
fprintf(fid, 'LISTENER_FILTER_HRTF_FILE = %s\n', conf.listener_filter_hrtf_file);
fprintf(fid, '\n');

% Input
fprintf(fid, '#########\n');
fprintf(fid, '# Input #\n');
fprintf(fid, '#########\n');
fprintf(fid, '\n');
fprintf(fid, 'ANECHOIC_FILE = %s\n', conf.anechoic_file);
fprintf(fid, '\n');

% Output
fprintf(fid, '##########\n');
fprintf(fid, '# Output #\n');
fprintf(fid, '##########\n');
fprintf(fid, '\n');
fprintf(fid, '# correction factor in dB\n');
fprintf(fid, 'MASTER_GAIN_DB = %f\n', conf.master_gain_db);
fprintf(fid, '\n');

% General
fprintf(fid, '###########\n');
fprintf(fid, '# General #\n');
fprintf(fid, '###########\n');
fprintf(fid, '\n');
fprintf(fid, '# in ºC\n');
fprintf(fid, 'TEMPERATURE = %f  # corresponds to a speed of sound of about %3.1f m/s\n', ...
    conf.temperature, ...
    speed_sound(conf.temperature));
fprintf(fid, '# in degrees\n');
fprintf(fid, 'ANGLE_THRESHOLD = %f\n', conf.angle_threshold);
fprintf(fid, '# in seconds\n');
fprintf(fid, 'BIR_LENGTH = %f\n', conf.bir_length);

fclose(fid);
retvalue = 1;

end

