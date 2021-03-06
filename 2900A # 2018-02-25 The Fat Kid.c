#pragma config(Sensor, in1,    brakePot,       sensorPotentiometer)
#pragma config(Sensor, in2,    mogoPot,        sensorPotentiometer)
#pragma config(Sensor, in4,    fourbarPot,     sensorPotentiometer)
#pragma config(Sensor, in5,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  sonar,          sensorSONAR_inch)
#pragma config(Sensor, dgtl3,  driveRightEnc,  sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  driveLeftEnc,   sensorQuadEncoder)
#pragma config(Sensor, dgtl12, autonLeft,      sensorTouch)
#pragma config(Motor,  port1,           claw,          tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           driveRight1,   tmotorVex393HighSpeed_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port3,           mogoRight,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           liftRight,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           fourbarRight,  tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           fourbarLeft,   tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           liftLeft,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           mogoLeft,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           driveLeft1,    tmotorVex393HighSpeed_MC29, openLoop, driveLeft)
#pragma config(Motor,  port10,          brake,         tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)

#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"
#include "TrueSpeed.h"
#include "setDrive_2.1.5.c"
#include "mattsFunctions_1.1.3.c"
#include "NERD_PID.c"
#define AUTODRIVELIB_USE_QUAD_ENCODERS
#define AUTODRIVELIB_DEBUG 5
#include "../C/2900C # 2018-02-17 autoDriveLib_5.3.0 for ES.c"

#define AUTOSTACK_TIMEOUT       3000
#define AUTOSTACK_SONAR_THRES   10
#define AUTOSTACK_LIFT_MORE     300

#define BRAKE_SPEED 100
#define BRAKE_UP    3000
#define BRAKE_MID   2150
#define BRAKE_DOWN  800

#define MOGO_SPEED  127
#define MOGO_UP     2100
#define MOGO_MID    1450
#define MOGO_DOWN   420

#define FOURBAR_SPEED   127
#define FOURBAR_UP      870
#define FOURBAR_MID     1700
#define FOURBAR_DOWN    2500

#define CLAW_SPEED  80
#define CLAW_TIME   200
#define CLAW_HOLD_SPEED 18

#define LIFT_SPEED  127
#define LIFT_TOP    1150
#define LIFT_BOTTOM 2920

enum TmonitorTarget {
	MANUAL,
	UP,
	DOWN,
	OPEN = UP,
	CLOSE = DOWN
};

/* GLOBALS */
bool autostackRunning = false;
TmonitorTarget
brakeTarget     = MANUAL,
mogoTarget      = MANUAL,
fourbarTarget   = MANUAL,
clawTarget      = CLOSE;
unsigned long clawTargetTime = 0;
int  liftTarget     = -1;

/* FUNCTIONS */
void setBrake(int speed, bool getTrueSpeed=true) {
	if (getTrueSpeed)
		speed = trueSpeed(speed);
	motor[brake] = speed;
}
void waitForBrakeDown() {
	while (SensorValue[brakePot] > BRAKE_DOWN)
		delay(20);
}
void waitForBrakeUp() {
	while (SensorValue[brakePot] < BRAKE_UP)
		delay(20);
}

void setClaw(int speed, bool getTrueSpeed=true) {
	if (getTrueSpeed)
		speed = trueSpeed(speed);
	motor[claw] = speed;
}
void setClawClose() {
	clawTarget = CLOSE;
	clawTargetTime = nSysTime + CLAW_TIME;
}
void setClawOpen() {
	clawTarget = OPEN;
	clawTargetTime = nSysTime + CLAW_TIME;
}
void waitForClaw() {
	while (clawTargetTime < nSysTime)
		delay(20);
}

void setMogo(int speed, bool getTrueSpeed=true) {
	if (getTrueSpeed)
		speed = trueSpeed(speed);
	motor[mogoLeft]  = speed;
	motor[mogoRight] = speed;
}
void waitForMogoDown() {
	while (SensorValue[mogoPot] > MOGO_DOWN)
		delay(20);
}
void waitForMogoUp() {
	while (SensorValue[mogoPot] < MOGO_UP)
		delay(20);
}

void setLift(int speed, bool getTrueSpeed=true) {
	if (getTrueSpeed)
		speed = trueSpeed(speed);
	motor[liftLeft]  = speed;
	motor[liftRight] = speed;
}

void setFourbar(int speed, bool getTrueSpeed=true) {
	if (getTrueSpeed)
		speed = trueSpeed(speed);
	motor[fourbarLeft]  = speed;
	motor[fourbarRight] = speed;
}
void waitForFourbarDown() {
	while (SensorValue[fourbarPot] < FOURBAR_DOWN)
		delay(20);
}
void waitForFourbarUp() {
	while (SensorValue[fourbarPot] > FOURBAR_UP)
		delay(20);
}

void autostack() {
	autostackRunning = true;
	writeDebugStreamLine("as: 0");

	clawTarget = MANUAL;
	setClaw(-21);

	// go up till there's no cone
	liftTarget = -1;
	while (SensorValue[sonar] < AUTOSTACK_SONAR_THRES) {
		setLift(LIFT_SPEED);
		if (SensorValue[liftPot] <= LIFT_TOP || vexRT[Btn8R]) {
			autostackRunning = false;
			if (vexRT[Btn8R]) waitForButtonRelease(Btn8R);
			return;
		}
		delay(20);
	}
	liftTarget = SensorValue[liftPot] - AUTOSTACK_LIFT_MORE;
	writeDebugStreamLine("as: 1");

	// raise fourbar
	fourbarTarget = UP;
	while (SensorValue[fourbarPot] > FOURBAR_UP) {
		if (vexRT[Btn8R]) {
			autostackRunning = false;
			return;
		}
		delay(20);
	}
	delay(100);
	writeDebugStreamLine("as: 2");

	// drop cone
	clawTarget = MANUAL;
	setClawOpen();
	for (int i=0; i<CLAW_TIME/20+10; i++) {
		if (vexRT[Btn8R]) {
			autostackRunning = false;
			return;
		}
		delay(20);
	}

	// lower fourbar
	fourbarTarget = DOWN;
	while (SensorValue[fourbarPot] < FOURBAR_DOWN) {
		if (vexRT[Btn8R]) {
			autostackRunning = false;
			return;
		}
		delay(20);
	}

	// lower lift
	liftTarget = LIFT_BOTTOM;

	autostackRunning = false;
}

task autostackTask() {
	while (true) {
		if (vexRT[Btn8R]) {
			waitForButtonRelease(Btn8R);
			autostack();
		}
		delay(20);
	}
}

task monitorTask() {
	PID liftPid;
	pidInit(liftPid, -0.15, 0.00, 0.00, 20, 100);
	while (true) {
		// lift
		if (liftTarget != -1)
			setLift(pidCalculate(liftPid, liftTarget, SensorValue[liftPot]));

		// brake
		if (brakeTarget == UP) {
			if (SensorValue[brakePot] < BRAKE_UP)
				setBrake(BRAKE_SPEED);
			else
				setBrake(0);
		}
		else if (brakeTarget == DOWN) {
			if (SensorValue[brakePot] > BRAKE_DOWN)
				setBrake(-BRAKE_SPEED);
			else
				setBrake(0);
		}

		// mogo
		if (mogoTarget == UP) {
			if (SensorValue[mogoPot] < MOGO_UP)
				setMogo(MOGO_SPEED);
			else
				setMogo(0);
		}
		else if (mogoTarget == DOWN) {
			if (SensorValue[mogoPot] > MOGO_DOWN)
				setMogo(-MOGO_SPEED);
			else
				setMogo(0);
		}

		// fourbar
		if (fourbarTarget == UP) {
			if (SensorValue[fourbarPot] > FOURBAR_UP)
				setFourbar(FOURBAR_SPEED);
			else
				setFourbar(0);
		}
		else if (fourbarTarget == DOWN) {
			if (SensorValue[fourbarPot] < FOURBAR_DOWN)
				setFourbar(-FOURBAR_SPEED);
			else
				setFourbar(0);
		}

		// claw
		if (clawTarget == OPEN) {
			if (clawTargetTime > nSysTime)
				setClaw(CLAW_SPEED);
			else
				setClaw(CLAW_HOLD_SPEED, false);
		}
		else if (clawTarget == CLOSE) {
			if (clawTargetTime > nSysTime || vexRT[Btn5D])
				setClaw(-CLAW_SPEED);
			else
				setClaw(-CLAW_HOLD_SPEED, false);
		}

		delay(20);
	}
}

void pre_auton() {
	clearDebugStream();
	setDriveInit();
	autoDriveInit(
	4.15,  // float wheelSize (inches)
	13,    // float wheelBaseDiameter (inches)
	20,    // minSpeed
	80     // maxSpeed
	);
	driveEncoderInit(driveRightEnc, driveRight1, 0.450, 0.550, 0.025, 15, 100);
	driveEncoderInit(driveLeftEnc,  driveLeft1,  0.450, 0.550, 0.025, 15, 100);
	//setDriveData.rightMultiplier = 0.95;

	bStopTasksBetweenModes = false;
	startTask(monitorTask);
	startTask(autostackTask);
}

task autonomous() {
	stopTask(usercontrol);
	allMotorsOff();
	writeDebugStreamLine("autonomous starting");

	bool left = (bool) SensorValue[autonLeft];
	long startTime = nSysTime;
	int targetTicks;
	//driveInches(24);
	//turnEncoder(90);
	//autoDriveWaitForQueue();

	// force claw shut with extra power
	clawTarget = MANUAL;
	motor[claw] = -28;
	// put mogo lift down and drive
	liftTarget = 2000;
	mogoTarget = DOWN;
	setDrive(60);
	delay(100);
	driveInches(47);
	fourbarTarget = DOWN;
	autoDriveWaitForQueue();

	// drop preload cone on mogo
	liftTarget = 2600;
	wait1Msec(20);
	setClawOpen();
	wait1Msec(100);
	liftTarget = 2000;
	// pick up mogo
	mogoTarget = UP;
	waitForMogoUp();

	// grab second cone
	driveInches(8);
	liftTarget = 2920;
	autoDriveWaitForQueue();
	setClawClose();
	wait1Msec(100);
	liftTarget = 2000;

	// drive back, score cone
	driveInches(-54);
	fourbarTarget = UP;
	waitForFourbarUp();
	delay(200);
	liftTarget = 2800;
	wait1Msec(400);
	setClawOpen();
	waitForClaw();
	liftTarget = 1500;
	autoDriveWaitForQueue();

	// turn 135 degrees
	if (left) {
		targetTicks = SensorValue[driveLeftEnc] - 90;
		while (SensorValue[driveLeftEnc] > targetTicks) {
			setDrive(-127, 127);
			delay(20);
		}
		stopDrive();
	}
	else {
		targetTicks = SensorValue[driveRightEnc] - 90;
		while (SensorValue[driveRightEnc] > targetTicks) {
			setDrive(127, -127);
			delay(20);
		}
		stopDrive();
	}

	// drive to center of field

	//bum bum bum bum bum bum bum bum bum bum bum bum bum
	//mr sandman
	//man me a sand
	//make it the cutest man car door hook hand

	driveInches(-27);
	autoDriveWaitForQueue();

	// turn towards 20pt
	if (left) {
		targetTicks = SensorValue[driveLeftEnc] - 340;
		while (SensorValue[driveLeftEnc] > targetTicks) {
			setDrive(-127, 127);
			delay(20);
		}
		stopDrive();
		} else {
		targetTicks = SensorValue[driveRightEnc] - 340;
		while (SensorValue[driveRightEnc] > targetTicks) {
			setDrive(127, -127);
			delay(20);
		}
		stopDrive();
	}

	// drop mogo in 20pt
	setDrive(127);
	delay(1200);
	setDrive(60);
	delay(100);

	// put mogo down
	mogoTarget = MANUAL;
	while (SensorValue[mogoPot] > MOGO_MID - 500) {
		setMogo(-127);
		delay(20);
	}
	setMogo(0);

	stopDrive();
	delay(200);

	// back up
	setDrive(-127);
	delay(600);
	mogoTarget = UP;
	delay(500);
	stopDrive();

	writeDebugStreamLine("autonomous finished in %.2f", (nSysTime - startTime) / 1000.0);
	allMotorsOff();
}

task usercontrol() {
	stopTask(autonomous);
	mogoTarget = MANUAL;
	fourbarTarget = MANUAL;
	brakeTarget = MANUAL;
	liftTarget = -1; // manual control
	allMotorsOff();
	while (true) {
		//writeDebugStreamLine("left: %d, right: %d", motor[driveLeft1], motor[driveRight1]);
		//writeDebugStreamLine("brakePot: %d", SensorValue[brakePot]);
		makeJoystickDeadzones();

		// Brake
		if (vexRT[Btn8D]) {
			brakeTarget = brakeTarget != DOWN ? DOWN : UP;
			waitForButtonRelease(Btn8D);
		}

		// Mogo
		if (vexRT[Btn6U]) {
			mogoTarget = mogoTarget != DOWN ? DOWN : UP;
			waitForButtonRelease(Btn6U);
		}


		if (!autostackRunning) {
			// Fourbar
			if (vexRT[Btn5U]) {
				fourbarTarget = fourbarTarget != DOWN ? DOWN : UP;
				waitForButtonRelease(Btn5U);
			}

			// Claw
			if (vexRT[Btn5D]) {
				if (clawTarget != OPEN)
					setClawOpen();
				else
					setClawClose();
				waitForButtonRelease(Btn5D);
			}

			// Lift
			setLift(vexRTT[Ch2]);
		}
		// Drive
		//arcadeControl(vexRTT[Ch4], vexRTT[Ch3], false);
		motor[driveLeft1]  = vexRT[Ch3] + vexRT[Ch4];
		motor[driveRight1] = vexRT[Ch3] - vexRT[Ch4];

		// Only run 50 times a second
		delay(20);
	} // while (true)
} // Usercontrol
