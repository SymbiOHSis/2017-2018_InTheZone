task autonomous() {
    startTask(monitorTask);
    // assume on the left side of the field
    if (autonSelection == AUTON_STATIONARY) {
        writeDebugStreamLine("AUTON_STATIONARY");
        // release claw rubber bands
        mogoTarget = MOGO_DOWN;
        liftTarget = 1700;
        delay(750);

        // drive forward to stationary
        liftTarget = 2400;
        fourbarTarget = FOURBAR_DOWN;
        delay(600);
        driveStraight(17, 1);
        delay(250);

        // drop cone
        liftTarget = 2200;
        delay(200);
        mogoTarget = MOGO_UP;
        clawTarget = MANUAL;
        setClaw(60);
        delay(500);
        setClaw(90);
        delay(200);

        // lift up to clear cone
        liftTarget = 2400;
        delay(500);
        setClaw(0);

        // turn and disrupt
        driveTurn(95, 5);
        delay(100);
        liftTarget = 1700;
        driveTime(-127, 750);
        if (autonSide == LEFT) {
            driveTime(-90, -127, 1000);
        }
        else {
            driveTime(-127, -90, 1000);
        }
        driveTime(-127, 1250);

        while (true) {
            delay(1000);
            setDrive(-80);
            delay(500);
            stopDrive();
        }
    }
    else {
        writeDebugStreamLine("AUTON_STATIONARY");
        // release claw rubber bands
        mogoTarget = MOGO_DOWN;
        liftTarget = 1700;
        delay(750);

        // drive forward to stationary
        liftTarget = 2400;
        fourbarTarget = FOURBAR_DOWN;
        delay(600);
        driveStraight(17, 1);
        delay(250);

        // drop cone
        liftTarget = 2200;
        delay(200);
        mogoTarget = MOGO_UP;
        clawTarget = MANUAL;
        setClaw(60);
        delay(500);
        setClaw(90);
        delay(200);

        // lift up to clear cone
        liftTarget = 2400;
        delay(500);
        setClaw(0);

        // reverse
        driveTime(-80, 500);
    }
    stopTask(monitorTask);
    allMotorsOff();
}
