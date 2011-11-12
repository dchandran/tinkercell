from Tkinter import *
from tooltip import ToolTip
from tc_api import *

DARK_GREEN  = "#009966"
LIGHT_GREEN = "#99CC99"
PINK        = "#FF6A6A"
PURPLE      = "#CC00FF"
TOOLTIP_DELAY = 900

class Window(Toplevel):
    """ A generic window meant to be subclassed.
        Comes with some useful "panels".
    """

    class ControlPanel(Frame):
        """ A titled panel for holding a set of control widgets
        """
        def __init__(self, parent, message=None, tooltip=None, **tkArgs):
            Frame.__init__(self, parent, tkArgs)
            if not "bg" in tkArgs:
                self.config(bg=parent.cget("bg"))
            if message != None:
                # WARNING: this interferes with using .grid() in ControlPanels
                Label(self, text=message, bg=self.cget("bg")).pack(anchor=W, padx=5)
            if tooltip != None:
                ToolTip(self, text=tooltip, follow_mouse=False, delay=TOOLTIP_DELAY)

    class ButtonPanel(Frame):
        """ A row of buttons.
            Usually goes at the bottom of a window.
        """
        #TODO make so this doesn't need a list of buttons
        def __init__(self, parent, buttonNames, tooltip=None, **tkArgs):
            Frame.__init__(self, parent, tkArgs)
            if not "bg" in tkArgs:
                self.config(bg=parent.cget("bg"))
            self.buttons = []
            for name in buttonNames:
                button = Button(self, text=name)
                button.config(highlightbackground=self.cget("bg"))
                button.pack(side=LEFT, padx=5, pady=5)
                self.buttons.append(button)
            if len(self.buttons) == 2:
                self.buttons[1].pack(side=RIGHT, padx=5, pady=5)
            if tooltip != None:
                ToolTip(self, text=tooltip, follow_mouse=False, delay=TOOLTIP_DELAY)

    def __init__(self, tooltip=None, **tkArgs):
        Toplevel.__init__(self, tkArgs)
        self.master.withdraw()
        #self.master.geometry("1x1-1-1")
        self.title("Window")
        self.protocol("WM_DELETE_WINDOW", self.destroy)
        self.bind("<Escape>", self.destroy)
        try:
            if sys.platform == "win32":
                iconFile = "icon_win.ico"
            elif sys.platform == "darwin":
                self.config(bg="#EDEDED")
                iconFile = "icon_mac.icns"
            else: # probably linux
                iconFile = "icon_lin.png"
            iconDir = "%s/python/wikidust/src" % tc_homeDir()
            try:
                self.iconbitmap( "%s/%s" % (iconDir, iconFile) )
            except:
                pass
        except: # probably running outside tinkercell
            pass
        if tooltip != None:
            ToolTip(self, text=tooltip, follow_mouse=False, delay=TOOLTIP_DELAY)
        self.focus_force()

    def destroy(self, event=None):
        #print self.__class__, self.geometry()
        Toplevel.destroy(self)
        if len(self.master.children) == 0:
            self.master.destroy()

