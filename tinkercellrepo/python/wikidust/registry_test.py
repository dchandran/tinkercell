import registry

def runTests():
    testNames = ("BBa_F2620", "f2620", "jerry", "ffg34",
                 "g435", "g3454", "bba_f2620", "bBa f4354")
    for name in testNames:
        print "%s is a biobrick name: %s" % (name, registry.isBiobrickName(name))

if __name__ == "__main__":
    runTests()

