import csv

#from sklearn.datasets
from pprint import pprint
from time import time

from sklearn import cross_validation
from sklearn.metrics import classification_report,accuracy_score
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.linear_model import LogisticRegression


def load_csv(filename):
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile)
        ip = []
        target = []
        for row in reader:
            target.append(row['Sentiment'])
            ip.append(row['SentimentText'])

        print "Data Loaded..."
        return ip,target


def vect_fit(filename):
    data,target = load_csv(filename)
    count_vector = CountVectorizer(max_df=0.5,ngram_range=(1,1))
    data = count_vector.fit_transform(data)
    tfidf_data = TfidfTransformer(norm = 'l2',use_idf = True).fit_transform(data)

    print"Data Vectorized..."
    return tfidf_data,target

def train(data,target):
    data_train,data_test,target_train,target_test = cross_validation.train_test_split(data,target,test_size=0.2,random_state =43)
    print"Training..."
    classifier = LogisticRegression(max_iter=10,class_weight='balanced').fit(data_train,target_train)
    print"Training Completed. Predict begins..."
    predicted = classifier.predict(data_test)
    print"Predicted"
    evaluate_model(target_test,predicted)

def evaluate_model(target_true,target_predicted):
    print classification_report(target_true,target_predicted)
    print "The accuracy score is {:.2%}".format(accuracy_score(target_true,target_predicted))

def main():
    filename = "dataset.csv"
    tf_idf,target = vect_fit(filename)
    train(tf_idf,target)

main()
