import glove
from sklearn import metrics
import numpy as np
from scipy import sparse

def get_glove(X):
    print("X_cooc 생성...")
    X_cooc = X.T @ X
    print("X_cooc 생성 완료")
    X_cooc.setdiag(0)
    result = X_cooc.toarray()
    dic = {}
    for idx1, doc in enumerate(result):
        tmpdic = {}
        for idx2, word2 in enumerate(doc):
            if word2 > 0:
                tmpdic[idx2] = word2
        dic[idx1] = tmpdic
    model = glove.Glove(dic, d=200, alpha=0.75, x_max=5.0)
    for epoch in range(150):
        err = model.train(batch_size=100, workers=4, step_size=0.05)
        print("epoch %d, error %.3f" % (epoch, err), flush=True)
    gloveVectors = model.W
    print("glove vectors shape: " + str(gloveVectors.shape))

    return gloveVectors

