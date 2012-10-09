function speed = speed_sound(temperature)
%SPEED_SOUND Summary of this function goes here
%   Detailed explanation goes here

speed = 331.4 * sqrt(1 + temperature / 273);
end

