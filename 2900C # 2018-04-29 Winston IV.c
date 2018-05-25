#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    mogoPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in3,    fourbarPot,     sensorPotentiometer)
#pragma config(Sensor, in4,    autonPot,       sensorPotentiometer)
#pragma config(Sensor, in5,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl9,  ledAutonRight,  sensorDigitalOut)
#pragma config(Sensor, dgtl10, ledAutonTwenty, sensorDigitalOut)
#pragma config(Sensor, dgtl11, ledAutonTen,    sensorDigitalOut)
#pragma config(Sensor, dgtl12, ledAutonStationary, sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           driveLeftFront, tmotorVex393TurboSpeed_MC29, openLoop, driveLeft)
#pragma config(Motor,  port3,           driveLeftBack, tmotorVex393TurboSpeed_MC29, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port4,           mogo,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           lift,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           fourbar,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           claw,          tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveRightBack, tmotorVex393TurboSpeed_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port9,           driveRightFront, tmotorVex393TurboSpeed_MC29, openLoop, reversed, driveRight)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// This code is for the VEX cortex platform
#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"

#include "truespeed.h"
#include "setDrive_2.2.0.c"
#include "mattsFunctions_1.3.0.c"
#include "NERD_PID.c"
#include "gyroLib2.c"

#define TICKS_PER_INCH   (261.333 / (PI * 3.25))
#define MOTOR_MAX_SPEED 90

#define MOGO_UP       2940
#define MOGO_MID      2100
#define MOGO_DOWN     1300
#define MOGO_THRES    80

#define LIFT_THRES    20
#define LIFT_STACK    1750
#define LIFT_UP       2840
#define LIFT_MID      2000
#define LIFT_DOWN     1480

#define FOURBAR_BTN   Btn5U
#define FOURBAR_UP    3000
#define FOURBAR_MID   1900
#define FOURBAR_DOWN  960
#define FOURBAR_THRES 0

#define CLAW_BTN      Btn5D
#define CLAW_SPEED    80
#define CLAW_TIME     250
#define CLAW_HOLD_SPEED_OPEN   16
#define CLAW_HOLD_SPEED_CLOSE -24
#define CLAW_OPEN     1
#define CLAW_CLOSE   -1

/* RANDOM DEFINES */
#define MANUAL  0
#define LEFT    1
#define RIGHT  -1
#define AUTON_NONE   0
#define AUTON_TEN    1
#define AUTON_TWENTY 2
#define AUTON_STATIONARY 3
#define LED_ON  0
#define LED_OFF 1

/* GLOBALS */
PID pidDriveStraight, pidDriveTurn, pidMogo, pidLift, pidFourbar;
int mogoTarget = MANUAL, liftTarget = MANUAL, fourbarTarget = MANUAL, clawTarget = CLAW_CLOSE;
unsigned long clawTargetTime;
int autonSelection = AUTON_NONE;
int autonSide      = LEFT;

/* FUNCTIONS */
void setMogo(int speed, bool doTrueSpeed=true) {
    if (doTrueSpeed)
        speed = trueSpeed(speed);
#ifdef MOTOR_MAX_SPEED
    if (abs(speed) > MOTOR_MAX_SPEED)
        speed = MOTOR_MAX_SPEED * sgn(speed);
#endif
    motor[mogo] = speed;
}
void setLift(int speed, bool doTrueSpeed=true) {
    if (doTrueSpeed)
        speed = trueSpeed(speed);
#ifdef MOTOR_MAX_SPEED
    if (abs(speed) > MOTOR_MAX_SPEED)
        speed = MOTOR_MAX_SPEED * sgn(speed);
#endif
    motor[lift] = speed;
}
void setFourbar(int speed, bool doTrueSpeed=true) {
    if (doTrueSpeed)
        speed = trueSpeed(speed);
#ifdef MOTOR_MAX_SPEED
    if (abs(speed) > MOTOR_MAX_SPEED)
        speed = MOTOR_MAX_SPEED * sgn(speed);
#endif
    motor[fourbar]  = speed;
}
void setClaw(int speed, bool doTrueSpeed=true) {
    if (doTrueSpeed)
        speed = trueSpeed(speed);
#ifdef MOTOR_MAX_SPEED
    if (abs(speed) > MOTOR_MAX_SPEED)
        speed = MOTOR_MAX_SPEED * sgn(speed);
#endif
    motor[claw] = speed;
}

void driveStraight(float inches, float threshold=0.5) {
    nMotorEncoder[driveLeftBack] = 0;
    SensorValue[gyro] = 0;
    int speed;
    int gyroSpeed;
    float gyroMultiplier = 0.5;
    int targetTicks = inches * TICKS_PER_INCH;
    writeDebugStreamLine("target = %d", targetTicks);
    long lastIncorrectTime = nSysTime;
    while (nSysTime - lastIncorrectTime < 200) {
        speed = pidCalculate(pidDriveStraight, targetTicks, nMotorEncoder[driveLeftBack]);
        gyroSpeed = -SensorValue[gyro] * gyroMultiplier;
        setDrive(speed - gyroSpeed, speed + gyroSpeed);
        if (abs(targetTicks - nMotorEncoder[driveLeftBack]) > threshold * TICKS_PER_INCH) {
            lastIncorrectTime = nSysTime;
        }
    }
    writeDebugStreamLine("done");
    stopDrive();
}

#define driveTurn(d, t) driveTurn_(d * autonSide, t)
void driveTurn_(float degrees, float threshold) {
    degrees += 3 * sgn(degrees);
    SensorValue[gyro] = 0;
    int speed;
    long lastIncorrectTime = nSysTime;
    while (nSysTime - lastIncorrectTime < 200) {
        speed = pidCalculate(pidDriveTurn, degrees, -SensorValue[gyro] / 10);
        setDrive(speed, -speed);
        if (fabs(degrees - -SensorValue[gyro] / 10) > threshold) {
            lastIncorrectTime = nSysTime;
        }
    }
    stopDrive();
}

void turnOffAutonLeds() {
    SensorValue[ledAutonRight]      = LED_OFF;
    SensorValue[ledAutonStationary] = LED_OFF;
    SensorValue[ledAutonTen]        = LED_OFF;
    SensorValue[ledAutonTwenty]     = LED_OFF;
}

/* TASKS */
task monitorTask() {
    int clawTargetLast = MANUAL;
    int mogoTargetLast = MANUAL;
    unsigned long mogoTargetTime = 0;
    while (true) {
        // mogo
        if (mogoTarget != MANUAL) {
            if (mogoTarget != mogoTargetLast) {
                mogoTargetTime = nSysTime + 1750;
                mogoTargetLast = mogoTarget;
            }
            if (mogoTarget == MOGO_UP && nSysTime > mogoTargetTime) {
                mogoTarget = MANUAL;
                setMogo(0);
            }
            else {
	            if (abs(mogoTarget - SensorValue[mogoPot]) < MOGO_THRES)
	                setMogo(0);
	            else
	                setMogo(pidCalculate(pidMogo, mogoTarget, SensorValue[mogoPot]));
	        }
        }

        // lift
        if (liftTarget != MANUAL) {
            setLift(pidCalculate(pidLift, liftTarget, SensorValue[liftPot]));
        }

        // fourbar
        if (fourbarTarget != MANUAL) {
            setFourbar(pidCalculate(pidFourbar, fourbarTarget, SensorValue[fourbarPot]));
        }

        // claw
        if (clawTarget != MANUAL) {
            if (clawTarget != clawTargetLast) {
                clawTargetTime = nSysTime + CLAW_TIME;
                clawTargetLast = clawTarget;
            }

            if (nSysTime < clawTargetTime) {
                setClaw(CLAW_SPEED * clawTarget);
            }
            else if (clawTarget == CLAW_OPEN) {
                setClaw(CLAW_HOLD_SPEED_OPEN, false);
            }
            else if (clawTarget == CLAW_CLOSE) {
                setClaw(CLAW_HOLD_SPEED_CLOSE, false);
            }
        }
        delay(20);
    }
}

/* MAIN */
void pre_auton() {
    clearDebugStream();
    setDriveInit();
    setDriveSettings.leftMaxSpeed  = 90;
    setDriveSettings.rightMaxSpeed = 90;

    pidInit(pidDriveStraight, 1.00, 5.00, 0.120, 0.5 * TICKS_PER_INCH, 1.5 * TICKS_PER_INCH);
    pidInit(pidDriveTurn,     2.00, 7.50, 0.200, 2, 10);
    pidInit(pidMogo,          0.18, 0.00, 0.002, MOGO_THRES,    300);
    pidInit(pidLift,          0.40, 1.00, 0.012, LIFT_THRES,    120);
    pidInit(pidFourbar,       0.10, 0.00, 0.003, FOURBAR_THRES, 180);
    turnOffAutonLeds();

    if (SensorValue[autonPot] < 80) {
        autonSide = LEFT;
        autonSelection = AUTON_STATIONARY;
        SensorValue[ledAutonRight]      = LED_ON;
        SensorValue[ledAutonStationary] = LED_ON;
    }
    else if (80 <= SensorValue[autonPot] && SensorValue[autonPot] < 900) {
        autonSide = LEFT;
        autonSelection = AUTON_TEN;
        SensorValue[ledAutonRight]      = LED_ON;
        SensorValue[ledAutonTen]        = LED_ON;
    }
    else if (900 <= SensorValue[autonPot] && SensorValue[autonPot] < 1550) {
        autonSide = LEFT;
        autonSelection = AUTON_TWENTY;
        SensorValue[ledAutonTwenty]     = LED_ON;
    }
    else if (2135 <= SensorValue[autonPot] && SensorValue[autonPot] < 2865) {
        autonSide = RIGHT;
        autonSelection = AUTON_TWENTY;
        SensorValue[ledAutonRight]      = LED_ON;
        SensorValue[ledAutonTwenty]     = LED_ON;
    }
    else if (2865 <= SensorValue[autonPot] && SensorValue[autonPot] < 3935) {
        autonSide = RIGHT;
        autonSelection = AUTON_TEN;
        SensorValue[ledAutonRight]      = LED_ON;
        SensorValue[ledAutonTen]        = LED_ON;
    }
    else if (3935 <= SensorValue[autonPot]) {
        autonSide = RIGHT;
        autonSelection = AUTON_STATIONARY;
        SensorValue[ledAutonRight]      = LED_ON;
        SensorValue[ledAutonStationary] = LED_ON;
    }

    // re-init gyro
    SensorType[gyro] = sensorNone;
    delay(250);
    SensorType[gyro] = sensorGyro;
    delay(1250);
}

#include "2900C # 2018-04-29 Winston IV Auton.c"

task usercontrol() {
    mogoTarget    = MANUAL;
    liftTarget    = MANUAL;
    fourbarTarget = MANUAL;
    clawTarget    = CLAW_CLOSE;
    startTask(monitorTask);
    while (true) {
        makeJoystickDeadzones(15);

        // drive
        arcadeControl();

        // lift
        setLift(vexRTT[Ch2]);

        // mogo
        if (vexRT[Btn6U] || vexRT[Btn6D]) {
            mogoTarget = SensorValue[mogoPot] < MOGO_MID ? MOGO_UP : MOGO_DOWN;
            waitForButtonRelease(Btn6U);
            waitForButtonRelease(Btn6D);
        }

        // fourbar
        if (vexRT[FOURBAR_BTN]) {
            fourbarTarget = SensorValue[fourbarPot] < FOURBAR_MID ? FOURBAR_UP : FOURBAR_DOWN;
            waitForButtonRelease(FOURBAR_BTN);
        }

        // claw
        if (vexRT[CLAW_BTN]) {
            clawTarget = clawTarget != CLAW_CLOSE ? CLAW_CLOSE : CLAW_OPEN;
            waitForButtonRelease(CLAW_BTN);
        }

        // reset button if motors burn out
        if (vexRT[Btn7U] || vexRT[Btn8U]) {
		    mogoTarget    = MANUAL;
		    liftTarget    = MANUAL;
		    fourbarTarget = MANUAL;
		    clawTarget    = MANUAL;
		    allMotorsOff();
        }

        // Only run 50 times a second
        delay(20);
    } // while (true)
} // Usercontrol