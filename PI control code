/*
linear speed: distancepercount*(delta counts/delta time)

distancepercount - constant
delta counts/delta speed - real time values

error = desired (inputted) - actual speed (from encoders)

New motor power = Old motor power + P + I + D

P = P*Error
P is experimentally determined

I = I*(Cumulative Error), I experimentally determined --> ESum = ESum + E

D = D*(Current - Previous Error) --> EDiff = E - EPrev
*/

#include <stdio.h>
#include <time.h>
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>


#define COUNTSPERDISTANCE 33.7408479355
#define DISTANCEPERCOUNTS (1/COUNTSPERDISTANCE)  // Example: define this as per your system
#define OG_MOTORPOWER -25.0 //find out what speed this translates to

FEHMotor right_motor(FEHMotor::Motor2, 9.0); 
FEHMotor left_motor(FEHMotor::Motor1, 9.0); 
//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);

float RightOldMotorPower = OG_MOTORPOWER, LeftOldMotorPower = OG_MOTORPOWER;

float expectedcount; //experimentally determined

float deltatime, ti, error = 0;

float leftcounts, rightcounts;

float PIAdjust(){
    
    float pterm, iterm;
    float p=0.5, i=0.1;

    float newleftcounts = left_encoder.Counts(), newrightcounts = right_encoder.Counts(), diffleftcounts = newleftcounts-leftcounts, diffrightcounts = newrightcounts-rightcounts; //read new counts, take difference
    float tf = time(NULL);
    deltatime = ti-tf;
    float leftcountpersec = leftcounts/deltatime;
    float rightcountpersec = rightcounts/deltatime;

    float error = leftcountpersec-rightcountpersec;
    float sumerror = sumerror + error;

    pterm = p*error;
    iterm = i*sumerror;

    float pi = pterm + iterm;

/*
P = P*Error
P is experimentally determined

I = I*(Cumulative Error), I experimentally determined --> ESum = ESum + E
*/
    
    return pi;
}

void driveSpecifiedDistance(float inches) {
    
    float integrated = 0;//whats the point in declaring this here? PIAdjust recalculates this each time anyway?
    float p = 0;

    float distanceTraveled = 0;

    while (distanceTraveled < inches) {

        ti = time(NULL);
        
        float leftcounts = left_encoder.Counts(), rightcounts = right_encoder.Counts();//read counts

        while ((ti-time(NULL))<0.1){};
        
        float control_signal = PIAdjust();

        right_motor.SetPercent(OG_MOTORPOWER + control_signal);

        distanceTraveled = DISTANCEPERCOUNTS*leftcounts + distanceTraveled;

        //left_motor.SetPercent(LeftOldMotorPower + PIAdjust());

        //RightOldMotorPower += PIAdjust();
        //LeftOldMotorPower += PIAdjust(); 
        
    }

    right_motor.Stop();
    left_motor.Stop();
}

int main() {
    right_motor.SetPercent(OG_MOTORPOWER);
    left_motor.SetPercent(OG_MOTORPOWER);

    driveSpecifiedDistance(5);

    return 0;
}
