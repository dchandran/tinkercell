import unittest
import getpass
import re
import os
import random
from test import randomString, TEST_IMAGES
import wikibrowser

USER_URL = "http://2010.igem.org/Team:Washington/Tools Created/New Software/Testing"
USERNAME = ''
PASSWORD = ''

class iGEMTest(unittest.TestCase):
    def setUp(self):
        global USERNAME
        global PASSWORD
        if '' in (USERNAME, PASSWORD):
            print "need igem login"
            USERNAME = raw_input("username: ")
            PASSWORD = getpass.getpass()
        self.browser = wikibrowser.Browser()
        # get real stuff
        br = wikibrowser.Browser() # extra browser for getting test info
        br.open("http://2010.igem.org/Special:ImageList")
        fileLink = re.compile('.*Image:.*')
        self.realFileNames = [link.text for link in br.links(url_regex=fileLink)]
        self.realCode = {"http://2010.igem.org/Team:Washington/Code_testing": '<area shape="rect" coords="'}
        self.pagesToUploadTo = ["http://2010.igem.org/Team:Washington/Tools Created/New Software/Testing"]
        self.codeToUpload = ["random test link: [[%s]]" % randomString(30), "and then there's this"]
        # make up fake stuff
        self.fakeImageNames = [randomString()+random.choice((".png", ".jpg")) for num in range(10)]
        self.filesToUpload = [TEST_IMAGES + name for name in os.listdir(TEST_IMAGES)]
        self.fakeCode = \
{"http://2011.igem.org/wiki/index.php?title=Main_Page":
"""<p>iGEM 2011 Teachers Workshop details have been finalized.
<ul>
<li><span style="font-weight:bold">27 May: Carmona, Spain</span>
</ul
</p>
""",
"http://2011.igem.org/Oil_Sands":
"[[File:OSLIiGEM2011sm2.JPG|link=http://www.osli.a/newideas/apply|center]]",
"http://2011.igem.org/Oil_Sands":
"this isn';t ev[[][[][]en remortuykj close***"
}

class LoadWiki(iGEMTest):
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

class BadURL(iGEMTest):
    def runTest(self):
        print "  handling bad URL",
        self.assertRaises( wikibrowser.BadURLError, self.browser.loadWiki, randomString(), USERNAME, PASSWORD )
        print "OK"

class BadLogin(iGEMTest):
    def runTest(self):
        print "  handling bad logins",
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, randomString(), PASSWORD )
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, USERNAME, randomString() )
        print "OK"

class FindExisting(iGEMTest):
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

class UploadFile(iGEMTest):
    def runTest(self):
        if raw_input("  upload images? ").lower() in ('n', 'no'):
            return
        print "  uploading test images",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue( self.browser.loggedIn() )
        for path in self.filesToUpload[:2]:
            name = os.path.basename(path)
            self.assertFalse( self.browser.fileExists(name) )
            self.browser.uploadFile(path, "test file uploaded by wikidust")
            self.assertTrue( self.browser.fileExists(name) )
        print "OK"

class UploadCode(iGEMTest):
    def runTest(self):
        if raw_input("  upload code? ").lower() in ('n', 'no'):
            return
        print "  uploading code",
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for page in self.pagesToUploadTo:
            for fragment in self.codeToUpload:
                self.browser.addWikitext(fragment, page)
                self.assertTrue( self.browser.codeExists(fragment, page) )
        print "OK"

def suite():
    print "testing with the igem wiki"
    suite = unittest.TestSuite()
    suite.addTest( FindExisting() )
    suite.addTest( LoadWiki()     )
    suite.addTest( BadURL()       )
    suite.addTest( BadLogin()     )
    suite.addTest( UploadFile()   )
    suite.addTest( UploadCode()   )
    return suite

