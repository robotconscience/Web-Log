import json
import os
import time
import requests
import string
import re
from PIL import Image
from StringIO import StringIO
from requests.exceptions import ConnectionError

def scrapeLocal(f, path):
  file_save_dir = path
  BASE_PATH = os.path.join(path, '')
  filename_length = 20
  filename_charset = string.ascii_letters + string.digits
  linestring = open(f).read()

  for match in re.findall(r'imgurl=(.*?(?:&|\.(?:jpg|gif|png|jpeg)))', linestring, re.I):
    print match
   
    try:
      url = match
      try:
        image_r = requests.get(url)
      except ConnectionError, e:
        print 'could not download %s' % url
        continue

      # Remove file-system path characters from name.
      title = match.replace('/', '').replace('\\', '')

      # print url
      print os.path.join(BASE_PATH, '%s') % title

      file = open(os.path.join(BASE_PATH, '%s') % title, 'w')
      try:
        Image.open(StringIO(image_r.content)).save(file, 'JPEG')
      except IOError, e:
        # Throw away some gifs...blegh.
        print e
        print 'could not save %s' % url
        continue
      finally:
        file.close()
    except:
      print "unable to open url " + match

def scrape(query,path):
  file_save_dir = path
  BASE_PATH = os.path.join(path, query)

  filename_length = 20
  filename_charset = string.ascii_letters + string.digits

  r = requests.get('https://www.google.com/search?q=' + query + '&site=imgs&source=lnms&tbm=isch&sa=X&ei=xHSJUqLrDLPB4APhwoCoCA&ved=0CAkQ_AUoAQ&biw=1438&bih=722')

  for match in re.findall(r'imgurl=(.*?(?:&|\.(?:jpg|gif|png|jpeg)))', r.text, re.I):
    
    try:
      # filename = ''.join(random.choice(filename_charset)
      #                  for s in range(filename_length))
      # urllib.urlretrieve (match,
      #                   os.path.join(file_save_dir, filename + '.jpg'))

      # print 

      url = match
      try:
        image_r = requests.get(url)
      except ConnectionError, e:
        print 'could not download %s' % url
        continue

      # Remove file-system path characters from name.
      title = match.replace('/', '').replace('\\', '')

      # print url
      print os.path.join(BASE_PATH, '%s') % title

      file = open(os.path.join(BASE_PATH, '%s') % title, 'w')
      try:
        Image.open(StringIO(image_r.content)).save(file, 'JPEG')
      except IOError, e:
        # Throw away some gifs...blegh.
        print e
        print 'could not save %s' % url
        continue
      finally:
        file.close()

    except:
      print "unable to open url " + match

def go(start, query, path):
  """Download full size images from Google image search.

  Don't print or republish images without permission.
  I used this to train a learning algorithm.
  """
  BASE_URL = 'https://ajax.googleapis.com/ajax/services/search/images?'\
             'v=1.0&q=' + query + '&start=%d'

  BASE_PATH = os.path.join(path, query)

  if not os.path.exists(BASE_PATH):
    os.makedirs(BASE_PATH)

#  start = 0 # Google's start query string parameter for pagination.
  dest = start + 60
  while start < dest: # Google will only return a max of 56 results.
    r = requests.get(BASE_URL % start)
    print BASE_URL % start
    for image_info in json.loads(r.text)['responseData']['results']:
      url = image_info['unescapedUrl']
      try:
        image_r = requests.get(url)
      except ConnectionError, e:
        print 'could not download %s' % url
        continue

      # Remove file-system path characters from name.
      title = image_info['titleNoFormatting'].replace('/', '').replace('\\', '')

      file = open(os.path.join(BASE_PATH, '%s.jpg') % title, 'w')
      try:
        Image.open(StringIO(image_r.content)).save(file, 'JPEG')
      except IOError, e:
        # Throw away some gifs...blegh.
        print e
        print 'could not save %s' % url
        continue
      finally:
        file.close()

    print start
    start += 4 # 4 images per page.

    # Be nice to Google and they'll be nice back :)
    time.sleep(1.5)

scrapeLocal('scraped/flames.html','newflames')
scrapeLocal('scraped/log.html','newlogs')
scrapeLocal('scraped/fireplace_empty.html','fireplace_empty')
scrapeLocal('scraped/fireplace.html','fireplace')

# page=1
# while page < 10:
#     go(page * 60, 'flame', 'flame')
#     go(page * 60, 'log', 'log')
#     go(page * 60, 'fireplace', 'fireplace')
#     page = page + 1