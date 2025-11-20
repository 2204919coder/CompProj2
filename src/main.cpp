/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       2204919                                                   */
/*    Created:      10/20/2025, 10:45:29 AM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;
vex::brain Brain;
controller Controller = controller(primary);

// define your global instances of motors and other devices here
motor RightTop = motor(PORT1, ratio18_1, true);
motor RightBottom = motor(PORT2, ratio18_1, false);
motor LeftTop = motor(PORT3, ratio18_1, false); 
motor LeftBottom = motor(PORT4, ratio18_1, true);
motor_group LeftWheel = motor_group(LeftBottom,LeftTop);
motor_group RightWheel = motor_group(RightTop,RightBottom);

motor BottomChain = motor(PORT20, ratio36_1, true);
motor TopChain = motor(PORT15, ratio36_1, false);

motor IntakeRight = motor(PORT12,true);
motor IntakeLeft = motor(PORT13,false);
motor_group Intake = motor_group(IntakeLeft,IntakeRight);

motor sorter = motor(PORT14,false); //! Allways use timeout!!!
optical colorSensor = optical(PORT10);
distance distanceSensor = distance(PORT16);



/*
------------------------------------------------------------------------
Where my declared functions go
------------------------------------------------------------------------
*/
//? Intake
void intakeIn() {Intake.spin(forward);}
void intakeOut() {Intake.spin(reverse);}
void intakeStop() {Intake.stop();}

//? Lift System
void spinUp() {
  TopChain.spin(forward);
  BottomChain.spin(forward);
  intakeIn();
}
void spinDown() {
  TopChain.spin(reverse);
  BottomChain.spin(reverse);
  intakeOut();
}
void stop() {
  TopChain.stop();
  BottomChain.stop();
  intakeStop();
}
void removeClog() {
  float moveTime = 0.2f;
  //Move forward and back 
  // if(TopChain.isSpinning() || BottomChain.isSpinning()) {
  //   //Save direction
  //   directionType currentDir = TopChain.direction();
  //   Controller.Screen.clearScreen();
  //   Controller.Screen.print((directionType)currentDir == forward ? "reverse" : "forward");
  //   //Forward and back
  //   TopChain.spin(((directionType)currentDir == forward ? reverse : forward));
  //   BottomChain.spin(((directionType)currentDir == forward ? reverse : forward));
  //   wait(moveTime,sec);
  //   //Restart chain lift
  //   TopChain.spin(currentDir);
  //   TopChain.spin(currentDir);
  // } else {
  TopChain.spin(forward);
  BottomChain.spin(forward);
  wait(moveTime,sec);
  TopChain.spin(reverse);
  BottomChain.spin(reverse);
  wait(moveTime,sec);
  TopChain.stop();
  BottomChain.stop();
  // }
}

timer t = timer();

void spinUpAdv() {
  //Disable block thing (adv logic) for 5 sec
  //
  
  if(t.value() > 5) {
    t.reset();
  }
  spinUp();
}

void advSpinCheckLogic() {
  if(t.value() > 5) {
    if(distanceSensor.objectDistance(distanceUnits::cm) < 50) {
      TopChain.stop();
    }
  }
}

//? Sorter
void calibrateDoor() { //* calibrateSortingDoor
  /*
  Set Max torque
  Set Timeout
  Close door
  Reset position to be zero
  Reset Max torque
  */
 sorter.setMaxTorque(20,pct);
 sorter.setTimeout(1,sec);
 sorter.spinFor(143,degrees); //Closes door
 sorter.resetPosition();
 sorter.setMaxTorque(50,pct);
}

bool isDoorAuto = true;
bool isdoorOpen = false;
void openDoor() {
  if(isdoorOpen) return;
  sorter.setTimeout(2,sec);
  sorter.spinFor(-155,degrees);
  isdoorOpen = true;
}
void closeDoor() {
  if(!isdoorOpen) return;
  sorter.setTimeout(2,sec);
  sorter.spinFor(155,degrees);
  isdoorOpen = false;
}

void checkDoor() {
  if(colorSensor.isNearObject()) {
    if(colorSensor.hue() >= 150 && colorSensor.hue() <= 310) { //* blue ish
      openDoor();
    } else { //* red ish
      closeDoor();
    }
  }
}

void toggleDoorAuto() {
  //! print on scree
  isDoorAuto = !isDoorAuto;
}

//! Emergency Stop (Stop all motors)
void stopAll() {
  BottomChain.stop();
  TopChain.stop();
  Intake.stop();
}




/*
----------------------------------------------------------------------
Controller buttons
----------------------------------------------------------------------
*/
int moveSpeedPct = 100;
int turnSpeedPct = 80;
int speedFactor = 20;

void displayInfo() {
  // Controller.Screen.clearScreen();
  Controller.Screen.setCursor(1,1);
  Controller.Screen.print("Move Speed: %d", moveSpeedPct);
  Controller.Screen.setCursor(2,1);
  Controller.Screen.print("Turn Speed: %d", turnSpeedPct);
}

void moveSpeedUp() {
  moveSpeedPct += (moveSpeedPct + speedFactor > 100 ? 0 : speedFactor);
}
void moveSpeedDown() {
  moveSpeedPct -= (moveSpeedPct - speedFactor < 0 ? 0 : speedFactor);
}

void turnSpeedUp() {
  turnSpeedPct += (turnSpeedPct + speedFactor > 100 ? 0 : speedFactor);
}
void turnSpeedDown() {
  turnSpeedPct -= (turnSpeedPct - speedFactor < 0 ? 0 : speedFactor);
}



void setup() {
  BottomChain.setVelocity(75,pct);
  TopChain.setVelocity(100,pct);
  Intake.setVelocity(100,pct);
  sorter.setVelocity(100,pct);
  sorter.setStopping(brake);
  colorSensor.setLightPower(75,pct);
  colorSensor.setLight(ledState::on);
  //Set door
  calibrateDoor();
}
void pre_auton(void) {
  
}

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

void usercontrol(void) {
  // Controller.rumble("...---...");
  
  // 1 is top, 2 is bottom
  // Controller.ButtonR1.pressed();
  Controller.ButtonR2.pressed(spinUpAdv);
  // Controller.ButtonL1.pressed();
  Controller.ButtonL2.pressed(spinDown);
  // Controller.ButtonA.pressed();
  Controller.ButtonB.pressed(stopAll);
  // Controller.ButtonX.pressed();
  // Controller.ButtonY.pressed();
  Controller.ButtonUp.pressed(moveSpeedUp);
  Controller.ButtonDown.pressed(moveSpeedDown);
  Controller.ButtonLeft.pressed(turnSpeedDown);
  Controller.ButtonRight.pressed(turnSpeedUp);
  LeftWheel.spin(forward);
  RightWheel.spin(forward);
  
  while (true) {
    // Controller.Screen.clearScreen();
    // Controller.Screen.setCursor(0,0);
    // Controller.Screen.print(colorSensor.hue());
    displayInfo();
    advSpinCheckLogic();
    if(isDoorAuto) {
      
      checkDoor();
    }
    int axis3 = Controller.Axis3.position();
    int axis1 = Controller.Axis1.position();
    if(axis1 >= 80 || axis1 <= -80) {
      LeftWheel.setVelocity(axis1 * turnSpeedPct/100,pct);
      RightWheel.setVelocity(-axis1 * turnSpeedPct/100,pct);
    } else {
      LeftWheel.setVelocity(axis3 * moveSpeedPct/100,pct);
      RightWheel.setVelocity(axis3 * moveSpeedPct/100,pct);
    }
    int axis2 = Controller.Axis2.position();
    // if(axis2 > 10) {
    //   spinUp();
    // } else if (axis2 < -10) {
    //   spinDown();
    // }
    // if(!isDoorAuto) {
    //   int axis4 = Controller.Axis4.position();
    //   if(axis4 > 10) {
    //     openDoor();
    //   } else if (axis4 < -10) {
    //     closeDoor();
    //   }
    // }
    wait(20, msec); // Sleep the task for a short amount of time to prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  setup();
  usercontrol();
  
  
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
