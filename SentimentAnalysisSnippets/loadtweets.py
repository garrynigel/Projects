#############################################################################
# Name  : Garry Dmello
# Script: loadtweets.py
# Description:   Test script for loading twitter feeds from API to json file
#
# Version 1.0 Initial Version : Loading starbucks tweets
#
#############################################################################
#
#   Packages to be imported
#

from tweepy import OAuthHandler
#import tweepy
import os
import sys


#
# Access credentials for twitter application
#

ckey = 'HmbIAnQSoEAAncTK2CHkTlaDI'
consumer_secret = 'jSdXWB4D5UokfyXUBaXDhEqMaKrnLU0Tepq6gYG04H06v7k42V'
access_token_key = '585599301-0PATI3Gq1w6w1EMkAbteqYOLvSnVWIi7Dx5NVtkV'
access_token_secret = 'XpW9yr8ZPMy08mw3YwnJ1jWqkbnNTLscYBlO8JfhpNRcu'


#
# Connect to twitter API
#

auth = OAuthHandler(ckey,consumer_secret)
auth.set_access_token(access_token_key,access_token_secret)
api = tweepy.API(auth, wait_on_rate_limit=True,
				   wait_on_rate_limit_notify=True)

#
# Parameters required
#

searchTag = '@GaelGreene'
maxTw = 100000
twPerPage = 100 # Max tweets allowed per query by API
fName = 'greenetweets.txt'

#
# Initialize start and end tweet ids and counts
#

start = None
end = -1L
twCount = 0

with open(fName,'w') as f:
    while twCount < maxTw:

        try:
            if(end <= 0):
                if (not start):
                    tweets = api.user_timeline(user_id=searchTag, count=twPerPage)
                else:
                    tweets = api.user_timeline(user_id=searchTag, count=twPerPage, since_id = start)
            else:
                if(not start):
                    tweets = api.user_timeline(user_id=searchTag, count=twPerPage ,max_id = str(end - 1 ))
                else:
                    tweets = api.user_timeline(user_id=searchTag, count=twPerPage ,max_id = str(end -1),since_id = start)
            if not tweets:
                print "No tweets found"
                break
            for tweet in tweets:
                f.write(str(tweet)+'\n')
                twCount += len(tweets)
                print "Downloaded tweets ",str(twCount)
                end = tweets[-1].id
        except tweepy.TweepError as e:
            print str(e)
            break
