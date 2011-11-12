from __future__ import with_statement
import os, subprocess
from window import *
import mechanize
import logging
try:
    from tc_api import tc_homeDir
except ImportError: # running outside TinkerCell
    def tc_homeDir():
        dir = raw_input("Path to TinkerCell directory? ")
        return os.path.expanduser(dir)

LOG_DIR = os.path.realpath(".") # add /?
MAIN_LOG = os.path.realpath( "%s/wikidust.log" % LOG_DIR)
ERROR_REPORT_FORM = \
"https://spreadsheets0.google.com/spreadsheet/viewform?hl=en_US&hl=en_US&formkey=dDJCSV96X0lBRENZUU5MRzBrYXhHQ1E6MQ#gid=0"

class WikiDustLogger(logging.Logger):
    def __init__(self, path=MAIN_LOG, level=logging.INFO):
        logging.Logger.__init__(self, "WikiDustLogger")
        form = logging.Formatter('%(asctime)s %(levelname)-8s %(message)s')
        hand = logging.FileHandler(path)
        hand.setFormatter(form)
        self.addHandler(hand)
        self.setLevel(logging.DEBUG)

class ReportSender(Window):
    class LogControls(Window.ControlPanel):
        def __init__(self, parent, logs):
            Window.ControlPanel.__init__(self, parent)
            message = "These files will be sent to the WikiDust developer:"
            if len(logs) == 0:
                message += "\n    No logs found in %s" % LOG_DIR
            else:
                for filename in logs:
                    message += "\n    %s" % filename
            Label(self, text=message, justify=LEFT).pack(side=LEFT, padx=5)

    class CommentControls(Window.ControlPanel):
        def __init__(self, parent):
            Window.ControlPanel.__init__(self, parent, message="Additional comments:")
            self.commentsField = Text(self)
            self.commentsField.pack(fill=BOTH, expand=True, padx=5)

    class ReplyControls(Window.ControlPanel):
        def __init__(self, parent):
            Window.ControlPanel.__init__(self, parent)
            message = "Enter your email if you'd like to hear when this problem is addressed:"
            self.emailField = Entry(self)
            self.emailField.pack(side=RIGHT, fill=X, expand=True, padx=5)
            Label(self, text=message).pack(padx=5)

    class SenderButtons(Window.ButtonPanel):
        def __init__(self, parent):
            buttons = ("Cancel", "Send error report")
            Window.ButtonPanel.__init__(self, parent, buttons)
            self.cancelButton = self.buttons[0]
            self.sendButton = self.buttons[1]

    def __init__(self):
        Window.__init__(self)
        # find log files
        self.logs = []
        for filename in os.listdir(LOG_DIR):
            try:
                if os.path.splitext(filename)[1] == ".log":
                    path = os.path.realpath( "%s/%s" % (LOG_DIR, filename) )
                    self.logs.append(path)
            except IOError: # svn file?
                continue
        # create GUI
        self.title("Error Report")
        self.logInfo     = self.LogControls(self, self.logs)
        self.commentInfo = self.CommentControls(self)
        self.replyInfo   = self.ReplyControls(self)
        self.buttons     = self.SenderButtons(self)
        self.buttons.cancelButton.config(command=self.destroy)
        self.buttons.sendButton.config(command=self.sendReport)
        for panel in (self.logInfo, self.commentInfo,
                      self.replyInfo, self.buttons):
            panel.pack(pady=5, fill=X, expand=True)
        self.resizable(False, False)
        self.browser = mechanize.Browser()

    def sendReport(self):
        email = self.replyInfo.emailField.get()
        comments = self.commentInfo.commentsField.get(1.0, END)
        log = ""
        for filename in self.logs:
            try:
                with open(filename, "rb") as file:
                    log += "\n################### %s ###################\n%s\n\n" % ( filename, file.read() ) 
            except IOError: # svn file?
                continue
        self.browser.open(ERROR_REPORT_FORM)
        self.browser.select_form(nr=0)
        self.browser.form["entry.0.single"] = log
        self.browser.form["entry.1.single"] = email
        self.browser.form["entry.2.single"] = comments
        self.browser.submit()
        self.destroy()

