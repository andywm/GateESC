/*------------------------------------------------------------------------------
 ()		File: commutator.h
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
template<int PhaseCount>
class Commutator
{	
public:
	static constexpr int StepCount = Maths::CalcFactorial(PhaseCount);
	static constexpr int PinCount = PhaseCount * 2;
	
private:
	/// All valid commutation steps for this motor.
	ECommutatorState CommutationTable[PhaseCount][StepCount];

	/// Which way to read the commutation table.
	ESpinDirection SpinDirection;

	/// Note the pins are stored in pairs of {sink, source}. To access a particular
	/// pin, do as follows ControlPins[$phase*2 + $pin_offset]
	/// where $phase is the current phase id.
	/// and $pin offset is 0 for sink, 1 for source.
	int ControlPins[PinCount];

	/// Current access index into the commutation table. 
	int CurrentStep;

private:
	// Internal DeclareCommutationStep (private template functions can go in cpp)
	template<typename ...PackedECommutatorState>
	int DeclareCommutationStep_Imp( int Step, PackedECommutatorState... Args );
	
public:
	Commutator();

	/// Set which phase is doing what for this commutation step.
	template<typename ...PackedECommutatorState>
	int DeclareCommutationStep( int Step, PackedECommutatorState... Args )
	{
		return DeclareCommutationStep_Imp(Step, Args...);
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
	void Execute();
};