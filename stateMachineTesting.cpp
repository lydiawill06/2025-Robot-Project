#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor1,9.0);
DigitalInputPin frontRight(FEHIO::P0_4);
DigitalInputPin backRight(FEHIO::P1_0);
DigitalInputPin frontLeft(FEHIO::P3_7);
DigitalInputPin backLeft(FEHIO::P3_5);
AnalogInputPin right_opto(FEHIO::P2_5);
AnalogInputPin middle_opto(FEHIO::P3_6);
AnalogInputPin left_opto(FEHIO::P1_7);

#define leftOffLow 0
#define leftOffHigh 2
#define rightOffLow 0
#define rightOffHigh 2

//States for state machine
enum LineStates { 
    MIDDLE, 
    RIGHT, 
    LEFT
}; 
   

//drive for 1 function

void move_forward() //using encoders
{
    int percent = 25;
    int counts = 20;
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
    while((leftOffLow <= left_opto.Value()) && (left_opto.Value() < leftOffHigh)) {
        turn_left();
    }
    while((rightOffLow <= right_opto.Value()) && (right_opto.Value() < rightOffHigh)) {
        turn_right();
    }
}

int main (void) {
    int state = MIDDLE; // Set the initial state
    while (true) { // I will follow this line forever!
        switch(state) { 
        // If I am in the middle of the line...
        case MIDDLE:
        // Set motor powers for driving straight
        right_motor.SetPercent(30);
        left_motor.SetPercent(30);

        if ( /* Right sensor is on line */ ) {
            state = RIGHT; // update a new state
        } 

        /* Code for if left sensor is on the line */
        break; 

        // If the right sensor is on the line... 
        case RIGHT:
        // Set motor powers for right turn
        /* Drive */

        if( /* I no longer need to turn right… */ ) { 
            /* update a new state */
        } 
        break; 

        // If the left sensor is on the line... 
         case LEFT:
        /* Mirror operation of RIGHT state */
        break; 
    default: // Error. Something is very wrong.
        break; 
    } 
    Sleep(0.25);
   } 
}
