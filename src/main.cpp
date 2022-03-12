#include <Arduino.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_helper.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
constexpr uint8_t CLOCKWISE = 0;
constexpr uint8_t ANTI_CLOCKWISE = 1;
constexpr unsigned long BASE_DURATION = 10000u;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct Pinout
{
  uint8_t sense_a         = 8;
  uint8_t sense_b         = 9;
  uint8_t sense_c         = 10;
  uint8_t dir_output      = 11;
  uint8_t dir_input       = 12;
  uint8_t phase_a_source  = 2;
  uint8_t phase_a_sink    = 3;
  uint8_t phase_b_source  = 4;
  uint8_t phase_b_sink    = 5;
  uint8_t phase_c_source  = 6;
  uint8_t phase_c_sink    = 7;
}Pins;

struct SpeedControlData
{
  unsigned int measured_rpm;
  unsigned int target_rpm;
  float duty_cycle;

  unsigned int unhandled_ticks = 0;
  unsigned long time_since_last_change = 0;
}SpeedControl;

Display_16by2<3> g_debugDisplay;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
enum PhaseState { V_NC = 0x0, V_SINK = 0x1, V_SOURCE = 0x2 };
struct TableEntry
{
  uint8_t sense_a, sense_b, sense_c;
  PhaseState phase_a, phase_b, phase_c;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct State
{
  uint8_t sense_a, sense_b, sense_c;
  uint8_t a_source, b_source, c_source;
  uint8_t a_sink, b_sink, c_sink;
  uint8_t CurrentDirection;
  unsigned long pulse_duration;
  unsigned long pulse_time_elapsed;
  bool midstep_change_required;
  bool active;
}CurrentState;

struct Timings
{
  unsigned long last_timestamp = 0;
  unsigned long this_timestamp = 0;
  unsigned long since = 0;
}Timing;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint8_t SenseStateToCommutationIdx[0x1<<4] = {UINT8_MAX};

//------------------------------------------------------------------------------
//3 phase bldc sequencing - 
// clockwise - CA CB AB AC BC BA 
// anti-clockwise - AB CB CA BA BC AC     
//------------------------------------------------------------------------------
TableEntry CommutationTable[12] =
{
    /*a  b  c     A          B          C */
    { 1, 0, 1, V_SOURCE,  V_SINK,     V_NC },
    { 1, 0, 0, V_SOURCE,  V_NC,       V_SINK },
    { 1, 1, 0, V_NC,      V_SOURCE,   V_SINK },
    { 0, 1, 0, V_SINK,    V_SOURCE,   V_NC },
    { 0, 1, 1, V_SINK,    V_NC,       V_SOURCE },
    { 0, 0, 1, V_NC,      V_SINK,     V_SOURCE },

    { 0, 0, 1, V_NC,      V_SOURCE,   V_SINK },
    { 0, 1, 1, V_SOURCE,  V_NC,       V_SINK },
    { 0, 1, 0, V_SOURCE,  V_SINK,     V_NC },
    { 1, 1, 0, V_NC,      V_SINK,     V_SOURCE },
    { 1, 0, 0, V_SINK,    V_NC,       V_SOURCE },
    { 1, 0, 1, V_SINK,    V_SOURCE,   V_NC }
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint8_t calcSenseState( uint8_t a, uint8_t b, uint8_t c, uint8_t direction)
{
  return (a << 0) + (b << 1) + (c << 2) + (direction << 3);
} 

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void makeSenseStateToCommutatioTable()
{
  for( uint8_t commutatorIdx = 0; commutatorIdx < 12; ++commutatorIdx )
  {
    const uint8_t direction = (commutatorIdx < 6)? CLOCKWISE : ANTI_CLOCKWISE;

    const TableEntry& entry = CommutationTable[ commutatorIdx ];
    const uint8_t uiniqueSensorState = calcSenseState(entry.sense_a, entry.sense_b, entry.sense_c, direction);
    SenseStateToCommutationIdx[uiniqueSensorState] = commutatorIdx;
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void calculateCommutationState()
{
  const uint8_t oldSenseSum = (CurrentState.sense_a + CurrentState.sense_b +  CurrentState.sense_c);
  CurrentState.sense_a = digitalRead( Pins.sense_a );
  CurrentState.sense_b = digitalRead( Pins.sense_b );
  CurrentState.sense_c = digitalRead( Pins.sense_c );
  const uint8_t newSenseSum = (CurrentState.sense_a + CurrentState.sense_b +  CurrentState.sense_c);

  CurrentState.midstep_change_required = (oldSenseSum == 0 || oldSenseSum != newSenseSum) && newSenseSum != 0;
  SpeedControl.unhandled_ticks += static_cast<unsigned int>( CurrentState.midstep_change_required );

  const uint8_t senseState = calcSenseState( CurrentState.sense_a, CurrentState.sense_b, CurrentState.sense_c, CurrentState.CurrentDirection );
  const uint8_t commutationIdx = SenseStateToCommutationIdx[senseState];
  const TableEntry& newState = CommutationTable[commutationIdx];

  const uint8_t to_sourcing[] = {0,0,1}; 
  CurrentState.a_source = to_sourcing[newState.phase_a];
  CurrentState.b_source = to_sourcing[newState.phase_b];
  CurrentState.c_source = to_sourcing[newState.phase_c];

  const uint8_t to_sinking[] = {0,1,0}; 
  CurrentState.a_sink = to_sinking[newState.phase_a];
  CurrentState.b_sink = to_sinking[newState.phase_b];
  CurrentState.c_sink = to_sinking[newState.phase_c];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void writeNullState()
{
  digitalWrite( Pins.phase_a_source, LOW );
  digitalWrite( Pins.phase_b_source, LOW );
  digitalWrite( Pins.phase_c_source, LOW );
  digitalWrite( Pins.phase_a_sink, LOW );
  digitalWrite( Pins.phase_b_sink, LOW );
  digitalWrite( Pins.phase_c_sink, LOW );
  CurrentState.active = false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void writeCommutationState()
{
  digitalWrite( Pins.phase_a_sink, CurrentState.a_sink );
  digitalWrite( Pins.phase_b_sink, CurrentState.b_sink );
  digitalWrite( Pins.phase_c_sink, CurrentState.c_sink );
  digitalWrite( Pins.phase_a_source, CurrentState.a_source );
  digitalWrite( Pins.phase_b_source, CurrentState.b_source );
  digitalWrite( Pins.phase_c_source, CurrentState.c_source );
  CurrentState.active = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void updatePWMparmaters()
{
  if( SpeedControl.unhandled_ticks == 0 && SpeedControl.time_since_last_change < 1e6 )
  {
    return;
  }

  static const float stepConstant = 3.0f * (M_PI/180.0f);

  int tickDelta = SpeedControl.unhandled_ticks;
  float timeInSeconds = static_cast<float>(SpeedControl.time_since_last_change) * static_cast<float>(1e-6);
  float traversedAngle = static_cast<float>(tickDelta) * stepConstant;
  float radiansPerSecond = traversedAngle/timeInSeconds; //w = da/dt

  float oneRotationTimeSeconds = (M_PI*2.0f) / radiansPerSecond;
  float rotationsPerMinute = 60.0f / oneRotationTimeSeconds;

  SpeedControl.measured_rpm = 0; //static_cast<int>(rotationsPerMinute);
  SpeedControl.time_since_last_change = 0;
  SpeedControl.unhandled_ticks = 0;

  if( SpeedControl.measured_rpm != SpeedControl.target_rpm )
  {
    if( SpeedControl.measured_rpm > SpeedControl.target_rpm )
    {
      if( SpeedControl.target_rpm  == 0)
      {
        SpeedControl.duty_cycle = 0.0f;
      }
      else
      {
        float overshoot = static_cast<float>(SpeedControl.measured_rpm) / static_cast<float>(SpeedControl.target_rpm);
        float decreaseFactor = 1.0f / overshoot; 
        SpeedControl.duty_cycle = DMax(SpeedControl.duty_cycle * decreaseFactor, 0.0f);
      }

    
    }
    else if( SpeedControl.measured_rpm < SpeedControl.target_rpm)
    {
      if( SpeedControl.measured_rpm == 0 )
      {
        SpeedControl.duty_cycle = 1.0f;
      }
      else
      {
        float undershoot = static_cast<float>(SpeedControl.measured_rpm) / static_cast<float>(SpeedControl.target_rpm);
        float increaseFactor = 1.0f / undershoot; 
    
        SpeedControl.duty_cycle = DMin( SpeedControl.duty_cycle * increaseFactor, 1.0f);
      }
    }
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void setup()
{
  // Bit Alocation. 16 by 2.
  // x - temp, format 4sf, at least 1 decimal.
  // y - air quality, as a percentage of resistance on the BME680
  // z - air quality hint from lookup table.
  // | 00| 01| 02| 03| 04| 05| 06| 07| 08| 09| 10| 11| 12| 13| 14| 15|
  //0| M | e | a | s | u | r | e | d | _ | x | x | x | R | P | M | _ |
  //1| T | a | r | g | e | t | _ | x | x | X | R | P | M | _ | _ | _ |
  //g_debugDisplay.reserve(0, 0, 0, 15);
  //g_debugDisplay.reserve(1, 1, 0, 15);

  // | 00| 01| 02| 03| 04| 05| 06| 07| 08| 09| 10| 11| 12| 13| 14| 15|
  //0| R | P | M | _ | M | x | x | x | _ | : | _ | T | x | x | x | _ |
  //1| D | u | t | y | x | x | x | % | _ | _ | _ | _ | _ | _ | _ | _ |
  g_debugDisplay.reserve(0, 0, 0, 7);
  g_debugDisplay.reserve(1, 0, 8, 15);
  g_debugDisplay.reserve(2, 1, 0, 15);

  g_debugDisplay.begin();

  //init current state.
  CurrentState.sense_a = 0;
  CurrentState.sense_b = 0;
  CurrentState.sense_c = 0;
  CurrentState.a_source = V_NC;
  CurrentState.b_source = V_NC;
  CurrentState.c_source = V_NC;
  CurrentState.a_sink = V_NC;
  CurrentState.b_sink = V_NC;
  CurrentState.c_sink = V_NC;
  CurrentState.pulse_duration = 0u;
  CurrentState.pulse_time_elapsed = 0u;
 // CurrentState.stall_timer = 0u;
 // CurrentState.current_pulse_duration = BASE_DURATION;
  CurrentState.CurrentDirection = CLOCKWISE;
  CurrentState.active = false;

  //core data structure
  makeSenseStateToCommutatioTable();

  //init pins
  pinMode( Pins.sense_a, INPUT );
  pinMode( Pins.sense_b, INPUT );
  pinMode( Pins.sense_c, INPUT );
  pinMode( Pins.phase_a_source, OUTPUT );
  pinMode( Pins.phase_a_sink, OUTPUT );
  pinMode( Pins.phase_b_source, OUTPUT );
  pinMode( Pins.phase_b_sink, OUTPUT );
  pinMode( Pins.phase_c_source, OUTPUT );
  pinMode( Pins.phase_c_sink, OUTPUT );

  //set initial state
  writeNullState();

  SpeedControl.target_rpm = 25;
  SpeedControl.measured_rpm = 0;
  SpeedControl.duty_cycle = 1.0f;
}

 unsigned long debugTime = 0;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void loop() 
{
  Timing.this_timestamp = micros();
  if( Timing.this_timestamp - Timing.last_timestamp > 0 )
  {
    Timing.since = Timing.this_timestamp - Timing.last_timestamp;
  }
  else if ( Timing.this_timestamp - Timing.last_timestamp > INT64_MAX )
  {
    Timing.since = UINT64_MAX - Timing.last_timestamp + Timing.this_timestamp;
  }
  Timing.last_timestamp = Timing.this_timestamp;

  calculateCommutationState();

  //pwm high duratiom/low duration


  SpeedControl.time_since_last_change +=  Timing.since;
  CurrentState.pulse_time_elapsed +=  Timing.since;

  if( CurrentState.pulse_time_elapsed >= CurrentState.pulse_duration )
  {
    CurrentState.pulse_time_elapsed  = 0u;
    updatePWMparmaters();

    if( CurrentState.active )
    {
      writeNullState();
      CurrentState.pulse_duration = BASE_DURATION * (1.0f - SpeedControl.duty_cycle);
    }
    else
    {
      writeCommutationState();
      CurrentState.pulse_duration = BASE_DURATION * (SpeedControl.duty_cycle);
    }
  }
  //update commutation
  else if(CurrentState.active && CurrentState.midstep_change_required > 0)
  {
      writeNullState();
      writeCommutationState();
  }

  if( debugTime < 200000  )
  {
    debugTime += Timing.since;
    return;
  }
  debugTime = 0u;
  //debugTime = 0u;

  g_debugDisplay.clear();

  char str[16];
  sprintf(&str[0], "RPM M%3d", SpeedControl.measured_rpm );
  g_debugDisplay.update(0, str);

  sprintf(&str[0], " : T%3d", SpeedControl.target_rpm );
  g_debugDisplay.update(1, str);


  //sprintf(&str[0], "Ticks %3d", (int)(SpeedControl.unhandled_ticks) );
  //g_debugDisplay.update(2, str);

  sprintf(&str[0], "Duty %3d%%", (int)(SpeedControl.duty_cycle*100.0f) );
  g_debugDisplay.update(2, str);

  /*
  sprintf(&str[0], "Measured %d RPM", SpeedControl.measured_rpm );
  g_debugDisplay.update(0, str);

  sprintf(&str[0], "Target %5d RPM", (int)SpeedControl.target_rpm );
  g_debugDisplay.update(1, str);
  */

  g_debugDisplay.draw();
}
