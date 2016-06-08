from bs4 import BeautifulSoup
import codecs

html_doc = open("source.html",'r').read()
soup = BeautifulSoup(html_doc, 'html.parser')

brands = soup.findAll('li')

with codecs.open('brandnames.txt','w',encoding='utf8') as f:

    for brand in brands:
        if brand is not None:
            f.write(unicode(brand.contents[0])+'\n')

with codecs.open("brandnames.txt", "r", encoding='utf8') as f:
    op = codecs.open('names.txt','w',encoding='utf8')
    for line in f:
        cleanedLine = line.strip()
        if cleanedLine: # is not empty
            op.write(cleanedLine+'\n')
    op.close()
