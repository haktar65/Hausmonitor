void RelaysInit()
{
  pinMode(relayWater, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(relayWater), triggerWater, CHANGE);
  pinMode(relayGas, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(relayGas), triggerGas, CHANGE);
} 

const unsigned long dampingValue = 500;
const unsigned long ULONG_MAX = 0xFFFFFFFF;
bool RelayDamping(unsigned long &LastTrigger)
{
  bool result = true;
  unsigned long LoopTime = millis();
  if (LastTrigger>0){
    if (LastTrigger<LoopTime){
      result = (LoopTime-LastTrigger)>=dampingValue;
    }else{
      result = ((ULONG_MAX-LastTrigger)+LoopTime)>=dampingValue;
    }
  }
  if (result) {
    LastTrigger = LoopTime;
  }
  return result;
}

unsigned long RelaysLastTriggerWater = 0;
void triggerWater()
{
  boolean rstate;
  rstate = (digitalRead(relayWater)==HIGH); 
  if (rstate||stateRelayWater)
  {
    stateRelayWater = rstate;
    if (rstate)
    {
      if (RelayDamping(RelaysLastTriggerWater)){
        counterWater++; 
      }
    } 
  }
}

unsigned long RelaysLastTriggerGas = 0;
void triggerGas()
{
  boolean rstate;
  rstate = (digitalRead(relayGas)==HIGH); 
  if (rstate||stateRelayGas)
  {
    stateRelayGas = rstate;
    if (rstate)
    {
      if (RelayDamping(RelaysLastTriggerGas)){
        counterGas++;
      }
    } 
  }
}

void CalcRelayCounterValues()
{
  long storeCountGas = counterGas;
  long storeCountWater = counterWater;

  String str = FaReadCounters();
 
  valueWater = str.substring(str.indexOf(":")+1,str.indexOf(";")).toInt() + storeCountWater;
  str = str.substring(str.indexOf(";")+1);
  valueGas = str.substring(str.indexOf(":")+1,str.indexOf(";")).toInt() + storeCountGas;
  counterWater -= storeCountWater;
  counterGas -= storeCountGas;
  FaWriteCounters();
}
