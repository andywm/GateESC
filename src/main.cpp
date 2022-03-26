#include <Arduino.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>
//#include "lcd_helper.h"
#include "framework.h"
#include "debug.h"

#define USERMAIN

int main(int argc, char**argv)
{
	Framework f;
	f.Run();
}