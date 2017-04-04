# -*- coding: utf-8 -*-
"""
Created on Mon Mar 27 21:39:50 2017

@author: Stefan
"""

import matplotlib.pyplot as plt
import numpy as np

r = 6.371e6         # Earth radii [m]
G = 6.67408e-11     # Gravity conatnt [m^3/(kg*s^2)]
M = 5.9723e24        # Earth mass [kg]
eta = 0.1           # Telescope view angel to perpendicular [rad]
O_T = 50e-3         # Radii of the telescope [m]
Sen = [4e3, 2e3]    # Pixel No of sensos

h = np.linspace(100e3, 500e3, 5)
H = np.matrix([h])

UV = np.matrix([np.linspace(280e-9, 380e-9, 101)]).T
VIS = np.matrix([np.linspace(380e-9, 680e-9, 301)]).T
NIR = np.matrix([np.linspace(780e-9, 1400e-9, 125)]).T
MIR = np.matrix([np.linspace(3e-6, 50e-6, 236)]).T

R = r + h

V_s = np.sqrt(G*M/R)        # Speed of satellite [m/s]
t = 2 * np.pi * R / V_s     # Time for one orbit [s]
V_g = r/R*np.sqrt(G*M/R)    # Ground track velocity
D_max = np.sqrt(R**2-r**2)  # Distance satellite to horizon
R_p = r*np.sqrt(1-r**2/R**2)    # Radii of the visible ground 
GR_UV = 1.22/O_T*UV*H       # Ground resolution UV
GR_VIS = 1.22/O_T*VIS*H
GR_NIR = 1.22/O_T*NIR*H
GR_MIR = 1.22/O_T*MIR*H

ta_2 = np.tan(eta)**2
x = (np.sqrt((ta_2+1)*r**2-R**2*ta_2)+R*ta_2)/(ta_2+1)
y = (np.tan(eta)*(R-np.sqrt((ta_2+1)*r**2-R**2*ta_2)))/(ta_2+1)
D = np.sqrt((x-R)**2+y**2)    
IFOV = D*np.sin(eta)        # Diameter of visiable area with telescope
Spa = IFOV/Sen[0]           # Spatial resolution 

plt.figure(1)
plt.plot(h/1000,t/60)
plt.grid(True)
plt.title('Orbit Time')
plt.xlabel('Orbit hight [km]')
plt.ylabel('Time for orbit [minutes]')
plt.savefig('NSat_1.png',dpi=199)


plt.figure(2)
plt.plot(h/1000,V_g/1000)
plt.grid(True)
plt.title('Ground Track Velocity')
plt.xlabel('Orbit hight [km]')
plt.ylabel('Velocity [km/s]')
plt.savefig('NSat_2.png', dpi=199)

plt.figure(3)
plt.subplot(211)
plt.plot(h/1000,IFOV/1000)
plt.grid(True)
plt.title('Instantaneous Field of View and Spatial Resolution')
plt.ylabel('Diameter [km]')
plt.subplot(212)
plt.plot(h/1000,Spa)
plt.grid(True)
plt.xlabel('Orbit hight [km]')
plt.ylabel('Spatial Resolution [m]')
plt.savefig('NSat_3.png', dpi=199)

plt.figure(4)
plt.plot(UV/1e-9,GR_UV)
plt.grid(True)
plt.title('Ground Resolution UV')
plt.xlabel('Wave Length [nm]')
plt.ylabel('Resolution [m]')
plt.savefig('NSat_4.png', dpi=199)

plt.figure(5)
plt.plot(VIS/1e-9,GR_VIS)
plt.grid(True)
plt.title('Ground Resolution VIS')
plt.xlabel('Wave Length [nm]')
plt.ylabel('Resolution [m]')
plt.savefig('NSat_5.png', dpi=199)

plt.figure(6)
plt.plot(NIR/1e-6,GR_NIR)
plt.grid(True)
plt.title('Ground Resolution NIR')
plt.xlabel('Wave Length [$\mu$m]')
plt.ylabel('Resolution [m]')
plt.savefig('NSat_6.png', dpi=199)

plt.figure(7)
plt.plot(MIR/1e-6,GR_MIR)
plt.grid(True)
plt.title('Ground Resolution MIR')
plt.xlabel('Wave Length [$\mu$m]')
plt.ylabel('Resolution [m]')
plt.savefig('NSat_7.png', dpi=199)