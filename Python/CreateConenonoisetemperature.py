import laspy
import numpy as np
import math
from datetime import datetime
import random
import transformations as trsf


hdr = laspy.header.Header()
inFile = laspy.file.File("simple.las", mode = "r")

outfile = laspy.file.File("conefilenonoiseTemperature.las", mode = "w",  header = inFile.header)
dtheta=2*math.pi/128.0
angle=0.0
zinit=0.0
alpha=0.0
tempe=-2.25
x=[]
y=[]
z=[]
h=[]
ones=[]
outrot= np.empty(shape=[2,2], dtype=float)
maxj=100
for j in range (0,maxj,1):
    zinit +=0.1
    alpha += 2*math.pi/maxj
    r=1.0 -zinit/(0.1*maxj)
    tempe = tempe + 2.25
    for i in range(0,128,1):
        angle +=dtheta
        dev=random.gauss(0.30, 0.010)
        #dev=0.0
        x.append(r*math.cos(angle)+dev)    
        dev=random.gauss(0.3, 0.010)
        #dev=0.0
       
        y.append(r*math.sin(angle)+dev)
        dev=random.gauss(0.9, 0.110)
        #dev=0.0
       
        z.append(zinit+dev)
        
        h.append(tempe)

allx = np.array(x) # Four Points
ally = np.array(y)
allz = np.array(z)
allh = np.array(h)

xmin = np.floor(np.min(allx))
ymin = np.floor(np.min(ally))
zmin = np.floor(np.min(allz))

outfile.header.offset = [xmin,ymin,zmin]
#outfile.header.offset = [0.0,0.0,0.0]

outfile.header.scale = [0.001,0.001,0.001]
#outfile.header.scale = [1.0, 1.0, 1.0]


outfile.x = allx
outfile.y = ally
outfile.z = allz

outfile.intensity = allh

outfile.close()
