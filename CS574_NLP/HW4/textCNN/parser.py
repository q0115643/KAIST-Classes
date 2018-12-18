from konlpy.tag import Kkma
from gensim.models import KeyedVectors
import re
import numpy as np
import utils
import torch
import random


def fileToMat(filename, w2vec, maxLen, label_set, train=True):
    kkma = Kkma()
    train_f = open(filename, 'r', encoding='utf-8')
    mat = []
    line_num = 0
    for line in train_f.read().splitlines():
        sen = {}
        line_splitted = line.split('\t')
        sbj = line_splitted[0].strip()
        obj = line_splitted[1].strip()
        relation = line_splitted[2].strip()
        sentence = line_splitted[3].strip()
        sentence_complete = re.sub('<< _obj_ >>', obj, re.sub('<< _sbj_ >>', sbj, sentence))
        sentence_complete = utils.clean_str(sentence_complete)
        tokens = [p[0] + '/' + p[1] for p in kkma.pos(sentence_complete) if p[0] + '/' + p[1] in w2vec.vocab]
        if maxLen < len(tokens):
            if train:
                maxLen = len(tokens)
            else:
                tokens = tokens[:maxLen]
        label_set.add(relation)
        sen['sbj'] = sbj
        sen['obj'] = obj
        sen['relation'] = relation
        sen['sentence'] = sentence
        sen['tokens'] = tokens
        mat.append(sen)
        line_num += 1
    train_f.close()
    return mat, label_set, maxLen, line_num

def sentenceToEmbedding(tokens, maxLen, w2vec):
    embedd = np.zeros((maxLen, 200))
    assert maxLen >= len(tokens)
    for i, token in enumerate(tokens):
        embedd[i, :] = w2vec[token]
    return embedd

def labelToIdx(label, label_list):
    return label_list.index(label)

def idxToLabel(idx, label_list):
    return label_list[idx]

def getAllData(trainFName, testFName, args):
    w2vec = KeyedVectors.load_word2vec_format('../kor-word2vec-kkma-200.bin', binary=True)
    train_mat, label_set, maxLen, train_size = fileToMat(trainFName, w2vec, 0, set(), train=True)
    random.shuffle(train_mat)
    test_mat, label_set, _, test_size = fileToMat(testFName, w2vec, maxLen, label_set, train=False)
    label_list = list(label_set)
    trainTensor = np.zeros((train_size, maxLen, 200))
    trainLabelTensor = np.zeros(train_size)
    for i, sentence in enumerate(train_mat):
        embed = sentenceToEmbedding(sentence['tokens'], maxLen, w2vec)
        label = sentence['relation']
        idx = labelToIdx(label, label_list)
        trainTensor[i, :, :] = embed
        trainLabelTensor[i] = idx
    testTensor = np.zeros((test_size, maxLen, 200))
    testLabelTensor = np.zeros(test_size)
    for i, sentence in enumerate(test_mat):
        # (maxLen, 200)
        embed = sentenceToEmbedding(sentence['tokens'], maxLen, w2vec)
        label = sentence['relation']
        testTensor[i, :, :] = embed
        idx = labelToIdx(label, label_list)
        testLabelTensor[i] = idx

    trainTensor = torch.from_numpy(trainTensor).float()
    if not args.static:
        trainTensor.requires_grad = True
    trainLabelTensor = torch.from_numpy(trainLabelTensor).long()
    testTensor = torch.from_numpy(testTensor).float()
    testLabelTensor = torch.from_numpy(testLabelTensor).long()
    return trainTensor, trainLabelTensor, testTensor, testLabelTensor, label_list








