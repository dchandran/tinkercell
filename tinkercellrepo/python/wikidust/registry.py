__all__ = ["RegistryError",
           "isBiobrickName", "isRegistryURL", "getURL", "getXML",
           "PartInfo"]

from datetime import datetime
import urllib, urllib2
import re
from xml.dom import minidom
from partdefinitions import USEFUL_REGISTRY_NODES

MAIN_PAGE               = "http://%s.igem.org/Main_Page" % datetime.now().year
LOGIN_PAGE              = "http://ung.igem.org/Login"
LOGIN_CONFIRMATION_PAGE = "http://ung.igem.org/Login_Confirmed"
PART_PREFIX             = "http://partsregistry.org/Part:"
XML_PREFIX              = "http://partsregistry.org/cgi/xml/part.cgi?part="

class RegistryError(Exception):
    "error interacting with the Registry"

def isBiobrickName(text):
    partPrefixes = ("bba_", "bba ")
    if text.lower().startswith(partPrefixes):
        text = text[4:]
    biobrickNamePattern = "[a-zA-Z][0-9]{4,}" # letter, then 4+ numbers
    match = re.match(biobrickNamePattern, text)
    return bool(match)

def isRegistryURL(text):
    if text.startswith(PART_PREFIX):
        biobrickName = text[ len(PART_PREFIX): ]
        return isBiobrickName(biobrickName)
    else:
        return False

def getURL(text):
    if text.startswith(("http://", "https://", "www.")):
        return text
    elif isBiobrickName(text):
        return PART_PREFIX + text
    else: # hope for the best
        return "http://" + urllib.quote(text)

def getXML(biobrickName):
    if not isBiobrickName(biobrickName):
        raise RegistryError("%s is not a valid Biobrick Name")
    response = urllib2.urlopen(XML_PREFIX + biobrickName)
    xmlDoc = minidom.parseString(response.read())
    return xmlDoc

class PartInfo(dict):
    def __init__(self, biobrickName=None):
        if biobrickName != None and not isBiobrickName(biobrickName):
            raise RegistryError("%s is not a valid Biobrick Name" % biobrickName)
        try:
            xml = getXML(biobrickName)
        except:
            raise RegistryError("unable to get %s from the Registry" % biobrickName)
        if len( xml.getElementsByTagName("ERROR") ) > 0:
            raise RegistryError("error while downloading part info for %s" % biobrickName)
        dict.__init__(self)
        for nodeName in USEFUL_REGISTRY_NODES:
            try:
                info = xml.getElementsByTagName(nodeName)[0].firstChild.nodeValue
                info = info.encode("utf-8")
                info = " ".join( info.splitlines() )
                self[nodeName] = info
            except:
                pass #print "failed to get %s" % nodeName

