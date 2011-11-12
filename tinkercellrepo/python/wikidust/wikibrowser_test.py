import test
import wikibrowser_testjeffwiki
import wikibrowser_testregistry
import wikibrowser_testgoldenbricks
#import wikibrowser_testigem
#import wikibrowser_testwikipedia

LOG_PATH = "wikibrowser.log"

def runTests():
    #test.resetLog(LOG_PATH)
    test.scrambleFilenames()
    wikis = ( wikibrowser_testjeffwiki,
              wikibrowser_testregistry,
              wikibrowser_testgoldenbricks,
              #wikibrowser_testigem,
              #wikibrowser_testwikipedia,
            )
    print "running tests"
    for wiki in wikis:
        if raw_input("run %s? " % wiki.__name__).lower() in ("n", "no"):
            continue
        else:
            wiki.suite().debug()
    print "all tests ran successfully"

if __name__ == "__main__":
    try:
        runTests()
    except SystemExit: # bug in IDLE
        pass

