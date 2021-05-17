#include <Arduino.h>
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
constexpr uint8_t CLOCKWISE = 0;
constexpr uint8_t ANTI_CLOCKWISE = 1;
uint8_t SenseStateToCommutationIdx[0x1<<4] = {UINT8_MAX};
uint8_t CurrentDirection = CLOCKWISE;
TableEntry CurrentState = { 0, 0, 0, V_NC, V_NC, V_NC };
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
void makeSafe()
{
  digitalWrite( Pins.phase_a_source, LOW );
  digitalWrite( Pins.phase_b_source, LOW );
  digitalWrite( Pins.phase_c_source, LOW );
  digitalWrite( Pins.phase_a_sink, LOW );
  digitalWrite( Pins.phase_b_sink, LOW );
  digitalWrite( Pins.phase_c_sink, LOW );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool readState()
{
  const uint8_t a = CurrentState.sense_a;
  const uint8_t b = CurrentState.sense_b; 
  const uint8_t c = CurrentState.sense_c;

  CurrentState.sense_a = digitalRead( Pins.sense_a );
  CurrentState.sense_b = digitalRead( Pins.sense_b );
  CurrentState.sense_c = digitalRead( Pins.sense_c );
  const uint8_t senseSum = (CurrentState.sense_a + CurrentState.sense_b +  CurrentState.sense_c);

  return (a + b + c) != senseSum && senseSum != 0 && senseSum != 3;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void setNewState()
{
  const uint8_t senseState = calcSenseState( CurrentState.sense_a, CurrentState.sense_b, CurrentState.sense_c, CurrentDirection );
  const uint8_t commutationIdx = SenseStateToCommutationIdx[senseState];
  const TableEntry& newState = CommutationTable[commutationIdx];

  CurrentState.phase_a = newState.phase_a;
  CurrentState.phase_b = newState.phase_b;
  CurrentState.phase_c = newState.phase_c;

  const uint8_t to_sourcing[] = {0,0,1}; 
  digitalWrite( Pins.phase_a_source, to_sourcing[CurrentState.phase_a] );
  digitalWrite( Pins.phase_b_source, to_sourcing[CurrentState.phase_b] );
  digitalWrite( Pins.phase_c_source, to_sourcing[CurrentState.phase_c] );

  const uint8_t to_sinking[] = {0,1,0}; 
  digitalWrite( Pins.phase_a_sink, to_sinking[CurrentState.phase_a] );
  digitalWrite( Pins.phase_b_sink, to_sinking[CurrentState.phase_b] );
  digitalWrite( Pins.phase_c_sink, to_sinking[CurrentState.phase_c] );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void setup()
{
  pinMode( Pins.sense_a, INPUT );
  pinMode( Pins.sense_b, INPUT );
  pinMode( Pins.sense_c, INPUT );
  pinMode( Pins.phase_a_source, OUTPUT );
  pinMode( Pins.phase_a_sink, OUTPUT );
  pinMode( Pins.phase_b_source, OUTPUT );
  pinMode( Pins.phase_b_sink, OUTPUT );
  pinMode( Pins.phase_c_source, OUTPUT );
  pinMode( Pins.phase_c_sink, OUTPUT );
  makeSenseStateToCommutatioTable();
  makeSafe();

  //pinMode( Pins.dir_output, OUTPUT );
  //pinMode( Pins.dir_input, INPUT_PULLUP );
  //digitalWrite( Pins.dir_output, HIGH );

 // if( digitalRead(Pins.dir_input) )
 // {
  //  CurrentDirection = CLOCKWISE;
  //}
 // else
 // {
  //  CurrentDirection = ANTI_CLOCKWISE;
 // }

 CurrentDirection = ANTI_CLOCKWISE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void loop() 
{
  if( readState() )
  {
    makeSafe();
    setNewState();
  }
}