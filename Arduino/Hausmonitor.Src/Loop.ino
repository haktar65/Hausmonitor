const int loopDelay = 1000;
unsigned int ledVal = 1;
unsigned long lastLoopTime = 0;
String strIn;

void loop() 
{

  SzTrigger();

  if (SzReadFlags == 0x7)
  {
    DaTTrigger();
    // put your main code here, to run repeatedly:
    ledVal = 0x01-ledVal;
    digitalWrite(LED_BUILTIN, ledVal);   // turn the LED on (HIGH is the voltage level)

    CalcRelayCounterValues();

    FaWriteAllValues();

    SzReadFlags = 0;
  }
}
