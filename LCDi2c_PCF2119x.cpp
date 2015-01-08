/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []	Implement LCD LiquidCrystall API Version
// []   for PCF2119x controller LCD based
// []   LCD API 1.0 4-3-2009 by dale@wentztech.com compliant
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
  
//----------------------------------------------------------------------------------
LCDi2c_PCF2119x::LCDi2c_PCF2119x(uint8_t i2c_addr, uint8_t num_col, uint8_t num_lines, char charset)
{
    Wire.begin();

    m_num_lines = num_lines;
    m_num_col = num_col;
    m_i2caddress = i2c_addr;
    m_charset = charset;

    m_actual_blink = blkoff;
    m_actual_cursor = crsroff;
    m_actual_active = actvron;
    m_actual_horizontal_orientation = horizontal_normal;
    m_actual_vertical_orientation = vertical_normal;
    m_actual_scroll = scrolloff;
    m_actual_addr_move = addr_inc;
}

//----------------------------------------------------------------------------------------
void LCDi2c_PCF2119x::init()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x30); // command: functions (1x32, basic set)
    Wire.write(0x04 | m_actual_addr_move | m_actual_scroll ); // command: entry mode (increment, no shift)
    Wire.write(0x14); // command: Curs_disp_shift (move cursor, right)

    Wire.write(0x31); // command: functions (1x32, extended instruction set)
    Wire.write(0x04 | m_actual_horizontal_orientation | m_actual_vertical_orientation ); // command: Disp_conf
    Wire.write(0x10); // command: Temp_ctl (TC1=0, TC2=0) Display depended
    Wire.write(0x42); // command: HV_gen  (HV stage 3) Display depended
    Wire.write(0x9F); // command: VLCDset (set VLCD, store VA) Display depended

    Wire.write(0x30); // command: functions (1x32, basic set)
    Wire.write(0x80); // command: DDRAM address = 0x00
    Wire.write(0x02); // command: return home
    Wire.endTransmission();
    waitBusy();

    clear();
    setDisplayControl();
}

//----------------------------------------------------------------------------------------
void LCDi2c_PCF2119x::begin(int cols,int rows)
{
    m_num_col   = cols;
    m_num_lines = rows;
    init();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::command(uint8_t value)
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(value); // command:
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
size_t LCDi2c_PCF2119x::write(uint8_t value)
{

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x40);
    Wire.write(ASCIItoLCD(value));
    Wire.endTransmission();
    waitBusy();
    return(1);
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::clear()
{
    if(m_charset == 'R')
    {
/* ==================[From datasheet]====================================================
        When using character set ‘R’, the following alternative instruction set has to be used:
        1. Switch display off (Display_ctl, bit D = 0).
        2. Write a blank pattern into all DDRAM addresses (Write_data).
        3. Switch display on (Display_ctl, bit D = 1).
    ======================================================================================   */
        active_mode last_active_mode = m_actual_active;

        noDisplay();

// Now we erase all 80 characters area because the no visible area
// is used on autoscroll mode
// Maximun bytes on a transmision = 32 so we trunk on 3 blocks to write 80 bytes
        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x80); // control byte. A control byte after next instruction
        Wire.write(0x80); // Set_DDRAM = 0x00
        Wire.write(0x40); // control byte Select Data Register

        for(int i=0;i<0x1B;i++)
            Wire.write(0x91); // White character
        Wire.endTransmission();

        waitBusy();
        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x80); // control byte. A control byte after next instruction
        Wire.write(0x9B); // Set_DDRAM = 0x1B
        Wire.write(0x40); // control byte Select Data Register

        for(int i=0;i<0x1B;i++)
            Wire.write(0x91); // White character
        Wire.endTransmission();

        waitBusy();
        Wire.beginTransmission(m_i2caddress);
        Wire.write(0x80); // control byte. A control byte after next instruction
        Wire.write(0xB6); // Set_DDRAM = 0x26
        Wire.write(0x40); // control byte Select Data Register

        for(int i=0;i<0x1A;i++)
            Wire.write(0x91); // White character
        Wire.endTransmission();

        waitBusy();

        home();

        if(last_active_mode == actvron)
            display();
    }
    else
        command(1); // Clear_display
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::home()
{
    command(2); // Return_home
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::display()
{
    m_actual_active = actvron;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::noDisplay()
{
    m_actual_active = actvoff;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::cursor()
{
    m_actual_cursor = crsron;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::noCursor()
{
    m_actual_cursor = crsroff;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::blink()
{
    m_actual_blink = blkon;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::noBlink()
{
    m_actual_blink = blkoff;
    setDisplayControl();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setCursor(uint8_t col, uint8_t row)
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00);
    Wire.write(0x80+16*row+col);
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::autoscroll()
{
// scroll area is all 80 character buffer not only visible area
    m_actual_scroll = scrollon;
    setEntryMode();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::noAutoscroll()
{
    m_actual_scroll = scrolloff;
    setEntryMode();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::leftToRight()
{
    m_actual_addr_move = addr_inc;
    setEntryMode();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::rightToLeft()
{
    m_actual_addr_move = addr_dec;
    setEntryMode();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::scrollDisplayLeft()
{
// Curs_disp_shift on shift operation don't modify either address pointer or data memory
// only change LCD internal index visible area.
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x18); // command: Curs_disp_shift shift to left
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::scrollDisplayRight()
{
// Curs_disp_shift on shift operation don't modify either address pointer or data memory
// only change internal index to visible area.
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x1C); // command: Curs_disp_shift shift to right
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::createChar(uint8_t char_num, uint8_t *rows)
{
    char_num &=0x0F;
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte

    /* ============= From Datasheet ============================
     * Only bit 0 to bit 5 of the CGRAM address are set by the Set_CGRAM command.
     * Bit 6 can be set using the Set_DDRAM command in the valid address range or
     * by using the auto-increment feature during CGRAM write.
     ===========================================================*/

    if(char_num >= 8)
        Wire.write(0x80 | (char_num  * 8)); // command: set bit6 CGRAM Address via set set_DDRAM command

    Wire.write(0x40 | ((char_num  * 8) & B00111111)); // command: set CGRAM Address

    Wire.endTransmission();

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x40); // control byte select data register
    for(int i=0;i<8;i++)
        Wire.write(rows[i]);
    Wire.endTransmission();

    waitBusy();

}

// Only for debug
#ifdef DEBUG
//----------------------------------------------------------------------------------------
void LCDi2c_PCF2119x::dump()
{
// Backup address point
    uint8_t addpnt = getAddressPoint();

    waitBusy();
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x80); // control byte. A control byte after next instruction
    Wire.write(0x80); // command: DDRAM address = 0x00
    Wire.write(0x40); // control byte Select Data Register
    Wire.endTransmission();

    Wire.requestFrom(m_i2caddress, uint8_t( 32)); // maximun transmission 32 bytes
    for(int i=1;Wire.available();i++)
    {
        char c = Wire.read();
        Serial.print(uint8_t(c),HEX);
        if(i%16 == 0)
            Serial.println("");
        else
            Serial.print(" ");
    }

    waitBusy();
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x80); // control byte. A control byte after next instruction
    Wire.write(0xA0); // command: DDRAM address = 0x20
    Wire.write(0x40); // control byte Select Data Register
    Wire.endTransmission();

    Wire.requestFrom(m_i2caddress, uint8_t( 32));
    for(int i=1;Wire.available();i++)
    {
        char c = Wire.read();
        Serial.print(uint8_t(c),HEX);
        if(i%16 == 0)
            Serial.println("");
        else
            Serial.print(" ");
    }

    waitBusy();
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x80); // control byte. A control byte after next instruction
    Wire.write(0xC0); // command: DDRAM address = 0x40
    Wire.write(0x40); // control byte Select Data Register
    Wire.endTransmission();

    Wire.requestFrom(m_i2caddress, uint8_t( 16));
    for(int i=1;Wire.available();i++)    // display turn around 80 bytes DDRAM
    {                                    // so we only read 16 bytes
        char c = Wire.read();
        Serial.print(uint8_t(c),HEX);
        if(i%16 == 0)
            Serial.println("");
        else
            Serial.print(" ");
    }

// Restore address point
    setAddressPoint(addpnt);
}
#endif
//----------------------------------------------------------------------------------
// Specific controller API
//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setAddressPoint(uint8_t newAddr)
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x80 | (newAddr & 0x7F)); // command: DDRAM address = newAddr
    Wire.endTransmission();
}

//----------------------------------------------------------------------------------------
uint8_t LCDi2c_PCF2119x::getAddressPoint()
{
    uint8_t Result=0xFF;

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte Select Instruction Register
    Wire.endTransmission();

    Wire.requestFrom(m_i2caddress, uint8_t( 1));
    if(Wire.available())
    {
        Result = Wire.read() & 0x7F;    // receive a byte as character
    }

    return(Result);
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::cursorLeft()
{
// Curs_disp_shift on move cursor operation  modify address pointer to correspond to the
// character under cursor.
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x10); // command: Curs_disp_shift move cursor to left
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::cursorRight()
{
// Curs_disp_shift on move cursor operation  modify address pointer to correspond to the
// character under cursor.
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x14); // command: Curs_disp_shift move cursor to right
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::normalHorizontalOrientation()
{
    m_actual_horizontal_orientation = horizontal_normal;
    setDisplayConfig();
}
//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::reverseHorizontalOrientation()
{
    m_actual_horizontal_orientation = horizontal_reverse;
    setDisplayConfig();
}
//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::normalVerticalOrientation()
{
    m_actual_vertical_orientation = vertical_normal;
    setDisplayConfig();
}
//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::reverseVerticalOrientation()
{
    m_actual_vertical_orientation = vertical_reverse;
    setDisplayConfig();
}

//----------------------------------------------------------------------------------
// Private API
//----------------------------------------------------------------------------------------
uint8_t LCDi2c_PCF2119x::isBusy()
{
    uint8_t Result = 0x80;

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte select Instruction Register
    Wire.endTransmission();

    Wire.requestFrom(m_i2caddress, uint8_t( 1));
    if(Wire.available())
    {
        Result = Wire.read() & 0x80;
    }
    return(Result);
}

//----------------------------------------------------------------------------------------
uint8_t LCDi2c_PCF2119x::waitBusy()
{
#define MAXRETRY    10
    int i;
    uint8_t Result;

    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte select Instruction Register
    Wire.endTransmission();

    for(i = 0,Result = 0x80 ;
        Result && (i < MAXRETRY) ;
        i++)
    {
        Wire.requestFrom(m_i2caddress, uint8_t( 1));
        if(Wire.available())
        {
            Result = Wire.read() & 0x80;
#ifdef DEBUG
            if( Result & 0x80)
                Serial.println("Busy");
#endif
        }
    }
    return(Result);
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setDisplayControl()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x08 | m_actual_active | m_actual_cursor | m_actual_blink); // command: display control (display on, cursor off, blink off)
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setDisplayConfig()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x31); // command: functions (1x32, extended instruction set)
    Wire.write(0x04 | m_actual_horizontal_orientation | m_actual_vertical_orientation ); // command: Disp_conf
    Wire.write(0x30); // command: functions (1x32, extended instruction set)
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
void LCDi2c_PCF2119x::setEntryMode()
{
    Wire.beginTransmission(m_i2caddress);
    Wire.write(0x00); // control byte
    Wire.write(0x04 | m_actual_scroll | m_actual_addr_move); // command: entry mode (increment, no shift)
    Wire.endTransmission();
    waitBusy();
}

//----------------------------------------------------------------------------------
unsigned char LCDi2c_PCF2119x::ASCIItoLCD(unsigned char ch)
{
    /// @todo revise for others charsets
   unsigned char c=ch;
   if(m_charset == 'R' || m_charset == 'F')
   {
       if ( ((ch >= 0x20) && (ch <= 0x3F)) || ((ch >= 0x41) && (ch <= 0x5A)) || ((ch >= 0x61) && (ch <= 0x7A)) )
           c = 0x80 + ch;
   }
   return c;
}

