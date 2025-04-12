#include <FEH.H>
#include <Arduino.h>

FEHMotor left_motor(FEHMotor::Motor1, 9.0);
FEHMotor right_motor(FEHMotor::Motor2, 9.0);
FEHMotor compost_motor(FEHMotor::Motor3, 5.0);

DigitalEncoder right_encoder(FEHIO::Pin11);
DigitalEncoder left_encoder(FEHIO::Pin8);
AnalogInputPin CDS_Sensor(FEHIO::Pin14);
FEHServo Arm_Servo(FEHServo::Servo0);
DigitalInputPin right_bump(FEHIO::Pin1);
DigitalInputPin left_bump(FEHIO::Pin5);

AnalogInputPin right_opto(FEHIO::Pin2);
AnalogInputPin middle_opto(FEHIO::Pin3);
AnalogInputPin left_opto(FEHIO::Pin6);

#define leftOffLow 2
#define leftOffHigh 4
#define rightOffLow 2
#define rightOffHigh 4
#define middleOffLow 2
#define middleOffHigh 4

#define leftOnLowLever 0
#define leftOnHighLever 2.7
#define rightOnLowLever 0
#define rightOnHighLever 3
#define middleOnLowLever 0
#define middleOnHighLever 3

#define noLight 2.0
#define blueMax 2.0
#define redMax 1.1



void ERCMain()
{
  //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();

  while(1)
  {
    LCD.Clear();
    Color = CDS_Sensor.Value();
    LCD.Write("CDS Value: ");
    LCD.WriteLine(Color);

    LCD.Write("   Right Opto: ");
    LCD.WriteLine(right_opto.Value());
    LCD.Write("   Middle Opto: ");
    LCD.WriteLine(middle_opto.Value());
    LCD.Write("   Left Opto: ");
    LCD.WriteLine(left_opto.Value());
    Sleep(5.0);
  }
}
