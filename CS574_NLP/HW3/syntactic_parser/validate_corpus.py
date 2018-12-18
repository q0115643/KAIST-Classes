import sys, getopt
import re


def readfile(filename):
	with open(filename, 'r') as my_file:
		file_txt = my_file.read()
	return file_txt


def getLineNumsFromLog(val_log):
	tmp = re.findall('line[^:]*:', str(val_log))
	numList = []
	for num_tmp in tmp:
		num = re.sub('ff', '', re.sub(':', '', re.sub('line ', '', num_tmp)))
		numList.append(int(num))
	numList = set(numList)
	return numList


def excludeLines(corpus, numList):
	sentences = corpus.split('\n\n')
	i = 1
	sen_idx = 0
	validates = [1] * len(sentences)
	for sentence in sentences:
		sentence_copy = '%s' % sentence
		sentence_copy = sentence_copy.split('\n')
		for line in sentence_copy:
			if i in numList:
				validates[sen_idx] = 0
			i += 1
		i += 1
		sen_idx += 1
	new_corpus = ''
	for sentence, validate in zip(sentences, validates):
		if validate == 1:
			new_corpus += sentence + '\n\n'
	return new_corpus
	print new_corpus


def saveNewCorpus(output_path, new_corpus):
	file = open(output_path, 'w')
	file.write(new_corpus)
	file.close()


def main(argv):
   val_log = ''
   corpus = ''
   try:
      opts, args = getopt.getopt(argv,"hc:v:o:",["corpus=","validation_log=","output_path"])
   except getopt.GetoptError:
      print 'validate_corpus.py -c <corpus> -v <validation_log> -o <output_path>'
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'validate_corpus.py -c <corpus> -v <validation_log> -o <output_path>'
         sys.exit()
      elif opt in ("-c", "--corpus"):
         corpus = arg
      elif opt in ("-v", "--validation_log"):
         val_log = arg
      elif opt in ("-o", "--output_path"):
      	 output_path = arg
   print 'Validation Log file is "', val_log
   print 'Corpus file is "', corpus
   val_log = readfile(val_log)
   corpus = readfile(corpus)
   numList = getLineNumsFromLog(val_log)
   new_corpus = excludeLines(corpus, numList)
   saveNewCorpus(output_path, new_corpus)

if __name__ == "__main__":
   main(sys.argv[1:])





































