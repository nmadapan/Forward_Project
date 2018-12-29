#pragma once

//
// $Id: TaurusConstants.h 12734 2013-10-01 17:25:52Z rnovoselov $
//
// $HeadURL: https://cmxt.sri.com/svn/MedSys/Project/Microgravity/Code/trunk/include/TaurusConstants.h $

#include "MathConstants.h"
#include "TaurusConstants.h"
#include "UsefulTypes.h"

namespace M7System {
    

static const int NumberOfTaurusJoints = 7; // Number of joints per arm (does not include the grip joint)
static const int NumberOfTaurusEncodersPerArm = NumberOfTaurusJoints + 1; // Number of encoders per arm (includes the grip encoder)
static const int MaxEncodersPerMCBus = 9; // Maximum encoders on a motor controller bus on the Taurus (8 for the arm and 1 for camera tilt)

static const unsigned int CameraMCCardIndex = 1; // MC card index that controls the camera housing tilt (aka pitch).
static const unsigned int CameraMotorIndex = 8;  // motor index on the MC card that controls camera pitch. Zero-referenced.


static const int NumberOfTaurusArms = 2;

static const int LEFT_ARM_INDEX = 0;
static const int RIGHT_ARM_INDEX = 1;

// in general, a robot system may have multiple camera subsystems, although Taurus only has one
static const int NumberOfTaurusCameraSystems = 1;

// number of video streams sent by remote system
static const int NumberOfTaurusVideoStreams = 2;

// number of Robot network converters (aka NetBurners)
static const int NumberOfTaurusNetworkConverters = 2;



/* 
   * [RYN 9/25/13] Reorganized Maxon motor parameter constants to match the order in SriCaoMotorParameters
   * NOTES:
   * - Motor type: 11 = EC motor w/ hall sensor only, 10 = EC motor w/ hall sensor + incremental encoder
   * - Thermal time constant winding values differ between the online catalog and the printed catalog.  
   *   Lower of the two values was chosen.
   * - Position sensor type: 1 = 3-ch incr encoder, 2 = 2-ch incr encoder
   * - Position sensor polarity: 0 = normal, 1 = inverted
   */
  static const tUInt16 MotorType[MaxEncodersPerMCBus] = {11, 11, 11, 11, 10, 10, 10, 10, 10};
  static const tUInt8 PolePairNumber[MaxEncodersPerMCBus] = {8, 8, 8, 8, 1, 1, 1, 1, 1 };
  static const tUInt32 MaximumMotorSpeed[MaxEncodersPerMCBus] =  {3360, 3360, 4760, 4760, 26600, 26600, 26600, 26600, 26600};
  static const tUInt16 ThermalTimeConstantWinding[MaxEncodersPerMCBus] = {184, 184, 115, 115, 6, 6, 6, 6, 6 };
  static const tInt32 MaximumContinuousCurrent[MaxEncodersPerMCBus] = {930, 930, 1000, 1000, 420, 420, 420, 420, 420};
  static const tInt32 MaximumPeakCurrent[MaxEncodersPerMCBus] = {1*930, 1*930, 1*1000, 1*1000, 1*420, 1*420, 1*420, 1*420, 2*420};

  static const tUInt32 EncoderPulseNumber[MaxEncodersPerMCBus] = {2048, 2048, 2048, 2048, 256, 256, 256, 256, 256};
  static const tUInt16 PositionSensorType[MaxEncodersPerMCBus] = {1, 1, 1, 1, 2, 2, 2, 2, 2 };
  static const tUInt16 PositionSensorPolarity[MaxEncodersPerMCBus] = {1, 1, 1, 1, 0, 0, 0, 0, 0 };

  static const tInt16 PositionGain[MaxEncodersPerMCBus] = {125, 125, 125, 125, 25, 25, 25, 25, 25 };
  static const tInt16 IntegralGain[MaxEncodersPerMCBus] = {300, 300, 310, 310, 235, 235, 235, 235, 235 };
  static const tInt16 DerivativeGain[MaxEncodersPerMCBus] = {260, 260, 270, 270, 16, 16, 16, 16, 16 };

  static const tUInt16 VelocityFeedForwardGain[MaxEncodersPerMCBus] = {0, 0, 0, 0, 0, 0, 0, 0, 0 };
  static const tUInt16 AccelFeedForwardGain[MaxEncodersPerMCBus] = {121, 121, 121, 121, 2, 2, 2, 2, 2};

  static const tInt16 CurrentRegulatorPGain[MaxEncodersPerMCBus] = {3500, 3500, 3300, 3300, 3200, 3200, 3200, 3200, 3200 };
  static const tInt16 CurrentRegulatorIGain[MaxEncodersPerMCBus] = {550, 550, 450, 450, 2500, 2500, 2500, 2500, 2500 };

  static const tInt16 SpeedRegulatorPGain[MaxEncodersPerMCBus] = {750, 750, 720, 720, 40, 40, 40, 40, 40 };
  static const tInt16 SpeedRegulatorIGain[MaxEncodersPerMCBus] = {85, 85, 86, 86, 18, 18, 18, 18, 18 };
  static const tUInt16 VelFFInSpeed[MaxEncodersPerMCBus] = {0, 0, 0, 0, 0, 0, 0, 0, 0 };
  static const tUInt16 AccelFFInSpeed[MaxEncodersPerMCBus] = {121, 121, 121, 121, 2, 2, 2, 2, 2};

  static const tInt32 MaximumProfileVelocity[MaxEncodersPerMCBus] = {12000, 12000, 12000, 12000, 3202, 19000, 3202, 3202, 12000 }; // RYN TODO: verify
  static const tInt32 MaximumAcceleration[MaxEncodersPerMCBus] = {10000, 10000, 10000, 10000, 10001, 30001, 10001, 10001, 10002 };  // RYN TODO: verify
  
  // (Now loaded from a persistance file)  static const tInt32 EncoderHomeValue[MaxEncodersPerMCBus] = {0, 0, 0, 0, 0, 0, 0, 0, 0 };

  // Camera-housing tilt encoder limits (the 9th motor on the right-arm motor controller)
  // NB: These limits are based on the camera being positioned with the rear roller wheel straight above the shoulder axis// TODO: verify startup camera configuration 

  // Encoder rate (counts per revolution) is based on the design parameters (gear ratios, etc.)
  // static const int CameraEncoderCountsPerMotorOutputRevolution = 67 * 1024;		// gear ratio * counts per revolution
  // 02/17/2012: using replacement motor with hall-effect sensors only
  static const int CameraEncoderCountsPerRevolution = EncoderPulseNumber[CameraMotorIndex] * 4;
  static const double CameraGearRatio = 67.49016;
  static const int CameraEncoderCountsPerMotorOutputRevolution = static_cast<int>(CameraGearRatio * CameraEncoderCountsPerRevolution + 0.5);		// gear ratio * counts per revolution (the 0.5 causes the result to effectlively round off instead of truncate).


  // TODO: calculate the true ratio of camera tilt and motor, rather than this estimate
  // this is the number of motor revolutions for one full revolution of camera tilt, even though camera tilt can only go about 0.5 revolutions
  static const double MotorRevToCameraRevolution = 9.16;					// ratio from spreadsheet				
  
  static const double EncoderCountsPerCameraRevolution = MotorRevToCameraRevolution * CameraEncoderCountsPerMotorOutputRevolution;	
  static const double EncoderCountsPerCameraRadian_calc = (double) EncoderCountsPerCameraRevolution / (2 * SRI_PI);
  static const double EncoderCountsPerCameraRadian = 100021.82;	// from spreadsheet
  

 
  // ****************** Arm joints ***********************
  // Note that gripper encoder is appended as last element for each arm
  // This static data is essentially class-static data, but is hidden/encapsulated
 

  // To support various start-up configurations, we use the known encoder rate and calculate the encoder min/max limits
  // from the start-up joint angles

  // For flexibility, we use separate encoder rate data for each arm (in case the encoder is changed in one arm and not the other)
  // Separate encoder data is only for initial development.  In production (sic) all arms must be mechanically identical since they can be swapped
  // or replaced easily

  // TODO: Make encoder counts match spreadsheet exactly (previously we had not updated distal joints because they were working good enough.
  // TODO: remove vestiges of old arm design (before lead-screw for wrist pitch)

#define TAURUS_PROTO_ARMS (0)
#if TAURUS_PROTO_ARMS
  // per spreadsheet for joints 1-4 (controlling the position of the end effector wrist center)
  // 04/03/2012: The left and right arm have different gear ratios in joint 3.  The right arm (marked '1R' under the 
  // shoulder sliding plastic cover) has a 32:1 gear reduction, resulting in a counts/radian = 83443.03.  
  // The left arm (marked '1L' under) has a 26:1 gear reduction, so we reduce the counts per by a proportional factor, 
  // for a value of 67797.46.
  // Note that for joints 5-8 we use measured values.  These last 4 joints are less important in the sense 
  // that they do not affect the position of the wrist pivot point

  //Values used in deployed systems as of 3/6/13	
  static const double LeftEncoderCountsPerRadian[MaxEncodersPerMCBus]  = { 97784.80, 97784.80, 67797.46, 65487.54, 11459.16,  6841.87, 13207.21, 13207.21 };  
  static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 97784.80, 97784.80, 83443.03, 65487.54, 11459.16,  6841.87, 12809.66, 12809.66, EncoderCountsPerCameraRadian };

  // Values measured on 04/02/2012:
  //static const double LeftEncoderCountsPerRadian[MaxEncodersPerMCBus]  = { 95582.47, 97262.77, 62696.80, 64607.29, 11459.16,  6841.87, 12809.66, 12809.66 };
  //static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 95395.47, 98523.28, 78083.30, 65277.44, 11459.16,  6841.87, 12809.66, 12809.66, EncoderCountsPerCameraRadian };
#endif

  
#define TAURUS_COBRA_ARMS (1)
#if TAURUS_COBRA_ARMS
  // Cobra-design arms, with lead-screw for wrist pitch (joint 6)
  // Values based on revised gear ratios (from Maxon literature) and Rodger Schmidt calculations
  static const double LeftEncoderCountsPerRadian[MaxEncodersPerMCBus]  = { 97970.00, 97970.00, 83866.15, 65611.56, 13223.29,  70240.42, 25677.93, 25677.93 }; // changed last two values to accomodate 131:1 gearbox test 4-27-13  TL
  static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 97970.00, 97970.00, 83866.15, 65611.56, 13223.29,  70240.42, 25677.93, 25677.93, EncoderCountsPerCameraRadian };
  //static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 97970.00, 97970.00, 83866.15, 65611.56, 13223.29,  70240.42, 13207.21, 13207.21, EncoderCountsPerCameraRadian };  //different setups for each instrument
  //static const double LeftEncoderCountsPerRadian[MaxEncodersPerMCBus]  = { 97970.00, 97970.00, 83866.15, 65611.56, 13223.29,  285978.84, 13207.21, 13207.21 };
 // static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 97970.00, 97970.00, 83866.15, 65611.56, 13223.29,  285978.84, 13207.21, 13207.21, EncoderCountsPerCameraRadian };
  
  //static const double LeftEncoderCountsPerRadian[MaxEncodersPerMCBus]  = { 97784.80, 97784.80, 83443.03, 65487.54, 11459.16, 300499, 12809.66, 12809.66, 0 };
  //static const double RightEncoderCountsPerRadian[MaxEncodersPerMCBus] = { 97784.80, 97784.80, 83443.03, 65487.54, 11459.16, 300499, 12809.66, 12809.66, EncoderCountsPerCameraRadian };


#endif
}

