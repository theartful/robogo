import numpy as np


# call this function in for only the first time 
def generateG(values,n,h):
		G = np.zeros((n,n))
		for l in values :
			l = l.split(" ")
			i = int(l[1][1])-1
			j = int(l[2][1])-1
			x = float(l[3])
			calValue = calculateValue(l[0],x,h)
			if (i >= 0):
					G[i,i] += calValue
			if (j >= 0):
					G[j,j] += calValue
			if i >=0 and j>=0:
					G[i,j] -= calValue
					G[j,i] -= calValue        
		return G


# detect what if resistance or capacitance
def calculateValue(type, value,h):
	if type == "C":
		calValue = value/h
	elif type == "R":
		calValue = 1/value
	return calValue


def generateB(values,n):
		B = np.zeros((n,len(values)))
		k = 0
		for l in values :
				l = l.split(" ")
				i = int(l[1][1])-1
				j = int(l[2][1])-1
				if i >= 0:
						B[i,k] = 1
				if j >= 0:
						B[j,k] = -1  
				k += 1      
		return B

def generateX(nodeNum,voltageNum):
		x = np.chararray((nodeNum+voltageNum,1),itemsize=16)
		for l in range (1,nodeNum+1):
				x[l-1] = 'V' + str(l)
		for l in range (1,voltageNum+1):
				x[nodeNum+l-1] = 'IV' + str(l)
		return x

def generateZ(voltages,currents,nodeNum):
		voltageNum = len(voltages)
		z = np.zeros((nodeNum+voltageNum,1))
		for l in currents:
				l = l.split(" ")
				i = int(l[1][1])-1
				j = int(l[2][1])-1
				x = float(l[3])
				if (i >= 0):
					z[i] += x
				if (j >= 0 ):
					z[j] -= x
		k = nodeNum    
		for l in voltages :
				l = l.split(" ")
				z[k] = l[3]
				k += 1  
		return z

def readFile(filename):	
		f = open(filename, "r")
		data = {}
		timestamp = f.readline()
		iterationNum = f.readline() 
		for line in f:
				if line[0] not in data.keys():
						data[line[0]] = []
						data[line[0]].append(line[0:])
				else:
						 data[line[0]].append(line[0:]) 
						    
		if "V" not in data.keys():
				data["V"] = []
		if "I" not in data.keys():
				data["I"] = []  
		if "i" not in data.keys():
				data["i"] = [] 
		if "C" not in data.keys():
				data["C"] = []		
		f.close()
		
		return data,iterationNum,timestamp

def getMatrix(d,nodeNum,iterationNum,h):
		timestamp = h
		voltageNum = len(d["V"])
		currentNum = len(d["I"])	
		G = generateG(d["R"]+d["C"],nodeNum,h)
		if d["C"] != 0 : 
			for c in d["C"] :
				c = c.split(" ")
				d["I"].append("Isrc "+c[1]+" "+c[2]+" "+str((float(c[4])*(float(c[3]))/h))+" "+c[3]+" 0\n")
		if d["i"] != 0 : 
			for i in d["i"] :
				i = i.split(" ")
				d["V"].append("Vsrc "+i[1]+" "+i[2]+" "+str((float(i[4])*(float(i[3]))/h))+" "+i[3]+" 0\n") 
		

		D = generateD(d["i"],voltageNum,len(d["V"]),h)
		B = generateB(d["V"],nodeNum)
		C = np.transpose(B)
		x = generateX(nodeNum,len(d["V"]))

		for k in range(0,iterationNum):
			print("iteration ",k," at t = ",float(k*h+h),":\n")
			if k != 0:
				if d["C"] != 0:
					d["I"] = updateCurrent(d["I"],currentNum,h,xValues)
				if d["i"] != 0:	
					d["V"] = updateVoltage(d["V"],voltageNum,nodeNum,h,xValues)	
			z = generateZ(d["V"],d["I"],nodeNum)
			xValues = solveMatrix([G,B,C,D,x,z],len(d["V"]))
			for i in range(0,len(x)):

				print(x[i] ,xValues[i])
			
			#print(xValues)
			print("---------------------------------------------------------")



def generateD(d,start,voltageNum,h):
		D = np.zeros((voltageNum,voltageNum))
		if len(d) != 0:
			for line in d:
				line = line.split(" ")
				i = int(line[1][1])-1
				j = int(line[2][1])-1
				D[start,start] = -float(line[3])/ h
				start += 1
		return D


def updateVoltage(d,start,n,h,values):
		num = 0
		newList = []
		for line in d:
			if num < start :
				newList.append(line)
			else:
				line = line.split(" ")
				i = n + num
				i1 = -1* float(values[i])
				line2 = "Vsrc "+line[1]+" "+line[2]+" "+ str(i1*(float(line[4])/h))+" "+line[4]+" 0\n"
				newList.append(line2)
			num += 1
		return newList


def updateCurrent(d,start,h,values):
		num = 0
		newList = []
		for line in d:
			if num < start :
				newList.append(line)
			else:
				line = line.split(" ")
				i = int(line[1][1])-1
				j = int(line[2][1])-1
				if i >= 0:
					v1 = float(values[i])
				else:
					v1 = 0 # ground
				if j>= 0 :
					v2 = float(values[j])
				else :
					v2 = 0 # ground
				voltage = (v1-v2)* float(line[4])/h
				line2 = "Isrc "+line[1]+" "+line[2]+" "+ str(((v1-v2)*(float(line[4])/h)))+" "+line[4]+" 0\n"
				newList.append(line2)
			num += 1
		
		return newList
			
def solveMatrix(data,voltageNum):
		G = data[0]
		B = data[1]
		C = data[2]
		D = data[3]
		x = data[4]
		z = data[5]

		p1 = np.concatenate((G,B),axis = 1)
		p2 = np.concatenate((C,D),axis = 1)
		A = np.concatenate((p1,p2))
		aInverse = np.linalg.inv(A)
		xValues = np.matmul(aInverse,z)
		return xValues

def checkValidity(d):
	nodeNum = 0
	nodes = []

	for l in ["V","R","I","C","i"] :
		if l in d.keys():
			for ll in d[l]:
				ll = ll.split(" ")
				nodes.append(ll[2])
				nodes.append(ll[1])
	for node in nodes :
		if nodes.count(node) < 2:
			return 0 
	return 1,len(set(nodes))

if __name__ == "__main__" :
	filename = input("Enter File name: ")

	values = readFile("testcases/"+filename)
	valuesDictionary = values[0]
	iterationNum = int(values[1])
	timestamp = float(values[2])
	validity = checkValidity(valuesDictionary)
	if validity[0] == 0:
		print("Error in circuit ")
	else:
		nodeNum = validity[1] -1
	data = getMatrix(valuesDictionary,nodeNum,iterationNum,timestamp)
