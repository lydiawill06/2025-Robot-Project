#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <stdlib.h>
#include <stdio.h>
#include <FEHMotor.h>
#include <FEHBattery.h>
#include <FEHServo.h>

FEHMotor left_motor(FEHMotor::Motor2, 9.0);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
DigitalEncoder right_encoder(FEHIO::P1_6);
DigitalEncoder left_encoder(FEHIO::P1_4);
AnalogInputPin CDS_Sensor(FEHIO::P2_7);
FEHServo Arm_Servo(FEHServo::Servo0);

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
    power = LimitPower(power, maxPower); // Limit power

    LCD.WriteLine("Integral Error: ");
    LCD.WriteLine(integral);
    LCD.WriteLine("Derivative Error:");
    LCD.WriteLine(derivative);
    LCD.WriteLine("Proportional Error:");
    LCD.WriteLine(error);

    return power;
}

// PID driving function (each motor independently controlled)
void PID_Drive(float maxPower, float targetDistance) 
{
    float leftError = 0, rightError = 0, leftIntegral = 0, rightIntegral = 0;
    float leftDerivative = 0, rightDerivative = 0;
    int leftCounts = 0, rightCounts = 0;
    float targetCounts = targetDistance * COUNTS_PER_INCH;

    // Reset encoders
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();

    while ((leftCounts + rightCounts) / 2.0 < targetCounts) 
    {
        leftCounts = left_encoder.Counts();
        rightCounts = right_encoder.Counts();

        float leftPower = PID_Control(targetCounts, leftCounts, leftError, leftIntegral, maxPower, leftDerivative);
        float rightPower = PID_Control(targetCounts, rightCounts, rightError, rightIntegral, maxPower, rightDerivative);

        left_motor.SetPercent(-leftPower);
        right_motor.SetPercent(-rightPower);

        // Update last error outside of PID function
        leftError = targetCounts - leftCounts;
        rightError = targetCounts - rightCounts;

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
    int quit = 0;
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts){
      //int current_count = left_encoder.Counts();
      //int old_encoder_count = left_encoder.Counts();
    };

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
    Arm_Servo.SetMin(863);
    Arm_Servo.SetMax(2410);
PID_Drive(25,17);
turn_left(47);
Arm_Servo.SetDegree(145);
Sleep(1.0);
move_forward(25,6);
Sleep(0.5);
Arm_Servo.SetDegree(146);
move_forward(25,0.5);
Sleep(0.5);

Arm_Servo.SetDegree(147);
move_forward(25,0.5);
Sleep(0.5);

Arm_Servo.SetDegree(120);
Sleep(0.5);
Arm_Servo.SetDegree(110);
Sleep(0.5);
Arm_Servo.SetDegree(100);
Sleep(0.5);

Arm_Servo.SetDegree(90);
Sleep(0.5);
Arm_Servo.SetDegree(80);
Sleep(0.5);
Arm_Servo.SetDegree(70);
Sleep(0.5);
Arm_Servo.SetDegree(60);
Sleep(0.5);
Arm_Servo.SetDegree(50);
Sleep(0.5);
Arm_Servo.SetDegree(40);
Sleep(0.5);

PID_Drive(-25,10);
turn_right(90);
move_forward(-25, 3);
turn_right(90);
PID_Drive(25,12.5);
turn_left(100);
PID_Drive(55,25);
//turn_left(10);
PID_Drive(25,12);
turn_right(10);

Arm_Servo.SetDegree(50);
Sleep(0.5);
Arm_Servo.SetDegree(60);
Sleep(0.5);
Arm_Servo.SetDegree(70);
Sleep(0.5);
Arm_Servo.SetDegree(80);
Sleep(0.5);
Arm_Servo.SetDegree(90);
Sleep(0.5);
Arm_Servo.SetDegree(100);
Sleep(0.5);
Arm_Servo.SetDegree(110);
Sleep(0.5);
move_forward(-25,2.5);
move_forward(25,1);
Arm_Servo.SetDegree(120);
Sleep(0.5);

move_forward(-25,6);
Arm_Servo.SetDegree(130);
move_forward(25,5);
move_forward(-25, 5);
  
 
    

    

    
    
}
