import laspy
import copy
import sys
import numpy as np
import datetime

PAS = 0.001

#Recuperation argument
try:
    fileName = sys.argv[1]
except IndexError:
    print("Erreur axe invalide")
    sys.exit(0)  
    
try:
    vectorA = float(sys.argv[2))
except ValueError:
    print("Erreur vectorA invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorA invalide")
    sys.exit(0) 
        
try:
    vectorB = float(sys.argv[3])
except ValueError:
    print("Erreur vectorB invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorB invalide")
    sys.exit(0) 
    
try:
    vectorC = float(sys.argv[4])
except ValueError:
    print("Erreur vectorC invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorC invalide")
    sys.exit(0)    
    
try:
    offset = float(sys.argv[5])
except ValueError:
    print("Erreur offset invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur offset invalide")
    sys.exit(0) 
 
try:
    marge = float(sys.argv[6])
except IndexError:
    marge = 0
except ValueError:
    marge = 0
    
#Lecture fichier las + ajout VLR
inFile = laspy.file.File(fileName, mode= "r")
oldVlrs = copy.copy(inFile.header.vlrs)
vlr = laspy.header.VLR("Exametrics",1, "\x00" * 0, description = "las file modified")
oldVlrs.append(vlr)
    
nbrPoint = len(inFile.points)
minValue = offset - marge
maxValue = offset + marge

tmp = 0
tabX = []
tabY = []
tabZ = []
tabG = []
offsetTmp = 0
float(offsetTmp)
while tmp < nbrPoint:
    dataX = inFile.x[tmp]
    dataY = inFile.y[tmp]
    dataZ = inFile.z[tmp]
    GPS = inFile.gps_time[tmp]
    offsetTmp = minValue
    while offsetTmp < maxValue:
        equation = dataX * vectorA + dataY * vectorB + dataZ * vectorC + offsetTmp
        if equation == 0:
            tabX.append(dataX)
            tabY.append(dataY)
            tabZ.append(dataZ)
            tabG.append(GPS)
        offsetTmp = offsetTmp + PAS
    tmp = tmp + 1
    print(tmp*100/nbrPoint)

allx = np.array(tabX) 
ally = np.array(tabY)
allz = np.array(tabZ)
allg = np.array(tabG)

now = datetime.datetime.now()
newFileName = now.strftime("%Y_%m_%d_%H_%M_") + fileName

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