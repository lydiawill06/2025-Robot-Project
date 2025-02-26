#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <stdlib.h>
#include <stdio.h>


//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor2,9.0);

void move_forward(int percent, int counts) //using encoders
{
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





int main(void)
{
    int motor_percent = 25; //Input power level here
    int expected_counts =  33.7408479355*30; //Input theoretical counts here

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Checkpoint 01");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed


    //Drive Across Lower Level
    move_forward(motor_percent, expected_counts); //see function
    Sleep(2.0); //Wait for counts to stabilize
    //Print out data
    LCD.Write("Theoretical Counts: ");
    LCD.WriteLine(expected_counts);
    LCD.Write("Motor Percent: ");
    LCD.WriteLine(motor_percent);
    LCD.Write("Actual LE Counts: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("Actual RE Counts: ");
    LCD.WriteLine(right_encoder.Counts());

    //Wait for Touch
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed



     //Drive Up Ramp

     //Set motor Percent and Expected Counts
     motor_percent = 60;
     expected_counts = 33.7408479355*20;

     move_forward(motor_percent, expected_counts); //see function
     Sleep(2.0); //Wait for counts to stabilize
     //Print out data
     LCD.Write("Theoretical Counts: ");
     LCD.WriteLine(expected_counts);
     LCD.Write("Motor Percent: ");
     LCD.WriteLine(motor_percent);
     LCD.Write("Actual LE Counts: ");
     LCD.WriteLine(left_encoder.Counts());
     LCD.Write("Actual RE Counts: ");
     LCD.WriteLine(right_encoder.Counts());


    //Drive Backwards down ramp

     //Set motor Percent and Expected Counts
     motor_percent = -20;
     expected_counts = 33.7408479355*25;

     move_forward(motor_percent, expected_counts); //see function
     Sleep(2.0); //Wait for counts to stabilize
     //Print out data
     LCD.Write("Theoretical Counts: ");
     LCD.WriteLine(expected_counts);
     LCD.Write("Motor Percent: ");
     LCD.WriteLine(motor_percent);
     LCD.Write("Actual LE Counts: ");
     LCD.WriteLine(left_encoder.Counts());
     LCD.Write("Actual RE Counts: ");
     LCD.WriteLine(right_encoder.Counts());




    return 0;
}
