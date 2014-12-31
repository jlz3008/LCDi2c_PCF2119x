/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Implement LCD API Version 1.0 4-3-2009 by dale@wentztech.com
// []   for PCF2119x controller LCD based
// []
// []   This library is based on a TontonJules work
// []   http://forum.arduino.cc/index.php?topic=33427.0
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

#include <Wire.h>

#include <string.h>			//needed for strlen()

#include <inttypes.h>

#include <Arduino.h>		//all things wiring / arduino
  
#include "LCDi2c_PCF2119x.h"
  

#define CMDDELAY   165        // Default Delay to wait after sending commands;
#define POSDELAY   10         // Long delay required by Position command
#define CHARDELAY  0          // Default char delay.


// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Class Constructor 
// []
// []	num_lines = 1-4
// []   num_col = 1-80
// []  
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

LCDi2c_PCF2119x::LCDi2c_PCF2119x(uint8_t num_lines, uint8_t num_col, uint8_t i2c_addr, char charset)
{
    Wire.begin();

    m_num_lines = num_lines;
    m_num_col = num_col;
    m_i2caddress = i2c_addr;
    m_charset = charset;

    m_cmdDelay = CMDDELAY;
    m_charDelay = CHARDELAY;
    m_actual_blink = blkoff;
    m_actual_cursor = crsroff;
    m_actual_active = actvron;
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	initiatize lcd after a short pause
// []
// []	
// []   Put the display in some kind of known mode
// []   Put the cursor at 0,0
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]


void LCDi2c_PCF2119x::init ()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x30); // command: functions (1x32, basic set)
    Wire.write(0x06); // command: entry mode (increment, no shift)
    Wire.write(0x14); // command: Curs_disp_shift (move cursor, right)

    Wire.write(0x31); // command: functions (1x32, extended instruction set)
    Wire.write(0x04); // command: Disp_conf (left-to-right, top-to-bottom)
    Wire.write(0x10); // command: Temp_ctl (TC1=0, TC2=0) Display depended
    Wire.write(0x42); // command: HV_gen  (HV stage 3) Display depended
    Wire.write(0x9F); // command: VLCDset (set VLCD, store VA) Display depended

    Wire.write(0x30); // command: functions (1x32, basic set)
    Wire.write(0x80); // command: DDRAM address = 0x00
    Wire.write(0x02); // command: return home
    Wire.endTransmission();
    delay(m_cmdDelay);

    clear();
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Override the default delays used to send commands to the display
// []
// []	The default values are set by the library
// []   this allows the programer to take into account code delays
// []   and speed things up.
// []   
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::setDelay (int cmdDelay,int charDelay)
{
    m_cmdDelay = cmdDelay;
    m_charDelay = charDelay;
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []   Send a command to the display. 
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::command(uint8_t value)
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(value); // command:
    Wire.endTransmission();
    delay(m_cmdDelay);
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []   Send a command to the display. 
// []
// []	This is also used by the print, and println
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

size_t LCDi2c_PCF2119x::write(uint8_t value)
{

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x40);
    Wire.write(ASCIItoLCD(value));
    Wire.endTransmission();
    delay(m_charDelay);
    return(1);
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Clear the display, and put cursor at 0,0 
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::clear()
{
    if(m_charset == 'R')
    {
/* ======================================================================================
        When using character set ‘R’, the following alternative instruction set has to be used:
        1. Switch display off (Display_ctl, bit D = 0).
        2. Write a blank pattern into all DDRAM addresses (Write_data).
        3. Switch display on (Display_ctl, bit D = 1).
    ======================================================================================   */
        active_mode last_active_mode = m_actual_active;

        off();

        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x00); // control byte
        Wire.write(0x80); // Set_DDRAM
        Wire.endTransmission();

        delay(m_charDelay);

        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x40); // control byte Write Data Register
        for(int i=0;i<31;i++)
            Wire.write(0x91); // White character
        Wire.endTransmission();

        delay(m_charDelay);

// I don't know why but last character is not write on 1 line mode.
        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x00); // control byte
        Wire.write(0x9F); // Set_DDRAM
        Wire.endTransmission();

        delay(m_charDelay);

        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x40); // control byte Write Data Register
        Wire.write(0x91); // White character
        Wire.endTransmission();

        delay(m_charDelay);

/* ========== 2 line mode ===========================
        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x40); // control byte Write Data Register
        for(int i=0;i<16;i++)
            Wire.write(0x91);
        Wire.endTransmission();

        delay(m_charDelay);

        Wire.beginTransmission(m_i2caddress );
        Wire.write(0x00); // control byte
        Wire.write(0xC0); // Set_DDRAM
        Wire.endTransmission();

        delay(m_charDelay);

        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x40); // control byte Write Data Register
        for(int i=0;i<16;i++)
            Wire.write(0x91);
        Wire.endTransmission();

        delay(m_charDelay);
*/
        home();
        if(last_active_mode == actvron)
            on();
    }
    else
        command(1); // Clear_display
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Home to custor to 0,0
// []
// []	Do not disturb data on the displayClear the display
// []
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::home()
{
    command(2); // Return_home
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn on the display
// []
// []	Depending on the display, might just turn backlighting on
// []
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::on()
{
    m_actual_active = actvron;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn off the display
// []
// []	Depending on the display, might just turn backlighting off
// []
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::off()
{
    m_actual_active = actvoff;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn on the underline cursor
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::cursor_on()
{
    m_actual_cursor = crsron;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn off the underline cursor
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::cursor_off()
{
    m_actual_cursor = crsroff;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn on the blinking block cursor
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::blink_on()
{
    m_actual_blink = blkon;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Turn off the blinking block cursor
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

void LCDi2c_PCF2119x::blink_off()
{
    m_actual_blink = blkoff;
    setDisplayControl();
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Position the cursor to position line,column
// []
// []	line is 0 - Max Display lines
// []	column 0 - Max Display Width
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]


void LCDi2c_PCF2119x::setCursor(uint8_t line_num, uint8_t x)
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00);
    Wire.write(0x80+16*line_num+x);
/* ========== 2 line mode
    if (line_num==0)
       Wire.write(0x80+x);
    else
       Wire.write(0xC0+x);
====================*/
    Wire.endTransmission();
    delay(POSDELAY);
}

//----------------------------------------------------------------------------------
// Private API
//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setDisplayControl()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x08 | m_actual_active | m_actual_cursor | m_actual_blink); // command: display control (display on, cursor off, blink off)
    Wire.endTransmission();
    delay(m_cmdDelay);
}
//----------------------------------------------------------------------------------
unsigned char LCDi2c_PCF2119x::ASCIItoLCD(unsigned char ch)
{
   unsigned char c=ch;
   if(m_charset == 'R')
   {
       if ( ((ch >= 0x20) && (ch <= 0x3F)) || ((ch >= 0x41) && (ch <= 0x5A)) || ((ch >= 0x61) && (ch <= 0x7A)) )
           c = 0x80 + ch;
   }
   return c;
}

//----------------------------------------------------------------------------------
// Expanded API not implemented yet
//----------------------------------------------------------------------------------
#ifdef _LCDEXPANDED

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Return the status of the display
// []
// []	Does nothing on this display
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]	

uint8_t LCDi2c_PCF2119x::status()
{
	return 0;
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Read data from keypad
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

uint8_t LCDi2c_PCF2119x::keypad ()
{

	
}

// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Load data for a custom character
// []
// []	Char = custom character number 0-7
// []	Row is array of chars containing bytes 0-7
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]


void LCDi2c_PCF2119x::load_custom_character(uint8_t char_num, uint8_t *rows)
{


}

void LCDi2c_PCF2119x::setBacklight(uint8_t new_val)
{
	
}

void LCDi2c_PCF2119x::setContrast(uint8_t new_val)
{
	
}

#endif
