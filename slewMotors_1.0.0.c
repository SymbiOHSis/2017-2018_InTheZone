/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                               Matthew Moran                                */
/*                                 2017-2018                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:    slewMotors.c                                                 */
/*    Brief:     Provides motor slew rate functionality                       */
/*    Created:   12 May 2018                                                  */
/*                                                                            */
/*    Revisions: 1.0.0 -> Initial release                                     */
/*----------------------------------------------------------------------------*/

#ifndef __SLEW_MOTORS__
#define __SLEW_MOTORS__ 100 // version
#define SETDRIVE_USE_SET_MOTOR // support for setDrive library by me.

// redefine min & max macros
#undef  max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#undef  min
#define min(x, y) (((x) < (y)) ? (x) : (y))

#ifndef SLEW_MOTORS_DEFAULT_RATE
#define SLEW_MOTORS_DEFAULT_RATE 10 // 375ms from -127 to +127
#endif

int slewMotorReq[kNumbOfTotalMotors];
int slewMotorSlewRate[kNumbOfTotalMotors];

task slewMotorsTask() {
    unsigned int i;
    int diff;
    while (true) {
        // loop through all motors
        for (i = 0; i < kNumbOfTotalMotors; i++) {
            // diff is the amount that needs to be added to get to the target
            diff = slewMotorReq[i] - motor[i];
            // add the diff, but not more than that motor's slew rate
            motor[i] += min(abs(diff), slewMotorSlewRate[i]) * sgn(diff);
        }
        delay(15);
    }
}

void slewMotorsInit(bool enableAll = true) {
    // set all motors to either the default slew rate, or slew disabled (rate = 256)
    for (unsigned int i = 0; i < kNumbOfTotalMotors; i++) {
        slewMotorSlewRate[i] = enableAll ? SLEW_MOTORS_DEFAULT_RATE : 256;
    }
}

void slewMotorEnable(tMotor port, int slewRate = 10) {
    slewMotorSlewRate[port] = slewRate;
}

void slewMotorDisable(tMotor port) {
    slewMotorSlewRate[port] = 256;
}

void setMotor(tMotor port, int speed) {
    // use this function to set the speed
    if (abs(speed) > 127)
        speed = 127 * sgn(speed);
    slewMotorReq[port] = speed;
}

void forceMotor(tMotor port, int speed) {
    // override the slew rate controls, this time only
    if (abs(speed) > 127)
        speed = 127 * sgn(speed);
    slewMotorReq[port] = speed;
    motor[port] = speed;
}

/*----------------------------------------------------------------------------*/
/** @brief    ROBOTC remove "Unreferenced function" warnings when compiling   */
/*----------------------------------------------------------------------------*/
void slewMotorsWarningEliminate() {
    if (false) {
        setMotor(port1, 0);
        forceMotor(port1, 0);
        slewMotorEnable(port1);
        slewMotorDisable(port1);
        slewMotorsWarningEliminate();
    }
}

#endif // #ifndef __SLEW_MOTORS__
