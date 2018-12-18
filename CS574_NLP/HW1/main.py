'''
KAIST 18S CS574
HW1
20140174 Hyungrok Kim
'''


import os
import re
import math
vocab = set()


#####################################
#                                   #
#          Help Functions           #
#                                   #
#####################################
'''
get_BoW(txt, mode): from one document return tokenized-BoW(bag of words)
    string, string -> list of string
'''


def get_BoW(txt, mode):
    global vocab
    BoW = dict()
    # with the document txt,
    # substitute ~n't => ~ not
    # erase '~(~'s, ~'re, ...) and html tags <~>
    # make all alphabets in lower form
    # erase other than alphabets and dot(.)
    # if not|no|never appears, change two words after by adding 'NOT_' in front
    #   or until dot(.)
    words = re.split(r"[^a-z.]+", re.sub("[.]", " . ", re.sub("'.*?\s|<.*?>", "", re.sub("n't", " not", txt)).lower()))
    if words[-1] == '':
        words.pop()
    flag = 0
    cnt = 2
    if mode == 'train':
        vocab.update(words)
        for word in words:
            if flag == 1:
                if word == '.':
                    flag = 0
                    continue
            if flag == 1 and cnt >= 0:
                word = "NOT_" + word
                cnt -= 1
            if word == "no" or word == "not" or word == "never":
                flag = 1
                cnt = 2
            if word not in BoW:
                BoW[word] = 1
                vocab.add(word)
    else:
        for word in words:
            if flag == 1:
                if word == '.':
                    flag = 0
                    continue
            if flag == 1 and cnt >= 0:
                word = "NOT_" + word
                cnt -= 1
            if word == "no" or word == "not" or word == "never":
                flag = 1
                cnt = 2
            if word in vocab:
                if word in BoW:
                    BoW[word] += 1
                else:
                    BoW[word] = 1
    return BoW


'''
parse_files(path, mode): from given path of file read file and get BoW, return it in a dictionary
    string, string -> dictionary with key of file_id and item of BoW
'''


def parse_files(path, mode):
    result = dict()
    dir = os.listdir(path)
    for file_name in dir:
        file = open(path + file_name, 'r', encoding='UTF-8').read()
        test_pos_file_name_splitted = re.findall('\d+', file_name)
        file_id = test_pos_file_name_splitted[0]
        file_rating = test_pos_file_name_splitted[1]
        print("Parsing file " + file_id + " from ./aclImdb/" + mode + "/...")
        result[file_id] = dict()
        result[file_id]['rating'] = file_rating
        result[file_id]['BoW'] = get_BoW(file, mode)
    return result


'''
read_files(): read files of train/pos/ & train/neg/ & test/pos/ & test/neg and get it organized in a dictionary
    -> dictionary of categorized document id and BoW
'''


def read_files():
    dataset = dict()
    dataset['test'] = dict()
    dataset['train'] = dict()
    dataset['train']['pos'] = parse_files('./aclImdb/train/pos/', 'train')
    dataset['train']['neg'] = parse_files('./aclImdb/train/neg/', 'train')
    dataset['test']['pos'] = parse_files('./aclImdb/test/pos/', 'test')
    dataset['test']['neg'] = parse_files('./aclImdb/test/neg/', 'test')
    return dataset


'''
get_word_word_vector(dataset, mode): from organized dataset build contingency table(not probability, count of appearance)
    dictionary, string -> (dictionary, int, int) | dictionary
'''


def get_word_word_vector(dataset, mode):
    word_vector = dict()
    total_word_count = 0
    total_doc_count = 0
    if mode == 'train':
        for file_id, rating_BoW in dataset.items():
            print("Building contingency table from file " + file_id + " from ./aclImdb/train/...")
            BoW = rating_BoW['BoW']
            total_doc_count += 1
            total_word_count += len(BoW)
            for word in BoW:
                if word in word_vector:
                    word_vector[word] += 1
                else:
                    word_vector[word] = 1
        return word_vector, total_doc_count, total_word_count
    else:
        for file_id, rating_BoW in dataset.items():
            print("Building contingency table from file " + file_id + " from ./aclImdb/test/...")
            word_vector[file_id] = rating_BoW['BoW']
        return word_vector


'''
get_NB_dataset(dataset): with dictionary of dataset, organize it more to use in classification
    dictionary -> dictionary
'''


def get_NB_dataset(dataset):
    nb_vec_train_pos, train_pos_doc_count, train_pos_word_count = get_word_word_vector(dataset['train']['pos'], 'train')
    nb_vec_train_neg, train_neg_doc_count, train_neg_word_count = get_word_word_vector(dataset['train']['neg'], 'train')
    nb_vec_set_test_pos = get_word_word_vector(dataset['test']['pos'], 'test')
    nb_vec_set_test_neg = get_word_word_vector(dataset['test']['neg'], 'test')
    nb_dataset = dict()
    nb_dataset['train'] = dict()
    nb_dataset['test'] = dict()
    nb_dataset['train']['pos'] = dict()
    nb_dataset['train']['neg'] = dict()
    nb_dataset['test']['pos'] = dict()
    nb_dataset['test']['neg'] = dict()
    nb_dataset['train']['pos']['word_vector'] = nb_vec_train_pos
    nb_dataset['train']['pos']['word_count'] = train_pos_word_count
    nb_dataset['train']['pos']['doc_count'] = train_pos_doc_count
    nb_dataset['train']['neg']['word_vector'] = nb_vec_train_neg
    nb_dataset['train']['neg']['word_count'] = train_neg_word_count
    nb_dataset['train']['neg']['doc_count'] = train_neg_doc_count
    nb_dataset['train']['doc_count'] = train_pos_doc_count + train_neg_doc_count
    nb_dataset['test']['pos']['word_vector_set'] = nb_vec_set_test_pos
    nb_dataset['test']['neg']['word_vector_set'] = nb_vec_set_test_neg
    return nb_dataset


'''
get_P_word_vector_class(p_w_pos, p_w_neg, word_vector, class_dataset, V): with given data, calculate P(document, class)
    float, float, dictionary, dictionary, int -> float, float
'''


def get_P_word_vector_class(p_w_pos, p_w_neg, word_vector, class_dataset, V):
    for word, word_count in word_vector.items():
        if word in class_dataset['pos']['word_vector']:
            p_w_pos += math.log(class_dataset['pos']['word_vector'][word] + 1)
        p_w_pos -= math.log(class_dataset['pos']['word_count'] + V)
        if word in class_dataset['neg']['word_vector']:
            p_w_neg += math.log(class_dataset['neg']['word_vector'][word] + 1)
        p_w_neg -= math.log(class_dataset['neg']['word_count'] + V)
    return p_w_pos, p_w_neg


#####################################
#                                   #
#          Train & Classify         #
#                                   #
#####################################
'''
train_nb(): read documents, train dataset in dictionary form.
    -> dictionary
'''


def train_nb():
    return get_NB_dataset(read_files())


'''
classify_nb(nb_dataset, p_pos_prior, p_neg_prior): with trained dataset, classify the test dataset
                                                    , get precision, recall, f1
    dictionary -> float, float, float
'''


def classify_nb(nb_dataset):
    print("Calculating the result...")
    V = len(vocab)
    true_pos = 0
    true_neg = 0
    false_pos = 0
    false_neg = 0
    p_pos_prior = math.log(nb_dataset['train']['pos']['doc_count']) - math.log(nb_dataset['train']['doc_count'])
    p_neg_prior = math.log(nb_dataset['train']['neg']['doc_count']) - math.log(nb_dataset['train']['doc_count'])
    for file_id, word_vector in nb_dataset['test']['pos']['word_vector_set'].items():
        p_pos, p_neg = get_P_word_vector_class(p_pos_prior, p_neg_prior, word_vector, nb_dataset['train'], V)
        if p_pos > p_neg:
            true_pos += 1
        else:
            false_neg += 1
    for file_id, word_vector in nb_dataset['test']['neg']['word_vector_set'].items():
        p_pos, p_neg = get_P_word_vector_class(p_pos_prior, p_neg_prior, word_vector, nb_dataset['train'], V)
        if p_pos > p_neg:
            false_pos += 1
        else:
            true_neg += 1
    precision = true_pos / (true_pos + false_pos)
    recall = true_pos / (true_pos + false_neg)
    f1 = 2 * precision * recall / (precision + recall)
    return precision, recall, f1


'''
run(): train and classify the dataset
'''


def run():
    nb_dataset = train_nb()
    precision, recall, f1 = classify_nb(nb_dataset)
    print("===============================================")
    print("Result. Sentiment Analysis")
    print("===============================================")
    print("Precision: " + str(precision))
    print("Recall: " + str(recall))
    print("F1: " + str(f1))
    print("===============================================")


'''
main function
'''
run()
