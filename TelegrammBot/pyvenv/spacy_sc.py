from textblob import TextBlob
from googletrans import Translator
import spacy
import re
from spacy.tokens import Span
try:
    nlp = spacy.load("ru_core_news_sm")
except OSError:
    from spacy.cli import download
    download("ru_core_news_sm")
    nlp = load("ru_core_news_sm")
def sent_analyze(label):
    tr = Translator()
    r = tr.translate(label).text
    return TextBlob(r).sentiment.polarity
def proc_text(label):
    lem = nlp(label)
    idd = -1
    for i in range(len(lem)):
        if re.search(r'(\d{2}).(\d{2}).(\d{4})',lem[i].text) != None:
            idd = i
    if idd != -1:
        custom_label = "DATE"
        lem.ents += (Span(lem, idd, idd + 1, label=custom_label),)
    res_str = "Токены:\n"
    for i in lem:
        res_str += i.text + " " + i.dep_ + " " + i.pos_ + " \n"
    res_str += "Сущности:\n"
    for i in lem.ents:
         res_str += i.text + " " + i.label_ + " \n"
    return res_str
