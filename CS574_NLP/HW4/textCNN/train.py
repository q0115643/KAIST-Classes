import sys
import torch
import torch.nn.functional as F
from gensim.models import KeyedVectors
from konlpy.tag import Kkma
import utils, parser


def train(trainDataLoader, testDataLoader, model, args):
    optimizer = torch.optim.Adam(model.parameters(), lr=args.lr)
    steps = 0
    best_acc = 0
    last_step = 0
    model.train()
    for epoch in range(1, args.epochs+1):
        print("\nStarting Epoch " + str(epoch) + "...")
        for batch in trainDataLoader:
            feature, target = batch[0], batch[1]

            optimizer.zero_grad()
            logit = model(feature)

            logit = F.softmax(logit, dim=1)
            loss = F.nll_loss(logit, target)
            loss.backward()
            optimizer.step()

            steps += 1
            if steps % args.log_interval == 0:
                corrects = (torch.max(logit, 1)[1].view(target.size()).data == target.data).sum()
                batch_size = target.size()[0]
                accuracy = 100.0 * float(corrects)/float(batch_size)
                sys.stdout.write(
                    '\rBatch[{}] - loss: {:.6f}  acc: {:.4f}%({}/{})'.format(steps,
                                                                             loss.item(),
                                                                             accuracy,
                                                                             corrects,
                                                                             batch_size))
            if steps % args.test_interval == 0:
                test_acc, predicted_labels = evalTest(testDataLoader, model, args)
                if test_acc > best_acc:
                    best_acc = test_acc
                    last_step = steps
                    savePredictedLabels(predicted_labels)
                else:
                    if steps - last_step >= args.early_stop:
                        print('early stop by {} steps.'.format(args.early_stop))


def eval(dataLoader, model, args):
    model.eval()
    corrects, avg_loss = 0, 0
    for batch in dataLoader:
        feature, target = batch[0], batch[1]

        logit = model(feature)
        logit = F.softmax(logit, dim=1)
        loss = F.nll_loss(logit, target, size_average=False)

        avg_loss += loss.item()
        corrects += (torch.max(logit, 1)
                     [1].view(target.size()).data == target.data).sum()

    size = len(dataLoader.dataset)
    avg_loss /= size
    accuracy = 100.0 * float(corrects)/float(size)
    print('\nEvaluation - loss: {:.6f}  acc: {:.4f}%({}/{}) \n'.format(avg_loss,
                                                                       accuracy,
                                                                       corrects,
                                                                       size))
    return accuracy, []

def evalTest(dataLoader, model, args):
    model.eval()
    corrects, avg_loss = 0, 0
    predicted_labels = {}
    for threshold in args.thresholds:
        predicted_labels[str(threshold)] = []
    for batch in dataLoader:
        feature, target = batch[0], batch[1]

        logit = model(feature)
        logit = F.softmax(logit, dim=1)

        for threshold in args.thresholds:
            for i in range(logit.size()[0]):
                labels_per_line = []
                for j in range(logit.size()[1]):
                    prob = logit.data.numpy()[i][j]
                    if prob > threshold:
                        labels_per_line.append(parser.idxToLabel(j, args.label_list))
                predicted_labels[str(threshold)].append(labels_per_line)


        loss = F.nll_loss(logit, target, size_average=False)

        avg_loss += loss.item()
        corrects += (torch.max(logit, 1)
                     [1].view(target.size()).data == target.data).sum()
    size = len(dataLoader.dataset)
    avg_loss /= size
    accuracy = 100.0 * float(corrects)/float(size)
    print('\nEvaluation - loss: {:.6f}  acc: {:.4f}%({}/{}) \n'.format(avg_loss,
                                                                       accuracy,
                                                                       corrects,
                                                                       size))
    return accuracy, predicted_labels

def predict(text, model, label_list):
    assert isinstance(text, str)
    model.eval()
    kkma = Kkma()
    text = utils.clean_str(text)
    w2vec = KeyedVectors.load_word2vec_format('../kor-word2vec-kkma-200.bin', binary=True)
    tokens = [p[0] + '/' + p[1] for p in kkma.pos(text) if p[0] + '/' + p[1] in w2vec.vocab]
    embed = parser.sentenceToEmbedding(tokens, 112, w2vec)
    x = torch.from_numpy(embed).float()
    x = x.view(1, x.size()[0], x.size()[1])
    with torch.no_grad():
        logit = model(x)
        logit = F.softmax(logit, dim=1)
    _, predicted = torch.max(logit, 1)
    return parser.idxToLabel(predicted[0], label_list)

def savePredictedLabels(predictedLabels):
    for threshold, value in predictedLabels.items():
        save_path = '../labels/threshold_{}.txt'.format(threshold)
        with open(save_path, 'w', encoding='utf-8') as myFile:
            for labels_per_line in value:
                for label in labels_per_line:
                    myFile.write(label + '\t')
                myFile.write('\n')


