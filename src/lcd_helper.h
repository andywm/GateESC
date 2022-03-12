/*------------------------------------------------------------------------------
    ()      File:   lcd_helper.h
    /\      Copyright (c) 2021 Andrew Woodward-May
   //\\       
  //  \\    Description:
              Display helper utility.
------------------------------
------------------------------
License Text - The MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------*/
#pragma once
#include <stdint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <printf.h> // AVR LIBC sprintf is godawful.

template<typename Type>
static inline const Type& DMax( const Type& A, const Type& B)
{
    return (A > B)? A : B;
}

template<typename Type>
static inline const Type& DMin( const Type& A, const Type& B)
{
    return (A < B)? A : B;
}

//------------------------------------------------------------------------------
// Display Helper
//------------------------------------------------------------------------------
template<uint8_t Width, uint8_t Height, uint8_t MaxReserved = 2>
class Display_T
{
public:
    Display_T( int8_t i2cChannel = 1 )
        : m_i2c(/*i2cChannel*/) //some devices have multiple i2c channels, need to wrap this in a define.
        , m_lcd(0x27, Width, Height, LCD_5x8DOTS)
    {
        memset( &m_state[0], '\0', Height*(Width+1) -1 );
        clear();
    }

    void begin()
    {
      m_lcd.begin();
	  m_lcd.backlight();
    }

    void reserve( uint8_t id, uint8_t line, uint8_t begin_inclusive, uint8_t end_inclusive )
    {
        if( id >= MaxReserved )
           return;

        m_ranges[id].line = line;
        m_ranges[id].begin = begin_inclusive;
        m_ranges[id].end = end_inclusive;
    }

    void update( uint8_t id, const char * str )
    {
        const uint8_t line = m_ranges[id].line;
        const uint8_t begin = m_ranges[id].begin;
        const uint8_t end = m_ranges[id].end;

        const int range = end-begin+1;
        const uint8_t size = DMin(range, (int)strlen(str));

        //fill
        memset(&(m_state[line][begin]), ' ', range );
        strncpy(&(m_state[line][begin]), str, size );
    }

    void draw()
    {
        m_lcd.clear();
        for(int line=0; line<Height; line++)
        {
          m_lcd.setCursor(0,line);
          m_lcd.printstr(&m_state[line][0]);
        }

      // m_lcd.setCursor(0,0);
      // m_lcd.printstr(&m_state[0][0]);

       // m_lcd.print("Hello World");

    }

    void clear()
    {
        for(int line=0; line < Height; line++)
        {
            memset( &m_state[line], ' ', Width );
        }
    }

private:
  struct Range 
  { 
      uint8_t line = 0;
      uint8_t begin =0;
      uint8_t end =0;
  } m_ranges[MaxReserved];

  TwoWire m_i2c;
  LiquidCrystal_I2C m_lcd;
  char m_state[Height][Width+1];
};

template<uint8_t Slots = 2>
class Display_16by2 : public Display_T<16, 2, Slots> {};