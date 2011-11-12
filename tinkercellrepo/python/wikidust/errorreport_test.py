import errorreport

def runTests():
    #log = WikiDustLogger()
    #log.debug("this is a debugging message")
    #log.info("this is some info")
    #log.warning("this is a warning")
    #log.error("this is an error message")
    #log.critical("this is a critical error message")
    errorreport.ReportSender().mainloop()

if __name__ == "__main__":
    runTests()

