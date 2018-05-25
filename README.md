# 2017-2018_InTheZone
ROBOTC code used to program Vex robots for the 2017-2018 season, In the Zone.

The most helpful files will be [setDrive_2.c](setDrive_2.2.2.c), [slewMotors_1.c](slewMotors_1.0.0.c), and [mattsFunctions_1.c](mattsFunctions_1.4.1.c)

<br/>

## setDrive_2.c

This library provides easy-to-use setDrive and stopDrive functions with minimal setup (simply add `setDriveInit();` to your pre_auton).

In order to work, you must have set which motors are on each driveSide in the Motor and Sensors Setup dialog box.
The robot must also drive forward when all drive motors are set to 100.

### Functions
- **setDriveInit()** - Initialises the setDriveData variables and checks what motors are on what side. You must run this function in pre_auton.
- **setDriveLeft(speed)** - Sets the left side of the drive to a speed. This does not adjust the right side of the drive.
- **setDriveRight(speed)** - Sets the right side of the drive to a speed. This does not adjust the left side of the drive.
- **setDrive(speed)** - Sets the both sides of the drive to a speed.
- **setDrive(leftSpeed, rightSpeed)** - Sets the left side of the drive to leftSpeed, and the right side of the drive to rightSpeed.
- **stopDriveLeft()** - Stop the left side of the drive. This does not adjust the right side of the drive.
- **stopDriveRight()** - Stop the right side of the drive. This does not adjust the left side of the drive.
- **stopDrive()** - Stops both side of the drive.

### Extras

- This library seemlessly fits in with my slewMotors_1.c library.
Simply `#include "slewMotors_1.c"` _before_ you include this file and everything will work as expected.

- This library seemlessly fits in with the truespeed.c library included in this repository.
Simply `#include "truespeed.c"` _before_ you include this file and everything will work as expected.

- There are a bunch of settings that advanced teams can adjust to adapt setDrive for your robot. These can be accessed in the setDriveData struct.
For example, you can limit the maximum speed your robot can drive at by writing `setDriveData.maxSpeed = 90;`.
If your left drive side always drives faster than your right then you could write `setDriveData.leftMultiplier = 0.95;` to decrease the left side's speed by 5%.

<br/>

## slewMotors_1.c

This library provides easy-to-use slew rate control for motors with minimal setup (simply enable motors in pre_auton and add `startTask(slewMotorsTask);` at the beginning of autonomous and user_control).

### Functions
- **slewMotorsInit(enableForAllMotors = true)** - Easy way to enable slew rate control for all your motors at once.
- **slewMotorEnable(motorPort, slewRate = 10)** - Enable slew rate control for a single motor.
- **slewMotorDisable(motorPort)** - Disable slew rate control for a single motor.
- **setMotor(motorPort, speed)** - Request the motor to run at a speed. Replace all instances of `motor[motorPort] = speed;` with `setMotor(motorPort, speed);`.
- **forceMotor(motorPort, speed)** - Overrides the slew rate control, this time only. 

<br/>

## mattsFunctions_1.c

This library is a bunch of my (Matt M, 2900C) most commonly used functions.

### Functions
- **makeJoystickDeadzones(threshold = 10)** - If the joystick is only being moved a little bit, assume it is zero. This function stores thresholded joystick values in the vexRTT array. To use the thresholded values simply replace `vexRT[Chx]` with `vexRTT[Chx]`. Don't try this with buttons....
- **waitForButtonPress(btn)** - Loops until the button is pressed.
- **waitForButtonRelease(btn)** - Loops until the button is released.
- **arcadeControl(xAxis = Ch4, yAxis = Ch2, divideByTwo = false)** - setDrive to single stick control. Default is the left joystick. Some drivers may prefer to divideByTwo so the speeds is never greater than 127, but this means that driving full speed ahead will only send 64 power to the motors.
- **quadraticScale(speed, maxSpeed)** - Returns the speed on a quadratic scale. This can be used to add extra control to small movements on the joystick, while retaining the full speed range.
- **driveTime(speedLeft, speedRight, timeMs)** - Sets the drive to the speeds for a set amount of time, then stops the drive.
- **driveTime(speed, timeMs)** - Sets both sides of the drive to the speed for a set amount of time, then stops the drive.
- **driveEncoder(speedLeft, speedRight, sensorPort, target, timeout = 0)** - Sets the drive to the speeds until the target (relative to the start position) is passed, then stops the drive. If timeout is specified, the function will end after a set time even if it has not finished.
- **driveEncoder(speed, sensorPort, target, timeout = 0)** - Sets both sides of the drive to the speed until the target (relative to the start position) is passed, then stops the drive. If timeout is specified, the function will end after a set time even if it has not finished.

### Prerequisites
- **setDrive_2.c** - `#include "setDrive_2.c"` _before_ you include this file.

<br/>

All work in this repository is distributed under the [GNU General Public License v3.0](LICENSE.md) unless otherwise specified.