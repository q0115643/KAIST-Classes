import os
import codecs
import numpy as np
import classify
import scipy.sparse as sp
import pickle

def read_files(data_loc, size, window, iter, min_df, max_df):
    """Read the data files.
    The returned object contains various fields that store the data.
    The data is preprocessed for use with scikit-learn.
    
    Description of each fileds of returned object are as follows.
    
    - count_vec: CountVectorizer used to process the data (for reapplication on new data)
    - trainX,devX,testX,unlabeledX: Array of vectors representing Bags of Words, i.e. documents processed through the vectorizer
    - le: LabelEncoder, i.e. a mapper from string labels to ints (stored for reapplication)
    - target_labels: List of labels (same order as used in le)
    - trainy,devy: Array of int labels, one for each document
    - test_fnames: List of test file names
    """

    class Data: pass
    speech = Data()

    print("-- train data")
    speech.train_data, speech.train_fnames, speech.train_labels = read_tsv(data_loc, "train.tsv")
    print(len(speech.train_data))

    print("-- dev data")
    speech.dev_data, speech.dev_fnames, speech.dev_labels = read_tsv(data_loc, "dev.tsv")
    print(len(speech.dev_data))

    print("-- test data")
    test_data, test_fnames = read_unlabeled(data_loc, 'test')

    #print("-- unlabeled data")
    #unlabeled_data, unlabeled_fnames = read_unlabeled(data_loc, 'unlabeled')
    #print(len(unlabeled_fnames))

    print("-- transforming data and labels")

    from sklearn.feature_extraction.text import CountVectorizer, TfidfVectorizer

    '''
    for word2vec method
    '''
    speech.count_vect = TfidfVectorizer(tokenizer=lambda doc: doc, lowercase=False, min_df=min_df, max_df=max_df)
    speech.trainX = speech.count_vect.fit_transform(speech.train_data)
    speech.vocab = speech.count_vect.get_feature_names()
    speech.vocab = [[word] for word in speech.vocab]

    print("trainX 생성 완료 shape: " + str(speech.trainX.shape))
    speech.devX = speech.count_vect.transform(speech.dev_data)
    speech.testX = speech.count_vect.transform(test_data)
    #speech.unlabeledX = speech.count_vect.transform(unlabeled_data)

    import word2vec_vectorize
    speech.word_vectors = word2vec_vectorize.get_w2v(speech.train_data, speech.vocab, size, window, iter)
    #speech.word_vectors = word2vec_vectorize.get_doc2vec(speech.all_sentences, speech.vocab, size, window, iter)
    print("word2vec 생성 완료: " + str(speech.word_vectors.shape))

    '''
    # 저장
    w2vFileName = 'w2v.p'
    w2vFilePath = os.path.join('w2v_learned', w2vFileName)
    with open(w2vFilePath, 'wb') as w2vFile:
        pickle.dump(speech.word_vectors, w2vFile)
        print('saved file: ' + w2vFilePath)
    # 로드
    w2vFileName = 'w2v.p'
    w2vFilePath = os.path.join('w2v_learned', w2vFileName)
    with open(w2vFilePath, "rb") as w2vFile:
        speech.word_vectors = pickle.load(w2vFile)
    '''


    # glove
    import glove_vectorize
    '''
    # glove
    # 저장
    speech.word_vectors = glove_vectorize.get_glove(speech.trainX)
    print("glove_vectors 생성 완료")
    gloveFileName = 'glove_vectors.p'
    gloveFilePath = os.path.join('glove_learned', gloveFileName)
    with open(gloveFilePath, 'wb') as gloveFile:
        pickle.dump(speech.glove_vectors, gloveFile)
        print('saved file: ' + gloveFilePath)
    
    # 로드
    gloveFileName = 'glove_vectors.p'
    gloveFilePath = os.path.join('glove_learned', gloveFileName)
    with open(gloveFilePath, "rb") as gloveFile:
        speech.word_vectors = pickle.load(gloveFile)
    '''

    speech.trainX = speech.trainX.A
    speech.testX = speech.testX.A
    speech.devX = speech.devX.A

    speech.train_doc_vec = classify.get_doc_vec(speech.trainX, speech.word_vectors)
    speech.test_doc_vec = classify.get_doc_vec(speech.testX, speech.word_vectors)
    speech.dev_doc_vec = classify.get_doc_vec(speech.devX, speech.word_vectors)
    #speech.unlabeled_doc_vec = word2vec_vectorize.get_doc_vec(speech.unlabeledX, speech.glove_vectors)



    speech.test_fnames = test_fnames

    from sklearn import preprocessing
    speech.le = preprocessing.LabelEncoder()
    speech.le.fit(speech.train_labels)
    speech.target_labels = speech.le.classes_
    speech.trainy = speech.le.transform(speech.train_labels)
    speech.devy = speech.le.transform(speech.dev_labels)
    return speech

def read_tsv(data_loc, fname):
    """Reads the labeled data described in tsv file.
    The returned object contains three fields that represent the unlabeled data.
    data: documents, each document is represented as list of words
    fnames: list of filenames, one for each document
    labels: list of labels for each document (List of string)
    """
    tf = codecs.open(data_loc + fname, 'r', encoding='utf-8')
    data = []
    labels = []
    fnames = []
    for line in tf:
        (ifname, label) = line.strip().split("\t")
        content = read_instance(data_loc, ifname)
        labels.append(label)
        fnames.append(ifname)
        data.append(content)
    tf.close()
    return data, fnames, labels

def read_unlabeled(data_loc, dname):
    """Reads the unlabeled data.
    The returned object contains two fields that represent the unlabeled data.
    data: documents, each document is represented as list of words
    fnames: list of filenames, one for each document
    """
    data = []
    fnames = []
    raw_fnames = os.listdir(data_loc+dname)
    for raw_fname in raw_fnames:
        fname = dname + '/' + raw_fname
        content = read_instance(data_loc, fname)
        data.append(content)
        fnames.append(fname)
    return data, fnames

def read_instance(data_loc, ifname):
    """Reads the document file.
    Each document file has a string represents sequence of words, 
    and each words are seperated by semicolon.
    This function convert this string into list of words and return it.
    """
    inst = data_loc + ifname
    ifile = codecs.open(inst, 'r', encoding='utf-8')
    content = ifile.read().strip()
    content = content.split(';')
    return content

def write_pred_kaggle_file(cls, outfname, speech):
    """Writes the predictions in Kaggle format.

    Given the classifier, output filename, and the speech object,
    this function write the predictions of the classifier on the test data and
    writes it to the outputfilename. 
    """
    yp = cls.predict(speech.test_doc_vec)
    labels = speech.le.inverse_transform(yp)
    f = codecs.open(outfname, 'w')
    f.write("FileIndex,Category\n")
    for i in range(len(speech.test_fnames)):
        fname = speech.test_fnames[i]
        f.write(fname + ',' + labels[i] + '\n')
    f.close()

def run(size, window, iter, min_df, max_df):
    print("Reading data")
    data_loc = "data/"
    speech = read_files(data_loc, size, window, iter, min_df, max_df)

    print("Training classifier")
    cls = classify.train_classifier(speech.train_doc_vec, speech.trainy)

    # cls = classify.semi_supervised_learning(cls, speech.train_doc_vec, speech.trainy, speech.unlabeled_doc_vec, speech.dev_doc_vec, speech.devy)

    print("Evaluating")
    train_acc = classify.evaluate(speech.train_doc_vec, speech.trainy, cls)
    dev_acc = classify.evaluate(speech.dev_doc_vec, speech.devy, cls)

    print("Writing Kaggle pred file")
    write_pred_kaggle_file(cls, "data/speech-pred.csv", speech)
    print("size: " + str(size) + "   window: " + str(window) + "   iter: " + str(iter))
    print("min_df: " + str(min_df) + "   max_df: " + str(max_df))
    print("train_acc: " + str(train_acc))
    print("dev_acc: " + str(dev_acc))
    print("=================================")

    return dev_acc

if __name__ == "__main__":
    # run(size, window, iter, min_df, max_df)
    print("=================================")
    size = 100
    window = 5
    iter = 10
    min_df = 1
    max_df = 1.0
    acc0 = run(size, window, iter, min_df, max_df)
    print("=====================")
    print("size: " + str(size) + "   window: " + str(window) + "   iter: " + str(iter))
    print("max_acc: " + str(acc0))
    print("=====================")





