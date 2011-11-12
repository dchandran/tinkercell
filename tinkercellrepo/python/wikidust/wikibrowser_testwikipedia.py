import unittest
import getpass
import re
import os
import random
from test import randomString, TEST_IMAGES
import wikibrowser

USER_URL = ''
USERNAME = ''
PASSWORD = ''
WATERMARK = """
This is a test image.
It's freely available for any use.
"""

class WikipediaTest(unittest.TestCase):
    def setUp(self):
        global USERNAME
        global PASSWORD
        if '' in (USERNAME, PASSWORD):
            print "need wikipedia login"
            USERNAME = raw_input("username: ")
            PASSWORD = getpass.getpass()
        global USER_URL
        USER_URL = "http://en.wikipedia.org/wiki/User:%s" % USERNAME # everyone gets their own test page
        self.browser = wikibrowser.Browser()
        # get real stuff
        br = wikibrowser.Browser() # extra browser for getting test info
        br.open("http://en.wikipedia.org/wiki/Special:ListFiles")
        fileLink = re.compile('.*File:.*')
        self.realFileNames = [link.text for link in br.links(url_regex=fileLink)]
        self.realFileNames = [name for name in self.realFileNames if name != '']
        self.realCode = \
{"http://en.wikipedia.org/wiki/User:Jeffdjohnson":
"""[[/map code sandbox]]

[[/screenshot sandbox]]
""",
"http://en.wikipedia.org/wiki/Trafford_Park":
"""and Urmston]]
|post_town= MANCHESTER
|postcode_area= M
|postcode_district= M17
|dial_code= 0161
|os_grid_reference= SJ785965
|static_image= [[Image:Trafford Park.png|240px]]
|static_image_caption=<small>A view over Trafford Park, from [[Eccles, Greater Manchester|Eccles]]</small>
}}
"""}
        self.pagesToUploadTo = ["http://en.wikipedia.org/wiki/User:Jeffdjohnson",
                                "http://en.wikipedia.org/wiki/User:Jeffdjohnson/map_code_sandbox2"]
        # make up fake stuff
        self.fakeImageNames = [randomString()+random.choice((".png", ".jpg")) for num in range(10)]
        self.filesToUpload = [os.path.realpath(TEST_IMAGES + "/" + name) for name in os.listdir(TEST_IMAGES)]
        self.fakeCode = \
{"http://en.wikipedia.org/wiki/Talk:Trafford_Park":
"""As written, the article, citing Canal Archive appeared to state that the Fourth Baronet de Trafford had been the landowner who strongly opposed the ship canal in the early 1880s. In fact, Sir Humphrey de Trafford, the Second Baronet was the landowner at this time. He died in 1886, and his son Sir Humphrey Francis de Trafford, was more positive towards the canal. Therefore I have removed the text mentioning the "4th Sir Humphrey de Traff
""",
"http://en.wikipedia.org/wiki/Portal:Geography":
"[[thisi skaldfhkasjdf lkusy fhsadkf jh"}
        self.codeToUpload = ["random test link: [[%s]]" % randomString(30), "and then there's this"]

class LoadWiki(WikipediaTest):
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

class BadURL(WikipediaTest):
    def runTest(self):
        print "  handling bad URL",
        self.assertRaises( wikibrowser.BadURLError, self.browser.loadWiki, randomString(), USERNAME, PASSWORD )
        print "OK"

class BadLogin(WikipediaTest):
    def runTest(self):
        print "  handling bad logins",
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, randomString(), PASSWORD )
        self.assertRaises( wikibrowser.BadLoginError, self.browser.loadWiki, USER_URL, USERNAME, randomString() )
        print "OK"

class UploadFile(WikipediaTest):
    def runTest(self):
        if not raw_input("  upload image? ") == "yes":
            return
        print "  uploading test image",
        wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        self.assertTrue( self.browser.loggedIn() )
        for path in self.filesToUpload[:1]:
            name = os.path.basename(path)
            self.assertFalse( self.browser.fileExists(name) )
            self.browser.uploadFile(path, WATERMARK)
            self.assertTrue( self.browser.fileExists(name) )
        wikibrowser.DEBUGGING = False
        print "OK"

class FindExisting(WikipediaTest):
    def testExistingFiles(self):
        print "  finding existing files",
        #wikibrowser.DEBUGGING = True
        self.browser.loadWiki(USER_URL, USERNAME, PASSWORD)
        for name in self.realFileNames[:5]:
            self.assertTrue( self.browser.fileExists(name) )
            print '.',
        for name in self.fakeImageNames[:5]:
            self.assertFalse( self.browser.fileExists(name) )
            print '.',
        #wikibrowser.DEBUGGING = False
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
        self.testExistingCode()
        self.testExistingFiles()

class UploadCode(WikipediaTest):
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
    print "testing with Wikipedia"
    suite = unittest.TestSuite()
    suite.addTest( UploadCode()   )
    suite.addTest( FindExisting() )
    suite.addTest( LoadWiki()     )
    suite.addTest( BadURL()       )
    suite.addTest( BadLogin()     )
    suite.addTest( UploadFile()   )
    return suite

