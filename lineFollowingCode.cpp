#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor1,9.0);
DigitalInputPin frontRight(FEHIO::P0_0);
DigitalInputPin backRight(FEHIO::P0_1);
DigitalInputPin frontLeft(FEHIO::P3_7);
DigitalInputPin backLeft(FEHIO::P3_6);
AnalogInputPin right_opto(FEHIO::P2_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);

#define leftOffLow 0
#define leftOffHigh 1
#define rightOffLow 0
#define rightOffHigh 1

//drive for 1 function

void move_forward() //using encoders
{
    int percent = 0.25;
    int counts = 40;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

int pressed() {
    if (frontRight.Value() == 0 || frontLeft.Value() == 0) {
        return 1;
    } else if (backRight.Value() == 0 || backLeft.Value() == 0) {
        return 1;
    } else {
        return 0;
    }
}

void turn_left(){
    right_motor.SetPercent(-50);
    left_motor.SetPercent(50);
}

void turn_right(){
    right_motor.SetPercent(50);
    left_motor.SetPercent(-50);
} 

void check_line(){
    while((leftOffLow <= left_opto.Value()) && (left_opto.Value() <= leftOffHigh)) {
        turn_left();
    }
    while((rightOffLow <= right_opto.Value()) && (right_opto.Value() <= rightOffHigh)) {
        turn_right();
    }
}

int main (void) {
    while(pressed() == 1){
    check_line();
    move_forward();
    }
}
