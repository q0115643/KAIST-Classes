import argparse
import train, mydataset, model

parser = argparse.ArgumentParser(description='CNN text classificer')
# learning
parser.add_argument('-lr', type=float, default=0.0007, help='initial learning rate [default: 0.0007]')
parser.add_argument('-epochs', type=int, default=256, help='number of epochs for train [default: 256]')
parser.add_argument('-batch-size', type=int, default=64, help='batch size for training [default: 64]')
parser.add_argument('-log-interval', type=int, default=1,
                    help='how many steps to wait before logging training status [default: 1]')
parser.add_argument('-test-interval', type=int, default=100,
                    help='how many steps to wait before testing [default: 100]')
parser.add_argument('-save-interval', type=int, default=500, help='how many steps to wait before saving [default:500]')
parser.add_argument('-early-stop', type=int, default=1000,
                    help='iteration numbers to stop without performance increasing')
parser.add_argument('-save-best', type=bool, default=True, help='whether to save when get best performance')
# model
parser.add_argument('-dropout', type=float, default=0.5, help='the probability for dropout [default: 0.5]')
parser.add_argument('-max-norm', type=float, default=3.0, help='l2 constraint of parameters [default: 3.0]')
parser.add_argument('-embed-dim', type=int, default=200, help='number of embedding dimension [default: 200]')
parser.add_argument('-kernel-num', type=int, default=100, help='number of each kind of kernel')
parser.add_argument('-kernel-sizes', type=str, default='2,3,4,5',
                    help='comma-separated kernel size to use for convolution')
parser.add_argument('-static', action='store_true', default=False, help='fix the embedding')
parser.add_argument('-thresholds', type=str, default='0.000001,0.00001,0.0001,0.001,0.01,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.999',
                    help='threshold values used to determine PR-points in test data [default:0.000001,0.00001,0.0001,0.001,0.01,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.999]')
args = parser.parse_args()


# load dataset
trainDataset, testDataset, label_list = mydataset.getDataset(args)
trainDataLoader = mydataset.getDataLoader(trainDataset, args.batch_size, shuffle=True)
testDataLoader = mydataset.getDataLoader(testDataset, args.batch_size, shuffle=False)

for batch in testDataLoader:
    args.embed_num = batch[0].size()[1]
    args.embed_dim = batch[0].size()[2]
    args.class_num = len(label_list)
    break

# update args and print
args.label_list = label_list
args.kernel_sizes = [int(k) for k in args.kernel_sizes.split(',')]
args.thresholds = [float(k) for k in args.thresholds.split(',')]

print("\nParameters:")
for attr, value in sorted(args.__dict__.items()):
    print("\t{}={}".format(attr.upper(), value))

# model
cnn = model.CNN_Text(args)

print()
try:
    train.train(trainDataLoader, testDataLoader, cnn, args)
except KeyboardInterrupt:
    print('\n' + '-' * 89)
    print('Exiting from training early')

