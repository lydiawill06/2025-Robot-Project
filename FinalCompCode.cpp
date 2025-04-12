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

// PID Constants (To be tuned)
float Kp = 0.385;  // Proportional gain
float Ki = 0.0025;  // Integral gain   0.01
float Kd = 0.150;  // Derivative gain  0.2

// Conversion factor for encoder counts to inches
const float COUNTS_PER_INCH = 33.7408479355;  // ≈ 33.76
const float COUNTS_PER_DEGREE = 2.28;

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

/*
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
          */

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

// PID_Turn function: Using separate PID variables for each motor.
void PID_Turn(float maxPower, float targetDegrees) 
{
    // Define separate PID variables for left and right motors
    float leftError = 0, rightError = 0;
    float leftIntegral = 0, rightIntegral = 0;
    float leftDerivative = 0, rightDerivative = 0;
    int leftCounts = 0, rightCounts = 0;

    // Convert desired turn (in degrees) to target encoder counts.
    // Use fabs() to ignore the sign when converting degrees to counts.
    float targetCounts = fabs(targetDegrees) * COUNTS_PER_DEGREE;

    // Determine turn direction:
    // For a positive targetDegrees, a counterclockwise turn
    // For a negative targetDegrees, a clockwise turn
    int leftMotorDirection = (targetDegrees > 0) ? 1 : -1;
    int rightMotorDirection = (targetDegrees > 0) ? -1 : 1;

    // Reset encoders
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();

    while ( ( (abs(leftCounts) + abs(rightCounts)) / 2.0) < targetCounts) {
        // Get current encoder counts (using absolute values so that turn progress is measured correctly)
        leftCounts = abs(left_encoder.Counts());
        rightCounts = abs(right_encoder.Counts());

        // Compute PID power for each motor
        float leftPower = PID_Control(targetCounts, leftCounts, leftError, leftIntegral, maxPower, leftDerivative);
        float rightPower = PID_Control(targetCounts, rightCounts, rightError, rightIntegral, maxPower, rightDerivative);

        // Command motors with appropriate direction
        left_motor.SetPercent(leftMotorDirection * leftPower);
        right_motor.SetPercent(rightMotorDirection * rightPower);

        // Update errors for next iteration
        leftError = targetCounts - leftCounts;
        rightError = targetCounts - rightCounts;

        Sleep(10); // Small delay for stability
    }

    // Stop motors after the turn is complete
    left_motor.Stop();
    right_motor.Stop();
}

void move_forward(int percent, int inches) //using encoders
{
    int bump_switch_counter = 0;
    int counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts){
      if((right_bump.Value() == 0) && (left_bump.Value() == 0))
      {
        //Turn off motors
        right_motor.Stop();
        left_motor.Stop();
        return;
      }

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

void check_color()
  { 
    
  //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();
  int start = TimeNow();
  while(Color > 2.0) {
    Color = CDS_Sensor.Value();
    if((TimeNow()-start) > 29){
      break;
    }
  }

  /*while(TimeNow()-start<=30)
  {
    Color = CDS_Sensor.Value();
    LCD.WriteLine(Color);
    int color_counter = 0;
    if(Color < 2.0)
    {
      color_counter++;
    }
    if (color_counter>=10){
      return;
    }
    int check = TimeNow();
    if(((check-start)%5)==0){
      //LCD.Write(check-start);
    }
  }
  */  
}

void check_light()
  { 
  //Read line color
  float Color;
  
  //Find value of CDS cell to determine color
  Color = CDS_Sensor.Value();

  while(Color > noLight)
  {
    Color = CDS_Sensor.Value();
    LCD.WriteLine(CDS_Sensor.Value());
    move_forward(25, 1);
  }


  if(Color > redMax && Color < blueMax) //Light is blue
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

  if(Color < redMax) //Light is red
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
  PID_Drive(25, 3);
  PID_Turn(45, 78);
  move_forward(45, 12);

  right_encoder.ResetCounts();
  int inches = 12;
  int counts = inches * 33.7408479355;

  right_motor.SetPercent(35);
  while(right_encoder.Counts() < counts)
  {}

  right_motor.Stop();
  
  inches = 6;
  int percent = 35;
  //Call the “move forward” function for 5 inches. 
  counts = inches * 33.7408479355;
  //Reset encoder counts
  right_encoder.ResetCounts();
  left_encoder.ResetCounts();

  //Set both motors to desired percent
  right_motor.SetPercent(-percent);
  left_motor.SetPercent(-percent-13);

  //While the average of the left and right encoder is less than counts,
  //keep running motors
  while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

  //Turn off motors
  right_motor.Stop();
  left_motor.Stop();
  Sleep(2.0);

  move_forward(-25, 1.25);
  turn_right(7);

  //Call “move5 forward” for backwards 5 inches. 

  percent = 35; 

  inches = 7;
  counts = inches * 33.7408479355;
  //Reset encoder counts
  right_encoder.ResetCounts();
  left_encoder.ResetCounts();

  //Set both motors to desired percent
  right_motor.SetPercent(percent);
  left_motor.SetPercent(percent + 18);

  //While the average of the left and right encoder is less than counts,
  //keep running motors
  while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

  //Turn off motors
  right_motor.Stop();
  left_motor.Stop();



    //Move away from window
    move_forward(35, 3);

    right_encoder.ResetCounts();
    inches = 12;
    counts = inches * 33.7408479355;
  
    right_motor.SetPercent(-35);
    while(right_encoder.Counts() < counts)
    {
      if((right_bump.Value() == 0) && (left_bump.Value() == 0))
        {
          //Turn off motors
          right_motor.Stop();
          left_motor.Stop();
          return;
        }
    }
    right_motor.Stop();

  }




void move_arm(int current_degree, int final_degree){

  //Initialize Timeout Counter
  int Timeout_Counter = 0;

  while (current_degree < final_degree)
  {
    int last_degree = current_degree;

    Arm_Servo.SetDegree(current_degree);
    current_degree += 1;

    //If arm can't move anymore, stop trying to move to prevent burning out motor
    /*
    if (current_degree == last_degree)
    {
        Timeout_Counter += 1;
    }
    if (Timeout_Counter = 5)
    {
        return;
    }
        */
    Sleep(0.005);
  }
  while (current_degree > final_degree)
  {
    int last_degree = current_degree;

    Arm_Servo.SetDegree(current_degree);
    current_degree -= 1;
        
    //If arm can't move anymore, stop trying to move to prevent burning out motor
    /*
    if (current_degree == last_degree)
    {
        Timeout_Counter += 1;
    }
    if (Timeout_Counter = 5)
    {
        return;
    }
        */
    Sleep(0.005);
  }
}

void appleBucketPickup(){

  Arm_Servo.SetDegree(142);
  Sleep(0.5);
  move_forward(35,5.75);
  Arm_Servo.SetDegree(143);
  move_forward(35,0.5);

  Arm_Servo.SetDegree(144);
  move_forward(35,0.5);

  move_arm(137, 60);

}

void placeAppleBucket(){

  move_arm(50, 115);
  
  move_forward(-25,2.5);
  move_forward(35,2);

  Arm_Servo.SetDegree(123);
  Sleep(0.5);
  
  move_forward(-35,6);
  Arm_Servo.SetDegree(126);

  move_forward(35,5);
  move_forward(-35, 5);
}

void move_to_lever (int lever){

  if (lever==0){
    turn_left(20);
    move_forward(25, 1);
  } 
  if (lever==1){
    turn_right(1);
    move_forward(25, 0.5);
  }
  if (lever==2){
    turn_right(20);
    move_forward(25, 1);
  }
}

void flip_lever(int lever){ 
  int checks = 0;
  int turns = 0;
  move_arm(120, 80);
  move_forward(25, 4);
  Arm_Servo.SetDegree(152);
  Sleep(1.0);
  move_forward(-25, 4);
  /*
  while((checks<3) && (!RCS.isLeverFlipped())){
      LCD.WriteLine(RCS.isLeverFlipped());
    Sleep(1.0);
  if ((!RCS.isLeverFlipped())){
    if (lever==0){
      turn_left(2);
      turns++;
    }
    else if (lever==2){
      turn_right(2);
      turns++;
    }
    move_arm(120, 80);
    move_forward(25, 4);
    Arm_Servo.SetDegree(152);
    Sleep(1.0);
    move_forward(-25, 4);
  }
  checks++;
}
*/
  Arm_Servo.SetDegree(170);
  Sleep(4.0);
  move_forward(25, 5.7);
  Arm_Servo.SetDegree(143);
  move_forward(-25, 5);
  /*
  if (lever==0){
    turn_right(2*turns);
  }
  else if (lever==2){
    turn_left(2*turns);
  }
    */
}

void move_from_lever (int lever){

  if (lever==0){
    turn_right(17);
    move_forward(-25, 1);
  } 
  if (lever==1){
    turn_left(1);
    move_forward(-25, 0.5);
  }
  if (lever==2){
    turn_left(20);
    move_forward(-25, 1);
  }
}

//currently will move in a circle if the optosensor has crossed over the line
void check_line(int move){
  int inches = 0;
  while(inches<move){
    move_forward(45, 1);
  while((leftOffLow <= left_opto.Value()) && (left_opto.Value() < leftOffHigh) &&
  (middleOffLow <= middle_opto.Value()) && (middle_opto.Value() < middleOffHigh)) 
  {
      turn_left(1);
  }
  while((rightOffLow <= right_opto.Value()) && (right_opto.Value() < rightOffHigh) &&
  (middleOffLow <= middle_opto.Value()) && (middle_opto.Value() < middleOffHigh)) {
      turn_right(1);
  }
  LCD.Write("right: ");
  LCD.Write(right_opto.Value());\
  LCD.Write(" middle: ");
  LCD.Write(middle_opto.Value());
  LCD.Write("  left: ");
  LCD.WriteLine(left_opto.Value());
  Sleep(3.0);
  inches++;
  if(inches%5==0){
    LCD.Clear();
  }
}
}

//currently will move in a circle if the optosensor has crossed over the line
void check_line_lever(){


  while((left_opto.Value() > leftOnHighLever) && (middle_opto.Value() > middleOnHighLever)) 
  {
      turn_left(1);
      move_forward(45, 0.25);
  }
  while((right_opto.Value() > rightOnHighLever) && (middle_opto.Value() > middleOnHighLever)) {
      turn_right(1);
      move_forward(45, 0.25);

  }
  LCD.WriteLine(right_opto.Value());
  LCD.WriteLine(middle_opto.Value());
  LCD.WriteLine(left_opto.Value());

}



void turn_compost()
{

  //Turn Compost Motor
  compost_motor.SetPercent(55);
  //Turn Motor for specified time
  int Start_Time = TimeNow();
  while((TimeNow() - Start_Time) < 3.5)
  {}

  Sleep(0.5);

  //Turn Compost Motor Back
  compost_motor.SetPercent(-55);
  //Turn Motor for specified time
  Start_Time = TimeNow();
  while((TimeNow() - Start_Time) < 3.5)
  {}
}



void ERCMain()
{
  RCS.InitializeTouchMenu("1240E5WEU"); // Run Menu to select Region (e.g., A, B, C, D)
  LCD.Clear();

  check_color();

  LCD.Clear();
  Arm_Servo.SetMin(600);
  Arm_Servo.SetMax(2400);
    //Read line color
  float Color;
  //Wait for light to turn red
  check_color();

  Sleep(3.0);
  /*
  move_forward(-45, 1);
  move_forward(45, 1);
*/

//go to apple bucket line
  PID_Drive(45,18.15);
  turn_left(50.0);

  //pick up apple bucket
  appleBucketPickup();
LCD.WriteLine("Apple Bucket Yay!!!");
  //drive to front of ramp & line up
  PID_Drive(-45,10);
  PID_Turn(45, -90);
  PID_Drive(-45, 3);
  PID_Turn(45, -85);
  PID_Drive(45,10.25);
  PID_Turn(43, 86);

  //go up ramp
  move_arm(60, 140);
  PID_Drive(60,24);
  move_forward(30, 0.5);
  move_forward(20, 0.5);
  Sleep(1.0);
  move_arm(140, 50);
  Sleep(1.0);
  PID_Drive(45,5.5);

  //Align with wall
  PID_Turn(45, -90);
  move_forward(35,9);
  Sleep(0.25);
  PID_Drive(-35,3);
  PID_Turn(45, 90);

  //Drive to table
  PID_Drive(45, 8);
  turn_right(21);

  //put the apple bucket down
  placeAppleBucket();
  LCD.WriteLine("Apple Bucket Gone!!!");

  //Square up with wall
  turn_right(69);
  move_forward(35, 9);
  Sleep(0.25);
  PID_Drive(-45, 3);

  //Square up with table
  PID_Turn(45,90);
  move_forward(45, 9);
  Sleep(0.25);

 
  PID_Drive(-45, 9);

  //Drive to fertilizer levers
  turn_left(46);
  PID_Drive(45, 18.5);
  //check_line_lever();
  LCD.WriteLine("Going to Lever!!!");


  int lever = RCS.GetLever(); // Get a 0, 1, or 2 indicating which lever to pull
  LCD.WriteLine(lever);
  lever = 0;

  move_to_lever(lever);


  flip_lever(lever);

  LCD.WriteLine("Hit it!!!");

  move_from_lever(lever);

  move_arm(150, 40);

  PID_Drive(-45, 13);
  turn_left(37);
  move_forward(45, 10.5);
  

  //Call “check line” 
  //check_line(3);

  move_forward(45, 3.5);

  //Call “check light” 
  //check_light();
  LCD.WriteLine("Moving to Lights!!!");

  PID_Drive(45, 10);
  PID_Drive(-45, 6);
  
  //move to window & open it
  LCD.WriteLine("Windows!!!");

  window();

  //Drive away from window
  LCD.WriteLine("Leaving!!!");

  PID_Drive(-45, 5);
  PID_Turn(45, 90);
  
  //Drive to wall
  PID_Drive(45, 20);
  move_forward(45, 5);
  PID_Drive(-35, 3.25);

  LCD.Clear();

  //Drive to table
  PID_Turn(45, 90);
  PID_Drive(35, 10);
  LCD.WriteLine("Button!!!");

  PID_Drive(-40, 50);
}
