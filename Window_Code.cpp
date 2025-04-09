void window()
{
    PID_Drive(25, 2.0);
    PID_Turn(45, 80);
    move_forward(45, 12);

    int inches = 5;
    int counts = inches * 33.7408479355;


    right_motor.SetPercent(35);
    while(right_encoder.Counts() < counts)
    {}

    right_motor.Stop();
    
    inches = 6;
    int percent = 35;
    //Call the “move forward” function for 5 inches. 
    counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent-15);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(2.0);
  
    //Call “move5 forward” for backwards 5 inches. 

    percent = -25; 

    counts = inches * 33.7408479355;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent + 15);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

}
