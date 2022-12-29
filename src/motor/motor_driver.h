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
#include "util/naff_maths_utilities.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Commutation TriState for a phase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
enum ECommutatorState
{
	EFloat = -1,	// Connected to neither ground or vcc
	ESink = 0, 		// Connected to ground
	ESource = 1, 	// Connected to vcc
};

using PinOffset = ECommutatorState;

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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Commutator <N Phase>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class MotorDriver
{	
private:
	const int PhaseCount;
	const int StepCount;

	/// All valid commutation steps for this motor.
	ECommutatorState* CommutationTable;

	/// Which way to read the commutation table.
	ESpinDirection SpinDirection;

	/// Note the pins are stored in pairs of {sink, source} for each phase.
	int* ControlPins;

	/// Current access index into the commutation table. 
	int CurrentStep;

	/// ActivePins
	int ActivePins[2] = {-1,-1};
	
public:
	MotorDriver(int Phases);

	/// Set which phase is doing what for this commutation step.
	template<typename ...PackedECommutatorState>
	int DeclareCommutationStep( int Step, PackedECommutatorState... Args )
	{
		ECommutatorState lazyunpack[PhaseCount] = {Args...};
		for( int i = 0; i< PhaseCount; ++i)
		{
			CommutationTable[Step*PhaseCount + i] = lazyunpack[i];
		}
	
		return Step;
	}

	/// Set io pins for phase control.
	/// 1. SourcePin - gate for vcc mosfet.
	/// 2. SinkPin - gate for ground mosfet.
	void DeclarePinsForPhase(int Phase, int SourcePin, int SinkPin);

	/// Set commutator state from commutation table for the current step index.
	void SetCommutatorStep(int Step);

	/// Set spin direction.
	void SetMotorDirection(ESpinDirection Direction);

	/// Set commutator state to close all connections.  
	void SetStopped();

	/// Write the current state to the io register to command the mosfet driver circuit.
	void UpdateCommutator();

	/// Drive ESC hardware
	void Drive();
};
