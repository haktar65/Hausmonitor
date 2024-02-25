""" Make File for Documentation """
thisMakeFileName = "Documentation"
import sys
import os
import re
import pathlib
from subprocess import call
myPath=pathlib.Path(sys.argv[0]).parent.resolve()

# iterate through our substructure
def iterateSubDirs(thePath, func):
    dirList = [d for d in os.listdir(thePath)  if re.match("^[0-9][0-9]_.*",d)]
    dirList.sort()
    for dir in dirList:
        func(thePath, dir)

# executes the make files in the subdirectories subsequently
def executeSub(thePath, subDir):
    call(["python", str(thePath / subDir / ".make.py")])

# handles the import from document lists
def handleImports(thePath):
    fileList = [d for d in os.listdir(thePath)  if d.startswith("_" + thisMakeFileName)]
    for ff in fileList:
        aff = thePath / ff
        srcFile = open(aff,'r')
        rlines = srcFile.readlines()
        wlines = []
        readState = "copy"
        importsFound = False
        for l in rlines:
            match readState:
                case "copy":
                    wlines.append(l)
                    if l.startswith("//>>IMPORT:"):
                        readState = "delete"
                        importsFound = True
                        sdir = l[11:].strip()
                        print("import " + sdir)
                        importList = [fi for fi in os.listdir(thePath/sdir) if fi.lower().endswith(".adoc")]
                        importList.sort()
                        for fi in importList:
                            wlines.append("include::"+sdir+"/"+fi+"[]\n\n")
                case "delete":
                    if l.startswith("//<<IMPORT:"):
                        wlines.append(l)
                        readState = "copy"
        srcFile.close()
        if importsFound:
            destFile = open(aff,'w')
            destFile.writelines(wlines)
            destFile.close()

print("Make " + thisMakeFileName)

#>>preiterate
#<<preiterate

iterateSubDirs(myPath, executeSub)

#>>postiterate
#<<postiterate
