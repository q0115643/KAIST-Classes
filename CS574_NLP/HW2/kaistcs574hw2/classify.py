from sklearn import metrics
from sklearn.linear_model import LogisticRegression
import numpy as np
from scipy import sparse

def train_classifier(X, y):
    """Train a classifier using the given training data.
    Trains a logistic regression on the input data with default parameters.
    cls = classify.train_classifier(speech.trainX, speech.trainy) 이걸로 호출.
    """
    mask = np.where(np.isnan(X))
    X[mask] = 0
    #print("train_classifier... X.shape: " + str(X.shape) + " Y.shape: " + str(y.shape))
    cls = LogisticRegression()
    #print("trying to fit...")
    cls.fit(X, y)
    #print("fit done")
    return cls

def semi_supervised_learning(cls, trainX, trainy, unlabeledX, devX, devy):
    length = unlabeledX.shape[0]
    max_acc = 0
    for i in range(length//100):
        tmp = unlabeledX[100*i:100*i + 100]
        y = cls.predict(tmp)
        trainy_ = np.append(trainy, y)
        trainX_ = sparse.vstack((trainX, tmp)).A
        cls.fit(trainX_, trainy_)
        acc = evaluate(devX, devy, cls)
        if acc > max_acc:
            cls.fit(trainX, trainy)
        else:
            trainX = trainX_
            trainy = trainy_
    return cls


def evaluate(X, yt, cls):
    """Evaluated a classifier on the given labeled data using accuracy."""
    mask = np.where(np.isnan(X))
    X[mask] = 0
    yp = cls.predict(X)
    acc = metrics.accuracy_score(yt, yp)
    #print("  Accuracy", acc)
    return acc


def get_doc_vec(X, word_vectors):
    mask = np.where(np.isnan(X))
    X[mask] = 0
    X_count = X.sum(axis=1)
    doc_vec = X @ word_vectors
    doc_vec = doc_vec / X_count[:, None]

    return doc_vec

