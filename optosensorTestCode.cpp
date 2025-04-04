#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHSD.h>

// Declarations for analog optosensors
AnalogInputPin right_opto(FEHIO::P2_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);

int main(void)
{
    float x, y; //for touch screen

    // Open output file and prepare for writing values to it
    FEHFile *fptr = SD.FOpen("OptTest.txt", "a");

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Analog Optosensor Testing");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

    // Record values for optosensors on and off of the straight line
    // Left Optosensor on straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (1/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Left Optosensor Value1: ", left_opto.Value());
    LCD.WriteLine("Left Optosensor Value1: ");
    LCD.WriteLine(left_opto.Value());
    Sleep(3.0);
    
    // Left Optosensor off straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor off straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (2/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Left Optosensor Value2: ", left_opto.Value());
    LCD.WriteLine("Left Optosensor Value2: ");
    LCD.WriteLine(left_opto.Value());
    Sleep(3.0);

    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (3/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Middle Optosensor Value1: ", middle_opto.Value());
    LCD.WriteLine("Middle Optosensor Value1: ");
    LCD.WriteLine(middle_opto.Value());
    Sleep(3.0);
    
    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (4/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Middle Optosensor Value2: ", middle_opto.Value());
    LCD.WriteLine("Middle Optosensor Value2: ");
    LCD.WriteLine(middle_opto.Value());
    Sleep(3.0);

    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (5/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Right Optosensor Value1: ", right_opto.Value());
    LCD.WriteLine("Right Optosensor Value1: ");
    LCD.WriteLine(right_opto.Value());
    Sleep(3.0);
    
    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (6/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(fptr, "Right Optosensor Value2: ", right_opto.Value());
    LCD.WriteLine("Right Optosensor Value2: ");
    LCD.WriteLine(right_opto.Value());
    Sleep(3.0);

    // Print end message to screen
    SD.FClose(fptr);
    LCD.Clear(BLACK);
    LCD.WriteLine("Test Finished");

    return 0;
}
