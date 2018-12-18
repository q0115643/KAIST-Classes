# -*- coding: utf-8 -*-
from neuralNetwork import NeuralNetworkClassifier as SolutionClassifier
from neuralNetworkSolution import NeuralNetworkClassifier as HomeworkClassifier
from scoringHW import loadData, runTestList

def main():

	data = loadData(SolutionClassifier, saveData = False)

	testList = [1, 2, 3]
	# Change the previous line to this to test time limit test
	# testList = [1, 2, 3]
	# Change the testList to None to do full test
	# testList = None
	# Pick any test number you want to do specific tests
	# ex) testList = [3]

	score, stScoreList = runTestList(data, HomeworkClassifier, SolutionClassifier, testList = testList, verbose = 1)
	print "Total score: %d" % (score)
	if testList is not None:
		for i in xrange(len(testList)):
			print "\tTest %d: %d" % (testList[i], stScoreList[i])
	else:
		for i in xrange(len(stScoreList)):
			print "\tTest %d: %d" % ((i+1), stScoreList[i])

if __name__=='__main__':
	main()