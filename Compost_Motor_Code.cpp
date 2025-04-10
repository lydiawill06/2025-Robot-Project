void turn_compost()
{
  //Drive to Composter
  PID_Drive(-35, 8);

  //Turn Compost Motor
  compost_motor.SetPercent(55);
  //Turn Motor for specified time
  int Start_Time = TimeNow();
  while((TimeNow() - Start_Time) < 3.5)
  {}

  Sleep(0.5);

  //Turn Compost Motor Back
  compost_motor.SetPercent(-55);
  //Turn Motor for specified time
  Start_Time = TimeNow();
  while((TimeNow() - Start_Time) < 3.5)
  {}

  PID_Drive(35, 8);
}
