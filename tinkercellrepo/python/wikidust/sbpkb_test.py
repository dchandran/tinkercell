import sbpkb

def runTests():
    list = sbpkb.candidateParts(searchString="tetr")
    for item in list:
        print item

if __name__ == "__main__":
    runTests()

