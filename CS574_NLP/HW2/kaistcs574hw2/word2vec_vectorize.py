from gensim.models import doc2vec, Word2Vec, Doc2Vec
from collections import namedtuple
import numpy as np

def get_w2v(sentences, vocab, size, window, epochs):
    model = Word2Vec(size=size, window=window, min_count=1, workers=8, iter=epochs, sg=1, sorted_vocab=0)
    model.build_vocab(vocab)
    model.train(sentences, total_examples=model.corpus_count, epochs=model.iter)
    # model.wv.index2word
    word_vectors = model.wv.syn0
    # print(model.wv.vocab)
    print(model.most_similar('북한'))
    return word_vectors