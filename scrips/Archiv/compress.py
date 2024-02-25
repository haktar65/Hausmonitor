import json
import os
from datetime import datetime, timedelta

thePath="../data/"


fileList = [d for d in os.listdir(thePath)  if d.startswith("data")]
rlines = []
fileList.sort()
for ff in fileList:
    srcFile = open(thePath+ff,'r')
    rlines += srcFile.readlines()
    srcFile.close

rlines[0] = "[" + rlines[0]
rlines[-1] = rlines[-1][0:-2] + "]"

w = open("gugus.json","w")
w.write("".join(rlines))
w.close()

j = json.loads("".join(rlines))

print(len(j))


fy = open("ydata2023","w")
fy.truncate(0)
ty = datetime.strptime("2023/01/01 0:00:00", '%Y/%m/%d %H:%M:%S')
dtj = timedelta(hours=1)

fm = open("mdata202311","w")
fm.truncate(0)
tm = ty
dtm = timedelta(minutes=5)

for je in j:
    td = datetime.strptime(je['T'], '%Y/%m/%d %H:%M:%S')+dtj
    if (td>ty):
        ty = datetime(year=td.year,month=td.month,day=td.day,hour=td.hour) + dtj
        s = '{"T":"'+je['T']+'","W":'+str(je['W'])+',"G":'+str(je['G'])+',"TP":'+str(je['TP'])+',"TN":'+str(je['TN'])+'},\n'
        fy.write(s)
    if (td>tm):
        tm = datetime(year=td.year,month=td.month,day=td.day,hour=td.hour, minute=5*(td.minute // 5))+dtm
        s = '{"T":"'+je['T']+'","W":'+str(je['W'])+',"G":'+str(je['G'])+',"TP":'+str(je['TP'])+',"TN":'+str(je['TN'])+'},\n'
        fm.write(s)
fj.close()
fm.close()