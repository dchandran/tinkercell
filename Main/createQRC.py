import sys,os

s = "<RCC>\n\
    <qresource prefix=\"/Graphics\">\n"

root = "../Graphics"
path = os.path.join(root, "")
for root, dirs, files in os.walk(path):
    for fileName in files:
        root = root.replace("\\","/")
        root = root.replace("../Graphics","")
        if fileName.count(".xml") > 0 or fileName.count(".PNG") > 0 or fileName.count(".png") > 0:
            s += ("         <file alias=\"" + root + "/" + fileName + "\">../Graphics" + root + "/" + fileName + "</file>\n")

s += "    </qresource>\n\
    <qresource prefix=\"/texts\">\n"

root = "../NodesTree"
path = os.path.join(root, "")
for root, dirs, files in os.walk(path):
    for fileName in files:
        root = root.replace("\\","/")
        root = root.replace("../NodesTree/","")
        if fileName.count(".nt") > 0 or fileName.count(".txt") > 0 or fileName.count(".TXT") > 0:
            s += ("         <file alias=\"" + root + fileName + "\">../NodesTree" + root + "/" + fileName + "</file>\n")

s += "    </qresource>\n\
</RCC>\n"

print s
