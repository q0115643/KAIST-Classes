import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm

threshold_list = []
threshold_list.append(open('../labels/threshold_1e-06.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_1e-05.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.0001.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.001.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.01.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.1.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.2.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.3.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.4.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.5.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.6.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.7.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.8.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.9.txt', 'r', encoding='utf-8'))
threshold_list.append(open('../labels/threshold_0.999.txt', 'r', encoding='utf-8'))

gold_f = open('../triples_test.tsv', 'r', encoding='utf-8')

key_strs = []
rel_dict = {}
gold_num = 0
for line in gold_f.read().splitlines():
    line_splitted = line.split('\t')
    sbj = line_splitted[0].strip()
    obj = line_splitted[1].strip()
    relation = line_splitted[2].strip()
    key_str = str((sbj, obj))
    key_strs.append(key_str)
    if key_str in rel_dict:
        rel_dict[key_str].append(relation)
    else:
        rel_dict[key_str] = [relation]
    gold_num += 1
gold_f.close()

precisions = []
recalls = []
for threshold_file in threshold_list:
    cor_num = 0
    predicted_num = 0
    for i, line in enumerate(threshold_file.read().splitlines()):
        tokens = line.split('\t')
        tokens = list(filter(None, tokens))
        for relation in tokens:
            if relation in rel_dict[key_strs[i]]:
                cor_num += 1
            predicted_num += 1
    precision = float(cor_num) / float(predicted_num)
    recall = float(cor_num) / float(gold_num)
    precisions.append(precision)
    recalls.append(recall)
    threshold_file.close()

colors = cm.rainbow(np.linspace(0, 1, len(precisions)))
labels = '0.000001,0.00001,0.0001,0.001,0.01,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.999'
labels = labels.split(',')

for x, y, c, l in zip(recalls, precisions, colors, labels):
    plt.scatter(x, y, color=c, label=l)
plt.ylabel('Precision')
plt.xlabel('Recall')
plt.axis([0, 1, 0, 1])
plt.legend(fontsize='small', title="Threshold")
plt.show()











