#include <FEH.H>
#include <Arduino.h>

FEHMotor left_motor(FEHMotor::Motor1, 9.0);
FEHMotor right_motor(FEHMotor::Motor2, 9.0);
DigitalEncoder right_encoder(FEHIO::Pin11);
DigitalEncoder left_encoder(FEHIO::Pin8);
AnalogInputPin CDS_Sensor(FEHIO::Pin14);
FEHServo Arm_Servo(FEHServo::Servo0);
FEHServo Com_Servo(FEHServo::Servo1);
DigitalInputPin right_bump(FEHIO::Pin1);
DigitalInputPin left_bump(FEHIO::Pin4);


// PID Constants (To be tuned)
float Kp = 0.385;  // Proportional gain
float Ki = 0.0025;  // Integral gain   0.01
float Kd = 0.150;  // Derivative gain  0.2

// Conversion factor for encoder counts to inches
const float COUNTS_PER_INCH = 33.7408479355;  // ≈ 33.76

// Function to limit power within a given range
float LimitPower(float power, float maxPower) 
{
    if (power > maxPower) return maxPower;
    if (power < -maxPower) return -maxPower;
    return power;
}



//PID control for individual motor
float PID_Control(float targetCounts, int currentCounts, float lastError, float &integral, float maxPower, float &derivative) {
    float error = targetCounts - currentCounts;
    integral += error;
    derivative = error - lastError;

    // Compute PID power
    float power = (Kp * error) + (Ki * integral) + (Kd * derivative);

    //Calculate actual power
    float actual_power;
    actual_power = (11.5/Battery.Voltage())*power ;
    power = LimitPower(actual_power, maxPower); // Limit power

    //LCD.WriteLine("Integral Error: ");
    //LCD.WriteLine(integral);
    //LCD.WriteLine("Derivative Error:");
    //LCD.WriteLine(derivative);
    //LCD.WriteLine("Proportional Error:");
    //LCD.WriteLine(error);

    return power;
}


// PID driving function (each motor independently controlled)
void PID_Drive(float maxPower, float targetDistance) 
{
  float leftError = 0, rightError = 0, leftIntegral = 0, rightIntegral = 0;
  float leftDerivative = 0, rightDerivative = 0;
  int leftCounts = 0, rightCounts = 0;
  float targetCounts = targetDistance * COUNTS_PER_INCH;

  int leftOscillationCount = 0, rightOscillationCount = 0;
  int oscillationThreshold = 5; // Number of consecutive sign changes to detect oscillation

  //If the robot hits a wall
  int CollisionCount = 0;
  int CollisionThreshold = 50;

  // Reset encoders
  left_encoder.ResetCounts();
  right_encoder.ResetCounts();

  while ((leftCounts + rightCounts) / 2.0 < targetCounts) 
  {
      leftCounts = left_encoder.Counts();
      rightCounts = right_encoder.Counts();

      float currentLeftError = targetCounts - leftCounts;
      float currentRightError = targetCounts - rightCounts;

      // Check for oscillations in left motor
      if (leftError * currentLeftError < 0) 
      { // Sign change detected
          leftOscillationCount++;
      } 
      else 
      {
          leftOscillationCount = 0; // Reset if no sign change
      }

      // Check for oscillations in right motor
      if (rightError * currentRightError < 0) 
      { // Sign change detected
          rightOscillationCount++;
      } 
      else 
      {
          rightOscillationCount = 0; // Reset if no sign change
      }

      // If oscillation threshold is exceeded, stop the motors
      if (leftOscillationCount >= oscillationThreshold || rightOscillationCount >= oscillationThreshold) 
      {
          left_motor.Stop();
          right_motor.Stop();
          LCD.WriteLine("Oscillation detected. Motors stopped.");
          Buzzer.Beep();
          return; // Exit the function
      }


      // Check for collsion in both motors
      if (rightError == currentRightError && leftError == currentLeftError)
      { // Sign change detected
          CollisionCount++;
      } 
      else 
      {
          CollisionCount = 0; // Reset if no sign change
      }

      // If collision threshold is exceeded, stop the motors
      if (CollisionCount >= CollisionThreshold) 
      {
          left_motor.Stop();
          right_motor.Stop();
          LCD.WriteLine("Collision detected. Motors stopped.");
          return; // Exit the function
      }

      float leftPower = PID_Control(targetCounts, leftCounts, leftError, leftIntegral, maxPower, leftDerivative);
      float rightPower = PID_Control(targetCounts, rightCounts, rightError, rightIntegral, maxPower, rightDerivative);

      left_motor.SetPercent(-leftPower);
      right_motor.SetPercent(-rightPower);

      // Update last errors
      leftError = currentLeftError;
      rightError = currentRightError;

      Sleep(10);  // Small delay for stability
  }

  // Stop motors
  left_motor.Stop();
  right_motor.Stop();
}

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
    int quit = 0, checks = 0;
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts){
      
      /*should quit out if stuck, not yet tested
      checks++;
      int current_count = left_encoder.Counts();
      int old_encoder_count = 0;
      if (checks%21 == 0){
      int old_encoder_count = left_encoder.Counts();
      }
      if (checks%43 == 0){
        if (old_encoder_count == current_count){
          return;
        }
      }*/
    };

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_left(int degree){
    //Reset Counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();


    int percent = 40;
    int counts = degree * 1.92857142857;
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

    int percent = 40;
    int counts = degree * 1.92857142857;
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

void window()
{
  /*  
  //When the voltage changes, call “move forward” function for 17 inches 
    move_forward(-25, 13);

    Sleep(2.0);
    
    //Call the “turn” function at 58 degrees to turn so that robot is parallel to window 
    turn_left(30);
  
    Sleep(2.0);
  
   
    move_forward(25, 4);
    Sleep(2.0);
  
    turn_left(15);
    move_forward(25, 1.5);
    Sleep(2.0);

    turn_right(20);
    move_forward(-25,1.5);
    Sleep(2.0);
    */
    int inches = 4;
    int percent = 35;
    //Call the “move forward” function for 5 inches. 
    int counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent-15);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(2.0);
  
    //Call “move5 forward” for backwards 5 inches. 

    percent = -25; 

    counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent + 15);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

}

void move_arm(int start, int degree){

  while (start < degree){
    Arm_Servo.SetDegree(start);
    start += 1;
    Sleep(0.005);
  }
  while (start > degree){
    Arm_Servo.SetDegree(start);
    start -= 1;
    Sleep(0.005);
  }
}

void appleBucketPickup(){

  Arm_Servo.SetDegree(142);
  Sleep(0.5);
  move_forward(35,6);
  Arm_Servo.SetDegree(143);
  move_forward(35,0.5);

  Arm_Servo.SetDegree(144);
  move_forward(35,0.5);

  move_arm(137, 37);

}

void placeAppleBucket(){

  move_arm(47, 115);
  
  move_forward(-25,2.5);
  move_forward(35,2);

  Arm_Servo.SetDegree(124);
  Sleep(0.5);
  
  move_forward(-35,6);
  Arm_Servo.SetDegree(126);

  move_forward(35,5);
  move_forward(-35, 5);
}

void move_to_lever (int lever){

  if (lever==0){
    turn_left(10);
    move_forward(25, 1.5);
  } 
  if (lever==1){
    turn_right(1);
    move_forward(25, 0.5);
  }
  if (lever==2){
    turn_right(22);
    move_forward(25, 1.5);
  }
}

void flip_lever(){ 
  move_arm(120, 80);
  move_forward(25, 6);
  Arm_Servo.SetDegree(152);
  Sleep(1.0);
  move_forward(-25, 4);
  Arm_Servo.SetDegree(170);
  Sleep(4.0);
  move_forward(25, 5);
  Arm_Servo.SetDegree(150);
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

void ERCMain()
{

  RCS.InitializeTouchMenu("1240E5WEU"); // Run Menu to select Region (e.g., A, B, C, D)

  Arm_Servo.SetMin(600);
  Arm_Servo.SetMax(2400);
    //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();

  while(Color > 2.0)
    {
    Color = CDS_Sensor.Value();
  }

  move_forward(-45, 1.5);
  move_forward(45, 1.5);
  
//go to apple bucket line
  PID_Drive(45,18.25);
  turn_left(49.5);

  //pick up apple bucket
  appleBucketPickup();

  //drive to front of ramp & line up
  PID_Drive(-45,10);
  turn_right(95);
  move_forward(-45, 3);
  turn_right(95);
  PID_Drive(45,11);
  turn_left(80);

  //go up ramp and to table
  PID_Drive(55,23);
  //turn_right(15);
  PID_Drive(45,13);
  turn_right(15);
/*
  //put the apple bucket down
  placeAppleBucket();
/*
  //run to middle from table
  turn_left(64);
  PID_Drive(25, 14.5);

  int lever = RCS.GetLever(); // Get a 0, 1, or 2 indicating which lever to pull
  LCD.WriteLine(lever);
  lever = 2;

  move_to_lever(lever);
  flip_lever();
  
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
  
  //move to window & open it
  window();
  
  //Call “move forward” to go backwards until optosensors sense a black line. If they do not after 12 inches, go to step 35 
  
  //Call “Turn Compost” 
  //turn_compost();
  
  //Call “move forward” to drive forward until bumper switches sense that the robot has hit the button (or the robot has traveled more than 22 inches) 
  move_forward(35, 10);
  move_forward(45, 5);
*/
}
