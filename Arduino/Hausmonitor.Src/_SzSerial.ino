const int CMaxValues = 7;
const int CMaxIDDigits = 8;
const byte CSzIds[CMaxValues][8] = {
          {0x77, 0x07, 0x01, 0x00, 0x01, 0x08, 0x00, 0xff},
          {0x77, 0x07, 0x01, 0x00, 0x02, 0x08, 0x00, 0xff},
          {0x77, 0x07, 0x01, 0x00, 0x10, 0x07, 0x00, 0xff}};


enum enumSzValue {
  vSzZWTP = 0,
  vSzZWTN,
  vSzZWTM,
};


enum enumSzReadSequence {
  rsSzZWSearchID = 0, 
  rsSzZWSearchFactor,
  rsSzZWReadFactor,
  rsSzZWReadValue
};

enumSzReadSequence SzReadSequence[CMaxValues] = {rsSzZWSearchID, rsSzZWSearchID, rsSzZWSearchID};
int countSzSequence[CMaxValues] = {0,0,0};
double SzFactor;
byte SzValueBuffer[8];
byte SzValueType;
unsigned int SzValueLength;

void SzInit()
{
  SzSerial.begin(SzBaudRate);
}

void SzTrigger()
{
  String strBuffer;

  char theByte = 0;
  while (SzSerial.available() > 0){
    while (SzSerial.available() > 0){
      theByte = SzSerial.read();
      strBuffer.concat(theByte);
    }
    delay(10);
  }
  if (strBuffer.length()>0){
    SMLParserReset();
    for (unsigned int ui = 0; ui<strBuffer.length(); ui++)
      SMLParser(strBuffer.charAt(ui));
  }
}

void SMLParserReset()
{
  for (int i = 0; i<CMaxValues; i++){
    SzReadSequence[i] = rsSzZWSearchID;
    countSzSequence[i] = 0;
  }
}

void SMLParser(byte theByte)
{
  int i = 0;
  long long lldata;
  long ldata;
  for (i = 0; i<CMaxValues; i++)
  {
    switch (SzReadSequence[i]){
      case rsSzZWSearchID:
        if (theByte == CSzIds[i][countSzSequence[i]]){
          countSzSequence[i]++;
          if (countSzSequence[i]==CMaxIDDigits)
          {
            countSzSequence[i] = 0;
            SzReadSequence[i] = rsSzZWSearchFactor;
          }
        }
        else{
          //failed
          countSzSequence[i] = 0;
          SzReadSequence[i] = rsSzZWSearchID;
        }
        break;
      case rsSzZWSearchFactor:
        if (theByte == 0x52){
          countSzSequence[i] = 0;
          SzReadSequence[i] = rsSzZWReadFactor;
        }
        else{
          if (countSzSequence[i]++>20){
            //failed
            countSzSequence[i] = 0;
            SzReadSequence[i] = rsSzZWSearchID;
            Console.println("FailFactor" + String(i));
          }
        }
        break;
      case rsSzZWReadFactor:
        if ((theByte!=0xFF)&&(theByte!=0x00)){
            countSzSequence[i] = 0;
            SzReadSequence[i] = rsSzZWSearchID;
            Console.println("FailFactor" + String(i));
        }
        else{
          countSzSequence[i] = -1;
          SzReadSequence[i] = rsSzZWReadValue;
        }
        break;
      case rsSzZWReadValue:
        if (countSzSequence[i]<0){
            SzValueType = theByte;
            SzValueLength = (SzValueType & 0x0f)-1;
            countSzSequence[i]++;
        }
        else 
        {
          SzValueBuffer[countSzSequence[i]] = theByte;
          countSzSequence[i]++;
          if (countSzSequence[i] == (int)(SzValueLength)){

            switch (i){
              case vSzZWTP:
                lldata = SzValueBuffer[0];
                for (int z = 1; z<8; z++)
                  lldata = (lldata << 8) + SzValueBuffer[z];
                
                if (lldata<1000000000){
                  SzZWTP = lldata;
                  SzReadFlags = SzReadFlags | 0x01;
                }
                break;
              case vSzZWTN:
                lldata = SzValueBuffer[0];
                for (int z = 1; z<8; z++)
                  lldata = (lldata << 8) + SzValueBuffer[z];
                
                if (lldata<1000000000){
                  SzZWTN = lldata;
                  SzReadFlags = SzReadFlags | 0x02;
                }
                break;
              case vSzZWTM:
                ldata = SzValueBuffer[0];
                for (int z = 1; z<4; z++)
                  ldata = (ldata << 8) + SzValueBuffer[z];

                if (abs(ldata)<100000){
                  SzZWTM = ldata;
                  SzReadFlags = SzReadFlags | 0x04;
                }
                break;
            }
            countSzSequence[i] = 0;
            SzReadSequence[i] = rsSzZWSearchID;
          }
        }
        break;
    }
  } 
}
