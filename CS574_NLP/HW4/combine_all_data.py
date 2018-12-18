import random


complete_f = open('triples_complete.tsv', 'w', encoding='utf-8')
triple_f = open('triples_new.nt.txt', 'r', encoding='utf-8')
for line in triple_f.read().splitlines():
    complete_f.write(line + '\n')
triple_f.close()
train_ds = open('ds_train.tsv', 'r', encoding='utf-8')
for line in train_ds.read().splitlines():
    complete_f.write(line + '\n')
train_ds.close()
test_ds = open('gold_test.tsv', 'r', encoding='utf-8')
for line in test_ds.read().splitlines():
    complete_f.write(line + '\n')
test_ds.close()
complete_f.close()
complete_f = open('triples_complete.tsv', 'r', encoding='utf-8')
lines = []
for line in complete_f.read().splitlines():
    lines.append(line)
random.shuffle(lines)
train_f = open('triples_train.tsv', 'w', encoding='utf-8')
test_f = open('triples_test.tsv', 'w', encoding='utf-8')
test_size = len(lines) // 10
for i, line in enumerate(lines):
    if i < test_size:
        test_f.write(line + '\n')
    else:
        train_f.write(line + '\n')







