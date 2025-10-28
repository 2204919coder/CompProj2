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
motor RightFront = motor(PORT12, ratio18_1, false);
motor RightBack = motor(PORT2, ratio18_1, false);
motor LeftFront = motor(PORT1, ratio18_1, true); 
motor LeftBack = motor(PORT11, ratio18_1, true);
motor_group LeftWheel = motor_group(LeftBack,LeftFront);
motor_group RightWheel = motor_group(RightFront,RightBack);

motor BottomChain = motor(PORT19, ratio36_1, true);
motor TopChain = motor(PORT20, ratio36_1, false);

motor IntakeRight = motor(PORT13,false);
motor IntakeLeft = motor(PORT14,true);
motor_group Intake = motor_group(IntakeLeft,IntakeRight);

void setup() {
  BottomChain.setVelocity(75,pct);
  TopChain.setVelocity(100,pct);
  Intake.setVelocity(100,pct);
  
  //Set door
  
}


/*
------------------------------------------------------------------------
 Where my declared functions go
------------------------------------------------------------------------
*/

//Lift System
void spinUp() {
  TopChain.spin(forward);
  BottomChain.spin(forward);
}

void spinDown() {
  TopChain.spin(reverse);
  BottomChain.spin(reverse);
}

void stop() {
  TopChain.stop();
  BottomChain.stop();
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

//Intake


/*
----------------------------------------------------------------------
Controller buttons
----------------------------------------------------------------------
*/

void controller_R2_Pressed() {
  // removeClog();
}

void pre_auton(void) {

}

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

void usercontrol(void) {
  // 1 is top, 2 is bottom
  Controller.ButtonL1.pressed(stop);
  Controller.ButtonL2.pressed(spinDown);
  Controller.ButtonR1.pressed(removeClog);
  Controller.ButtonR2.pressed(spinUp);
  LeftWheel.spin(forward);
  RightWheel.spin(forward);
  while (true) {
    if(Controller.Axis3.position() != 0) {
      LeftWheel.setVelocity(Controller.Axis3.position() * 100,pct);
      RightWheel.setVelocity(Controller.Axis3.position() * 100,pct);
    } else {
      LeftWheel.setVelocity(Controller.Axis1.position() * 100,pct);
      RightWheel.setVelocity(-Controller.Axis1.position() * 100,pct);
    }
    
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
