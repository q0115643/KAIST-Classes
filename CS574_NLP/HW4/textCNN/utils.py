import re

def clean_str(string):
    string = re.sub(r"[^\u3131-\u318E\uAC00-\uD7A30-9A-Za-z\(\),!?\'\`\.]", " ", string)
    string = re.sub(r",", " , ", string)
    string = re.sub(r"!", " ! ", string)
    string = re.sub(r"\(", " \( ", string)
    string = re.sub(r"\)", " \) ", string)
    string = re.sub(r"\?", " \? ", string)
    string = re.sub(r"\s{2,}", " ", string)
    return string.strip()

