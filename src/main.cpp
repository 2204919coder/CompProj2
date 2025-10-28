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

void testRun() {
  BottomChain.setVelocity(75,pct);
  TopChain.setVelocity(100,pct);
  Intake.setVelocity(100,pct);
  TopChain.spinFor(forward,4000,deg,false);
  BottomChain.spinFor(forward,3000,deg,false);
  Intake.spinFor(forward,7,seconds);
  
  //Set door


  // LeftFront.setVelocity(100,pct);
  // LeftBack.setVelocity(100,pct);
  // RightFront.setVelocity(100,pct);
  // RightBack.setVelocity(100,pct);
  
  // LeftFront.spin(forward);
  // LeftBack.spin(forward);
  // RightFront.spin(forward);
  // RightBack.spin(forward);
  
}
/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  LeftWheel.spin(forward);
  RightWheel.spin(forward);
  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................
    LeftWheel.setVelocity(Controller.Axis3.position() * 100,pct);
    RightWheel.setVelocity(Controller.Axis3.position() * 100,pct);
    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // testRun();
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
