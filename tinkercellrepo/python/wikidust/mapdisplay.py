__all__ = ["CodeDisplayPrompt"]

import os
from window import *

CODEDISPLAY_GEOMETRY = "700x450+150+150"

class CodeDisplayPrompt(Window):
    """ displays the path to a screenshot and the code for a matching imagemap
    """

    class PathControls(Window.ControlPanel):
        def __init__(self, parent, path):
            message = "Your image can be found here:"
            Window.ControlPanel.__init__(self, parent, message)
            self.pathField = Entry(self, state=NORMAL, exportselection=True)
            self.pathField.insert(END, path)
            self.pathField.pack(fill=X, expand=True, padx=5, pady=5)
            buttonArea = Frame(self)
            Button(buttonArea, text="Copy Path to Clipboard", command=self.copyPath).pack(side=RIGHT, anchor=W)
            Button(buttonArea, text="Open Folder", command=self.openFolder).pack(side=RIGHT, anchor=W)
            buttonArea.pack(side=RIGHT, padx=5)
   
        def copyPath(self, event=None):
            self.clipboard_clear()
            self.clipboard_append( self.pathField.get() )

        def openFolder(self):
            folder = os.path.dirname( self.pathField.get() )
            os.startfile(folder)

    class CodeControls(Window.ControlPanel):
        def __init__(self, parent, code):
            Window.ControlPanel.__init__(self, parent, "And here's the code to make it interactive:")
            self.codeField = Text(self, state=NORMAL, exportselection=True)
            self.codeField.insert(END, code)
            buttonsArea = Frame(self)
            Button(buttonsArea, text="Close", command=parent.destroy, padx=5).pack(side=LEFT, padx=5, pady=5)
            Button(buttonsArea, text="Copy Code to Clipboard", command=self.copyCode, \
                   width=20).pack(side=RIGHT, padx=5, pady=2)
            buttonsArea.pack(side=BOTTOM, fill=X, expand=True)
            self.codeField.pack(fill=BOTH, expand=True, padx=5, pady=5)
 
        def copyCode(self, event=None):
            self.clipboard_clear()
            self.clipboard_append( self.codeField.get(1.0, END) )

    def __init__(self, path="/path/to/screenshot.png", code="this is some sample code"):
        Window.__init__(self)
        self.title("Export Map")
        self.PathControls(self, path).pack(fill=X, expand=True)
        self.CodeControls(self, code).pack(fill=X, expand=True)
        self.geometry(CODEDISPLAY_GEOMETRY)
        self.resizable(False, False)
        self.focus_force()

    def destroy(self, event=None):
        global CODEDISPLAY_GEOMETRY
        CODEDISPLAY_GEOMETRY = self.geometry()
        Window.destroy(self)

if __name__ == "__main__":
    CodeDiaplayPrompt()

