import unittest
import getpass
import re
import os
import random
from test import randomString, TEST_IMAGES
import wikibrowser

USER_URL = "https://courses.washington.edu/synbio/goldenbricks/index.php?title=Main_Page"
USERNAME = ''
PASSWORD = ''

class GoldenbricksTest(unittest.TestCase):
    def setUp(self):
        global USERNAME
        global PASSWORD
        if '' in (USERNAME, PASSWORD):
            print "need UW NetID"
            USERNAME = raw_input("username: ")
            PASSWORD = getpass.getpass()
        self.browser = wikibrowser.Browser()
        # real stuff
        self.correctURLs = {"index":  "https://courses.washington.edu/synbio/goldenbricks/index.php",
                            "login":  "https://courses.washington.edu/synbio/goldenbricks/index.php?title=Special:UserLogin",
                            "upload": "https://courses.washington.edu/synbio/goldenbricks/index.php?title=Special:Upload"}
        br = wikibrowser.Browser()
        br.loadWiki(USER_URL, USERNAME, PASSWORD)
        br.open( "%s?title=Special:ListFiles" % self.correctURLs["index"] )
        fileLink = re.compile('.*File:.*')
        self.realFileNames = [link.text for link in br.links(url_regex=fileLink)]
        self.realCode = \
{"https://courses.washington.edu/synbio/goldenbricks/index.php?title=Main_Page":
"""==Some test space below for demonstrating wiki things==
[[Test]]
[[Test1]]
"""}
        self.pagesToUploadTo = ("https://courses.washington.edu/synbio/goldenbricks/index.php?title=Test2",)
        self.codeToUpload = ["random test link: [[%s]]" % randomString(30),
                             "and then there's this"]
        self.lockedPages = ["https://courses.washington.edu/synbio/goldenbricks/index.php?title=Oscillators"]
        # fake stuff
        self.fakeImageNames = [randomString()+random.choice((".png", ".jpg")) for num in range(10)]
        self.filesToUpload = [os.path.realpath(TEST_IMAGES + "/" + name) for name in os.listdir(TEST_IMAGES)]
        self.fakeCode = \
{"https://courses.washington.edu/synbio/goldenbricks/index.php?title=Main_Page":
"""==Some test spaceas needed fslku jfh jhhh this [[stuff]]
"""}

class LoadWiki(GoldenbricksTest):
    def runTest(self):
        print "  loading wiki",
        self.assertFalse( self.browser.wikiLoaded() )
        self.assertFalse( self.browser.loggedIn() )
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue( self.browser.wikiLoaded() )
        self.assertTrue( self.browser.loggedIn() )
        self.browser.logout()
        self.assertTrue( self.browser.wikiLoaded() )
        self.assertFalse( self.browser.loggedIn() )
        print "OK"

class BadURL(GoldenbricksTest):
    def runTest(self):
        print "  handling bad URL",
        self.assertRaises( wikibrowser.BadURLError,   self.browser.loadWiki, randomString(), USERNAME, PASSWORD )
        print "OK"

class BadLogin(GoldenbricksTest):
    def runTest(self):
        print "  handling bad logins",
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, randomString(), PASSWORD )
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, USERNAME, randomString() )
        print "OK"

class GuessPages(GoldenbricksTest):
    def runTest(self):
        print "  guessing pages",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue(self.browser.userURL == USER_URL)
        self.assertTrue( len(self.browser.guessURL) == len(self.correctURLs) )
        for url in self.correctURLs:
            try:
                self.assertTrue( self.browser.guessURL[url] == self.correctURLs[url] )
            except AssertionError:
                print self.browser.guessURL[url]
                raise
        print "OK"

class FindExisting(GoldenbricksTest):
    def testExistingFiles(self):
        print "  finding existing files",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for name in self.realFileNames:
            self.assertTrue( self.browser.fileExists(name) )
        for name in self.fakeImageNames:
            self.assertFalse( self.browser.fileExists(name) )
        print "OK"

    def testExistingCode(self):
        print "  finding existing code",
        #wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for url in self.realCode:
            self.assertTrue( self.browser.codeExists(self.realCode[url], url) )
        for url in self.fakeCode:
            self.assertFalse( self.browser.codeExists(self.fakeCode[url], url) )
        #wikibrowser.DEBUGGING = False
        print "OK"

    def runTest(self):
        self.testExistingFiles()
        self.testExistingCode()

class UploadFile(GoldenbricksTest):
    def runTest(self):
        if raw_input("  upload images? ").lower() in ('n', 'no'):
            return
        print "  uploading test images",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for path in self.filesToUpload[:3]:
            name = os.path.basename(path)
            self.assertFalse( self.browser.fileExists(name) )
            self.browser.uploadFile(path, "test file uploaded by wikidust")
            self.assertTrue( self.browser.fileExists(name) )
        print "OK"

class UploadCode(GoldenbricksTest):
    def runTest(self):
        print "  uploading code",
        #wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for page in self.pagesToUploadTo:
            for fragment in self.codeToUpload:
                self.browser.addWikitext(fragment, page)
                self.assertTrue( self.browser.codeExists(fragment, page) )
        #wikibrowser.DEBUGGING = False
        print "OK"

class LockedPage(GoldenbricksTest):
    def runTest(self):
        print "  handling locked pages",
        self.browser.loadWiki(USER_URL, USERNAME,PASSWORD)
        for page in self.lockedPages:
            self.assertRaises(wikibrowser.PageLockedError, self.browser.goToSource, page)
        print "OK"

#class LoadWiki(GoldenbricksTest):
#    def testFindPages(self):
#        # urls should start empty
#        for url in self.browser.pages:
#            self.assertEquals(self.browser.pages[url], None)
#        # then the Browser should discover them
#        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
#        self.assertEquals(self.browser.pages["index"], INDEX_URL)
#        self.assertEquals(self.browser.pages["code"], USER_URL)
#        self.assertEquals(self.browser.pages["login"], LOGIN_URL)
#        self.assertEquals(self.browser.pages["upload"], UPLOAD_URL)
#        #self.assertEquals(self.browser.fileURL, FILE_URL)

    #def testPrematureLogin(self):
    #    self.assertFalse( self.browser.loggedIn() )
    #    self.assertRaises( self.browser.login(self.username, self.password),
    #                       wikibrowser.StateError                            )
    #    self.assertFalse( self.browser.loggedIn() )

def suite():
    print "testing with Goldenbricks wiki"
    suite = unittest.TestSuite()
    suite.addTest( LockedPage()   )
    suite.addTest( UploadCode()   )
    suite.addTest( FindExisting() )
    suite.addTest( UploadFile()   )
    suite.addTest( LoadWiki()     )
    suite.addTest( BadURL()       )
    suite.addTest( BadLogin()     )
    suite.addTest( GuessPages()   )
    return suite

