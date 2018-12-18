import parser
import torch.utils.data as data_utils


def getDataset(args, train_filename='../triples_train.tsv', test_filename='../triples_test.tsv'):
    trainTensor, trainLabelTensor, testTensor, testLabelTensor, label_list = parser.getAllData(train_filename, test_filename, args)
    train = data_utils.TensorDataset(trainTensor, trainLabelTensor)
    test = data_utils.TensorDataset(testTensor, testLabelTensor)
    return train, test, label_list


def getDataLoader(dataset, batch_size, shuffle=False):
    return data_utils.DataLoader(dataset, batch_size=batch_size, shuffle=shuffle)

