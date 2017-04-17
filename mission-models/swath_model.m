r = 6.371e6;         % Earth radii [m]
G = 6.67408e-11;     % Gravity conatnt [m^3/(kg*s^2)]
M = 5.9723e24;        % Earth mass [kg]
eta = 0.1;           % Telescope view angel to perpendicular [rad]
O_T = 50e-3;         % Radii of the telescope [m]
Sen = [4e3, 2e3];   % pixels of sensor

h = linspace(100e3, 500e3, 5); % single row matrix of altitudues

% build up wavelength single column matrix for bands in the spectrum.
UV = linspace(280e-9, 380e-9, 101)'; 
VIS = linspace(380e-9, 680e-9, 301)';
NIR = linspace(780e-9, 1400e-9, 125)';
MIR = linspace(3e-6, 50e-6, 236)';
 
R = r + h; % model the radius of the satellite - altitude h above earth radius r

V_s = sqrt(G*M./R);        % Speed of satellite [m/s]

figure('Name','velocity vs altitude');
plot(h./1000, V_s./1000);
xlabel('Satellite altitude (km)');
ylabel('Satellite velocity(km/s)');

t = 2 * pi * R ./ V_s;     % Time for one orbit [s]

figure('Name','orbital period vs altitude');
f2 = plot(h./1000,t);
xlabel('Satellite altitude(km)');
ylabel('time for one orbit(s)');

V_g = r./R.*sqrt(G*M./R);    % Ground track velocity
D_max = sqrt(R.^2-r.^2);  % Distance satellite to horizon
R_p = r.*sqrt(1-r.^2./R.^2);    % Radii of the visible ground 

k = 1.22/O_T;
GR_UV = k.*UV*h;       % Ground resolution UV at different altitudes

figure('Name','resolution vs ultraviolet');
f3 = plot(UV./1e-9,GR_UV); 
xlabel('wavelength(nm)');
ylabel('Ground resolution(m)');
legend('altitude(100km)','altitude(200km)','altitude(300km)','altitude(400km)','altitude(500km)');

GR_VIS = k.*VIS*h;

figure('Name','resolution vs visual wavelengths');
f3 = plot(VIS./1e-6,GR_VIS); 
xlabel('wavelength(um)');
ylabel('Ground resolution(m)');
legend('altitude(100km)','altitude(200km)','altitude(300km)','altitude(400km)','altitude(500km)');

GR_NIR = k.*NIR*h;
figure('Name','resolution vs near infrared');
f3 = plot(NIR./1e-6,GR_NIR); 
xlabel('wavelength(um)');
ylabel('Ground resolution(m)');
legend('altitude(100km)','altitude(200km)','altitude(300km)','altitude(400km)','altitude(500km)');

GR_MIR = k.*MIR*h;
figure('Name','resolution vs medium infrared');
f3 = plot(MIR./1e-6,GR_MIR); 
xlabel('wavelength(um)');
ylabel('Ground resolution(m)');
legend('altitude(100km)','altitude(200km)','altitude(300km)','altitude(400km)','altitude(500km)');


% work out the field of view and spatial resolution.
ta_2 = tan(eta).^2;
x = (sqrt((ta_2+1)*r.^2-R.^2*ta_2)+R*ta_2)/(ta_2+1);
y = (tan(eta)*(R -sqrt((ta_2+1)*r.^2-R.^2*ta_2)))/(ta_2+1);
D = sqrt((x-R).^2+y.^2);
IFOV = D*sin(eta);        % Diameter of visiable area with telescope
Spa = IFOV/Sen(1);           % Spatial resolution 

figure('Name','Field of View vs resolution');
subplot(211);
plot(h./1000,IFOV./1000)
grid();
title('Instantaneous Field of View and Spatial Resolution');
ylabel('Diameter [km]');
subplot(212);
plot(h./1000,Spa);
grid();
xlabel('Orbit hight [km]');
ylabel('Spatial Resolution [m]');
