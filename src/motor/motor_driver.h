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
	EAntiClockwise
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

	/// All valid commutation steps for this motor.
	//ECommutatorState* CommutationTable;

	MotorWinding WindingTable[GlobalMotor::StepCount];
	int WindingLookupCW[GlobalMotor::StepCount];
	int WindingLookupACW[GlobalMotor::StepCount];

	/// Which way to read the commutation table.
	ESpinDirection SpinDirection;

	/// Note the pins are stored in pairs of {sink, source} for each phase.
	int ControlPins[GlobalMotor::PhaseCount*2];

	/// Current access index into the commutation table. 
	int CurrentStep {-1};

	/// ActivePins
	MotorWinding ActiveWinding;
	
	//PWM
	float Duty {0.8f};
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

	/// Set commutator state to close all connections.  
	void SetStopped();

	/// Write the current state to the io register to command the mosfet driver circuit.
	void CloseAllWindings();

	/// Drive ESC hardware
	void Drive();
};
