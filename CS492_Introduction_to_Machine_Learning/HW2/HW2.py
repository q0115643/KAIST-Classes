# HW2.py
# -----------
# CS492(E) Homework1
# Logistic Regression and Support Vector Machine
# -----------
# 20140174
# Hyungrok Kim

import numpy as np
np.seterr(divide='ignore', invalid='ignore')
import matplotlib.pyplot as plt
import math
from scipy.special import expit
from scipy.spatial.distance import pdist, squareform
from cvxopt import matrix, solvers

'''
read file and make 5-fold sets, Testing datasets
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
            n = 123 - lastIdx
            for _ in range(n):
                row.append(0)
            row = np.array(row)
            x.append(row)

def getData(filename):
    x = []
    readfile(filename, x)
    x = np.array(x)
    return x

def getTrainingDataset():
    x = getData("a2a")
    foldXs, foldYs, wholeX, wholeY =getRandomCrossValidation(x)
    return foldXs, foldYs, wholeX, wholeY

def getRandomCrossValidation(x):
    foldXs = []
    foldYs = []
    np.random.shuffle(x)
    wholeX = x[:, 1:]
    wholeY = x[:, :1]
    length = int(math.floor(x.shape[0]/5.0))
    temp1 = x[:length, :]
    temp2 = x[length:length*2, :]
    temp3 = x[length*2:length*3, :]
    temp4 = x[length*3:length*4, :]
    temp5 = x[length*4:, :]
    foldXs.append(temp1[:, 1:])
    foldXs.append(temp2[:, 1:])
    foldXs.append(temp3[:, 1:])
    foldXs.append(temp4[:, 1:])
    foldXs.append(temp5[:, 1:])
    foldYs.append(temp1[:, :1])
    foldYs.append(temp2[:, :1])
    foldYs.append(temp3[:, :1])
    foldYs.append(temp4[:, :1])
    foldYs.append(temp5[:, :1])
    return foldXs, foldYs, wholeX, wholeY

def getTestingDataset():
    x = []
    with open("a2a.t") as f:
        lineCount = 0
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
            n = 123 - lastIdx
            for _ in range(n):
                row.append(0)
            row = np.array(row)
            x.append(row)
            lineCount += 1
            if lineCount == 1000:
                break
    x = np.array(x)
    testX = x[:, 1:]
    testY = x[:, :1]
    return testX, testY

def sigmoid(XBeta):
    return expit(XBeta)

def getGrad(trainingX, trainingY, beta):
    gradient = -(trainingX * trainingY).T.dot(1-sigmoid(trainingY * trainingX.dot(beta)))
    return gradient

def getCost(X, Y, beta):
    return (np.sum(-np.log(sigmoid(Y * X.dot(beta)))))

def gradientDescent(trainingX, trainingY, stepSize, iterationNum, gradientCost):
    beta = np.zeros((trainingX.shape[1], 1))  # initial beta: all zero
    gradientCost.append(getCost(trainingX, trainingY, beta))
    for i in range(iterationNum):
        grad = getGrad(trainingX, trainingY, beta)
        beta = beta - stepSize * grad
        cost = getCost(trainingX, trainingY, beta)
        gradientCost.append(cost)
    return beta

def getPredictionError(X, Y, beta):
    temp = Y * X.dot(beta)
    errors = 0
    for item in temp[:, 0]:
        if item < 0:
            errors += 1
    predictionError = errors / Y.shape[0]
    return predictionError

def getPredictionErrorWithGaussian(Y, wx):
    temp = Y * wx
    errors = 0
    for item in temp[:, 0]:
        if item < 0:
            errors += 1
    predictionError = errors / Y.shape[0]
    return predictionError

def runLogisticRegression(trainingX, trainingY, testX, testY, stepSize, iterationNum, gradientCosts):
    gradientCost = []
    # first, validation set is foldXs[4], foldYs[4]
    beta = gradientDescent(trainingX, trainingY, stepSize, iterationNum, gradientCost)
    predictionError = getPredictionError(testX, testY, beta)
    gradientCosts.append(gradientCost)
    return predictionError

def getAlphaWithGaussianK(trainingX, trainingY, C, sigma):
    sampleNum = trainingX.shape[0]
    K = gaussianKernel(trainingX, sigma)
    K = trainingY.dot(trainingY.T) * K
    P = matrix(K)
    q = matrix(-np.ones((sampleNum, 1)))
    temp1 = -np.eye(sampleNum)
    temp2 = np.eye(sampleNum)
    G = matrix(np.vstack((temp1, temp2)))
    temp1 = np.zeros(sampleNum)
    temp2 = np.ones(sampleNum) * C
    h = matrix(np.hstack((temp1, temp2)))
    A = matrix(trainingY.T)
    b = matrix(np.zeros(1))
    solvers.options['show_progress'] = False
    solution = solvers.qp(P, q, G, h, A, b)
    alphas = np.array(solution['x'])
    return alphas

def getAlpha(trainingX, trainingY, C):
    sampleNum = trainingX.shape[0]
    K = trainingY * trainingX
    K = K.dot(K.T)
    P = matrix(K)
    q = matrix(-np.ones((sampleNum, 1)))
    temp1 = -np.eye(sampleNum)
    temp2 = np.eye(sampleNum)
    G = matrix(np.vstack((temp1, temp2)))
    temp1 = np.zeros(sampleNum)
    temp2 = np.ones(sampleNum) * C
    h = matrix(np.hstack((temp1, temp2)))
    A = matrix(trainingY.T)
    b = matrix(np.zeros(1))
    solvers.options['show_progress'] = False
    solution = solvers.qp(P, q, G, h, A, b)
    alphas = np.array(solution['x'])
    return alphas

def runSVM(trainingX, trainingY, validationX, validationY, C):
    alphas = getAlpha(trainingX, trainingY, C)
    # recover w
    w = np.sum(alphas * trainingY * trainingX, axis=0)
    w = w.reshape(w.shape[0], 1)
    errorTraining = getPredictionError(trainingX, trainingY, w)
    errorValidation = getPredictionError(validationX, validationY, w)
    return errorTraining, errorValidation

def getGaussianKernelwithtwo(X1, X2, sigma):
    result = np.zeros((X1.shape[0], X2.shape[0]))
    for i, x1 in enumerate(X1):
        for j, x2 in enumerate(X2):
            x1 = x1.flatten()
            x2 = x2.flatten()
            result[i, j] = np.exp(-np.sum(np.power((x1-x2), 2))/float(2*(sigma**2)))
    return result

def runSVMforWholeTrainingSetWithGaussianK(trainingX, trainingY, testX, testY, C, sigma):
    alphas = getAlphaWithGaussianK(trainingX, trainingY, C, sigma)
    temp = getGaussianKernelwithtwo(trainingX, testX, sigma)
    wx = np.sum(alphas * trainingY * temp, axis=0)
    wx = wx.reshape(wx.shape[0], 1)
    error = getPredictionErrorWithGaussian(testY, wx)
    return error

def runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma):
    alphas = getAlphaWithGaussianK(trainingX, trainingY, C, sigma)
    wxtrain = np.sum(alphas * trainingY * gaussianKernel(trainingX, sigma), axis=0)
    wxtrain = wxtrain.reshape(wxtrain.shape[0], 1)
    temp = getGaussianKernelwithtwo(trainingX, validationX, sigma)
    wxvalid = np.sum(alphas * trainingY * temp, axis=0)
    wxvalid = wxvalid.reshape(wxvalid.shape[0], 1)
    errorTraining = getPredictionErrorWithGaussian(trainingY, wxtrain)
    errorValidation = getPredictionErrorWithGaussian(validationY, wxvalid)
    return errorTraining, errorValidation

def runSVMfor5fold(foldXs, foldYs, C):
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[2], foldXs[3]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[2], foldYs[3]))
    validationX = foldXs[4]
    validationY = foldYs[4]
    trainingError1, validationError1 = runSVM(trainingX, trainingY, validationX, validationY, C)
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[2], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[2], foldYs[4]))
    validationX = foldXs[3]
    validationY = foldYs[3]
    trainingError2, validationError2 = runSVM(trainingX, trainingY, validationX, validationY, C)
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[3], foldYs[4]))
    validationX = foldXs[2]
    validationY = foldYs[2]
    trainingError3, validationError3 = runSVM(trainingX, trainingY, validationX, validationY, C)
    trainingX = np.vstack((foldXs[0], foldXs[2], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[2], foldYs[3], foldYs[4]))
    validationX = foldXs[1]
    validationY = foldYs[1]
    trainingError4, validationError4 = runSVM(trainingX, trainingY, validationX, validationY, C)
    trainingX = np.vstack((foldXs[1], foldXs[2], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[1], foldYs[2], foldYs[3], foldYs[4]))
    validationX = foldXs[0]
    validationY = foldYs[0]
    trainingError5, validationError5 = runSVM(trainingX, trainingY, validationX, validationY, C)
    averageTrainingError = (trainingError1 + trainingError2 + trainingError3 + trainingError4 + trainingError5)/5
    averageValidationError = (validationError1 + validationError2 + validationError3 + validationError4 +
                              validationError5)/5
    return averageTrainingError, averageValidationError

def pickCbyErrors(Cs, validationErrors):
    index_min = np.argmin(validationErrors)
    return Cs[index_min]

def runSVMforWholeTrainingSet(wholeTrainingX, wholeTrainingY, testX, testY, C):
    alphas = getAlpha(wholeTrainingX, wholeTrainingY, C)
    w = np.sum(alphas * wholeTrainingX * wholeTrainingY, axis=0)
    w = w.reshape(w.shape[0], 1)
    predictionError = getPredictionError(testX, testY, w)
    return predictionError

def gaussianKernel(X, sigma):
    pairwise_dists = squareform(pdist(X, 'euclidean'))
    K = np.exp(- pairwise_dists ** 2 / sigma ** 2)
    return K

def runP3(foldXs, foldYs, C, sigma):
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[2], foldXs[3]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[2], foldYs[3]))
    validationX = foldXs[4]
    validationY = foldYs[4]
    trainingError1, validationError1 = runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma)
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[2], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[2], foldYs[4]))
    validationX = foldXs[3]
    validationY = foldYs[3]
    trainingError2, validationError2 = runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma)
    trainingX = np.vstack((foldXs[0], foldXs[1], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[1], foldYs[3], foldYs[4]))
    validationX = foldXs[2]
    validationY = foldYs[2]
    trainingError3, validationError3 = runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma)
    trainingX = np.vstack((foldXs[0], foldXs[2], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[0], foldYs[2], foldYs[3], foldYs[4]))
    validationX = foldXs[1]
    validationY = foldYs[1]
    trainingError4, validationError4 = runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma)
    trainingX = np.vstack((foldXs[1], foldXs[2], foldXs[3], foldXs[4]))
    trainingY = np.vstack((foldYs[1], foldYs[2], foldYs[3], foldYs[4]))
    validationX = foldXs[0]
    validationY = foldYs[0]
    trainingError5, validationError5 = runSVMwithG(trainingX, trainingY, validationX, validationY, C, sigma)
    averageTrainingError = (trainingError1 + trainingError2 + trainingError3 + trainingError4 + trainingError5) / 5
    averageValidationError = (validationError1 + validationError2 + validationError3 + validationError4 +
                              validationError5) / 5
    return averageTrainingError, averageValidationError

'''
run code
'''
def run():
    # get categorized dataset
    foldXs, foldYs, wholeTrainingX, wholeTrainingY = getTrainingDataset()
    testX, testY = getTestingDataset()

    # Problem 1
    print("\nProblem 1. Logistic Regression")
    gradientCosts = []
    gditerationNum = 100
    print("\tNumber of iterations: " + str(gditerationNum))
    stepSize1 = 0.001
    predictionErrorLS1 = runLogisticRegression(wholeTrainingX, wholeTrainingY, testX, testY, stepSize1, gditerationNum,
                                             gradientCosts)
    print("\tStep Size: " + str(stepSize1))
    print("\tPrediction Error: " + str(predictionErrorLS1))
    stepSize2 = 0.0003
    predictionErrorLS2 = runLogisticRegression(wholeTrainingX, wholeTrainingY, testX, testY, stepSize2, gditerationNum,
                                             gradientCosts)
    print("\tStep Size: " + str(stepSize2))
    print("\tPrediction Error: " + str(predictionErrorLS2))
    stepSize3 = 0.0001
    predictionErrorLS3 = runLogisticRegression(wholeTrainingX, wholeTrainingY, testX, testY, stepSize3, gditerationNum,
                                             gradientCosts)
    print("\tStep Size: " + str(stepSize3))
    print("\tPrediction Error: " + str(predictionErrorLS3))

    # Problem 2
    print("\nProblem 2. Support Vector Machine with the linear kernel")
    C1P2 = 0.
    C2P2 = 0.01
    C3P2 = 0.02
    C4P2 = 0.03
    C5P2 = 0.04
    CsP2 = [C1P2, C2P2, C3P2, C4P2, C5P2]
    print("\tC candidates: " + str(C1P2) + ", " + str(C2P2) + ", " + str(C3P2) + ", " + str(C4P2) + ", " + str(C5P2))
    trainingError1P2, validationError1P2 = runSVMfor5fold(foldXs, foldYs, C1P2)
    trainingError2P2, validationError2P2 = runSVMfor5fold(foldXs, foldYs, C2P2)
    trainingError3P2, validationError3P2 = runSVMfor5fold(foldXs, foldYs, C3P2)
    trainingError4P2, validationError4P2 = runSVMfor5fold(foldXs, foldYs, C4P2)
    trainingError5P2, validationError5P2 = runSVMfor5fold(foldXs, foldYs, C5P2)
    trainingErrorsP2 =[trainingError1P2, trainingError2P2, trainingError3P2, trainingError4P2, trainingError5P2]
    validationErrorsP2 = [validationError1P2, validationError2P2, validationError3P2, validationError4P2,
                          validationError5P2]
    CP2 = pickCbyErrors(CsP2, validationErrorsP2)
    predictionErrorP2 = runSVMforWholeTrainingSet(wholeTrainingX, wholeTrainingY, testX, testY, CP2)
    print("\tThe Best Performing C: " + str(CP2) + "\n\tPrediction Error: " + str(predictionErrorP2))

    # Problem 3
    print("\nProblem 3. Support Vector Machine with the Gaussian kernel")
    C1 = 1.0
    C2 = 2.0
    C3 = 3.0 # best performing
    C4 = 4.0
    C5 = 5.0
    sigma1 = 4.0
    sigma2 = 4.5 # best performing one of these two
    sigma3 = 5.0 # best performing
    sigma4 = 5.5
    sigma5 = 6.0
    Cs = [C1, C2, C3, C4, C5]
    sigmas = [sigma1, sigma2, sigma3, sigma4, sigma5]
    print("\tC candidates: " + str(C1) + ", " + str(C2) + ", " + str(C3) + ", " + str(C4) + ", " + str(C5))
    print("\tsigma candidates: " + str(sigma1) + ", " + str(sigma2) + ", " + str(sigma3) + ", " + str(sigma4) + ", " +
          str(sigma5))
    trainingError1C, validationError1C = runP3(foldXs, foldYs, C1, sigma3)
    trainingError2C, validationError2C = runP3(foldXs, foldYs, C2, sigma3)
    trainingError3C, validationError3C = runP3(foldXs, foldYs, C3, sigma3)
    trainingError4C, validationError4C = runP3(foldXs, foldYs, C4, sigma3)
    trainingError5C, validationError5C = runP3(foldXs, foldYs, C5, sigma3)
    trainingErrorsC = [trainingError1C, trainingError2C, trainingError3C, trainingError4C, trainingError5C]
    validationErrorsC = [validationError1C, validationError2C, validationError3C, validationError4C, validationError5C]
    C = pickCbyErrors(Cs, validationErrorsC)
    trainingError1Sigma, validationError1Sigma = runP3(foldXs, foldYs, C, sigma1)
    trainingError2Sigma, validationError2Sigma = runP3(foldXs, foldYs, C, sigma2)
    trainingError3Sigma, validationError3Sigma = runP3(foldXs, foldYs, C, sigma3)
    trainingError4Sigma, validationError4Sigma = runP3(foldXs, foldYs, C, sigma4)
    trainingError5Sigma, validationError5Sigma = runP3(foldXs, foldYs, C, sigma5)
    trainingErrorsSigma = [trainingError1Sigma, trainingError2Sigma, trainingError3Sigma, trainingError4Sigma,
                           trainingError5Sigma]
    validationErrorsSigma = [validationError1Sigma, validationError2Sigma, validationError3Sigma, validationError4Sigma,
                             validationError5Sigma]
    sigma = pickCbyErrors(sigmas, validationErrorsSigma)
    predictionError = runSVMforWholeTrainingSetWithGaussianK(wholeTrainingX, wholeTrainingY, testX, testY, C, sigma)
    print("\tThe Best Performing C: " + str(C) + "\n\tThe Best Performing sigma: " + str(sigma) +
          "\n\tPrediction Error: " + str(predictionError))

    # for graph plot
    # Problem 1
    plt.figure(1)
    xSpace = np.linspace(0, gditerationNum, num=gditerationNum + 1)
    plt.plot(xSpace, gradientCosts[0], label='step size: ' + format(stepSize1, 'f').rstrip('0').rstrip('.'))
    plt.plot(xSpace, gradientCosts[1], label='step size: ' + format(stepSize2, 'f').rstrip('0').rstrip('.'))
    plt.plot(xSpace, gradientCosts[2], label='step size: ' + format(stepSize3, 'f').rstrip('0').rstrip('.'))
    plt.axis([0, gditerationNum, gradientCosts[0][-1] * 0.8, gradientCosts[0][0] * 1.2])
    plt.xlabel('iterations')
    plt.ylabel('cost')
    plt.title('Cost per Iteration (initial beta is zero)')
    plt.legend()
    plt.show()
    # Problem 2
    plt.figure(2)
    width = 0.25
    pos = list(range(len(CsP2)))
    plt.bar(pos, trainingErrorsP2, width, alpha=0.5, label='Training Set')
    plt.bar([p + width for p in pos], validationErrorsP2, width, alpha=0.5, label='Validation Set')
    plt.xticks([p + 0.5 * width for p in pos], CsP2)
    plt.xlabel('Hyperparameter C')
    plt.ylabel('Avg. Prediction Error')
    plt.title('[Linear kernel] Avg. Prediction Error vs C')
    plt.legend(['Training Set', 'Validation Set'])
    plt.show()
    # Problem 3
    plt.figure(3)
    width = 0.25
    pos = list(range(len(Cs)))
    plt.bar(pos, trainingErrorsC, width, alpha=0.5, label='Training Set')
    plt.bar([p + width for p in pos], validationErrorsC, width, alpha=0.5, label='Validation Set')
    plt.xticks([p + 0.5 * width for p in pos], Cs)
    plt.xlabel('Hyperparameter C')
    plt.ylabel('Avg. Prediction Error')
    plt.title('[Gaussian kernel] Avg. Prediction Error vs C when [sigma=' + str(sigma3) + ']')
    plt.legend(['Training Set', 'Validation Set'])
    plt.show()
    plt.figure(4)
    width = 0.25
    pos = list(range(len(sigmas)))
    plt.bar(pos, trainingErrorsSigma, width, alpha=0.5, label='Training Set')
    plt.bar([p + width for p in pos], validationErrorsSigma, width, alpha=0.5, label='Validation Set')
    plt.xticks([p + 0.5 * width for p in pos], sigmas)
    plt.xlabel('Hyperparameter sigma')
    plt.ylabel('Avg. Prediction Error')
    plt.title('[Gaussian kernel] Avg. Prediction Error vs sigma when [C=' + str(C) + ']')
    plt.legend(['Training Set', 'Validation Set'])
    plt.show()

'''
main
'''
run()