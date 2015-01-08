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

#ifndef LCDi2c_PCF2119x_h
#define LCDi2c_PCF2119x_h


#include <inttypes.h>

#include "Print.h"


class LCDi2c_PCF2119x : public Print {
	
public: 
	

    LCDi2c_PCF2119x( uint8_t i2c_addr, uint8_t num_col, uint8_t num_lines, char charset='R');

    void init();

	void command(uint8_t value);
	
    void begin(int cols,int rows);
	
    virtual size_t write(uint8_t);
	
	void clear();
	
	void home();
	
    void display();
	
    void noDisplay();
	
    void cursor();
	
    void noCursor();
	
    void blink();
	
    void noBlink();
	
    void setCursor(uint8_t col, uint8_t row );

    void autoscroll();
    void noAutoscroll();

    void leftToRight();
    void rightToLeft();

    void scrollDisplayLeft();
    void scrollDisplayRight();

    void createChar(uint8_t char_num, uint8_t *rows) { } // not implemented yet

//----------------------------------------------------------------------------------
// LCD API 1.0 4-3-2009 by dale@wentztech.com
//----------------------------------------------------------------------------------
    void on()  { display(); }
    void off() { noDisplay(); }
    void blink_on()  { blink(); }
    void blink_off() { noBlink(); }
    void cursor_on() { cursor(); }
    void cursor_off(){ noCursor(); }
    void printstr(const char c[] ) { print(c);} /// @todo make more efficient version
    uint8_t status(){return isBusy();}

//-- Unsupported functions ---------------------------------------------------------
    uint8_t keypad (){return 0;}
    uint8_t init_bargraph(uint8_t graphtype){return 0;}
    void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
    void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
    void setContrast(uint8_t new_val){}

    void setBacklight(uint8_t new_val) { }
    void setDelay (int cmdDelay,int charDelay) {}
    void load_custom_character(uint8_t char_num, uint8_t *rows) { createChar(char_num, rows);}

//----------------------------------------------------------------------------------
// specific controller functions
//----------------------------------------------------------------------------------
    void setAddressPoint(uint8_t newAddr);
    uint8_t getAddressPoint();

    void cursorLeft();
    void cursorRight();

    void normalHorizontalOrientation();
    void normalVerticalOrientation();
    void reverseHorizontalOrientation();
    void reverseVerticalOrientation();

#ifdef DEBUG
// Only for debug. Dump to serial DDRAM content
// Serial must be initied on main program
    void dump();
#endif

private:

    enum blink_mode { blkoff = 0,blkon = 1} ;
    enum cursor_mode { crsroff = 0,crsron = 2};
    enum active_mode { actvoff = 0,actvron = 4};
    enum horizontal_orientation { horizontal_normal = 0,horizontal_reverse = 2};
    enum vertical_orientation { vertical_normal = 0,vertical_reverse = 1};
    enum scroll_mode { scrolloff = 0,scrollon = 1} ;
    enum addr_move { addr_dec = 0,addr_inc = 2} ;

    uint8_t m_num_lines;
    uint8_t m_num_col;
    uint8_t m_i2caddress;

    blink_mode m_actual_blink;
    cursor_mode m_actual_cursor;
    active_mode m_actual_active;
    horizontal_orientation m_actual_horizontal_orientation;
    vertical_orientation m_actual_vertical_orientation;
    scroll_mode m_actual_scroll;
    addr_move m_actual_addr_move;

    int m_actual_write;

    char m_charset;

    uint8_t isBusy();
    uint8_t waitBusy();
    void setDisplayControl();
    void setDisplayConfig();
    void setEntryMode();

    unsigned char ASCIItoLCD(unsigned char ch);
	
};

#endif

