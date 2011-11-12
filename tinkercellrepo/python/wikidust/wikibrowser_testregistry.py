import unittest
import getpass
import re
import os
import random
from test import randomString, TEST_IMAGES
import wikibrowser

USER_URL = "http://partsregistry.org/wiki/index.php?title=Part:BBa_K314000"
USERNAME = ''
PASSWORD = ''

class RegistryTest(unittest.TestCase):
    def setUp(self):
        global USERNAME
        global PASSWORD
        if '' in (USERNAME, PASSWORD):
            print "need registry (igem) login"
            USERNAME = raw_input("username: ")
            PASSWORD = getpass.getpass()
        self.browser = wikibrowser.Browser()
        # get real stuff
        br = wikibrowser.Browser() # extra browser for getting test info
        br.open("http://partsregistry.org/Special:ImageList")
        fileLink = re.compile('.*Image:.*')
        self.realFileNames = [link.text for link in br.links(url_regex=fileLink)]
        self.realCode = \
{"http://partsregistry.org/Help:Spring_2011_DNA_distribution": "=What's included in the Spring 2011 Distribution=",
"http://partsregistry.org/Main_Page": 'The Registry is based on the principle of "get some, give some".'}
        # make up fake stuff
        self.fakeImageNames = [randomString()+random.choice((".png", ".jpg")) for num in range(10)]
        self.filesToUpload = [os.path.realpath(TEST_IMAGES + "/" + name) for name in os.listdir(TEST_IMAGES)]
        self.fakeCode = \
{"http://partsregistry.org/Main_Page":
"""est new features here | See new features here
Related resources & links

    iGEM
    The BioBricks Foundation
    OpenWetWare
    BioBrick Assembly Kit a
""",
"http://partsregistry.org/wiki/index.php/Part:BBa_K145010":
"""<!-- -->
<span class='h3bb'>Sequence and Features</span>
<partinfo>BBa_K145010 SequenceAndFeatures</partinfo>

stuff
<!-- Uncomment this to enable Functional Parameter display 
===Functional Parameters===
<partinfo>BBa_K145010 parameters</partinfo>
<!-- -->
"""}
        self.codeToUpload = ["random test link: [[%s]]" % randomString(30), "and then there's this"]
        self.pagesToUploadTo = ["http://partsregistry.org/wiki/index.php?title=Part:BBa_K314000"]

class LoadWiki(RegistryTest):
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

class BadURL(RegistryTest):
    def runTest(self):
        print "  handling bad URL",
        self.assertRaises( wikibrowser.BadURLError,   self.browser.loadWiki, randomString(), USERNAME, PASSWORD )
        print "OK"

class BadLogin(RegistryTest):
    def runTest(self):
        print "  handling bad logins",
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, randomString(), PASSWORD )
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, USERNAME, randomString() )
        print "OK"

class FindExisting(RegistryTest):
    def testExistingFiles(self):
        print "  finding existing files",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for name in self.realFileNames[:10]:
            self.assertTrue( self.browser.fileExists(name) )
        for name in self.fakeImageNames[:10]:
            self.assertFalse( self.browser.fileExists(name) )
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

class UploadFile(RegistryTest):
    def runTest(self):
        if raw_input("  upload images? ").lower() in ('n', 'no'):
            return
        print "  uploading test images",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue( self.browser.loggedIn() )
        for path in self.filesToUpload[:1]:
            name = os.path.basename(path)
            self.assertFalse( self.browser.fileExists(name) )
            self.browser.uploadFile(path, "test file uploaded by wikidust")
            self.assertTrue( self.browser.fileExists(name) )
        print "OK"

class UploadCode(RegistryTest):
    def runTest(self):
        if raw_input("  upload code? ").lower() in ('n', 'no'):
            return
        print "  uploading code",
        #wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for page in self.pagesToUploadTo:
            for fragment in self.codeToUpload:
                self.browser.addWikitext(fragment, page)
                self.assertTrue( self.browser.codeExists(fragment, page) )
        #wikibrowser.DEBUGGING = False
        print "OK"

def suite():
    print "testing with the registry"
    suite = unittest.TestSuite()
    suite.addTest( FindExisting() )
    suite.addTest( LoadWiki()     )
    suite.addTest( BadURL()       )
    suite.addTest( BadLogin()     )
    suite.addTest( UploadFile()   )
    suite.addTest( UploadCode()   )
    return suite

