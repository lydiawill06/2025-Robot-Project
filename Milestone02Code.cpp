#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <stdlib.h>
#include <stdio.h>
#include <FEHMotor.h>
#include <FEHBattery.h>

FEHMotor left_motor(FEHMotor::Motor2, 9.0);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
DigitalEncoder right_encoder(FEHIO::P1_6);
DigitalEncoder left_encoder(FEHIO::P1_4);
AnalogInputPin CDS_Sensor(FEHIO::P1_7);

void move_forward(int percent, int inches) //using encoders
{
    int counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_left(int degree){
    //Reset Counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();


    int percent = 25;
    int counts = degree * 2.1000000;
    right_motor.SetPercent(-1*(percent));
    left_motor.SetPercent((percent));
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_right(int degree){
    //Reset Counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    int percent = 25;
    int counts = degree * 2.1000000;
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-1* (percent));
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
} 

  void check_light()
  { 
  //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();


  if(Color > 0.9 && Color < 2.0) //Light is blue
  {
  //Go to Blue
  LCD.WriteLine("BLUE");
  turn_left(90);
  move_forward(25, 2.5);
  turn_right(90);
  move_forward(25, 7.5);

  //Sleep for 2 Seconds
  Sleep(2.0);

  //Go Back to Starting Position
  move_forward(-25, 6.5);
  turn_left(90);
  move_forward(-25, 2.5);
  turn_right(90);
  }

  if(Color < 0.9) //Light is red
  {
  //Go to Red
  LCD.WriteLine("RED");
  turn_right(90);
  move_forward(25, 2.5);
  turn_left(90);
  move_forward(25, 7.5);

  //Sleep for 2 Seconds
  Sleep(2.0);

  //Go Back to Starting Position
  move_forward(-25, 6.5);
  turn_right(90);
  move_forward(-25, 2.5);
  turn_left(90);
  }
}

int main (void) 
{
  //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();

  while(Color > 2.0)
  {
    Color = CDS_Sensor.Value();
  }

  //Drive Up Ramp
  turn_right(65);
  move_forward(25, 8); //see function
  turn_left(22);
  move_forward(45, 33); //see function
  Sleep(2.0); //Wait for counts to stabilize
  turn_left(90);
  move_forward(25, 18);

  //Press Humidifer Button
  check_light();

  //Drive backwards and then down ramp all the way to the button
  move_forward(-25, 14.5);
  turn_left(90);
  move_forward(35, 42);
}
