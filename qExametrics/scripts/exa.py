import laspy
import copy
import sys
import numpy as np
import datetime

# see https://math.stackexchange.com/questions/1472049/check-if-a-point-is-inside-a-rectangular-shaped-area-3d

fileName = ""
P1 = np.array([0.0,0.0,0.0])
P2 = np.array([0.0,0.0,0.0])
P4 = np.array([0.0,0.0,0.0])
P5 = np.array([0.0,0.0,0.0])


# file
try:
    fileName = sys.argv[1]
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
    
# Point 1 x
try:
	P1[0] = float(sys.argv[2])
except ValueError:
    sys.stderr.write("P1[0] argv[2] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 1 y
try:
    P1[1] = float(sys.argv[3])
except ValueError:
    sys.stderr.write("P1[1] argv[3] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 1 z
try:
    P1[2] = float(sys.argv[4])
except ValueError:
    sys.stderr.write("P1[2] argv[4] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)

# Point 2 x
try:
    P2[0] = float(sys.argv[5])
except ValueError:
    sys.stderr.write("P2[0] argv[5] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 2 y
try:
    P2[1] = float(sys.argv[6])
except ValueError:
    sys.stderr.write("P2[1] argv[6] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 2 z
try:
    P2[2] = float(sys.argv[7])
except ValueError:
    sys.stderr.write("P2[2] argv[7] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)

# Point 4 x
try:
    P4[0] = float(sys.argv[8])
except ValueError:
    sys.stderr.write("P4[0] argv[8] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 4 y
try:
    P4[1] = float(sys.argv[9])
except ValueError:
    sys.stderr.write("P4[1] argv[9] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 4 z
try:
    P4[2] = float(sys.argv[10])
except ValueError:
    sys.stderr.write("P4[2] argv[10] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
    
# Point 5 x
try:
    P5[0] = float(sys.argv[11])
except ValueError:
    sys.stderr.write("P5[0] argv[11] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 5 y
try:
    P5[1] = float(sys.argv[12])
except ValueError:
    sys.stderr.write("P5[1] argv[12] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
# Point 5 z
try:
    P5[2] = float(sys.argv[13])
except ValueError:
    sys.stderr.write("P5[2] argv[13] Value Error")
    sys.exit(-2)
except IndexError:
    sys.stderr.write("Argument index access error")
    sys.exit(-1)
    
#Lecture fichier las + ajout VLR
inFile = laspy.file.File(fileName, mode= "r")
oldVlrs = copy.copy(inFile.header.vlrs)
vlr = laspy.header.VLR("Exametrics",1, "\x00" * 0, description = "las file modified")
oldVlrs.append(vlr)
    
nbrPoint = len(inFile.points)

tmp = 0
tabX = []
tabY = []
tabZ = []
tabG = []

# directions
u = np.cross((P1 - P4), (P1 - P5))
v = np.cross((P1 - P2), (P1 - P5))
w = np.cross((P1 - P2), (P1 - P4))

"""u = P1 - P2
v = P1 - P4
w = P1 - P5"""

uP1 = np.dot(u, P1)
uP2 = np.dot(u, P2)
vP1 = np.dot(v, P1)
vP4 = np.dot(v, P4)
wP1 = np.dot(w, P1)
wP5 = np.dot(w, P5)


"""print uP1 # 10.46
print uP2 # -46.55
print vP1
print vP4
print wP1
print wP5"""

pointInCount = 0
while tmp < nbrPoint:
    dataX = inFile.x[tmp]
    dataY = inFile.y[tmp]
    dataZ = inFile.z[tmp]
    GPS = inFile.gps_time[tmp]

    point = np.array([dataX, dataY, dataZ])

    up = np.dot(u, point)
    vp = np.dot(v, point)
    wp = np.dot(w, point)

    """print up # -1.56
    print vp
    print wp"""


    if(uP1 < uP2):
    	uState = (uP1 <= up) and (up <= uP2)
    else:
    	uState = (uP2 <= up) and (up <= uP1)

    if(vP1 < vP4):
    	vState = (vP1 <= vp) and (vp <= vP4)
    else:
    	vState = (vP4 <= vp) and (vp <= vP1)

    if(wP1 < wP5):
    	wState = (wP1 <= wp) and (wp <= wP5)
    else:
    	wState = (wP5 <= wp) and (wp <= wP1)

    #print str(tmp) + " " + str(uState) + " " + str(vState) + " " + str(wState)

    if(uState and vState and wState):
       	tabX.append(dataX)
        tabY.append(dataY)
        tabZ.append(dataZ)
        tabG.append(GPS)
        pointInCount = pointInCount + 1
        #print "add\n"

    tmp = tmp + 1

allx = np.array(tabX) 
ally = np.array(tabY)
allz = np.array(tabZ)
allg = np.array(tabG)
    
now = datetime.datetime.now()
newFileName = fileName[:-4] + now.strftime("%Y_%m_%d_%H_%M_") + ".las"
    

   
empty = False
if(len(allx) <= 0):
	sys.stderr.write("allx empty")
	empty = True

if(len(ally) <= 0):
	sys.stderr.write("ally empty")
	empty = True

if(len(allz) <= 0):
	sys.stderr.write("allz empty")
	empty = True

if empty is False:
	#Sauvegarde et fermeture fichier las
	outFile = laspy.file.File(newFileName,  mode= "w", vlrs = oldVlrs, header = inFile.header)
    
	#outFile.header.offset = [xmin,ymin,zmin]
	outFile.header.scale = [0.001,0.001,0.001]
	outFile.x = allx
	outFile.y = ally
	outFile.z = allz
	outFile.set_gps_time(allg)
    
	outFile.close()
	sys.stdout.write("File created, " + str(pointInCount) + " points found (" + newFileName + ")")
else:
	sys.stdout.write("No points found, file not created")

