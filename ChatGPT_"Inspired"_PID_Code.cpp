#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <stdlib.h>
#include <stdio.h>
#include <FEHMotor.h>
#include <FEHBattery.h>

// Define motors and Encoders
FEHMotor leftMotor(FEHMotor::Motor2, 9.0);
FEHMotor rightMotor(FEHMotor::Motor1, 9.0);
DigitalEncoder right_encoder(FEHIO::P1_6);
DigitalEncoder left_encoder(FEHIO::P1_4);

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

// PID control for individual motor
float PID_Control(float targetCounts, int currentCounts, float lastError, float &integral, float maxPower, float &derivative) {
    float error = targetCounts - currentCounts;
    integral += error;
    derivative = error - lastError;

    // Compute PID power
    float power = (Kp * error) + (Ki * integral) + (Kd * derivative);
    power = LimitPower(power, maxPower); // Limit power

    LCD.WriteLine("Integral Error:");
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

        leftMotor.SetPercent(-leftPower);
        rightMotor.SetPercent(-rightPower);

        // Update last error outside of PID function
        leftError = targetCounts - leftCounts;
        rightError = targetCounts - rightCounts;

        Sleep(10);  // Small delay for stability
    }

    // Stop motors
    leftMotor.Stop();
    rightMotor.Stop();
}

// PID turning function (each motor independently controlled)
void PID_Turn(float targetDegrees, float maxPower) 
{
    float leftError = 0, rightError = 0, leftIntegral = 0, rightIntegral = 0;
    float leftDerivative = 0, rightDerivative = 0;
    int leftCounts = 0, rightCounts = 0;

    // Adjusted turning ratio
    const float COUNTS_PER_DEGREE = 1.7;  // Adjust based on testing
    float targetCounts = targetDegrees * COUNTS_PER_DEGREE;

    // Reset encoders
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();

    while ((abs(leftCounts) + abs(rightCounts)) / 2.0 < targetCounts) {
        leftCounts = abs(left_encoder.Counts());
        rightCounts = abs(right_encoder.Counts());

        float leftPower = PID_Control(targetCounts, leftCounts, leftError, leftIntegral, maxPower, leftDerivative);
        float rightPower = PID_Control(targetCounts, rightCounts, rightError, rightIntegral, maxPower, rightDerivative);

        // Determine turn direction
        if (targetDegrees > 0) // Turn right
        {
            leftMotor.SetPercent(-leftPower);
            rightMotor.SetPercent(rightPower);
        }
        else // Turn left
        {
            leftMotor.SetPercent(leftPower);
            rightMotor.SetPercent(-rightPower);
        }

        // Update last error outside of PID function
        leftError = targetCounts - leftCounts;
        rightError = targetCounts - rightCounts;

        Sleep(10);  // Small delay for stability
    }

    // Stop motors
    leftMotor.Stop();
    rightMotor.Stop();
}

// Main function
int main() 
{
    float actual_power = 11.5/Battery.Voltage();
    float maxPower = 55.0*actual_power;  // Max motor power (adjust as needed)
    float x, y; // For touch screen

    while (!LCD.Touch(&x, &y)); // Wait for screen to be pressed
    while (LCD.Touch(&x, &y)); // Wait for screen to be unpressed

    // Example: Drive forward 10 inches
    PID_Drive(17, maxPower);

    // Example: Turn 90 degrees to the right
   // PID_Turn(90.0, maxPower);

    Sleep(4.0);

    // Example: Drive backward 6 inches
    PID_Drive(10.0, maxPower);
    Sleep(4.0);


    PID_Drive(5.0, maxPower);
    Sleep(4.0);




    LCD.WriteLine("Left Encoder Counts:");
    LCD.WriteLine(left_encoder.Counts());
    LCD.WriteLine("Right Encoder Counts:");
    LCD.WriteLine(right_encoder.Counts());

}
