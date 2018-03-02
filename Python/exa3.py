import laspy
import copy
import sys
import numpy as np
import datetime

#Recuperation argument
try:
    fileName = "conefilenonoise.las" #sys.argv[1]
except IndexError:
    print("Erreur axe invalide")
    sys.exit(0)  
    
try:
    minX = 3 #float(sys.argv[2])
except ValueError:
    print("Erreur vectorA invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorA invalide")
    sys.exit(0) 
        
try:
    minY = 2 #float(sys.argv[3])
except ValueError:
    print("Erreur vectorB invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorB invalide")
    sys.exit(0) 
    
try:
    minZ = 1.5 #float(sys.argv[4])
except ValueError:
    print("Erreur vectorC invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur vectorC invalide")
    sys.exit(0)    
    
try:
    maxX = 6 #float(sys.argv[5])
except ValueError:
    print("Erreur offset invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur offset invalide")
    sys.exit(0) 
 
try:
    maxY = 5 #float(sys.argv[6])
except ValueError:
    print("Erreur offset invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur offset invalide")
    sys.exit(0) 
    
try:
    maxZ = 4 #float(sys.argv[7])
except ValueError:
    print("Erreur offset invalide")
    sys.exit(0) 
except IndexError:
    print("Erreur offset invalide")
    sys.exit(0) 
    
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
while tmp < nbrPoint:
    dataX = inFile.x[tmp]
    dataY = inFile.y[tmp]
    dataZ = inFile.z[tmp]
    GPS = inFile.gps_time[tmp]
    if dataX <= maxX and dataX >= minX and dataY <= maxY and dataY >= minY and dataZ <= maxZ and dataZ >= minZ:
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