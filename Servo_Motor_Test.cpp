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
AnalogInputPin CDS_Sensor(FEHIO::P2_1);
FEHServo Arm_Servo(FEHServo::Servo3);

int main()
{
//enter the minimum and maximum servo values
    //from the .TouchCalibrate() function
    Arm_Servo.SetMin(863);
    Arm_Servo.SetMax(2410);
 
    //set the servo angle to it's initial angle
    Arm_Servo.SetDegree(90.);
     
    //wait 5 seconds
    Sleep(5.0);
 
    //move the arm to next angle
    Arm_Servo.SetDegree(114.5);
}
