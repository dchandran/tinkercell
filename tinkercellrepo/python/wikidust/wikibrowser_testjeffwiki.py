# encoding: utf-8
import unittest
import os, sys
import random, string
import re
from test import randomString, TEST_IMAGES
import getpass
import wikibrowser

USER_URL = "http://wikidust.sbolstandard.org/index.php?title=Sandbox:Imagemaps"
USERNAME = ''
PASSWORD = ''

class JeffwikiTest(unittest.TestCase):
    def setUp(self):
        # basic setup
        global USERNAME
        global PASSWORD
        if '' in (USERNAME, PASSWORD): # first setUp
            print "need jeffwiki login"
            USERNAME = raw_input("Username: ")
            PASSWORD = getpass.getpass()
        self.browser = wikibrowser.Browser()
        # get real stuff
        br = wikibrowser.Browser() # extra browser for getting test info
        br.open("http://wikidust.sbolstandard.org/index.php/Special:ListFiles")
        fileLink = re.compile('.*File:.*')
        self.realFileNames = [link.text for link in br.links(url_regex=fileLink)]
        self.correctURLs = {"index":  "http://wikidust.sbolstandard.org/index.php",
                            "login":  "http://wikidust.sbolstandard.org/index.php/Special:UserLogin",
                            "upload": "http://wikidust.sbolstandard.org/index.php/Special:Upload"}
        self.editablePages    = ("http://wikidust.sbolstandard.org/index.php?title=Sandbox:Imagemaps",
                                 "http://wikidust.sbolstandard.org/index.php?title=Sandbox:Randompage")
        self.nonEditablePages = ("http://wikidust.sbolstandard.org/index.php/Main_Page",
                                 "http://wikidust.sbolstandard.org/index.php/Special:BrokenRedirects",
                                 "http://wikidust.sbolstandard.org/index.php/Special:RecentChanges")
        #TODO make this jeffwiki-specific
        self.viewablePages    = ("http://en.wikipedia.org/wiki/Obama_%28disambiguation%29",
                                 "http://en.wikipedia.org/wiki/Barack_Obama")
        self.realCode = {"http://wikidust.sbolstandard.org/index.php/Sandbox:Imagemaps": 
                         "</imagemap>"}
        self.pagesToUploadTo = ("http://wikidust.sbolstandard.org/index.php?title=Sandbox:Imagemaps",)
        # make up fake stuff
        self.fakeImageNames = [randomString()+random.choice((".png", ".jpg")) for num in range(10)]
        self.fakeCode = {"http://wikidust.sbolstandard.org/index.php?title=Sandbox:Imagemaps":
                         "this isn';t really in the page"}
        self.codeToUpload = ["random test link: [[%s]]" % randomString(30),
                             "and then there's this"]
        self.filesToUpload = [os.path.realpath(TEST_IMAGES + "/" + name) for name in os.listdir(TEST_IMAGES)]

class LoadWiki(JeffwikiTest):
    def runTest(self):
        print "  loading wiki",
        self.assertFalse( self.browser.wikiLoaded() )
        self.assertFalse( self.browser.loggedIn()   )
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue( self.browser.wikiLoaded() )
        self.assertTrue( self.browser.loggedIn()   )
        self.browser.logout()
        self.assertTrue( self.browser.wikiLoaded() )
        self.assertFalse( self.browser.loggedIn()  )
        print "OK"

class BadURL(JeffwikiTest):
    def runTest(self):
        print "  handling bad URL",
        self.assertRaises( wikibrowser.BadURLError,   self.browser.loadWiki, randomString(), USERNAME, PASSWORD )
        print "OK"

class BadLogin(JeffwikiTest):
    def runTest(self):
        print "  handling bad logins",
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, randomString(), PASSWORD )
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, USERNAME, randomString() )
        print "OK"

class GuessPages(JeffwikiTest):
    def runTest(self):
        print "  guessing pages",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue(self.browser.userURL == USER_URL)
        self.assertTrue( len(self.browser.guessURL) == len(self.correctURLs) )
        for url in self.correctURLs:
            self.assertTrue( self.browser.guessURL[url] == self.correctURLs[url] )
        print "OK"

class FindExisting(JeffwikiTest):
    def testExistingFiles(self):
        print "  finding existing files",
        #wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for name in self.realFileNames[:10]:
            self.assertTrue( self.browser.fileExists(name) )
        for name in self.fakeImageNames[:10]:
            self.assertFalse( self.browser.fileExists(name) )
        #wikibrowser.DEBUGGING = False
        print "OK"

    def testExistingCode(self):
        print "  finding existing code",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for url in self.realCode:
            self.assertTrue( self.browser.codeExists(self.realCode[url], url) )
        for url in self.fakeCode:
            self.assertFalse( self.browser.codeExists(self.fakeCode[url], url) )
        print "OK"

    def runTest(self):
        self.testExistingCode()
        self.testExistingFiles()

class UploadFile(JeffwikiTest):
    def runTest(self):
        print "  uploading test images",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for path in self.filesToUpload[:3]:
            name = os.path.basename(path)
            self.assertFalse( self.browser.fileExists(name) )
            self.browser.uploadFile(path, "test file uploaded by wikidust")
            self.assertTrue( self.browser.fileExists(name) )
        print "OK"

#    def testCanEditView(self):
#        for url in self.editablePages:
#            self.assertTrue( self.browser.canEdit(url) )
#        for url in self.nonEditablePages:
#            self.assertFalse( self.browser.canEdit(url) )
#        for url in self.viewablePages:
#            self.assertTrue( self.browser.canViewSource(url) ) #fails sometimes for no reason?
#        #test non-viewable pages too?

#    def testInAForm(self):
#        self.browser.open( self.browser.loginURL )
#        self.assertFalse( self.browser.inAForm() ) 
#        self.browser.select_form(nr=0)
#        self.assertTrue( self.browser.inAForm() )

#class UploadImagemap(JeffwikiTest):
#    def setUp(self):
#        JeffwikiTest.setUp(self)
#        self.browser.loadWiki(USER_URL)

class UploadCode(JeffwikiTest):
    def runTest(self):
        print "  uploading code",
        for page in self.pagesToUploadTo:
            for fragment in self.codeToUpload:
                self.browser.addWikitext(fragment, page)
                self.assertTrue( self.browser.codeExists(fragment, page) )
        print "OK"

def suite():
    print "testing with Jeffwiki"
    suite = unittest.TestSuite()
    suite.addTest( FindExisting() )
    suite.addTest( UploadCode()   )
    suite.addTest( UploadFile()   )
    suite.addTest( LoadWiki()     )
    suite.addTest( BadURL()       )
    suite.addTest( BadLogin()     )
    #suite.addTest( GuessPages()   )
    return suite

