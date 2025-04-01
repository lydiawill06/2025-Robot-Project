#include <FEH.h>
#include <Arduino.h>

FEHMotor left_motor(FEHMotor::Motor1, 9.0);
FEHMotor right_motor(FEHMotor::Motor2, 9.0);
DigitalEncoder right_encoder(FEHIO::Pin11);
DigitalEncoder left_encoder(FEHIO::Pin8);
AnalogInputPin CDS_Sensor(FEHIO::Pin14);
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



void ERCMain()
{
  Sleep(2.0);
  PID_Drive(45, 10);
  Sleep(4.0);
  PID_Drive(45, 8);
  Sleep(4.0);
  PID_Drive(40, 6);
  Sleep(4.0);
  PID_Drive(30, 4);
  Sleep(4.0);
  PID_Drive(30, 2);
}
