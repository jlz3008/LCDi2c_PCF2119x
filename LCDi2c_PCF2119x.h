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

#ifndef LCDi2c_PCF2119x_h
#define LCDi2c_PCF2119x_h


//#define _LCDEXPANDED				// If defined turn on advanced functions

#include <inttypes.h>

#include "Print.h"


class LCDi2c_PCF2119x : public Print {
	
public: 
	

    LCDi2c_PCF2119x(uint8_t num_lines, uint8_t num_col, uint8_t i2c_addr, char charset='R');
	
	void command(uint8_t value);
	
	void init();
	
	void setDelay(int,int);
	
    virtual size_t write(uint8_t);
	
	void clear();
	
	void home();
	
	void on();
	
	void off();
	
	void cursor_on();
	
	void cursor_off();
	
	void blink_on();
	
	void blink_off();
	
	void setCursor(uint8_t Line, uint8_t Col );

    void leftToRight();
    void rightToLeft();
    void topToBottom();
    void bottomToTop();

private:

    enum blink_mode { blkoff = 0,blkon = 1} ;
    enum cursor_mode { crsroff = 0,crsron = 2};
    enum active_mode { actvoff = 0,actvron = 4};
    enum horizontal_orientation { left2right = 0,right2left = 2};
    enum vertical_orientation { top2bottom = 0,bottom2top = 1};

    uint8_t m_num_lines;
    uint8_t m_num_col;
    uint8_t m_i2caddress;

    blink_mode m_actual_blink;
    cursor_mode m_actual_cursor;
    active_mode m_actual_active;
    horizontal_orientation m_actual_horizontal_orientation;
    vertical_orientation m_actual_vertical_orientation;

    int m_actual_write;

    char m_charset;


    int m_cmdDelay ;
    int m_charDelay;

    void setDisplayControl();
    void setDisplayConfig();

    unsigned char ASCIItoLCD(unsigned char ch);


public:
	// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
	// []
	// []	Extended Functions
	// []
	// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
	
	
#ifdef _LCDEXPANDED		

	
	uint8_t status();
	
	void load_custom_character(uint8_t char_num, uint8_t *rows);
	
	uint8_t keypad();
	
	void printstr(const char[]);
	 
	void setBacklight(uint8_t new_val);
	
	void setContrast(uint8_t new_val);
	 
		
#endif
	
private:
	
	
};

#endif

