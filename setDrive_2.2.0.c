/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                               Matthew Moran                                */
/*                                    2017                                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:     setDrive.c                                                  */
/*    Brief:      sets drive motors to speed                                  */
/*    Created:    15 April 2017                                               */
/*                                                                            */
/*    Revisions:  0.1.0                                                       */
/*                0.2.0 -> changed from underscored_motors to camelCaseMotors */
/*    2017-05-08: 2.0.0 -> added setDriveInit & stopDrive functions           */
/*                      -> takes drive sides from pragma motor definitions    */
/*                         (this means motors can be named any way!)          */
/*    2017-07-03: 2.1.0 -> added setDrive(left, right)                        */
/*    2017-09-10: 2.1.1 -> added drive side adjustments (advanced use only)   */
/*                           usage: setDriveSettings.leftMultiplier = 0.9;    */
/*    2017-10-03: 2.1.2 -> make sure abs(speed) isn't above 127               */
/*    2017-10-04: 2.1.3 -> doxygen comments                                   */
/*    2018-01-08: 2.1.4 -> setDriveSettings Adders are now integers           */
/*    2018-02-16: 2.1.5 -> speed is a float (to work with leftMultiplier)     */
/*    2018-02-16: 2.1.6 -> changed 'abs(speed)/speed' to 'sgn(speed)'         */
/*    2018-04-05: 2.2.0 -> added max/min speeds for each side in settings     */
/*                      -> fixed bug where array index could try to be a float*/
/*                      -> made trueSpeed lookup per side, was per motor      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// Stop recursive includes
#ifndef __SETDRIVE__
#define __SETDRIVE__    220

struct _setDriveSettings {
    TMotorList  leftMotors,     rightMotors;
    int         leftMotorsNumb, rightMotorsNumb;
    int         leftAdder,      rightAdder;
    float       leftMultiplier, rightMultiplier;
    float       leftMinSpeed,   rightMinSpeed;
    float       leftMaxSpeed,   rightMaxSpeed;
};

struct _setDriveSettings setDriveSettings;

/*----------------------------------------------------------------------------*/
/** @brief    Initializes the setDrive functions                              */
/*----------------------------------------------------------------------------*/
void setDriveInit() {
    getMotorsWithDriveSideType(driveLeft,  setDriveSettings.leftMotors );
    getMotorsWithDriveSideType(driveRight, setDriveSettings.rightMotors);

    setDriveSettings.leftAdder       = 0;
    setDriveSettings.leftMultiplier  = 1;
    setDriveSettings.leftMinSpeed    = 0;
    setDriveSettings.leftMaxSpeed    = 127;
    setDriveSettings.rightAdder      = 0;
    setDriveSettings.rightMultiplier = 1;
    setDriveSettings.rightMinSpeed   = 0;
    setDriveSettings.rightMaxSpeed   = 127;

    setDriveSettings.leftMotorsNumb  = 0;
    setDriveSettings.rightMotorsNumb = 0;

    for (int i=0; i<4; i++) {
        if (setDriveSettings.leftMotors[i] != -1)
            setDriveSettings.leftMotorsNumb++;
        if (setDriveSettings.rightMotors[i] != -1)
            setDriveSettings.rightMotorsNumb++;
    }
}

/*----------------------------------------------------------------------------*/
/** @brief    Sets the speed of the specified motor                           */
/** @param  index   the name or index of the motor                            */
/** @param  speed   the speed to set the motors                               */
/*----------------------------------------------------------------------------*/
void setDriveMotor(tMotor index, int speed) {
#ifdef __SMARTMOTORLIB__
    // Use JPearman's smart motor library if included
    SetMotor(setDriveSettings.leftMotors[i], speed);
#else
    motor[index] = speed;
#endif
}

/*----------------------------------------------------------------------------*/
/** @brief    Sets the speed of the left drive motors                         */
/** @param  speed   the speed to set the motors                               */
/*----------------------------------------------------------------------------*/
void setDriveLeft(float fSpeed) {
    // apply adjustments
    int speed = round(fSpeed * setDriveSettings.leftMultiplier + setDriveSettings.leftAdder);

    // Use TrueSpeed if not using JPearman's smart motor library
    //   and it's included
#ifdef __TRUESPEED__
    speed = trueSpeed(speed);
#else
    // make sure it's not higher than 127 or lower than -127
    if (abs(speed) > 127)
        speed = 127 * sgn(speed);
#endif

    // make sure speed is within boundaries
    if (abs(speed) < setDriveSettings.leftMinSpeed)
        speed = setDriveSettings.leftMinSpeed * sgn(speed);
    else if (abs(speed) > setDriveSettings.leftMaxSpeed)
        speed = setDriveSettings.leftMaxSpeed * sgn(speed);

    // set speeds
    for (int i=0; i<setDriveSettings.leftMotorsNumb; i++){
        setDriveMotor(setDriveSettings.leftMotors[i], speed);
    }
}

/*----------------------------------------------------------------------------*/
/** @brief    Sets the speed of the right drive motors                        */
/** @param  speed   the speed to set the motors                               */
/*----------------------------------------------------------------------------*/
void setDriveRight(float fSpeed) {
    // apply adjustments
    int speed = round(fSpeed * setDriveSettings.rightMultiplier + setDriveSettings.rightAdder);

    // Use TrueSpeed if not using JPearman's smart motor library
    //   and it's included
#ifdef __TRUESPEED__
    speed = trueSpeed(speed);
#else
    // make sure it's not higher than 127 or lower than -127
    if (abs(speed) > 127)
        speed = 127 * sgn(speed);
#endif

    // make sure speed is within boundaries
    if (abs(speed) < setDriveSettings.rightMinSpeed)
        speed = setDriveSettings.rightMinSpeed * sgn(speed);
    else if (abs(speed) > setDriveSettings.rightMaxSpeed)
        speed = setDriveSettings.rightMaxSpeed * sgn(speed);

    // set speeds
    for (int i=0; i<setDriveSettings.rightMotorsNumb; i++){
        setDriveMotor(setDriveSettings.rightMotors[i], speed);
    }
}

/*----------------------------------------------------------------------------*/
/** @brief    Sets the speed of all drive motors                              */
/** @param  speed   the speed to set the motors                               */
/*----------------------------------------------------------------------------*/
void setDrive(float speed) {
    setDriveLeft (speed);
    setDriveRight(speed);
}

/*----------------------------------------------------------------------------*/
/** @brief    Sets the speed of all drive motors                              */
/** @param  speed   the speed to set the motors                               */
/*----------------------------------------------------------------------------*/
void setDrive(float leftSpeed, float rightSpeed) {
    setDriveLeft (leftSpeed);
    setDriveRight(rightSpeed);
}

/*----------------------------------------------------------------------------*/
/** @brief    Stops the left drive motors                                     */
/*----------------------------------------------------------------------------*/
void stopDriveLeft() {
    setDriveLeft(0);
}

/*----------------------------------------------------------------------------*/
/** @brief    Stops the right drive motors                                    */
/*----------------------------------------------------------------------------*/
void stopDriveRight() {
    setDriveRight(0);
}

/*----------------------------------------------------------------------------*/
/** @brief    Stops all drive motors                                          */
/*----------------------------------------------------------------------------*/
void stopDrive() {
    stopDriveLeft();
    stopDriveRight();
}

/*----------------------------------------------------------------------------*/
/** @brief    ROBOTC remove "Unreferenced function" warnings when compiling   */
/*----------------------------------------------------------------------------*/
void setDriveWarningEliminate() {
    if (false) {
        setDrive(0);
        stopDrive();
        setDriveWarningEliminate();
    }
}
#endif // __SETDRIVE__
