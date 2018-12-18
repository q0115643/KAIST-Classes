from SPARQLWrapper import SPARQLWrapper, JSON
import time
import re

def getAbstract(entity):
    query_prefix = 'prefix dbpedia: <http://ko.dbpedia.org/resource/> prefix dbpedia-owl: <http://dbpedia.org/ontology/> select ?abstract where {  <http://ko.dbpedia.org/resource/'
    query_sufix = '> dbpedia-owl:wikiPageRedirects*/dbpedia-owl:abstract ?abstract .}'
    sparql = SPARQLWrapper("http://ko.dbpedia.org/sparql")
    sparql.setReturnFormat(JSON)
    query = query_prefix + entity + query_sufix
    sparql.setQuery(query)  # the previous query as a literal string
    try:
        result = sparql.query().convert()['results']['bindings'][0]['abstract']['value']
    except:
        return
    return str(result)

start = time.time()
triple_f = open('triples.nt.txt', 'r', encoding='utf-8')
start = time.time()
train_f = open('triples.nt.txt', 'r', encoding='utf-8')
ent_set = set()
for line in train_f.read().splitlines():
    line_splitted = line.split('\t')
    sbj = line_splitted[0].strip()
    obj = line_splitted[2].strip()
    ent_set.add(sbj)
    ent_set.add(obj)
ent_list = list(ent_set)
triple_f.close()
cnt = 0
unk_f = open('unk_ent_dbpedia.txt', 'w', encoding='utf-8')
for ent in ent_list:
    cnt += 1
    abstract_str = getAbstract(ent)
    if not abstract_str:
        unk_f.write(ent + '\n')
        continue
    ent_f_name = re.sub('\/', '_', ent)
    f_name = './dbpediaTXT/' + ent_f_name + '.txt'
    with open(f_name, 'w', encoding='utf-8') as myFile:
        myFile.write(abstract_str)
        end = time.time()
        print(str(cnt) + ': ' + ent + ' saved, ' + str(end - start) + ' passed\n')
        print(ent)
        print(abstract_str + '\n')
unk_f.close()


