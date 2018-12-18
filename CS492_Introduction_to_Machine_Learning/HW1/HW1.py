# HW1.py
# -----------
# CS492(E) Homework1
# Optimization problem and Linear Regression
# -----------
# 20140174
# Hyungrok Kim

import numpy as np
np.seterr(divide='ignore', invalid='ignore')
import matplotlib.pyplot as plt
import math

'''
read file and make random Training, Testing datasets
'''
def readfile(filename, x):
    with open(filename) as f:
        for line in f:
            data = line.split()
            target = float(data[0])
            row = [target]
            lastIdx = 0
            for i, (idx, value) in enumerate([item.split(':') for item in data[1:]]):
                n = int(idx) - (lastIdx + 1)
                for _ in range(n):
                    row.append(0)
                row.append(float(value))
                lastIdx = int(idx)
            n = 13 - lastIdx
            for _ in range(n):
                row.append(0)
            row.append(1) # decide to add bias parameter beta0
            x.append(row)

def getData(filename):
    x = []
    readfile(filename, x)
    x = np.array(x)
    return x

def getCaterizedData(x):
    np.random.shuffle(x)
    numData = x.shape[0]
    trainingDataNum = round(numData * 0.8)
    training, test = x[:trainingDataNum, :], x[trainingDataNum:, :]
    trainingY, trainingX = training[:, :1], training[:, 1:]
    testY, testX = test[:, :1], test[:, 1:]
    return testY, testX, trainingY, trainingX

def getRandomSets(numberOfTrials):
    testYs = []
    testXs = []
    trainingYs = []
    trainingXs = []
    x = getData("dataset.txt")
    for i in range(0, numberOfTrials):
        testY, testX, trainingY, trainingX = getCaterizedData(x)
        testYs.append(np.copy(testY))
        testXs.append(np.copy(testX))
        trainingXs.append(np.copy(trainingX))
        trainingYs.append(np.copy(trainingY))
    return testYs, testXs, trainingYs, trainingXs


'''
get prediction error
'''
def getAverageResidual(testX, testY, beta):
    residual = testY - testX.dot(beta)
    return np.linalg.norm(residual, ord=1) / testY.shape[0]


'''
functions for problem1
'''
def getAnalyticBeta(trainingY, trainingX):
    beta = np.linalg.inv(trainingX.T.dot(trainingX)).dot(trainingX.T).dot(trainingY)
    return beta

def testP1(testYs, testXs, trainingXs, trainingYs, analyticBetas, numberOfTrials):
    averageError = []
    for i in range(0, numberOfTrials):
        analyticBetas.append(getAnalyticBeta(trainingYs[i], trainingXs[i]))
        averageError.append(getAverageResidual(testXs[i], testYs[i], analyticBetas[i]))
    return averageError

def runP1Trials(testYs, testXs, trainingXs, trainingYs, analyticBetas, numberOfTrials):
    print("\nProblem 1. using anaytic solution")
    print("\ttrial  | prediction error")
    average = 0
    error = testP1(testYs, testXs, trainingXs, trainingYs, analyticBetas, numberOfTrials)
    for i in range(0, numberOfTrials):
        average += error[i]
        print("\ttrial " + str(i+1) + ": " + str(error[i]))
    average = average / numberOfTrials
    print("\tIn average: " + str(average))
    # test without bias
    print("\n\tWithout Bias(beta0)")
    average = 0
    analyticBetasWithoutBias = []
    testXsWithoutBias = []
    trainingXsWithoutBias = []
    for i in range(0, len(testXs)):
        testXsWithoutBias.append(np.copy(testXs[i])[:, 0:testXs[i].shape[1]-2])
    for i in range(0, len(trainingXs)):
        trainingXsWithoutBias.append(np.copy(trainingXs[i])[:, 0:trainingXs[i].shape[1]-2])
    error = testP1(testYs, testXsWithoutBias, trainingXsWithoutBias, trainingYs, analyticBetasWithoutBias, numberOfTrials)
    for i in range(0, numberOfTrials):
        average += error[i]
        print("\ttrial " + str(i+1) + ": " + str(error[i]))
    average = average / numberOfTrials
    print("\tIn average: " + str(average))

'''
functions for problem2
'''
def getGrad(trainingX, trainingY, beta):
    gradient = trainingX.T.dot(trainingX.dot(beta) - trainingY)
    return gradient

def gradientDescent(trainingY, trainingX, stepSize, iterations, gdCostsIn1stSet, stepSizeIdx, trialIdx):
    beta = np.zeros((trainingX.shape[1], 1)) # initial beta
    gdCostsIn1stSet[stepSizeIdx, 0] = getLoss(trainingY, trainingX, beta)
    for i in range(1, iterations):
        grad = getGrad(trainingX, trainingY, beta)
        beta = beta - stepSize * grad
        if trialIdx == 0:
            gdCostsIn1stSet[stepSizeIdx, i] = getLoss(trainingY, trainingX, beta)
    return beta

def testP2(testYs, testXs, trainingXs, trainingYs, numberOfTrials, stepSize, iterations, gdCostsIn1stSet, stepSizeIdx):
    errors = []
    for trialIdx in range(0, numberOfTrials):
        beta = gradientDescent(trainingYs[trialIdx], trainingXs[trialIdx], stepSize, iterations, gdCostsIn1stSet, stepSizeIdx, trialIdx)
        errors.append(getAverageResidual(testXs[trialIdx], testYs[trialIdx], beta))
    return errors

def runP2Trials(testYs, testXs, trainingXs, trainingYs, stepSizes, iterations, numberOfTrials, gdCostsIn1stSet):
    print("\nProblem 2. using gradient descent")
    stepSizeIdx = 0
    for stepSize in stepSizes:
        average = 0
        errors = testP2(testYs, testXs, trainingXs, trainingYs, numberOfTrials, stepSize, iterations, gdCostsIn1stSet, stepSizeIdx)
        print("\tstep size: " + format(stepSize, 'f').rstrip('0').rstrip('.'))
        print("\ttrial  | prediction error")
        for i in range(0, numberOfTrials):
            average += errors[i]
            print("\ttrial " + str(i + 1) + ": " + str(errors[i]))
        average = average / numberOfTrials
        print("\tIn average: " + str(average) + "\n")
        stepSizeIdx += 1

def getLoss(Y, X, beta):
    return np.linalg.norm((Y - X.dot(beta)), 'fro')


'''
functions for problem3
'''
def getPartialGrad(trainingX, trainingY, beta, betaIdx):
    return getGrad(trainingX, trainingY, beta)[betaIdx, 0]

def coordnateDescent(trainingY, trainingX, stepSize, iterations, coodGdCostsIn1stSet, trialIdx):
    beta = np.zeros((trainingX.shape[1], 1))  # initial beta
    coodGdCostsIn1stSet[0, 0] = getLoss(trainingY, trainingX, beta)
    count = 1
    for betaIdx in range(0, beta.shape[0]):
        for i in range(0, round(iterations/beta.shape[0])):
            partialGrad = getPartialGrad(trainingX, trainingY, beta, betaIdx)
            beta[betaIdx, 0] = beta[betaIdx, 0] - stepSize * partialGrad
            if trialIdx == 0:
                coodGdCostsIn1stSet[0, count] = getLoss(trainingY, trainingX, beta)
            count += 1
    return beta

def testP3(testYs, testXs, trainingXs, trainingYs, iterations, stepSize, numberOfTrials, coodGdCostsIn1stSet):
    errors = []
    for trialIdx in range(0, numberOfTrials):
        beta = coordnateDescent(trainingYs[trialIdx], trainingXs[trialIdx], stepSize, iterations, coodGdCostsIn1stSet, trialIdx)
        errors.append(getAverageResidual(testXs[trialIdx], testYs[trialIdx], beta))
    return errors

def runP3Trials(testYs, testXs, trainingXs, trainingYs, iterations, stepSize, numberOfTrials, coodGdCostsIn1stSet):
    print("\nProblem 3. using coordinate descent")
    average = 0
    errors = testP3(testYs, testXs, trainingXs, trainingYs, iterations, stepSize, numberOfTrials, coodGdCostsIn1stSet)
    print("\tstep size: " + format(stepSize, 'f').rstrip('0').rstrip('.'))
    print("\ttrial | prediction error")
    for i in range(0, numberOfTrials):
        average += errors[i]
        print("\ttrial " + str(i + 1) + ": " + str(errors[i]))
    average = average / numberOfTrials
    print("\tIn average: " + str(average) + "\n")


'''
Run code
'''
def run():
    # number of trials (given 10)
    numberOfTrials = 10

    # arrays of 10 random datasets
    testYs, testXs, trainingYs, trainingXs = getRandomSets(numberOfTrials)

    # Problem 1.
    analyticBetas = []
    runP1Trials(testYs, testXs, trainingXs, trainingYs, analyticBetas, numberOfTrials)

    # Problem 2.
    #  set number of iterations
    iterations = 500
    # set step sizes
    largeStepSize = 0.01
    smallStepSize = 0.00001
    properStepSize = 0.0005
    stepSizes = [largeStepSize, smallStepSize, properStepSize]
    # initialize array to implement gradient descent costs for each iteration(for first random training dataset)
    gdCostsIn1stSet = np.zeros((len(stepSizes), iterations))
    # set array of analytic cost for first random training dataset
    analyticCostIn1stSet = getLoss(trainingYs[0], trainingXs[0], getAnalyticBeta(trainingYs[0], trainingXs[0]))
    analyticCostsIn1stSet = np.full((1, iterations), analyticCostIn1stSet)
    # run algorithm for problem2
    runP2Trials(testYs, testXs, trainingXs, trainingYs, stepSizes, iterations, numberOfTrials, gdCostsIn1stSet)
    # draw plot for saved data
    xSpace = np.linspace(1, iterations, num=iterations)
    plt.plot(xSpace, gdCostsIn1stSet[0], label='large step size: ' + format(stepSizes[0], 'f').rstrip('0').rstrip('.'))
    plt.plot(xSpace, gdCostsIn1stSet[1], label='small step size: ' + format(stepSizes[1], 'f').rstrip('0').rstrip('.'))
    plt.plot(xSpace, gdCostsIn1stSet[2], label='proper step size: ' + format(stepSizes[2], 'f').rstrip('0').rstrip('.'))
    plt.plot(xSpace, analyticCostsIn1stSet[0], label='optimal value')
    plt.axis([0, iterations, analyticCostIn1stSet*2/3, gdCostsIn1stSet[0][0]*1.2])
    plt.xlabel('iterations')
    plt.ylabel('cost')

    # Problem 3.
    # set step size
    coordStepSize = 0.003
    # set number of iterations by value greater or equal than previous iterations and multiple of number of variable
    coordinateIterations = round(math.ceil(iterations / analyticBetas[0].shape[0])) * analyticBetas[0].shape[0]
    # initialize array to implement coordinate descent costs for each iteration(for first random training dataset)
    coodGdCostsIn1stSet = np.zeros((1, coordinateIterations + 1))
    # set x space again by number of iterations
    coordXSpace = np.linspace(1, coordinateIterations + 1, num=coordinateIterations + 1)
    # run algorithm for problem3
    runP3Trials(testYs, testXs, trainingXs, trainingYs, coordinateIterations, coordStepSize, numberOfTrials, coodGdCostsIn1stSet)
    # draw plot for saved data
    plt.plot(coordXSpace, coodGdCostsIn1stSet[0], label='coordinate descent with step size: ' + format(coordStepSize, 'f').rstrip('0').rstrip('.'))

    # show the graph
    plt.title('Cost per Iteration (initial beta is zero)')
    plt.legend()
    plt.show()

'''
Execute
'''
run()