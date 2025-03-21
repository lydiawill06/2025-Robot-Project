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
AnalogInputPin CDS_Sensor(FEHIO::P2_1);

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
void PID_Drive(float targetDistance, float maxPower) 
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

    //Calculate actual power
    float actual_percent;
    actual_percent = (11.5/Battery.Voltage())*percent ;

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-actual_percent);
    left_motor.SetPercent(-actual_percent);

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
    //Calculate actual power
    float actual_percent;
    actual_percent = (11.5/Battery.Voltage())*percent ;

    int counts = degree * 2.08;
    right_motor.SetPercent(-1*(actual_percent));
    left_motor.SetPercent((actual_percent));
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
    //Calculate actual power
    float actual_percent;
    actual_percent = (11.5/Battery.Voltage())*percent ;

    int counts = degree * 2.08;
    right_motor.SetPercent(actual_percent);
    left_motor.SetPercent(-1* (actual_percent));
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
    int inches = 5.5;
    int percent = 30;
    //Call the “move forward” function for 5 inches. 
    int counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Calculate actual power
    float actual_percent;
    actual_percent = (11.5/Battery.Voltage())*percent ;

    //Set both motors to desired percent
    right_motor.SetPercent(-actual_percent);
    left_motor.SetPercent(-actual_percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < (counts*(2/3)));
    actual_percent = actual_percent+2;
    right_motor.SetPercent(-actual_percent);
    left_motor.SetPercent(-actual_percent-(actual_percent));

    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    actual_percent = actual_percent-4;
    right_motor.SetPercent(-actual_percent);
    left_motor.SetPercent(-actual_percent-5);


    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(2.0);

    //turn_left(5);
  
    //Call “move5 forward” for backwards 5 inches. 
    inches = 6;

    percent = 30; 
    //Calculate actual power
    actual_percent = (11.5/Battery.Voltage())*percent ;

    counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(actual_percent);
    left_motor.SetPercent(actual_percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < (counts*(2/3)));
    actual_percent = actual_percent+2;
    right_motor.SetPercent(actual_percent);
    left_motor.SetPercent(actual_percent+(actual_percent));

    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    actual_percent = actual_percent-4;
    right_motor.SetPercent(actual_percent);
    left_motor.SetPercent(actual_percent+5);

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

//Drive Up Ramp
/*turn_right(65);
PID_Drive(8, 40); //see function
turn_left(24);
PID_Drive(26, 50); //see function
Sleep(2.0); //Wait for counts to stabilize
turn_left(86);*/

//Drive Up Ramp
turn_right(65);
move_forward(25, 8); //see function
turn_left(24);
move_forward(45, 27.5); //see function
Sleep(2.0); //Wait for counts to stabilize
turn_left(83);
move_forward(25, 13); //see function
Sleep(2.0);

turn_left(30);
 move_forward(25,2);
 Sleep(2.0);
 turn_right(35);
 move_forward(-25,2);
 Sleep(2.0);

window();

}
