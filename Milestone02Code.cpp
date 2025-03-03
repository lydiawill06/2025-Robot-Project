#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

FEHMotor left_motor(FEHMotor::Motor1, 9.0);
FEHMotor right_motor(FEHMotor::Motor2, 9.0);
DigitalEncoder right_encoder(FEHIO::P1_5);
DigitalEncoder left_encoder(FEHIO::P1_4);

void move_forward(int inches) //using encoders
{
    int percent = 25;
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
    int counts = degree * 2.1000000;
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
    int counts = degree * 2.1000000;
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-1* (percent));
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
} 

void check_light(){
  //ADD: read line color

  //left line
  turn_left(90);
  move_forward(2);
  turn_right(90);
  move_forward(7);

  while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
  while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
  
  turn_right(90);
  move_forward(2);
  turn_left(90);
  move_forward(7);
  
}

int main (void) {
  float x, y;
  while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
  while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
  check_light();
}
