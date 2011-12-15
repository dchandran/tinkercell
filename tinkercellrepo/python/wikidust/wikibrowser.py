from __future__ import with_statement

#TODO make sure window appears on top
#TODO keep from "not responding" during uplods
#TODO raise only user-suitable errors
#TODO update testing on wikipedia etc.

__all__ = ["BrowsingError", "SiteLayoutError", "FileExistsError", "PageLockedError"
           "BadURLError", "BadLoginError", "Browser"]

import mechanize
import os
import urllib
from urlparse import urlparse
import re
from errorreport import WikiDustLogger

class BrowsingError(Exception):       pass
class SiteLayoutError(BrowsingError): pass
class FileExistsError(BrowsingError): pass
class PageLockedError(BrowsingError): pass
class StateError(BrowsingError):      pass # remove?
class BadInputError(BrowsingError):   pass
class BadURLError(BadInputError):     pass
class BadLoginError(BadInputError):   pass

LOGIN_LINKS         = ["Log in / create account", "Log in", "log in"]
LOGOUT_LINKS        = ["Log out", "log out"]
EXISTING_FILE_LINKS = ["Full resolution"]
NEW_FILE_LINKS      = ["upload it", "upload one"]
SOURCE_LINKS        = ["Edit", "Create", "View source", "edit this page"]
EDITING_LINKS       = ["Cancel", "Editing help"]

LOGIN_REQUIRED_MESSAGES = ["<strong>Login error</strong>", ">Not logged in</h1>"]
PAGE_LOCKED_MESSAGES    = ["This page has been locked to prevent editing.",
                           "<p>You do not have permission",
                           ">You can view and copy the source of this page"]
UW_NETID_RELAY_FORM = '<form method=post action="https://weblogin.washington.edu/" name=relay>'

LOG = WikiDustLogger()

class Browser(mechanize.Browser):
    def __init__(self):
        """ creates a mechanize.Browser with some custom settings
            creates (empty) custom attributes
        """
        LOG.info("")
        LOG.info("Creating Browser")
        mechanize.Browser.__init__( self, factory=mechanize.RobustFactory() )
        self.set_handle_equiv(True)
        self.set_handle_redirect(True)
        self.set_handle_robots(False)
        self.addheaders = [('User-agent', 'Mozilla/5.0')]
        self.userURL = None
        self.guessURL = {"index":None, "login":None, "upload":None}

    def loadWiki(self, url, username='', password=''):
        # make sure given URL is kind of OK
        try:
            response = self.open(url)
        except Exception, err:
            if str(err).find("HTTP Error 404: Not Found") != -1:
                LOG.debug("ignoring 404 error on new page")
            else:
                LOG.error("failed to open %s" % url)
                raise BadURLError("unable to open %s" % url)
        try:
            parsed = urlparse(url)
        except:
            raise BadURLError("unable to parse %s" % url)
        self.userURL = url
        # guess useful URLs from the given one
        pathToIndex = ''.join( parsed.path.rpartition('index.php')[:-1] )
        self.guessURL["index"]  = "%s://%s%s" % (parsed.scheme, parsed.netloc, pathToIndex )
        self.guessURL["login"]  = "%s?title=Special:UserLogin" % self.guessURL["index"]
        self.guessURL["upload"] = "%s?title=Special:Upload" % self.guessURL["index"]
        # log in
        # shouldn't this normally happen before anything else?
        if self.onUWSecureSite(): # need to login before doing anything else
            self.uwLogin(username, password)
        else:
            if username != "" or password != "":
                self.login(username, password)
            else:
                LOG.info("no login info given. going ahead anyway")
                pass

    def onUWSecureSite(self):
        try:
            if re.match( "https.*washington\.edu", self.geturl() ):
                LOG.debug("url suggests netid site")
                return True
        except mechanize.BrowserStateError: # no page loaded yet
            LOG.debug("no page loaded, so not on netid site")
            return False
        html = self._response.read()
        #if html in ('', None):
        #    html = self.reload().read()
        if html.find(UW_NETID_RELAY_FORM) != -1:
            LOG.debug("netid relay form found")
            return True
        else:
            LOG.debug("netid relay form not found")
            return False

    def oniGEMSite(self):
        return self.urlContains( ["igem", "partsregistry"] )

    def onWikipediaSite(self):
        return self.urlContains( ["wikipedia", "wikimedia"] )

    def onOpenWetWare(self):
        return self.urlContains( ["openwetware"] )

    def urlContains(self, keywords):
        try:
            url = self.geturl()
            LOG.debug("url: %s" % self.geturl())
        except mechanize.BrowserStateError: # no page yet
            return False
        for word in keywords:
            if url.find(word) != -1:
                return True
        return False

    def wikiLoaded(self):
        if self.userURL == None:
            return False
        for guess in self.guessURL:
            if self.guessURL[guess] == None:
                return False
        return True
##
##    def loginIfNeeded(self, function): # do args go here too?
##        # how do decorators/handlers work in a class?
##        # is login handled in mapupload so far?
##        try:
##            function() # does this need self?
##        except LinkNotFoundError, FormNotFoundError, SiteLayoutError:
##            if self.loginRequired:
##                LOG.debug("logging in")
##                self.login(username, password)
##                LOG.debug("retrying")
##                function()
##            else:
##                LOG.debug("login isn't the problem")
##                raise

    def login(self, username, password):
        if self.onUWSecureSite():
            return self.uwLogin(username, password)
        elif self.oniGEMSite():
            return self.igemLogin(username, password)
        else:
            return self.mwLogin(username, password)

    def uwLogin(self, username, password):
        if self.uwLoggedIn():
            LOG.debug("already logged in with netid")
            return
        LOG.info("logging in with netid")
        if '' in (username, password):
            raise BadLoginError("UW NetID username and password required")
        try:
            self.open("https://weblogin.washington.edu/")
            self.skipRelay()
            self.select_form(nr=0)
            self.form["user"] = username
            self.form["pass"] = password
            #LOG.debug("before submitting, %s" % self)
            html = self.submit().read()
            if html.find("Login failed.  Please re-enter.") != -1: # extra space required
                raise BadLoginError("Username or password rejected")
        except Exception, e:
            LOG.error("%s\n%s" % (type(e), e))
            raise

    def skipRelay(self):
        try:
            while True:
                self.select_form(name="relay")
                LOG.info("skipping relay")
                self.submit()
        except: # probably mechanize.FormNotFoundError or httperror_seek_wrapper
            LOG.debug("no relay detected")
            pass

    def igemLogin(self, username, password):
        "logs into the parts registry and igem wiki"
        LOG.info("logging in to registry + igem wiki")
        if '' in (username, password):
            raise BadLoginError("iGEM username and password required")
        try:
            self.open("http://ung.igem.org/Login")
            self.select_form(name="form_new_user")
            self.form["username"] = username
            self.form["password"] = password
            html = self.submit(name="Login").read()
            if html.find("That username is not valid") != -1:
                raise BadLoginError("Username rejected")
            elif html.find("That username is valid, but the password is not") != -1:
                raise BadLoginError("Password rejected")
            else:
                LOG.debug("login appears to have been successful")
        except Exception, e:
            LOG.error("%s\n%s" % (type(e), e))
            raise

    def mwLogin(self, username, password):
        "logs in to a standard mediawiki site"
        LOG.info("logging in normally")
        if self.loggedIn():
            return
        elif '' in (username, password):
            raise BadLoginError("Username and password required")
        elif self.userURL == None:
            raise StateError("Can't log in without a URL")
        else:
            LOG.debug("going ahead with login")
        current = self.geturl()
        def fillForm():
            self.select_form(name="userlogin")
            self.form["wpName"] = username
            self.form["wpPassword"] = password
            self.tickCheckbox("wpRemember")
            html = self.submit().read()
            LOG.debug("submitted login form and got to %s" % self.title())
            if self.saysLoginRequired(html):
                raise BadLoginError("Username or password rejected")
            else:
                LOG.debug("login appears to have gone ok")
        try:
            # click the login button to get the right page
            self.open( self.userURL )
            self.followOneOf(LOGIN_LINKS)
            fillForm()
            self.open(current)
        except Exception, e:
            if type(e) == BadLoginError:
                LOG.error("login failed with bad username or password")
                raise
            else:
                LOG.warning("error getting through login from userURL. guessing instead")
                try:
                    # open the default login page
                    self.open( self.guessURL["login"] )
                    fillForm()
                    self.open(current)
                except:
                    LOG.error("that didn't work either. login failed")
                    raise

    def tickCheckbox(self, boxName):
        "marks the first 'boxName' checkbox in the current form"
        if not self.inAForm():
            raise StateError("Must be in a form to tick a checkbox")
        for control in self.controls:
            if control.name == boxName:
                assert str(control).find("CheckboxControl") != -1, \
                       BadInputError("%s is not a checkbox" % control)
                control.items[0].selected = True
                return
        raise SiteLayoutError("%s checkbox not found" % boxName)

    def inAForm(self):
        "returns whether a form is selected and ready to fill out"
        try:
            return self.form != None
        except AttributeError:
            return False

    def loggedIn(self):
        if self.onUWSecureSite():
            return self.uwLoggedIn()
        else:
            return self.mwLoggedIn()

    def uwLoggedIn(self):
        LOG.debug("checking if logged in with NetID")
        html = self.open("https://weblogin.washington.edu/").read()
        self.skipRelay()
        loggedIn = html.find("You still are logged in to the weblogin") != -1
        if loggedIn:
            LOG.debug("logged in")
        else:
            LOG.debug("not logged in")
        return loggedIn

    def mwLoggedIn(self):
        "returns whether the browser is logged in to a standard mediawiki site"
        LOG.debug("checking if logged in")
        try:
            self.geturl()
        except mechanize.BrowserStateError: # no page loaded yet
            LOG.debug("not logged in")
            return False
        def checkLinks():
            LOG.debug("checking links")
            try:
                self.findOneOf(LOGOUT_LINKS)
                LOG.debug("logged in")
                return True
            except SiteLayoutError: # not logged in; check if logged out
                try:
                    self.findOneOf(LOGIN_LINKS)
                    LOG.debug("not logged in.")
                    return False
                except SiteLayoutError: # not logged out either; panic
                    raise SiteLayoutError("Not sure if logged in")
        try:
            return checkLinks()
        except SiteLayoutError:
            LOG.warning("couldn't tell if logged in. trying with the user-supplied page")
            try:
                self.open( self.userURL )
                return checkLinks()
            except:
                LOG.error("that didn't work either. printing links: %s" \
                      % [link.text for link in self.links()])
                raise

    def logout(self):
        if self.onUWSecureSite():
            return self.uwLogout()
        else:
            return self.mwLogout()

    def uwLogout(self):
        "logs out of the UW NetID"
        html = self.open("http://washington.edu/computing/weblogin/logout.html").read()
        if html.find("You already logged out of the") != -1:
            LOG.debug("already logged out of netid")
            return
        else:
            LOG.info("logging out of netid")
            self.select_form(nr=0) # logout form
            html = self.submit().read()
            if html.find("You successfully logged out of the") != -1:
                LOG.debug("logout was successful")
            else:
                LOG.warning("not sure if netid logout worked")

    def mwLogout(self):
        "logs out of a standard mediawiki site"
        LOG.info("logging out of standard mediawiki site")
        if self.loggedIn():
            LOG.debug("clicking log out link")
            try:
                self.followOneOf(LOGOUT_LINKS)
            except SiteLayoutError:
                raise SiteLayoutError("Couldn't find logout link")
            if self.loggedIn():
                raise BrowsingError("Logout failed")
        else: # no need to logout
            LOG.debug("already logged out")
            return

    def followOneOf(self, linkNames):
        link = self.findOneOf(linkNames)
        self.follow_link(link)

    def findOneOf(self, linkNames):
        for name in linkNames:
            try:
                link = self.find_link(text=name)
                if link != None:
                    LOG.debug("found link: %s" % link.text)
                    return link
            except mechanize.LinkNotFoundError:
                continue
        LOG.debug("none of these links found: %s" % linkNames)
        raise SiteLayoutError("%s doesn't have any of these links: %s" % (self.geturl(), linkNames))

    def uploadFile(self, filePath, description=None, replace=False):
        name = os.path.basename(filePath)
        if self.fileExists(name):
            if replace:
                LOG.info("replacing %s" % name)
            else:
                LOG.info("aborting")
                raise FileExistsError("%s already exists on the wiki" % name)
        self.goToFilePage(name)
        try:
            self.followOneOf(NEW_FILE_LINKS + ["Upload file"])
        except SiteLayoutError:
            LOG.warning("couldn't find an upload link. trying the default upload page instead")
            if self.oniGEMSite():
                parsed = urlparse(self.userURL)
                uploadForm = "%s://%s/Special:Upload" % (parsed.scheme, parsed.hostname)
            else:
                uploadForm = "%s?title=Special:Upload" % self.guessURL["index"]
            LOG.debug("going to %s" % uploadForm)
            self.open(uploadForm)
        try:
            if self.oniGEMSite():
                try:
                    self.select_form(nr=1)
                except mechanize.FormNotFoundError: # not logged in?
                    html = self.reload().read()
                    if self.saysLoginRequired(html):
                        raise BadLoginError("Must login to do that")
                    else: # nope, something else
                        raise
            else:
                self.select_form(nr=0)
            if not self.inAForm and "wpUploadFile" in self.form:
                raise SiteLayoutError
            self.form["wpDestFile"] = self.wikiEncode(name)
            if description != None:
                self.form["wpUploadDescription"] = description
            if self.onWikipediaSite():
                self.controls[3].set_value_by_label(["Own work, Creative Commons Attribution"])
            try:
                self.tickCheckbox("wpIgnoreWarning")
            except:
                pass
            mime = "image/%s" % os.path.splitext(name)[1].strip('.') #TODO assume png?
            with open(filePath, 'rb') as img:
                self.form.add_file(img, mime, filePath)
                self.submit()
            LOG.info("%s uploaded" % name)
        except Exception, e:
            if type(e) == BadLoginError:
                raise
            else:
                LOG.error("%s\n%s" % (type(e), e))
                raise SiteLayoutError( "Problem filling out upload form at %s" % self.geturl() )

    def fileExists(self, filename):
        """ returns whether 'filename' is already a file on the wiki
            used to avoid accidentally replacing an image
        """
        filename = self.wikiEncode(filename)
        self.goToFilePage(filename)
        self.skipRelay() #TODO move to goToFilePage?
        try:
            self.findOneOf( EXISTING_FILE_LINKS + [filename] )
            LOG.info("%s exists on the wiki" % filename)
            return True
        except SiteLayoutError:
            try:
                self.findOneOf(NEW_FILE_LINKS)
                LOG.info("%s does not exist on the wiki" % filename)
                return False
            except SiteLayoutError:
                LOG.error("not sure if %s exists" % filename)
                LOG.debug("dumping links: %s" % [link.text for link in self.links()])
                if not self.loggedIn():
                    raise BadLoginError("Not sure if %s exists... you probably just need to log in." % filename)
                else:
                    raise SiteLayoutError("Not sure if %s exists on the wiki" % filename)

    def getImageURL(self, filename):
        filename = self.wikiEncode(filename)
        self.goToFilePage(filename)
        LOG.debug( "got %s and went to %s" % (filename, self.geturl()) )
        try:
            link = self.findOneOf(EXISTING_FILE_LINKS + [filename])
            LOG.debug("returning link: %s" % link.absolute_url)
            return link.absolute_url
        except SiteLayoutError:
            LOG.debug("link not found. returning None")
            return None

    def goToFilePage(self, filename):
        if not self.wikiLoaded():
            raise StateError("Can't go to file page without knowing index url")
        filename = self.wikiEncode(filename)
        parsed = urlparse(self.userURL)
        if self.oniGEMSite():
            fileURL = "%s://%s/Image:%s" % (parsed.scheme, parsed.hostname, filename)
        elif self.onOpenWetWare():
            fileURL = "%s://%s/wiki/Image:%s" % (parsed.scheme, parsed.hostname, filename)
        elif self.onWikipediaSite():
            fileURL = "%s://%s/wiki/File:%s" % (parsed.scheme, parsed.netloc, filename)
        else: # regular mediawiki site hopefully
            fileURL = "%s?title=File:%s" % (self.guessURL["index"], filename)
        LOG.debug("going to %s" % fileURL)
        try:
            self.open(fileURL)
            LOG.debug( "went to %s" % self.geturl() )
        except Exception, e:
            if str(e).find("HTTP Error 404: Not Found") != -1:
                LOG.debug("ignoring 404 error on new page")
            else:
                LOG.error( "%s\n%s" % (type(e), e) )
                raise SiteLayoutError("Couldn't find file page %s" % fileURL)

    def wikiEncode(self, text):
        LOG.debug("sanitizing %s" % text)
        text = str( text ).replace(" ", "_") # put in underscores
        text = text[0].upper() + text[1:] # capitalize first letter
        #text = urllib.quote_plus(text) # escape anything else
        LOG.debug("came out as %s" % text)
        return text

    def addWikitext(self, wikitext, url):
        try:
            self.open(url)
            self.skipRelay()
        except Exception, e:
            if str(e).find("HTTP Error 404: Not Found") != -1:
                LOG.debug("ignoring 404 error on new page")
            else:
                LOG.error( "%s\n%s" % (type(e), e) )
                raise SiteLayoutError("Unable to find %s" % url)
        try:
            self.goToSource(url)
            LOG.debug( "got to %s" % self.geturl() )
            self.form["wpTextbox1"] = '\n\n'.join( (wikitext, self.form["wpTextbox1"]) )
            self.submit()
        except Exception, e:
            if type(e) in (BadURLError, PageLockedError):
                raise
            else:
                LOG.error( "%s\n%s" % (type(e), e) )
                raise SiteLayoutError( "Problem adding wikitext to %s" % self.geturl() )
        LOG.info("code added to %s" % url)
        if not self.codeExists(wikitext, url):
            LOG.error("can't verify that code was added to %s" % url)
            raise BrowsingError("Can't verify that code was added to %s correctly" % url)

    def codeExists(self, code, url):
        """ whether the wikitext on 'url' contains 'code'
            used to verify that 'code' was uploaded successfully
        """
        self.goToSource(url)
        code     = ' '.join( code.split() )
        wikitext = ' '.join( self.form["wpTextbox1"].split() )
        LOG.debug("searching for: %s" % code)
        index = wikitext.find(code)
        LOG.debug("index of code: %s" % index)
        return index != -1

    def goToSource(self, url=None):
        if url == None:
            LOG.debug("no url given. trying the current one")
            try:
                self.geturl()
            except mechanize.BrowserStateError: # not loaded yet
                raise BadURLError("Can't go to source because no URL given")
        else:
            url = url.replace(' ', '_') #TODO full wikiEncode?
            try:
                LOG.debug("going to %s" % url)
                self.open(url)
                self.skipRelay()
            except Exception, err:
                if str(err).find("HTTP Error 404: Not Found") != -1:
                    LOG.debug("ignoring 404 error on new page")
                else:
                    LOG.error( "%s\n%s" % (type(err), err) )
                    raise SiteLayoutError("Can't open %s" % url)
        LOG.debug( "looking for source of %s" % self.geturl() )
        LOG.debug("all links: %s" % [link.text for link in self.links()])
        try:
            try:
                self.followOneOf(SOURCE_LINKS)
            except SiteLayoutError: # might already be on the source page
                LOG.debug("checking if this is already a source page")
                self.findOneOf(EDITING_LINKS)
            LOG.debug( "got to %s" % self.geturl() )
            try:
                self.select_form(name="editform")
            except mechanize.FormNotFoundError:
                html = self.reload().read()
                for message in PAGE_LOCKED_MESSAGES:
                    if html.find(message) != -1:
                        raise PageLockedError(message)
                for form in self.forms():
                    LOG.debug("Found form: %s" % form)
                raise SiteLayoutError("Source page has no editform")
            try:
                self.form["wpTextbox1"]
            except mechanize.FormNotFoundError:
                raise SiteLayoutError
            source = str( self.form["wpTextbox1"][:900] )
            source = source.replace('\n', ' ')
            LOG.debug("source:%s" % source)
        except Exception, e:
            if type(e) == PageLockedError:
                raise
            else:
                LOG.error( "%s\n%s" % (type(e), e) )
                raise SiteLayoutError( "Unexpected source page layout at %s" % self.geturl() )

    #def complainAboutLogin(self, html):
    #    if self.saysLoginRequired(html):
    #        raise BadLoginError("Must login to do that")

    def saysLoginRequired(self, html):
        for message in LOGIN_REQUIRED_MESSAGES:
            if html.find(message) != -1:
                LOG.info("login required: %s" % message)
                return True
        LOG.debug("no evidence that login is required")
        return False

