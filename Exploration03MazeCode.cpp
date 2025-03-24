#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <math.h>
#include <FEHBattery.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR -1
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE (11.5/Battery.Voltage()) * 25;
#define ONE_DEGREE_COUNTS 2.43

FEHMotor left_motor(FEHMotor::Motor1, 9.0);
FEHMotor right_motor(FEHMotor::Motor2, 9.0);
DigitalEncoder right_encoder(FEHIO::P3_0);
DigitalEncoder left_encoder(FEHIO::P3_1);

DigitalInputPin distanceSensor(FEHIO::P3_7); // Distance sensor should be plugged into Port 0 in Bank 3

void driveUntilSensorDetected()
{
    // Drive backwards until an object is detected
    while (distanceSensor.Value() != 0){
    left_motor.SetPercent(1 * LEFT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25);
    right_motor.SetPercent(-1 * RIGHT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25);
    }
    /* TODO: Drive until a wall is detected */

    left_motor.Stop();
    right_motor.Stop();
}

void turn_left(int degree){
  //Reset Counts
  right_encoder.ResetCounts();
  left_encoder.ResetCounts();
  
  int counts = ONE_DEGREE_COUNTS * degree;

  right_motor.SetPercent(-1 * (RIGHT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25));
  left_motor.SetPercent(-1 * (LEFT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25));
  while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
  

  //Turn off motors
  right_motor.Stop();
  left_motor.Stop();
}

void turn_right(int degree){
  //Reset Counts
  right_encoder.ResetCounts();
  left_encoder.ResetCounts();

  int counts = ONE_DEGREE_COUNTS * degree;

  right_motor.SetPercent((RIGHT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25));
  left_motor.SetPercent(1 * (LEFT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25));
  while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

  //Turn off motors
  right_motor.Stop();
  left_motor.Stop();
} 


void drive(float distance)
{
    /* TODO: drive <code>distance</code> using shaft encoding or timing */
    float counts = 40.48901752269*distance;

     //Reset encoder counts
     right_encoder.ResetCounts();
     left_encoder.ResetCounts();
 
     //Set both motors to desired percent
    left_motor.SetPercent(-1 * LEFT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25);
    right_motor.SetPercent(1 * RIGHT_MOTOR_CORRECTION_FACTOR * (11.5/Battery.Voltage()) * 25);
 
     //While the average of the left and right encoder is less than counts,
     //keep running motors
     while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
 
     //Turn off motors
     right_motor.Stop();
     left_motor.Stop();
}

int main(void)
{
    // Drive until the distance sensor senses a wall
    driveUntilSensorDetected(); 

    // Move forward a little bit for the Crayola bot to have some space to turn
    drive(3);
    turn_left(90);


    /* TODO: Complete the rest of the maze navigation */
    driveUntilSensorDetected(); 
    drive(3);
    turn_right(100);

    driveUntilSensorDetected(); 
}
