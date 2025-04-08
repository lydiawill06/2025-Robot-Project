
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
DigitalInputPin left_bump(FEHIO::Pin4);

AnalogInputPin right_opto(FEHIO::Pin2);
AnalogInputPin middle_opto(FEHIO::Pin3);
AnalogInputPin left_opto(FEHIO::Pin6);



#define leftOffLow 2.5
#define leftOffHigh 4
#define rightOffLow 2.5
#define rightOffHigh 4
#define middleOffLow 2.5
#define middleOffHigh 4



//drive for 1 function

void move_forward() //using encoders
{
    int percent = 25;
    int counts = 20;
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
/*int pressed() {
    if (frontRight.Value() == 0 || frontLeft.Value() == 0) {
        return 1;
    } else if (backRight.Value() == 0 || backLeft.Value() == 0) {
        return 1;
    } else {
        return 0;
    }
}*/

void turn_left(){
    int percent = 25;
    int counts = 10;
    right_motor.SetPercent((percent));
    left_motor.SetPercent(-1 * (percent));
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
}

void turn_right(){
    int percent = 25;
    int counts = 10;
    right_motor.SetPercent((-1* (percent)));
    left_motor.SetPercent(percent);
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
} 

//currently will move in a circle if the optosensor has crossed over the line
void check_line(){
    while((leftOffLow <= left_opto.Value()) && (left_opto.Value() < leftOffHigh) &&
    (middleOffLow <= middle_opto.Value()) && (middle_opto.Value() < middleOffHigh)) 
    {
        turn_left();
        LCD.Write("left");

    }
    while((rightOffLow <= right_opto.Value()) && (right_opto.Value() < rightOffHigh) &&
    (middleOffLow <= middle_opto.Value()) && (middle_opto.Value() < middleOffHigh)) {
        turn_right();
        LCD.Write("right");

    }
    LCD.Write(right_opto.Value());
    LCD.Write("/n");
    LCD.Write(middle_opto.Value());
    LCD.Write("/n");
    LCD.Write(left_opto.Value());
    LCD.Write("/n");



}

void ERCMain()
{
    while(true)
    {
    check_line();
    move_forward();
    }
}
