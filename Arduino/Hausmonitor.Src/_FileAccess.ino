const char* CFaCCName = "/mnt/sda1/data/currentCounter";
const char* CFaCCDName = "/mnt/sda1/data/ccntdata";
const char* CFaCDName = "/mnt/sda1/data/data";
const char* CFaCMName = "/mnt/sda1/data/mdata";
const char* CFaCYName = "/mnt/sda1/data/ydata";
String strCurDate;

void FaInit()
{
  FileSystem.begin();
  DaTRead();

}

String FaReadCounters()
{
  String result;
  File dFile = FileSystem.open(CFaCCName, FILE_READ);
  dFile.seek(0);
  result = dFile.readString();
  dFile.close();
  return result;
}

void FaWriteCounters()
{
  File dFile = FileSystem.open(CFaCCName, FILE_WRITE);
  dFile.seek(0);
  sprintf(chBuf,"W:%li;G:%li;", valueWater, valueGas);
  dFile.println(chBuf);
  dFile.close();
}

char llbuffer1[20];
char llbuffer2[20];
int ll2buffer(char* cbuf, long long wll)
{
  long long all = 1;
  int i = 1;
  int ic = 0;

  if (wll<0){
    cbuf[ic++] = '-';
    wll = -wll;
  }

  while ((i++<14)&&(wll>all)){
    all *= 10;
  }
  wll = wll%all;

  do{
    all /=10;
    cbuf[ic++] = '0'+wll/all;
    wll = wll%all;
  } while (all>1);

  cbuf[ic++] = 0;
  return ic;
}

void FaWriteAllValues()
{
  if (DaTTimeValue>=nextDData){
    ll2buffer(llbuffer1, SzZWTP);
    ll2buffer(llbuffer2, SzZWTN);
    sprintf(chBuf,"{\"T\":\"%s\",\"W\":%li,\"G\":%li,\"TP\":%s,\"TN\":%s,\"TM\":%li},", DaTRead().c_str(), valueWater, valueGas, llbuffer1, llbuffer2, SzZWTM);
    Console.println(chBuf);

    String fName = String(CFaCDName)+DaTDate;
    File dFile = FileSystem.open(fName.c_str(), FILE_APPEND);
    dFile.println(chBuf);
    dFile.close();
    calcNextDTime();

    chBuf[strlen(chBuf)-1] = 0;
    File dFileC = FileSystem.open(CFaCCDName, FILE_WRITE);
    dFileC.seek(0);
    dFileC.println(chBuf);
    dFileC.close();
    calcNextDTime();

    if (DaTTimeValue>=nextMData){
      sprintf(chBuf,"{\"T\":\"%s\",\"W\":%li,\"G\":%li,\"TP\":%s,\"TN\":%s},", DaTRead().c_str(), valueWater, valueGas, llbuffer1, llbuffer2);
      Console.print("mdata:");
      Console.println(chBuf);
      fName = String(CFaCMName)+DaTMonth;
      File dFileM = FileSystem.open(fName.c_str(), FILE_APPEND);
      dFileM.println(chBuf);
      dFileM.close();
      calcNextMTime();
    }

    if (DaTTimeValue>=nextYData){
      sprintf(chBuf,"{\"T\":\"%s\",\"W\":%li,\"G\":%li,\"TP\":%s,\"TN\":%s},", DaTRead().c_str(), valueWater, valueGas, llbuffer1, llbuffer2);
      Console.print("ydata:");
      Console.println(chBuf);
      fName = String(CFaCYName)+DaTYear;
      File dFileY = FileSystem.open(fName.c_str(), FILE_APPEND);
      dFileY.println(chBuf);
      dFileY.close();
      calcNextYTime();
    }
  }
}