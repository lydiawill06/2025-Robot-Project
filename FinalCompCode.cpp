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
AnalogInputPin CDS_Sensor(FEHIO::P2_7);

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
    int counts = degree * 2.08;
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
    int counts = degree * 2.08;
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

  while(Color > 2.0)
  {
    Color = CDS_Sensor.Value();
    LCD.WriteLine(CDS_Sensor.Value());
    move_forward(25, 1);
  }


  if(Color > 1.1 && Color < 2.0) //Light is blue
  {
  //Go to Blue
  LCD.WriteLine("BLUE");
  turn_left(90);
  move_forward(25, 2.5);
  turn_right(90);
  move_forward(25, 5);

  //Sleep for 2 Seconds
  Sleep(2.0);

  //Go Back to Starting Position
  move_forward(-25, 5);
  turn_left(90);
  move_forward(-25, 2.5);
  turn_right(90);
  }

  if(Color < 1.1) //Light is red
  {
  //Go to Red
  LCD.WriteLine("RED");
  turn_right(90);
  move_forward(25, 2.5);
  turn_left(90);
  move_forward(25, 5);

  //Sleep for 2 Seconds
  Sleep(2.0);

  //Go Back to Starting Position
  move_forward(-25, 5);
  turn_right(90);
  move_forward(-25, 2.5);
  turn_left(90);
  }
}

/*
Line_Follow(in percentage) 

  Drive straight if only middle optosensor is over the line 

  *Steal optosensor code from exploration 02 

  Follow line until optosensors read that it has reached the end of the line (all 3 optosensors off the line) 

Check_Line() 

  If right and left optosensors values show that they are not on the line, and the middle optosensor value shows that it is, do nothing 

  If right and middle optosensors values show that they are not on the line, and the left optosensor value shows that it is, call Turn() to turn slightly left 

  If left and middle optosensors values show that they are not on the line, and the right optosensor value shows that it is, call Turn() to turn slightly right 

Arm_Move(inches) 

  Convert input (height from floor in inches) to degrees for servo motor 

Apple_Bucket_Pickup 

  Hard – coded to pick up apple bucket with arm 

  Move arm to 4.75 inches 

  Move forward 1-3 inches 

  Move arm to 8 inches 

Place_Apple_Bucket 

  Servo arm moves down a hard-coded value. Robot then backs up. 

  Move arm to 7 inches 

  Move backward 

Move_to_Lever 

  Will find the correct lever that needs to be flipped and drive to it 

Flip_Lever 

  Arm pushes lever down (down to 3 inches/whatever will push the lever & not break it) 

  Calls “move forward” to back up (w/ negative percentage) 

  Arm moves down (down to 0 inches) 

  Calls “move forward” to go back to lever (w/ positive percentage) 

  Arm pushes lever back up (whatever value will push the lever up & not break it) 

Turn_Compost 

  Rotate servo motor on the back 180 degrees. 

  Call “move forward” to go back 3 inches. 

  Rotate servo motor on the back -180 degrees. 

  Call “move forward” to go forward 3 inches. 

  Rotate servo motor on the back 180 degrees. 

  Rotate servo motor on the back -180 degrees. 

  Call “move forward” to go back 3 inches. 

  Rotate Servo motor on the back 180 degrees 

  Call “Move forward” to go forward 3 inches. 

  Rotate Servo motor on the back -180 degrees 
*/

int main (void)
{

  //While the cds cell’s voltage does not show that the light has lit up, do not move 
   //Read line color
   float Color;
  
   //Find value of CDS cell to determine color
   Color = CDS_Sensor.Value();
 
   while(Color > 2.0)
   {
     Color = CDS_Sensor.Value();
   }

  //When the voltage changes, call “move forward” function for 17 inches 
  move_forward(25, 17);
  
  //Call the “turn” function at 58 degrees to turn so that robot is parallel to window 
  turn_left(58);
  
  //Call the “move forward” function for 5 inches. 
  move_forward(25, 5);
  
  //Call “move5 forward” for backwards 5 inches. 
  move_forward(-25, 5);
  
  //Call “turn” and turn 50 degrees 
  turn_left(50);
  
  //“move forward” 18 inches (in case of error) 
  move_forward(25, 18);
  
  //If optosensors sense a black line (should happen around 14 inches), call “line follow” function. 
  
  //Call “Apple Bucket Pickup” function 
  //apple_bucket_pickup();
  
  //Call “turn” and turn 130 Degrees 
  turn_left(130);
  
  //Call “move forward” for 24 inches 
  move_forward(25,24);
  
  //Call “turn” to turn 84 degrees so that front of robot faces ramp 
  turn_left(84);
  
  //Call “move forward” for 31 inches. This time, increase the power so that the robot can go up the ramp. Decrease power after robot finishes going up the ramp 
  move_forward(45, 31);
  //Call “Place Apple Bucket” 
  //place_apple_bucket();
  //Call “turn” and turn 78 degrees 
  turn_left(78);
  
  //Call “move forward” for about 22 inches 
  move_forward(25, 22);
  
  //Call “move to lever” 
  //move_to_lever();
  
  //Call “Flip lever” 
  //flip_lever();
  
  //Call “turn” and turn –37 degrees 
  turn_right(37);
  
  //Call “move forward” to move backward 16 inches 
  move_forward(-25, 16);
  
  //Call “turn” and turn 54 degrees 
  turn_left(54);
  
  //Call “check line” 
  //check_line();
  
  //Call “check light” 
  check_light();
  
  //Call “turn” to turn -27 degrees clockwise 
  turn_right(27);
  
  //Call “move forward” to drive backwards 25 inches 
  move_forward(-25,25);
  
  //“turn” -63 degrees 
  turn_right(63);
  
  //“move forward” to go backwards 33 inches. 
  move_forward(-25, 33);
  
  //“Turn” -82 degrees clockwise 
  turn_right(82);
  
  //Call “move forward” to go backwards until optosensors sense a black line. If they do not after 12 inches, go to step 35 
  
  //Call “Turn Compost” 
  //turn_compost();
  
  //Call “move forward” to drive forward until bumper switches sense that the robot has hit the button (or the robot has traveled more than 22 inches) 
  move_forward(35, 22);
  move_forward(45, 2);

}
