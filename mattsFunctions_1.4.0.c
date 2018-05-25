/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                               Matthew Moran                                */
/*                                    2017                                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:    mattsFunctions.c                                             */
/*    Brief:     Basic library of my commonly used functions                  */
/*    Created:   3 October 2017                                               */
/*                                                                            */
/*    Revisions: 1.0.0 -> Initial release                                     */
/*               1.0.1 -> Fixed backwards arcade control turning              */
/*               1.0.2 -> Arcade control default joystick threshold           */
/*                           increased from 5 to 10                           */
/*               1.1.0 -> Added global variable vexRTT (thresholded joystick) */
/*                          and the update function - makeJoystickDeadzones() */
/*               1.1.1 -> Added dontDivideByTwo parameter for arcadeControl() */
/*               1.1.2 -> Changed parameter to divideByTwo for arcadeControl()*/
/*               1.1.3 -> Added deadzoneThreshold to makeJoystickDeadzones    */
/*               1.1.4 -> Changed param max to maxSpeed to fix name conflict  */
/*               1.2.0 -> Added driveTime function                            */
/*               1.3.0 -> Added min/max macros                                */
/*               1.4.0 -> Added driveEncoder                                  */
/*----------------------------------------------------------------------------*/

#ifndef __MATTS_FUNCTIONS__
#define __MATTS_FUNCTIONS__ 140 // version

#undef  max
#define max(a,b)                \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })
#undef  min
#define min(a,b)                \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

int vexRTT[6];  // Thresholded joystick
/*----------------------------------------------------------------------------*/
/** @brief    Make joystick deadzones / thresholds                            */
/*                puts values into global variable vexRTT                     */
/*----------------------------------------------------------------------------*/
void makeJoystickDeadzones(int deadzoneThreshold = 10) {
    for (int i = 0; i < 6; i++)
        vexRTT[i] = abs(vexRT[i]) > deadzoneThreshold ? vexRT[i] : 0;
}

/*----------------------------------------------------------------------------*/
/** @brief    Delays a until button is pressed (vexRT[btn] == 1)              */
/** @param[btn] the button to wait for                                        */
/*----------------------------------------------------------------------------*/
void waitForButtonPress(TVexJoysticks btn) {
    while (!vexRT[btn])
        delay(20);
}

/*----------------------------------------------------------------------------*/
/** @brief    Delays a until button is released (vexRT[btn] == 0)             */
/** @param[btn] the button to wait for                                        */
/*----------------------------------------------------------------------------*/
void waitForButtonRelease(TVexJoysticks btn) {
    while (vexRT[btn])
        delay(30);
}

/*----------------------------------------------------------------------------*/
/** @brief    Set drive to arcade control                                     */
/** @param[x = vexRT[Ch4]] the x axis of movement (on the controller)         */
/** @param[y = vexRT[Ch3]] the y axis of movement (on the controller)         */
/** @param[divideByTwo = true] changes sensitivity of turning                 */
/*----------------------------------------------------------------------------*/
void arcadeControl(int x, int y, bool divideByTwo = false) {
    if (divideByTwo)
        setDrive((y + x) / 2, (y - x) / 2);
    else
        setDrive(y + x, y - x);
}
void arcadeControl(bool divideByTwo = false) {
    int x = abs(vexRT[Ch4]) > 10 ? vexRT[Ch4] : 0;
    int y = abs(vexRT[Ch3]) > 10 ? vexRT[Ch3] : 0;
    if (divideByTwo)
        setDrive((y + x) / 2, (y - x) / 2);
    else
        setDrive(y + x, y - x);
}

/*-----------------------------------------------------------------------------*/
/** @brief  Returns the value on a quadratic scale where 0 is min, 127 is max  */
/** @note   I use it for making small joystick movements smaller,
/**             whilst retaining full speed control                            */
/** @param[speed]  the speed to convert                                        */
/** @detail    returns: abs(speed) * speed / 127                               */
/**                abs(speed) preserves the sign/polarity of speed             */
/**                / max because it returns a number between 0 - max           */
/*-----------------------------------------------------------------------------*/
int acceleration(int speed, int maxSpeed = 127) {
    return abs(speed) * speed / maxSpeed;
}

/*-----------------------------------------------------------------------------*/
/** @brief  Drives for a specified amount of time                              */
/** @param[speed]  the left speed of the drive (or speed for both sides)       */
/** @param[speedRight]  the right speed of the drive                           */
/** @param[time]   the time to wait before stopping the drive                  */
/*-----------------------------------------------------------------------------*/
void driveTime(int speed, int time) {
    setDrive(speed);
    delay(time);
    setDrive(0);
}
void driveTime(int speedLeft, int speedRight, int time) {
    setDrive(speedLeft, speedRight);
    delay(time);
    setDrive(0);
}

/*-----------------------------------------------------------------------------*/
/** @brief  Drives until a specified target                                    */
/** @param[speed]  the left speed of the drive (or speed for both sides)       */
/** @param[speedRight]  the right speed of the drive                           */
/** @param[port]    the sensor port to compare against the target              */
/** @param[target]  the target to drive to                                     */
/** @param[timeout] the max time to wait before stopping the drive             */
/*-----------------------------------------------------------------------------*/
void driveEncoder(int speed, tSensors port, int target, int timeout = 0) {
    unsigned long endTime = 0;
    if (timeout)
        endTime = nSysTime + timeout;
    bool goingUp = target > 0;
    target += SensorValue[port];

    setDrive(speed);
    while (true) {
        if (timeout && endTime < nSysTime)
            break;
        if (goingUp && SensorValue[port] > target)
            break;
        else if (!goingUp && SensorValue[port] < target)
            break;
    }
    setDrive(0);
}
void driveEncoder(int speedLeft, int speedRight, tSensors port, int target, int timeout = 0) {
    unsigned long endTime = 0;
    if (timeout)
        endTime = nSysTime + timeout;
    bool goingUp = target > 0;
    target += SensorValue[port];

    setDrive(speedLeft, speedRight);
    while (true) {
        if (timeout && endTime < nSysTime)
            break;
        if (goingUp && SensorValue[port] > target)
            break;
        else if (!goingUp && SensorValue[port] < target)
            break;
    }
    setDrive(0);
}
void driveEncoder(int speed, tMotor port, int target, int timeout = 0) {
    unsigned long endTime = 0;
    if (timeout)
        endTime = nSysTime + timeout;
    bool goingUp = target > 0;
    target += nMotorEncoder[port];

    setDrive(speed);
    while (true) {
        if (timeout && endTime < nSysTime)
            break;
        if (goingUp && nMotorEncoder[port] > target)
            break;
        else if (!goingUp && nMotorEncoder[port] < target)
            break;
    }
    setDrive(0);
}
void driveEncoder(int speedLeft, int speedRight, tMotor port, int target, int timeout = 0) {
    unsigned long endTime = 0;
    if (timeout)
        endTime = nSysTime + timeout;
    bool goingUp = target > 0;
    target += nMotorEncoder[port];

    setDrive(speedLeft, speedRight);
    while (true) {
        if (timeout && endTime < nSysTime)
            break;
        if (goingUp && nMotorEncoder[port] > target)
            break;
        else if (!goingUp && nMotorEncoder[port] < target)
            break;
    }
    setDrive(0);
}

/*----------------------------------------------------------------------------*/
/** @brief    ROBOTC remove "Unreferenced function" warnings when compiling   */
/*----------------------------------------------------------------------------*/
void mattsFunctionsWarningEliminate() {
    if (false) {
        makeJoystickDeadzones();
        waitForButtonPress(Btn8D);
        waitForButtonRelease(Btn8D);
        arcadeControl();
        acceleration(0);
        driveTime(0, 0);
        driveEncoder(0, dgtl1, 0);
        mattsFunctionsWarningEliminate();
    }
}

#endif
