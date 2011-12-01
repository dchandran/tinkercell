# Encoding: utf-8
__all__ = ["PartAnnotator"]

from Tkinter import *
from tc_api import *
import window
import registry
import string
import re
import sbpkb
import time
import webbrowser
from partdefinitions import TC_DEFAULT_NAMES, TC_2_SBPKB, REGISTRY_2_TC

ANNOTATOR_GEOMETRY       = "404x490+80+100"
ANNOTATOR_HIGHLIGHTCOLOR = "#CC00FF"
ANNOTATOR_SEARCHDELAY    = 1 # minimum seconds to wait between database queries
ANNOTATOR_ACTIVE         = False

class PartAnnotator(window.Window):
    """ searches the SBPkb for parts and adds links to them
    """

    class SearchBox(window.Window.ControlPanel):
        def __init__(self, parent, restrict):
            window.Window.ControlPanel.__init__(self, parent)
            # search field
            self.searchField = Entry(self, highlightbackground=parent.cget("bg"))
            self.searchField.pack(fill=X, expand=True)
            # type restriction checkbox
            controlsArea = Frame(self)
            self.restrictCheck = Checkbutton(controlsArea, variable=restrict)
            self.restrictCheck.pack(side=LEFT)
            controlsArea.pack(fill=X, expand=True)
        def clear(self):
            self.searchField.delete(0, END)
            #self.restrictCheck.set(1)

    class ResultsPanel(window.Window.ControlPanel):
        def __init__(self, parent):
            window.Window.ControlPanel.__init__(self, parent)
            self.results = []
            self.display = Listbox(self, selectmode=BROWSE,
                                   listvariable=self.results,
                                   activestyle="dotbox",
                                   highlightthickness=0, relief=SUNKEN,
                                   selectborderwidth=0)
            self.scroll = Scrollbar(self, orient=VERTICAL)
            self.display.config(yscrollcommand=self.scroll.set)
            self.scroll.config(command=self.display.yview)
            self.config(bg=parent.cget("bg"))
            self.scroll.pack(side=RIGHT, fill=Y)
            self.display.pack(fill=BOTH, expand=True)
        def add(self, textToAdd):
            self.display.insert(END, textToAdd)
        def clear(self):
            self.display.delete(0, END)
        def mouseScroll(self, mouseWheelEvent):
            "implements scrolling. the Tkinter team forgot this I guess?"
            self.display.yview_scroll( mouseWheelEvent.delta/-120, "units" )
        def arrowScroll(self, keyPressEvent): #TODO fix jumping to bottom
            "makes arrow keys work even if pressed outside the results box"
            self.display.focus_set()
            if self.display.curselection() == ():
                self.display.selection_set(0)
            elif keyPressEvent.keysym == "Down":
                self.display.event_generate("<Down>")
            elif keyPressEvent.keysym == "Up":
                self.display.event_generate("<Up>")

    class ResultButtons(window.Window.ButtonPanel):
        def __init__(self, parent):
            buttons = ("Use Selected", "Open Selected in Browser")
            window.Window.ButtonPanel.__init__(self, parent, buttons)
            self.config(bg=parent.cget("bg"))
            self.selectedButton = self.buttons[0]
            self.previewButton = self.buttons[1]
            self.disable()
        def enable(self):
            self.selectedButton.config(state=NORMAL)
            self.previewButton.config(state=NORMAL)
        def disable(self):
            self.selectedButton.config(state=DISABLED)
            self.previewButton.config(state=DISABLED)

    def __init__(self):
        global ANNOTATOR_ACTIVE
        if ANNOTATOR_ACTIVE:
            raise Exception("Annotator already running")
        else:
            ANNOTATOR_ACTIVE = True
        try:
            selectedParts = fromTC( tc_selectedItems()       ) # careful, some aren't parts
            validParts    = fromTC( tc_itemsOfFamily("part") )
            self.partsToAnnotate = [part for part in selectedParts if part in validParts]
            if len( self.partsToAnnotate ) == 0:
                raise Exception("No parts selected")
            window.Window.__init__(self)
            # set variables
            self.lastUpdate  = 0 # keeps track of the last time update() was called
            self.currentPart = self.partsToAnnotate[0]
            self.results     = []
            tc_highlight(self.currentPart, window.PURPLE)
            # create search box
            self.restrictType = BooleanVar(value=1)
            self.restrictType.trace("w", self.update)
            self.searchBox = self.SearchBox(self, self.restrictType)
            self.searchField = self.searchBox.searchField
            self.restrictCheck = self.searchBox.restrictCheck
            self.searchField.bind("<KeyRelease>", self.queueUpdate)
            self.searchBox.pack(side=TOP, fill=X, padx=2, pady=2)
            self.updateLabel()
            # create results list
            self.resultsBox = self.ResultsPanel(self)
            self.resultsBox.display.bind("<Double-Button-1>", self.useSelected)
            self.resultsBox.display.bind("<Return>", self.useSelected)
            self.searchBox.bind("<Up>", self.resultsBox.arrowScroll)
            self.searchBox.bind("<Down>", self.resultsBox.arrowScroll)
            self.bind("<MouseWheel>", self.resultsBox.mouseScroll)
            self.resultsBox.pack(fill=BOTH, expand=True, padx=2, pady=2)
            # create buttons
            self.resultButtons = self.ResultButtons(self)
            self.resultButtons.selectedButton.config(command=self.useSelected)
            self.resultButtons.previewButton.config(command=self.openSelected)
            self.resultButtons.pack(side=BOTTOM, anchor=CENTER, pady=2)
            # adjust window settings
            global ANNOTATOR_GEOMETRY
            try:
                self.geometry(ANNOTATOR_GEOMETRY)
            except AttributeError: # first launch
                self.geometry("404x490-50-100")
                ANNOTATOR_GEOMETRY = self.geometry()
            self.minsize(width=270, height=150)
            self.title("Annotate %s" % tc_getName(self.currentPart))
            # display everything
            self.searchField.focus_force()
            self.after(100, self.update) # perform an initial search
            self.mainloop()
        except:
            raise
        finally:
            time.sleep(2) # lets you read the labels before they disappear
            self.clearOverlays()
            ANNOTATOR_ACTIVE = False

    def clearOverlays(self):
        "clears overlays, while leaving the same parts selected"
        selected = fromTC( tc_selectedItems() )
        tc_deselect()
        for part in selected:
            tc_select(part)

    def destroy(self, event=None):
        global ANNOTATOR_GEOMETRY
        ANNOTATOR_GEOMETRY = self.geometry()
        window.Window.destroy(self)

    def nextPart(self, event=None):
        """ switches to the next part in partsToAnnotate,
            or closes if this is the last one
        """
        #TODO only wait if displaying; renaming should be immediate?
        #print [tc_getName(part) for part in self.partsToAnnotate]
        if self.currentPart == self.partsToAnnotate[-1]:
            self.destroy()
        else:
            currentIndex = self.partsToAnnotate.index(self.currentPart)
            self.currentPart = self.partsToAnnotate[currentIndex + 1]
            self.searchBox.clear()
            self.resultsBox.display.delete(0, END)
            self.update_idletasks()
            self.update()
            #time.sleep(1)
            self.clearOverlays()
            self.title("Annotate %s" % tc_getName(self.currentPart))
            tc_highlight(self.currentPart, ANNOTATOR_HIGHLIGHTCOLOR)
            self.updateLabel()
            self.searchField.focus_force()

    def updateLabel(self):
        tcType = tc_getFamily(self.currentPart)
        sbpkbType = TC_2_SBPKB[tcType]
        label = 'Only show %s parts' % sbpkbType
        self.restrictCheck.config(text=label)

    def useSelected(self, event=None):
        #TODO get XML for exact names?
        if self.resultsBox.display.curselection()==() or self.results==None:
            return
        else:
            selectedResult = self.resultsBox.display.get( self.resultsBox.display.curselection() )
            partName = string.split(selectedResult, ' ')[0]
            self.annotate(partName)
            self.nextPart()

    def openSelected(self, event=None):
        if self.resultsBox.display.curselection()==() or self.results==None:
            return
        else:
            selectedResult = self.resultsBox.display.get( self.resultsBox.display.curselection() )
            partName = string.split(selectedResult, ' ')[0] # update this if ' ' gets changed
            webbrowser.open_new_tab("http://partsregistry.org/Part:%s" % partName)
            #print "Open Selected in Browser: %s" % partName

    def update(self, *whatever):
        #TODO display error if updating fails
        # get results
        if self.restrictType.get():
            currentType = tc_getFamily(self.currentPart)
        else:
            currentType = None
        partLabel = tc_getName(self.currentPart)
        searchString = self.searchField.get()
        self.results = sbpkb.candidateParts(currentType, partLabel, searchString)
        # display results
        self.resultsBox.clear()
        if self.results == None: # error
            self.resultButtons.disable()
            self.resultsBox.add("Unable to reach the database.")
            self.resultsBox.add("Is your internet connection OK?")
        elif self.results == []: # nothing found
            self.resultButtons.disable()
            if self.restrictType.get():
                self.resultsBox.add('No parts found.')
                self.resultsBox.add('Uncheck the "Only show..." box or try a different keyword.')
            else:
                self.resultsBox.add('No parts found.')
                self.resultsBox.add('Try a different keyword.')
        else:
            self.resultButtons.enable()
            for tuple in self.results:
                diyTab = max((12 - len(tuple[0])), 1) * "  " #tabs are weird on Windows :(
                text   = diyTab.join(tuple)
                self.resultsBox.add(text)

    def queueUpdate(self, event=None):
        """ calls update(), but only if the user hasn't typed
            anything for ANNOTATOR_SEARCHDELAY seconds
        """
        def updateIfStillNeeded():
            timeSinceUpdate = time.time() - self.lastUpdate
            if timeSinceUpdate  <= ANNOTATOR_SEARCHDELAY \
            and self.lastUpdate != 0:
                return # updated already
            else:
                self.lastUpdate = time.time()
                self.update()
        self.after(ANNOTATOR_SEARCHDELAY*1000, updateIfStillNeeded)

    def annotate(self, biobrickName):
        biobrickName = biobrickName.encode('utf-8')
        currentName  = tc_getName(self.currentPart)
        def replace(): # rename the part
            currentName = biobrickName
            tc_rename(self.currentPart, currentName)
        def label(): # just add an overlay showing the biobrick name
            tc_displayText(self.currentPart, biobrickName)
            tc_setDisplayLabelColor(ANNOTATOR_HIGHLIGHTCOLOR, "#000000")
        if currentName.rstrip("1234567890") in TC_DEFAULT_NAMES: # set by tinkercell
            replace()
        elif registry.isBiobrickName(currentName): # probably set by wikidust
            if biobrickName.find(currentName) != -1: # nevermind, short name by user
                label()
            else:
                replace()
        else: # custom name by user
            label()
        #tc_setTextAttribute(self.currentPart, "biobrickname", biobrickName) #redundant?
        partInfo = registry.PartInfo(biobrickName)
        # read annotation from a list
        annotationList = fromTC( tc_getTextData(self.currentPart,"annotation") )
        annotationDict = {}
        keys   = annotationList[0] #row names
        values = annotationList[2][0] #first column
        for index in range( len(keys) ):
            annotationDict[ keys[index] ] = values[index]
        # add info to annotation
        for attribute in partInfo:
            #TODO add other stuff as text attributes?
            if not attribute in REGISTRY_2_TC or not REGISTRY_2_TC[attribute] in annotationDict:
                pass #print "skipping %s because it's not implemented" % attribute
            #elif annotationDict[ REGISTRY_2_TC[attribute] ] != "":
            #    pass #print "skipping %s because it's already set" % attribute
            else: # ready to add attribute
                annotationDict[ REGISTRY_2_TC[attribute] ] = partInfo[attribute]
                #print "%s.%s = %s" % (currentName, REGISTRY_2_TC[attribute], partInfo[attribute]) 
        # flatten back into a list and save
        annotationList = []
        rownames = []
        for key in annotationDict.keys():
            rownames.append(key)
            annotationList.append( annotationDict[key] )
        #print annotationList
        tc_setTextData(self.currentPart, "annotation", toTC([annotationList], rows=rownames))
        try:
            sequence = partInfo["seq_data"]
            sequence = sequence.replace(" ", "")
            #print sequence
            #tc_setSequence(self.currentPart, sequence)
        except KeyError: # no sequence
            pass

