#include <FEHIO.h>
DigitalInputPin distanceSensor(FEHIO::P3_0);

int main(void) 
  { 
  int objectInRange = distanceSensor.Value();
  } 
