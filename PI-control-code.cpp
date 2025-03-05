
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
#include <FEHSD.h> 


#define COUNTSPERDISTANCE 33.7408479355
#define DISTANCEPERCOUNTS 0.02963767  // Example: define this as per your system
#define OG_MOTORPOWER 10.0 //find out what speed this translates to

FEHMotor right_motor(FEHMotor::Motor2, 9.0); 
FEHMotor left_motor(FEHMotor::Motor1, 9.0); 
//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P1_6);
DigitalEncoder left_encoder(FEHIO::P1_4);

float RightOldMotorPower = OG_MOTORPOWER, LeftOldMotorPower = OG_MOTORPOWER;

float expectedcount; //experimentally determined

float deltatime, ti, sumerror = 0;

float leftcounts, rightcounts;

float PIAdjust(float ti, float leftcounts, float rightcounts){
        
    //FEHFile *fptr = SD.FOpen("PIDtest.txt","w");

    float pterm, iterm;
    float p=0.5, i=0.0;

    float newleftcounts = left_encoder.Counts(), newrightcounts = right_encoder.Counts(), diffleftcounts = newleftcounts-leftcounts, diffrightcounts = newrightcounts-rightcounts; //read new counts, take difference
    float tf = TimeNow();
    float deltatime = tf-ti;
    float leftcountpersec = leftcounts/deltatime;
    float rightcountpersec = rightcounts/deltatime;

    float error = leftcountpersec-rightcountpersec;
    float sumerror = sumerror + error;
    
    //SD.FPrintf(fptr,"error: %f", error);
    //SD.FPrintf(fptr,"sum error: %f", sumerror);
    

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
    
    FEHFile *fptr = SD.FOpen("PIDtest.txt","w");
    float i = 0;//whats the point in declaring this here? PIAdjust recalculates this each time anyway?
    float p = 0;

    float distanceTraveled = 0;

    SD.FPrintf(fptr,"check 1");
    //LCD.WriteLine("check 1");

    while (distanceTraveled < inches) {

        ti = TimeNow();
        
        float leftcounts = left_encoder.Counts(), rightcounts = right_encoder.Counts();//read counts
        
        SD.FPrintf(fptr,"left count: %f", leftcounts);
        SD.FPrintf(fptr,"right count: %f", rightcounts);

        //LCD.WriteLine("left count:");
        //LCD.WriteLine(leftcounts);
        //LCD.WriteLine("right count:");
        //LCD.WriteLine(rightcounts);

        SD.FPrintf(fptr,"check 2");
        //LCD.WriteLine("check 2");

        float tf = TimeNow();
        while ((tf-ti)<0.01){ 
            
            SD.FPrintf(fptr,"check 2.5");
            //LCD.WriteLine("check 2.5");
            tf = TimeNow(); };
            
        //LCD.WriteLine("check 3");
        SD.FPrintf(fptr,"check 3");
        
        float control_signal = PIAdjust(ti, leftcounts, rightcounts);
        float adjustedpower = OG_MOTORPOWER + control_signal;
        if (adjustedpower>50){adjustedpower = 50;}

        SD.FPrintf(fptr,"adjust: %f", control_signal);
        //LCD.WriteLine("adjust: ");
        //LCD.WriteLine(control_signal);

        right_motor.SetPercent(adjustedpower);
        
        SD.FPrintf(fptr,"adjustpower: %f", adjustedpower);
        //LCD.WriteLine("adjustedpower: ");
        //LCD.WriteLine(adjustedpower);

        distanceTraveled = DISTANCEPERCOUNTS*diffleftcounts;
        
        SD.FPrintf(fptr,"distance traveled within loop: %f", DISTANCEPERCOUNTS*leftcounts);
        //LCD.WriteLine(DISTANCEPERCOUNTS*leftcounts);
        
        SD.FPrintf(fptr,"distance traveled total: %f", distanceTraveled);
        //LCD.WriteLine("distanceTraveled: ");
        //LCD.WriteLine(distanceTraveled);

        //left_motor.SetPercent(LeftOldMotorPower + PIAdjust());

        //RightOldMotorPower += PIAdjust();
        //LeftOldMotorPower += PIAdjust(); 
        
    }

    right_motor.Stop();
    left_motor.Stop();
}

int main() {
    
    FEHFile *fptr = SD.FOpen("PIDtest.txt","w");
    
    right_motor.SetPercent(OG_MOTORPOWER);
    left_motor.SetPercent(OG_MOTORPOWER);

    SD.FPrintf(fptr,"start");
    //LCD.WriteLine("start");

    driveSpecifiedDistance(5);

    SD.FClose(fptr);

    return 0;
}

//issue is with encoder counts, left motor stays low, right motor shoots up to cover the discrepency in counts
