#include <Arduino.h>

struct State
{
  uint32_t high;
  uint32_t ground;

  uint8_t sequence_pointer;
}g_state;

struct Phase
{
    const uint32_t high_pin; 
    const uint32_t ground_pin; 
};

namespace phase
{
  enum ID { A, B, C };

  struct SequenceElement
  {
    ID high;
    ID low;
  };

  //digital pins for phases.
  constexpr Phase phases[] = { { 2, 3 }, { 4, 5 }, { 6, 7 } };

  //3 phase bldc sequencing - clockwise - CA CB AB AC BC BA
  constexpr SequenceElement sequencing[] = { {C, A}, {C, B}, {A, B}, {A, C}, {B, C}, {B, A} };

  //3 phase bldc sequencing - anti-clockwise - AB CB CA BA BC AC

}

void setup() 
{
  for( int i = 0; i < 3; ++i )
  {
    pinMode(phase::phases[i].high_pin, OUTPUT); 
    pinMode(phase::phases[i].ground_pin, OUTPUT); 
  }

  const phase::SequenceElement& first = phase::sequencing[g_state.sequence_pointer];
  g_state.high = phase::phases[first.high].high_pin;
  g_state.ground = phase::phases[first.low].ground_pin;
  g_state.sequence_pointer = 0;
}

void setActivePhases(const Phase& asHighPhase, const Phase& asGroundPhase)
{
  digitalWrite(g_state.high, LOW);
  digitalWrite(g_state.ground, LOW);

  g_state.high = asHighPhase.high_pin;
  g_state.ground = asGroundPhase.ground_pin;

  digitalWrite(g_state.high, HIGH);
  digitalWrite(g_state.ground, HIGH);
}

void loop() 
{
  const phase::SequenceElement& element = phase::sequencing[g_state.sequence_pointer++];
  setActivePhases(phase::phases[element.high], phase::phases[element.low]);

  if( g_state.sequence_pointer >= 6 )
  {
    g_state.sequence_pointer = 0;
  }

  delay(100);
}