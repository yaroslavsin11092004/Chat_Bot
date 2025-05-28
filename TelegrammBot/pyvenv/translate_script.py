from googletrans import Translator
def translate(row):
    tr = Translator()
    return tr.translate(row).text