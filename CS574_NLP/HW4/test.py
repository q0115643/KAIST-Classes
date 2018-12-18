import requests
import time
import datetime
import wikipedia
import time

def get_content_from_wiki_by_query(query):
    try:
        # print(datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S'))
        prefix = "https://ko.wikipedia.org/w/api.php?format=json&action=query&titles="
        r = requests.get(prefix + query)

        pageid = list(r.json()['query']['pages'].keys())[0]

        wikipedia.set_lang("ko")
        article = wikipedia.page(pageid=pageid)

        # print(datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S'))
        return article.content, article.summary
    except Exception:
        return None, None

def main():
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
    cnt = 0
    for ent in ent_list:
        cnt += 1
        content, summary = get_content_from_wiki_by_query(ent)
        f_name = './wikiTXT/' + ent + '.txt'
        if content or summary:
            with open(f_name, 'w', encoding='utf-8') as myFile:
                myFile.write(summary + '\n' + content)
                end = time.time()
                print(str(cnt) + ': ' + ent + ' saved, ' + str(end - start) + ' passed')
        else:
            with open('unk_entities.txt', 'w', encoding='utf-8') as myFile:
                myFile.write(ent + '\n')
                end = time.time()
                print(str(cnt) + ': ' + ent + ' not in wiki, ' + str(end-start) + ' passed')
    # content, summary = get_content_from_wiki_by_query('경기도')
    # print(content)
    # print(summary)



if __name__ == "__main__":
    main()
