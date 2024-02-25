import os
import time
import datetime
import win_inet_pton

from pyModbusTCP.client import ModbusClient

def callInverter(addr, nr):
    i = 0
    while True:
        regs_l = c.read_holding_registers(addr, nr)
        time.sleep(2)
        if regs_l:
            break
        i += 1
        if (i>5):
            break
        else:
#            print '.',
            c.close()
            c.open()
            time.sleep(1)

    if not regs_l:
        print('failed!')

    return regs_l

def words2float(b1, b2):
    w2 = (b1 << 16) + b2
    if (w2 & (1<<31) == (1<<31)):
        w2 = -(1<<32)+w2
    return float(w2)

def theprint(a):
#    print a
    return

# init modbus client
c = ModbusClient(host="192.168.178.67",port=502,debug=False)
c.open()
time.sleep(1)

regs_l = callInverter(32016, 2)
if regs_l:
    pvV = float(regs_l[0])/10
    pvA = float(regs_l[1])/100
    theprint('PV: {0:0.1f}V {1:0.2f}A {2:0.3f}W'.format(pvV, pvA, pvV*pvA))
else:
    exit

regs_l = callInverter(32078, 6)
if regs_l:
    pvPPeek = words2float(regs_l[0], regs_l[1])/1000
    pvPAct = words2float(regs_l[2], regs_l[3])/1000
    pvPRea = words2float(regs_l[4], regs_l[5])/1000
    theprint('PVPower: Spitze={0:0.3f}kW, Aktiv={1:0.3f}kW, Reaktiv={2:0.3f}kW'.format(pvPPeek, pvPAct, pvPRea))

regs_l = callInverter(32106, 10)
if regs_l:
    pvESum = words2float(regs_l[0], regs_l[1])/100
    pvEDay = words2float(regs_l[8], regs_l[9])/100
#    print('Data %s' % regs_l)
    theprint('PVEnergy: Day={0:0.2f}kW, Total={1:0.2f}kW'.format(pvEDay, pvESum))

data = '"PV":{' + '"V":{0:0.1f},"A":{1:0.2f},"LP":{2:0.3f},"LA":{3:0.3f},"LR":{4:0.3f},"ED":{4:0.3f},"ES":{4:0.3f}'.format(pvV, pvA, pvPPeek, pvPAct, pvPRea, pvEDay, pvESum) + '},'
ydata = '"PV":{' + '"ED":{0:0.3f},"ES":{1:0.3f}'.format(pvEDay, pvESum) + '},'

regs_l = callInverter(37001, 4)
if regs_l:
    accChar = words2float(regs_l[0], regs_l[1])/1000
    accV = float(regs_l[2])/10
    accSOC = float(regs_l[3])/10
    theprint('Akku: Leistung={0:0.3f}W, Spannung={1:0.1f}V, Kapazitaet={2:0.1f}%'.format(accChar, accV, accSOC))

regs_l = callInverter(37780, 8)
if regs_l:
    accCharS = words2float(regs_l[0], regs_l[1])/100
    accDiscS = words2float(regs_l[2], regs_l[3])/100
    accCharD = words2float(regs_l[4], regs_l[5])/100
    accDiscD = words2float(regs_l[6], regs_l[7])/100
#    theprint('Meter %s' % regs_l)
    theprint('Akku: Tages-Ladung={0:0.2f}kWh, Tages-Entladung={1:0.2f}kWh, Gesamt-Ladung={2:0.2f}kWh, Gesamt-Entladung={3:0.2f}kWh'.format(accCharD,accDiscD,accCharS,accDiscS))

data += '"ACC":{' + '"Ch":{0:0.3f},"V":{1:0.1f},"SOC":{2:0.1f},"ChD":{3:0.2f},"DchD":{4:0.2f},"ChS":{5:0.2f},"DchS":{6:0.2f}'.format(accChar, accV, accSOC, accCharD,accDiscD,accCharS,accDiscS) + '},'
ydata += '"ACC":{' + '"SOC":{0:0.1f},"ChD":{1:0.2f},"DchD":{2:0.2f},"ChS":{3:0.2f},"DchS":{4:0.2f}'.format(accSOC, accCharD,accDiscD,accCharS,accDiscS) + '},'


regs_l = callInverter(37113, 10)
if regs_l:
    mtrPowr = words2float(regs_l[0], regs_l[1])/1000
    mtrPFed = words2float(regs_l[6], regs_l[7])/100
    mtrPUse = words2float(regs_l[8], regs_l[9])/100

data += '"MTR":{' + '"Pwr":{0:0.3f},"Fed":{1:0.2f},"Use":{2:0.2f}'.format(mtrPowr,mtrPFed,mtrPUse) + '}'
ydata += '"MTR":{' + '"Fed":{0:0.2f},"Use":{1:0.2f}'.format(mtrPFed,mtrPUse) + '}'

curTime = datetime.datetime.now()
data = '{"T":"' + curTime.strftime('%Y/%m/%d %H:%M:%S') + '",' + data + '}'
ydata = '{"T":"' + curTime.strftime('%Y/%m/%d %H:%M:%S') + '",' + ydata + '}'

fd = open('../data/cinvdata',"r")
lastData = fd.readline()
fd.close

fd = open('../data/cinvdata',"w")
fd.truncate(0)
fd.write(data)
fd.close

fd = open('../data/minvdata'+curTime.strftime('%Y%m'),"a")
fd.write(data + ',\n')
fd.close



if (lastData<>''):
    lhor = lastData[6:19]
    chor = curTime.strftime('%Y/%m/%d %H')
    if (lhor<>chor):
        fd = open('../data/yinvdata'+curTime.strftime('%Y'),"a")
        fd.write(ydata + ',\n')
        fd.close
else:
    fd = open('../data/yinvdata'+curTime.strftime('%Y'),"a")
    fd.write(ydata + ',\n')
    fd.close

c.close()