#TODO also test handling of bad input

import unittest
import test
import mapupload
import wikibrowser
import os, sys
from getpass import getpass

#LOG_PATH = "mapupload.log"
#log = WikiDustLogger(path=LOG_PATH)

class MapUploadTest(unittest.TestCase):
    def __init__(self, url, username, password, error):
        unittest.TestCase.__init__(self)
        self.url = url
        self.username = username
        self.password = password
        self.error = error

    def setUp(self):
        def noQuestions(message):
            return False
        mapupload.askQuestion = noQuestions # don't ask to send error reports
        mapupload.DEFAULT_URL = self.url
        self.uploader = mapupload.MapUploader()
        self.checker = wikibrowser.Browser()
        self.uploader.screenshotPath = test.testImages()[0]
        self.uploader.mapCode = "[PASTE URL OF IMAGE HERE test image:%s]" % os.path.basename(self.uploader.screenshotPath)
        if self.error == type(None): # else it won't be needed
            self.checker.loadWiki(self.url, self.username, self.password)
        self.uploader.username.set(self.username)
        self.uploader.password.set(self.password)

    def runTest(self):
        try:
            name = os.path.basename( self.uploader.screenshotPath )
            code = self.uploader.mapCode
            url = self.uploader.userSuppliedURL.get()
            if self.error == None:
                self.assertFalse( self.checker.fileExists(name) )
                self.assertFalse( self.checker.codeExists(code, url) )
                self.uploader.upload()
                self.assertTrue( self.checker.fileExists(name) )
                self.assertTrue( self.checker.codeExists(name, url) ) # actual code may change
                self.assertEqual( self.error, type(self.uploader.lastSuppressed) )
            else:
                try:
                    self.uploader.upload()
                    self.assertEqual( self.error, type(self.uploader.lastSuppressed) )
                except Exception, e:
                    self.assertEqual( self.error, type(e) )
            print "%s is right" % self.error.__name__
        except AssertionError:
            print "%s should be %s" % (type(self.uploader.lastSuppressed), self.error)
            print "details: %s" % self.uploader.lastSuppressed
            raise SystemExit

	def tearDown(self):
		self.uploader.destroy()
		self.checker.logout()

TEST_WIKIS = {}
JEFFWIKI     = TEST_WIKIS["jeffwiki"]     = {}
GOLDENBRICKS = TEST_WIKIS["goldenbricks"] = {}
REGISTRY     = TEST_WIKIS["registry"]     = {}
WIKIPEDIA    = TEST_WIKIS["wikipedia"]    = {}

# correct urls, usernames, passwords, and resulting exception types
JEFFWIKI_DEFAULTS     = ( "http://wikidust.sbolstandard.org/index.php/Sandbox:Unittesting",
                          raw_input("JeffWiki username: "),
                          getpass("JeffWiki password: "), type(None)                        )
GOLDENBRICKS_DEFAULTS = ( "https://courses.washington.edu/synbio/goldenbricks/index.php?title=Test_3",
                          raw_input("UW netid username: "),
                          getpass("UW netid password: "), type(None)                                   )
REGISTRY_DEFAULTS     = ( "http://partsregistry.org/wiki/index.php?title=Part:BBa_K314000",
                          raw_input("Registry username: "),
                          getpass("Registry password: "), type(None)                        )
WIKIPEDIA_DEFAULTS    = ( "http://en.wikipedia.org/wiki/User:Jeffdjohnson/map_code_sandbox2",
                          raw_input("Wikipedia username: "),
                          getpass("Wikipedia password: "), type(None)                         )

# test cases
JEFFWIKI["correct"]  = JEFFWIKI_DEFAULTS
JEFFWIKI["locked"]   = ("http://wikidust.sbolstandard.org/index.php/Main_Page", "", "",          wikibrowser.PageLockedError)
JEFFWIKI["badurl"]   = ("tinkercell.com",            JEFFWIKI_DEFAULTS[1], JEFFWIKI_DEFAULTS[2], wikibrowser.BadURLError)
JEFFWIKI["wrongurl"] = ("http://www.tinkercell.com", JEFFWIKI_DEFAULTS[1], JEFFWIKI_DEFAULTS[2], wikibrowser.SiteLayoutError)
JEFFWIKI["nologin"]  = (JEFFWIKI_DEFAULTS[0],        "",                   "",                   JEFFWIKI_DEFAULTS[3])
JEFFWIKI["useronly"] = (JEFFWIKI_DEFAULTS[0],        JEFFWIKI_DEFAULTS[1], "",                   wikibrowser.BadLoginError)
JEFFWIKI["passonly"] = (JEFFWIKI_DEFAULTS[0],        "",                   "stuff",              wikibrowser.BadLoginError)
JEFFWIKI["baduser"]  = (JEFFWIKI_DEFAULTS[0],        "jef",                JEFFWIKI_DEFAULTS[2], wikibrowser.BadLoginError)
JEFFWIKI["badpass"]  = (JEFFWIKI_DEFAULTS[0],        JEFFWIKI_DEFAULTS[1], "stuff",              wikibrowser.BadLoginError)

GOLDENBRICKS["correct"]  = GOLDENBRICKS_DEFAULTS
GOLDENBRICKS["locked"]   = ("https://courses.washington.edu/synbio/goldenbricks/index.php?title=Oscillators",
                                                         GOLDENBRICKS_DEFAULTS[1], GOLDENBRICKS_DEFAULTS[2], wikibrowser.PageLockedError)
GOLDENBRICKS["badurl"]   = ("tinkercell.com",            GOLDENBRICKS_DEFAULTS[1], GOLDENBRICKS_DEFAULTS[2], wikibrowser.BadURLError)
GOLDENBRICKS["wrongurl"] = ("http://www.tinkercell.com", GOLDENBRICKS_DEFAULTS[1], GOLDENBRICKS_DEFAULTS[2], wikibrowser.SiteLayoutError)
GOLDENBRICKS["nologin"]  = (GOLDENBRICKS_DEFAULTS[0],    "",                       "",                       wikibrowser.BadLoginError)
GOLDENBRICKS["useronly"] = (GOLDENBRICKS_DEFAULTS[0],    GOLDENBRICKS_DEFAULTS[1], "",                       wikibrowser.BadLoginError)
GOLDENBRICKS["passonly"] = (GOLDENBRICKS_DEFAULTS[0],    "",                       GOLDENBRICKS_DEFAULTS[2], wikibrowser.BadLoginError)
GOLDENBRICKS["baduser"]  = (GOLDENBRICKS_DEFAULTS[0],    "jeff2",                  GOLDENBRICKS_DEFAULTS[2], wikibrowser.BadLoginError)
GOLDENBRICKS["badpass"]  = (GOLDENBRICKS_DEFAULTS[0],    GOLDENBRICKS_DEFAULTS[1], "stuff!",                 wikibrowser.BadLoginError) 

REGISTRY["correct"]  = REGISTRY_DEFAULTS
REGISTRY["locked"]   = ("http://partsregistry.org/wiki/index.php?title=MediaWiki:Monobook.css", #BE SURE THIS DOESN't WORK!!
                                              REGISTRY_DEFAULTS[1], REGISTRY_DEFAULTS[2], wikibrowser.PageLockedError)
REGISTRY["badurl"]   = ("www.amazon.com",     REGISTRY_DEFAULTS[1], REGISTRY_DEFAULTS[2], wikibrowser.BadURLError)
REGISTRY["wrongurl"] = ("http://amazon.com",  REGISTRY_DEFAULTS[1], REGISTRY_DEFAULTS[2], wikibrowser.SiteLayoutError)
REGISTRY["nologin"]  = (REGISTRY_DEFAULTS[0], "",                   "",                   wikibrowser.BadLoginError)
REGISTRY["useronly"] = (REGISTRY_DEFAULTS[0], REGISTRY_DEFAULTS[1], "",                   wikibrowser.BadLoginError)
REGISTRY["passonly"] = (REGISTRY_DEFAULTS[0], "",                   REGISTRY_DEFAULTS[2], wikibrowser.BadLoginError)
REGISTRY["baduser"]  = (REGISTRY_DEFAULTS[0], "eljefee",            REGISTRY_DEFAULTS[2], wikibrowser.BadLoginError)
REGISTRY["badpass"]  = (REGISTRY_DEFAULTS[0], REGISTRY_DEFAULTS[1], "wrongpass",          wikibrowser.BadLoginError)

WIKIPEDIA["correct"]  = WIKIPEDIA_DEFAULTS
WIKIPEDIA["badurl"]   = ("http://com",          WIKIPEDIA_DEFAULTS[1], WIKIPEDIA_DEFAULTS[2], wikibrowser.BadURLError)
WIKIPEDIA["wrongurl"] = ("http://ebay.com",     WIKIPEDIA_DEFAULTS[1], WIKIPEDIA_DEFAULTS[2], wikibrowser.SiteLayoutError)
WIKIPEDIA["nologin"]  = (WIKIPEDIA_DEFAULTS[0], "",                    "",                    wikibrowser.BadLoginError)
WIKIPEDIA["useronly"] = (WIKIPEDIA_DEFAULTS[0], WIKIPEDIA_DEFAULTS[1], "",                    wikibrowser.BadLoginError)
WIKIPEDIA["passonly"] = (WIKIPEDIA_DEFAULTS[0], "",                    WIKIPEDIA_DEFAULTS[2], wikibrowser.BadLoginError)
WIKIPEDIA["baduser"]  = (WIKIPEDIA_DEFAULTS[0], "jhasdghasdjhkasf",    WIKIPEDIA_DEFAULTS[2], wikibrowser.BadLoginError)
WIKIPEDIA["badpass"]  = (WIKIPEDIA_DEFAULTS[0], WIKIPEDIA_DEFAULTS[1], "notright345u6",       wikibrowser.BadLoginError)
WIKIPEDIA["locked"]   = ("http://en.wikipedia.org/wiki/Main_Page",
                                                WIKIPEDIA_DEFAULTS[1], WIKIPEDIA_DEFAULTS[2], wikibrowser.PageLockedError)

def suite():
    suite = unittest.TestSuite()
    for wiki in TEST_WIKIS:
        if "" in TEST_WIKIS[wiki]["correct"][1:3]: # username or password not entered
            continue
        else:
            if raw_input("test %s? " % wiki) == "yes":
                wiki = TEST_WIKIS[wiki]
                for login in wiki:
                    #if login == "correct":
                    #    continue
                    print "  adding test case %s" % login
                    login = wiki[login]
                    suite.addTest( MapUploadTest(url=login[0], username=login[1], password=login[2], error=login[3]) )
    return suite

def runTests():
    suite().debug()

if __name__ == "__main__":
    runTests()

