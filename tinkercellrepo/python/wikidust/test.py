from __future__ import with_statement
import os
import random, string
import errorreport

TEST_LOG = "test.log"
TEST_IMAGES = os.path.realpath("./testimages/")
log = errorreport.WikiDustLogger(TEST_LOG)

#TODO include weird characters to improve sanitizing
def randomString(length=20):
    gibberish = []
    for num in range(length):
        gibberish.append(random.choice( string.ascii_uppercase +
                                        string.ascii_lowercase +
                                        string.digits            ))
    return ''.join(gibberish)

def scrambleFilenames(directory=TEST_IMAGES):
    #print "scrambling filenames in %s" % directory
    for old in os.listdir(directory):
        #print '    ' + old,
        ext = os.path.splitext(old)[1]
        old = directory + '/' + old
        new = randomString() + ext
        #print '-->', new
        new = directory + '/' + new
        os.rename(old, new)

def testImages():
    scrambleFilenames()
    paths = []
    for filename in os.listdir(TEST_IMAGES):
        path = os.path.realpath(TEST_IMAGES + "/" + filename)
        paths.append(path)
    return paths
        
def resetLog(path=TEST_LOG):
    print "resetting %s" % path
    with open(path, 'w') as log:
        log.write('')

def runTests():
    print testImages()

if __name__ == "__main__":
    try:
        runTests()
    except SystemExit: # bug in IDLE
        pass

