String lastDaT;

unsigned long lastDaTTime = 0;


void DaTInit()
{
  DaTRead();

  calcNextDTime();
  calcNextMTime();
  calcNextYTime();
}

void DaTTrigger()
{
  DaTRead();
}

void calcNextDTime()
{
  tmElements_t tme;
  tme = DaTTimeTM;
  tme.Second = tme.Second - (tme.Second % 10);
  nextDData = makeTime(tme) + 10;
}

void calcNextMTime()
{
  tmElements_t tme;
  tme = DaTTimeTM;
  tme.Minute = tme.Minute - (tme.Minute % 5);
  tme.Second = 0;
  nextMData = makeTime(tme) + 300;
}

void calcNextYTime()
{
  tmElements_t tme;
  tme = DaTTimeTM;
  tme.Minute = 0;
  tme.Second = 0;
  nextYData = makeTime(tme)+3600;
}

void DaTMakeValues(String theDate){
  String strWork = theDate;
  int ye, mo, da, ho, mi, se;

  sscanf(theDate.c_str(), "%d/%d/%d %d:%d:%d", &ye, &mo, &da, &ho, &mi, &se);
  DaTTimeTM.Year = ye-1970;
  DaTTimeTM.Month = mo;
  DaTTimeTM.Day = da;
  DaTTimeTM.Hour = ho;
  DaTTimeTM.Minute = mi;
  DaTTimeTM.Second = se;

  DaTTimeValue = makeTime(DaTTimeTM);

  DaTDate = theDate.substring(0, theDate.indexOf(" "));
  DaTMonth = DaTDate.substring(0, DaTDate.lastIndexOf("/"));
  DaTYear = DaTDate.substring(0, DaTDate.indexOf("/"));
  DaTDate.replace("/","");
  DaTMonth.replace("/","");

}

String DaTRead() //ca. 45ms
{
  String strValue;
  String strWork;

  unsigned long myDaT = millis();
  if ((lastDaTTime==0)||(lastDaTTime>myDaT)||(myDaT-1000>lastDaTTime))
  {
    lastDaTTime = myDaT;
    date.begin("/bin/date");
    date.addParameter("+%Y/%m/%d %T");
    date.run();
    lastDaT = date.readString();
    lastDaT.trim();

    DaTMakeValues(lastDaT);

  }
  return lastDaT;
}
