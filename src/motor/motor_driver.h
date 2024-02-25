/*------------------------------------------------------------------------------
 ()		File: motor_driver.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Manages motor sequencing (commutation). 
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "motor/global_motor.h"
#include "util/naff_maths_utilities.h"
#include "util/timer.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef unsigned char uint8_t;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Commutation TriState for a phase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
enum PinOffset
{
	ESink = 0, 		// Connected to ground
	ESource = 1, 	// Connected to vcc
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Motor Spin Direction
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
enum class ESpinDirection
{
	EClockwise,
	EAntiClockwise,
};

struct MotorWinding
{
	int Sink;
	int Source;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Commutator <N Phase>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class MotorDriver
{
private:
	int RegisteredWinding {0};

	/// Winding Table
	MotorWinding WindingTable[GlobalMotor::StepCount];

	/// Commutation Tables
	int WindingLookupCW[GlobalMotor::StepCount];
	int WindingLookupACW[GlobalMotor::StepCount];

	/// Which way to read the commutation table.
	ESpinDirection SpinDirection;

	/// Note the pins are stored in pairs of {sink, source} for each phase.
	int ControlPins[GlobalMotor::PhaseCount*2];

	/// Current access index into the commutation table. 
	int CurrentStep {-1};

	int Duty {0};

	/// Motor Enable State
	bool MotorOn {false};

	/// ActivePins
	MotorWinding ActiveWinding;
	
	//PWM
public:

	void Ready();

	/// Set io pins for phase control.
	/// 1. SourcePin - gate for vcc mosfet.
	/// 2. SinkPin - gate for ground mosfet.
	void DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin);

	/// Declare Winding Pair Sink => Source
	int DeclareWinding(int SinkPhase, int SourcePhase);

	/// Bind Input State to Winding ID for clockwise spin
	void BindClockwiseWinding(int State, int WindingIdx);

	/// Bind Input State to Winding ID for anti-clockwise spin
	void BindAntiClockwiseWinding(int State, int WindingIdx);

	/// Set commutator state from commutation table for the current step index.
	void SetCommutatorStep(int Step);

	/// Set spin direction.
	void SetMotorDirection(ESpinDirection Direction);

	/// Raise enable flag.
	void StartMotor();

	/// Lower enable flag. Set commutator state to close all connections.  
	void StopMotor();

	/// Write the current state to the io register to command the mosfet driver circuit.
	void CloseAllWindings();

	/// Set PWM Drive Duty
	void SetDuty(uint8_t PWM);

	/// Drive ESC hardware
	void Drive();
};
