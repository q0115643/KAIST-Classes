import time
import re
import glob
import nltk.data


def chechObjInSentence(obj, sentence):
    obj = clean_obj(obj)
    if obj in sentence:
        return True
    return False

def clean_obj(obj):
    obj = re.sub('\([^\)]*\)', ' ', obj)
    obj = re.sub(r"[^\u3131-\u318E\uAC00-\uD7A3A-Za-z0-9]", " ", obj)
    obj = re.sub(r'\s\s+', ' ', obj)
    return obj.strip()

start = time.time()
triples = []
triple_f = open('triples.nt.txt', 'r', encoding='utf-8')
for line in triple_f.read().splitlines():
    line_splitted = line.split('\t')
    raw_sbj = line_splitted[0].strip()
    raw_obj = line_splitted[2].strip()
    relation = line_splitted[1].strip()
    triples.append((raw_sbj, raw_obj, relation))
triple_f.close()

dbpediaFiles = glob.glob("./dbpediaTXT/*.txt")
dbpediaNames = [re.sub('\.txt', '', re.sub('\.\/dbpediaTXT\/', '', name)) for name in dbpediaFiles]
cnt = 0
found = 0
unk_names = []
tokenizer = nltk.data.load('./train_punkt/korean.pickle')
new_triples_f = open('triples_new.nt.txt', 'w', encoding='utf-8')
sen_f = open('sentences.txt', 'w', encoding='utf-8')
for (raw_sbj, raw_obj, relation) in triples:
    raw_sbj_f_name = re.sub('\/', '_', raw_sbj)
    raw_obj_f_name = re.sub('\/', '_', raw_obj)
    sentences = []
    if raw_sbj_f_name in dbpediaNames:
        with open('./dbpediaTXT/' + raw_sbj_f_name + '.txt') as myFile:
            sentences.append(tokenizer.tokenize(myFile.read().strip()))
    if raw_obj_f_name in dbpediaNames:
        with open('./dbpediaTXT/' + raw_obj_f_name + '.txt') as myFile:
            sentences.append(tokenizer.tokenize(myFile.read().strip()))
    if sentences == []:
        cnt += 1
        unk_names.append((raw_sbj, raw_obj, relation))
        continue
    found_bool = False
    for sentence in sentences:
        for i, sen in enumerate(sentence):
            sen = re.sub('언어 오류\([^\)]*\)', '', sen)
            cleaned_sbj = clean_obj(raw_sbj)
            cleaned_obj = clean_obj(raw_obj)
            if len(cleaned_sbj) > len(cleaned_obj) and found_bool == False:
                if cleaned_sbj in sen:
                    if re.search(cleaned_sbj + '\([^\)]*\)', sen):
                        sen = re.sub(cleaned_sbj + '\([^\)]*\)', ' << _sbj_ >> ', sen)
                    else:
                        sen = re.sub(cleaned_sbj, ' << _sbj_ >> ', sen)
                    if cleaned_obj in sen:
                        if re.search(cleaned_obj + '\([^\)]*\)', sen):
                            sen = re.sub(cleaned_obj + '\([^\)]*\)', ' << _obj_ >> ', sen)
                        else:
                            sen = re.sub(cleaned_obj, ' << _obj_ >> ', sen)
                        new_triples_f.write(raw_sbj + '\t' + raw_obj + '\t' + relation + '\t' + sen + '\n')
                        found_bool = True
                        found += 1
                        print(sen)
            elif found_bool == False:
                if cleaned_obj in sen:
                    if re.search(cleaned_obj + '\([^\)]*\)', sen):
                        sen = re.sub(cleaned_obj + '\([^\)]*\)', ' << _obj_ >> ', sen)
                    else:
                        sen = re.sub(cleaned_obj, ' << _obj_ >> ', sen)
                    if cleaned_sbj in sen:
                        if re.search(cleaned_sbj + '\([^\)]*\)', sen):
                            sen = re.sub(cleaned_sbj + '\([^\)]*\)', ' << _sbj_ >> ', sen)
                        else:
                            sen = re.sub(cleaned_sbj, ' << _sbj_ >> ', sen)
                        new_triples_f.write(raw_sbj + '\t' + raw_obj + '\t' + relation + '\t' + sen + '\n')
                        found_bool = True
                        found += 1
                        print(sen)
    found_bool = False
print(unk_names)
wikiFiles = glob.glob("./wikiTXT/*.txt")
wikiNames = [re.sub('\.txt', '', re.sub('\.\/wikiTXT\/', '', name)) for name in dbpediaFiles]
for raw_sbj, raw_obj, relation in unk_names:
    sentences = []
    found_bool = False
    if raw_sbj in wikiNames:
        with open('./wikiTXT/' + raw_sbj + '.txt') as myFile:
            sentences.append(tokenizer.tokenize(myFile.read().strip()))
    if raw_obj in wikiNames:
        with open('./wikiTXT/' + raw_obj + '.txt') as myFile:
            sentences.append(tokenizer.tokenize(myFile.read().strip()))
    for sentence in sentences:
        for i, sen in enumerate(sentence):
            sen = re.sub('언어 오류\([^\)]*\)', '', sen)
            cleaned_sbj = clean_obj(raw_sbj)
            cleaned_obj = clean_obj(raw_obj)
            if len(cleaned_sbj) > len(cleaned_obj) and found_bool == False:
                if cleaned_sbj in sen:
                    if re.search(cleaned_sbj + '\([^\)]*\)', sen):
                        sen = re.sub(cleaned_sbj + '\([^\)]*\)', ' << _sbj_ >> ', sen)
                    else:
                        sen = re.sub(cleaned_sbj, ' << _sbj_ >> ', sen)
                    if cleaned_obj in sen:
                        if re.search(cleaned_obj + '\([^\)]*\)', sen):
                            sen = re.sub(cleaned_obj + '\([^\)]*\)', ' << _obj_ >> ', sen)
                        else:
                            sen = re.sub(cleaned_obj, ' << _obj_ >> ', sen)
                        new_triples_f.write(raw_sbj + '\t' + raw_obj + '\t' + relation + '\t' + sen + '\n')
                        found += 1
                        found_bool = True
                        print(sen)
            else:
                if cleaned_obj in sen and found_bool == False:
                    if re.search(cleaned_obj + '\([^\)]*\)', sen):
                        sen = re.sub(cleaned_obj + '\([^\)]*\)', ' << _obj_ >> ', sen)
                    else:
                        sen = re.sub(cleaned_obj, ' << _obj_ >> ', sen)
                    if cleaned_sbj in sen:
                        if re.search(cleaned_sbj + '\([^\)]*\)', sen):
                            sen = re.sub(cleaned_sbj + '\([^\)]*\)', ' << _sbj_ >> ', sen)
                        else:
                            sen = re.sub(cleaned_sbj, ' << _sbj_ >> ', sen)
                        new_triples_f.write(raw_sbj + '\t' + raw_obj + '\t' + relation + '\t' + sen + '\n')
                        found += 1
                        found_bool = True
                        print(sen)
print(found)
new_triples_f.close()









