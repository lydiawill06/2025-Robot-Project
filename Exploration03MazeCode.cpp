#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <math.h>

// Can change the following if one motor is slower than another.
#define LEFT_MOTOR_CORRECTION_FACTOR 1
#define RIGHT_MOTOR_CORRECTION_FACTOR 1

// Set a motor percentage of no more than 50% power
#define MOTOR_PERCENTAGE 25
#define ONE_DEGREE_COUNTS 2.47333333

FEHMotor left_motor(FEHMotor::Motor0, 9.0);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);

DigitalInputPin distanceSensor(FEHIO::P3_0); // Distance sensor should be plugged into Port 0 in Bank 3

void driveUntilSensorDetected()
{
    // Drive backwards until an object is detected
    left_motor.SetPercent(-1 * LEFT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);
    right_motor.SetPercent(-1 * RIGHT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);

    /* TODO: Drive until a wall is detected */

    left_motor.Stop();
    right_motor.Stop();
}

void turn(float degree)
{
    int counts = ONE_DEGREE_COUNTS * degree;
    if (degree<0){
        //turn right
        counts = counts * -1;
        right_motor.SetPercent((RIGHT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE));
        left_motor.SetPercent(-1 * (LEFT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE));
        while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    } 
    
    if (degree>0){
        //turn left
        right_motor.SetPercent((RIGHT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE));
        left_motor.SetPercent(-1 * (LEFT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE));
        while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    }
}

void drive(float distance)
{
    /* TODO: drive <code>distance</code> using shaft encoding or timing */
    float counts = 40.48901752269*distance;

     //Reset encoder counts
     right_encoder.ResetCounts();
     left_encoder.ResetCounts();
 
     //Set both motors to desired percent
     right_motor.SetPercent(RIGHT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);
     left_motor.SetPercent(LEFT_MOTOR_CORRECTION_FACTOR * MOTOR_PERCENTAGE);
 
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
    drive(1);
    turn(-90);

    /* TODO: Complete the rest of the maze navigation */
    driveUntilSensorDetected(); 
    drive(1);
    turn(90);

    driveUntilSensorDetected(); 
}

