import csv

#from sklearn.datasets
from pprint import pprint
from time import time
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.linear_model import LogisticRegression
from sklearn.pipeline import Pipeline
from sklearn.grid_search import GridSearchCV


with open('dataset.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    ip = []
    target = []
    count = 1;
    for row in reader:
        target.append(row['Sentiment'])
        ip.append(row['SentimentText'])
        count+=1
        if(count == 10000):
            break

pipeline = Pipeline([
('vect',CountVectorizer()),
('tfidf',TfidfTransformer()),
('logReg',LogisticRegression())
])

parameters = {
    'vect__max_df': (0.5, 0.75, 1.0),
    #'vect__max_features': (None, 5000, 10000, 50000),
    'vect__ngram_range': ((1, 1), (1, 2)),  # unigrams or bigrams
    'tfidf__use_idf': (True, False),
    'tfidf__norm': ('l1', 'l2'),
    #'clf__alpha': (0.00001, 0.000001),
    #'clf__penalty': ('l2', 'elasticnet')
    #'clf__n_iter': (10, 50, 80),
    'logReg__max_iter':(10,50,100),
    'logReg__class_weight':('auto','balanced')
}

if __name__ == "__main__":
    grid_search =GridSearchCV(pipeline,parameters,n_jobs=-1,verbose=1)
    print"Performing grid search..."
    print"pipeline:",[name for name, _ in pipeline.steps]
    print"parameters:"
    pprint(parameters)

    t0 = time()
    grid_search.fit(ip,target)
    print"done in %0.3fs" %(time() - t0)
    print"/n"

    print"Best score: %0.3f" % grid_search.best_score_
    print"Best parameters set:"
    best_parameters = grid_search.best_estimator_.get_params()
    for param_name in sorted(parameters.keys()):
        print"\t%s: %r" % (param_name, best_parameters[param_name])
