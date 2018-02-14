import laspy
import copy
import sys
import numpy as np
import datetime

FILENAME = "conefilenonoise.las" 

#Lecture fichier las + ajout VLR
inFile = laspy.file.File(FILENAME, mode= "r")
oldVlrs = copy.copy(inFile.header.vlrs)
vlr = laspy.header.VLR("Exametrics",1, "\x00" * 0, description = "las file modified")
oldVlrs.append(vlr)

#Recuperation argument
try:
    axe = int(sys.argv[1])
except ValueError:
    print("Erreur axe invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur axe invalide")
    sys.exit(0)     
    
try:
    offset = float(sys.argv[4])
except ValueError:
    print("Erreur offset invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur offset invalide")
    sys.exit(0) 
 
try:
    marge = float(sys.argv[5])
except IndexError:
    marge = 0
except ValueError:
    marge = 0

nbrPoint = len(inFile.points)
minValue = offset - marge
maxValue = offset + marge
tmp = 0
tabX = []
tabY = []
tabZ = []
tabG = []
while tmp < nbrPoint:
    dataX = inFile.x[tmp]
    dataY = inFile.y[tmp]
    dataZ = inFile.z[tmp]
    GPS = inFile.gps_time[tmp]
    if axe == 1:
        if dataX > minValue and dataX < maxValue:
            tabX.append(dataX)
            tabY.append(dataY)
            tabZ.append(dataZ)
            tabG.append(GPS)
    elif axe == 2:
        if dataY > minValue and dataY < maxValue:
            tabX.append(dataX)
            tabY.append(dataY)
            tabZ.append(dataZ)       
            tabG.append(GPS)
    else:
        if dataZ > minValue and dataZ < maxValue:
            tabX.append(dataX)
            tabY.append(dataY)
            tabZ.append(dataZ)     
            tabG.append(GPS)
    tmp = tmp + 1
    print(tmp*100/nbrPoint)

allx = np.array(tabX) 
ally = np.array(tabY)
allz = np.array(tabZ)
allg = np.array(tabG)

now = datetime.datetime.now()
newFileName = now.strftime("%Y_%m_%d_%H_%M_") + FILENAME

#Sauvegarde et fermeture fichier las
outFile = laspy.file.File(newFileName,  mode= "w", vlrs = oldVlrs, header = inFile.header)

xmin = np.floor(np.min(allx))
ymin = np.floor(np.min(ally))
zmin = np.floor(np.min(allz))

outFile.header.offset = [xmin,ymin,zmin]
outFile.header.scale = [0.001,0.001,0.001]

outFile.x = allx
outFile.y = ally
outFile.z = allz
outFile.set_gps_time(allg)

outFile.close()