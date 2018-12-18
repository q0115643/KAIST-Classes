
import samples
import util
import numpy as np

TEST_SET_SIZE = 100
DIGIT_DATUM_WIDTH=28
DIGIT_DATUM_HEIGHT=28
FACE_DATUM_WIDTH=60
FACE_DATUM_HEIGHT=70


def basicFeatureExtractorDigit(datum):
	"""
	Returns a set of pixel features indicating whether
	each pixel in the provided datum is white (0) or gray/black (1)
	"""
	features = util.Counter()
	for x in range(DIGIT_DATUM_WIDTH):
		for y in range(DIGIT_DATUM_HEIGHT):
			if datum.getPixel(x, y) > 0:
				features[(x,y)] = 1
			else:
				features[(x,y)] = 0
	return features

def basicFeatureExtractorFace(datum):
	"""
	Returns a set of pixel features indicating whether
	each pixel in the provided datum is an edge (1) or no edge (0)
	"""
	features = util.Counter()
	for x in range(FACE_DATUM_WIDTH):
		for y in range(FACE_DATUM_HEIGHT):
			if datum.getPixel(x, y) > 0:
				features[(x,y)] = 1
			else:
				features[(x,y)] = 0
	return features
	
def basicFeatureDataToNumpyArray(basicFeatureData):
	"""
	Convert basic feature data(Counter) to N x d numpy array
	"""
	N = len(basicFeatureData)
	D = len(basicFeatureData[0])
	keys = basicFeatureData[0].keys()

	data = np.zeros((N, D))
	for i in range(N):
		for j in range(D):
			data[i][j] = basicFeatureData[i][keys[j]]
			
	#data_std = np.std(data, 0)
	#mask = data_std == 0
	#data = data[:, ~mask]
	
	return data

def getPrincipleComponents(basicFeatureData, pc_count):
	"""
	Returns top-k principle components for dimensionality reduction (PCA)
	"""
	data = basicFeatureDataToNumpyArray(basicFeatureData)

	data -= np.mean(data, 0) # mean centering
	C = np.cov(data.T)
	E, V = np.linalg.eigh(C)
	key = np.argsort(E)[::-1][:pc_count]
	E, V = E[key], V[:, key]

	return V # V: top-k eigenvectors

def loadDataset(setName, mode, numTraining, principleComponents):
	if(setName == "digitdata"):
		featureFunction = basicFeatureExtractorDigit
		rawTrainingData = samples.loadDataFile("digitdata/trainingimages", numTraining,DIGIT_DATUM_WIDTH,DIGIT_DATUM_HEIGHT)
		trainingLabels = samples.loadLabelsFile("digitdata/traininglabels", numTraining)
		rawValidationData = samples.loadDataFile("digitdata/validationimages", TEST_SET_SIZE,DIGIT_DATUM_WIDTH,DIGIT_DATUM_HEIGHT)
		validationLabels = samples.loadLabelsFile("digitdata/validationlabels", TEST_SET_SIZE)
		rawTestData = samples.loadDataFile("digitdata/testimages", TEST_SET_SIZE,DIGIT_DATUM_WIDTH,DIGIT_DATUM_HEIGHT)
		testLabels = samples.loadLabelsFile("digitdata/testlabels", TEST_SET_SIZE)
	elif(setName == "facedata"):
		featureFunction = basicFeatureExtractorFace
		rawTrainingData = samples.loadDataFile("facedata/facedatatrain", numTraining,FACE_DATUM_WIDTH,FACE_DATUM_HEIGHT)
		trainingLabels  = samples.loadLabelsFile("facedata/facedatatrainlabels", numTraining)
		rawValidationData = samples.loadDataFile("facedata/facedatavalidation", TEST_SET_SIZE,FACE_DATUM_WIDTH,FACE_DATUM_HEIGHT)
		validationLabels  = samples.loadLabelsFile("facedata/facedatavalidationlabels", TEST_SET_SIZE)
		rawTestData = samples.loadDataFile("facedata/facedatatest", TEST_SET_SIZE,FACE_DATUM_WIDTH,FACE_DATUM_HEIGHT)
		testLabels  = samples.loadLabelsFile("facedata/facedatatestlabels", TEST_SET_SIZE)

	if mode == 'raw':
		trainingData = basicFeatureDataToNumpyArray(map(featureFunction, rawTrainingData))
		validationData = basicFeatureDataToNumpyArray(map(featureFunction, rawValidationData))
		testData = basicFeatureDataToNumpyArray(map(featureFunction, rawTestData))
	else:
		trainingData = np.dot(basicFeatureDataToNumpyArray(map(featureFunction, rawTrainingData)), principleComponents)
		validationData = np.dot(basicFeatureDataToNumpyArray(map(featureFunction, rawValidationData)), principleComponents)
		testData = np.dot(basicFeatureDataToNumpyArray(map(featureFunction, rawTestData)), principleComponents)

	return [trainingData, trainingLabels, validationData, validationLabels, testData, testLabels]

def makeDataset(numList):
	# totalSet =  [rawDataSet, pcaDataSet]
	# (raw, pca)DataSet = [digitDataSet, faceDataSet]
	# (digit, face)DataSet =[dataSet(num=numList[0]), dataSet(num=numList[1]), ...]
	# dataset = [trainingData, trainingLabels, validationData, validationLabels, testData, testLabels]

	print "Getting principle components..."
	dimension = 13
	pa1 = getPrincipleComponents(map(basicFeatureExtractorDigit, samples.loadDataFile("digitdata/trainingimages",5000,DIGIT_DATUM_WIDTH,DIGIT_DATUM_HEIGHT)), dimension)
	pa2 = getPrincipleComponents(map(basicFeatureExtractorFace, samples.loadDataFile("facedata/facedatatrain",451,FACE_DATUM_WIDTH,FACE_DATUM_HEIGHT)), dimension)
	pa = [pa1, pa2]

	totalSet = []
	for i in xrange(2):
		if i == 0:
			mode = 'raw'
		else:
			mode = 'pca'

		modeList = []
		for j in xrange(2):
			if j == 0:
				setName = 'digitdata'
			else:
				setName = 'facedata'

			setList = []
			for num in numList[j]:
				setList.append(loadDataset(setName, mode, num, pa[j]))
			print "Load data " + setName + " " + mode
			modeList.append(setList)
		totalSet.append(modeList)
	return totalSet