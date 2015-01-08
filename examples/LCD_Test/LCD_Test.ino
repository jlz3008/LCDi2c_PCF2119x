/*
 Copyright © 2014 José Luis Zabalza  License LGPLv3+: GNU
 LGPL version 3 or later <http://www.gnu.org/copyleft/lgpl.html>.
 This is free software: you are free to change and redistribute it.
 There is NO WARRANTY, to the extent permitted by law.
*/
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
// []
// []       i2c LCD library Display Test Demo
// []	    Based on a  dale@wentztech.com work
// []
// []
// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]


#define VERSION "1.1"

#include <inttypes.h>
#include <Wire.h>

#include <LCDi2c_PCF2119x.h>

LCDi2c_PCF2119x lcd = LCDi2c_PCF2119x(0x3B,16,2);

uint8_t rows = 2;
uint8_t cols = 16;

void lcdtest_basic();
void Cursor_Type();
void Count_Numbers();
void Characters();
void Every_Line(int lines);
void Every_Pos(int lines,int cols);
void Orientation_test();
void Autoscroll_test();
void ShiftAndCursorMove_test();
void RightAndLeft_test();


void setup()
{
#ifdef DEBUG
    Serial.begin(9600);  // start serial for output for debug
    Serial.println("Start");
#endif

    lcd.init();                          // Init the display, clears the display

    lcd.print("Hello World!");       // Classic Hello World!
    delay(1000);
}

void loop()
{
    lcdtest_basic();
}

void lcdtest_basic()
{

    lcd.clear();
    lcd.print ("Cursor Test");
    delay(1000);
    Cursor_Type();

    lcd.clear();
    lcd.print("Characters Test");
    delay(1000);
    Characters();
    delay(1000);

    lcd.clear();
    lcd.print("Every Line");
    delay(1000);
    Every_Line(rows);
    delay(1000);

    lcd.clear();
    lcd.print("Every Position");
    delay(1000);
    Every_Pos(rows,cols);
    delay(1000);

    Orientation_test();

    Autoscroll_test();

    ShiftAndCursorMove_test();

    RightAndLeft_test();

}

void Cursor_Type()
{
    lcd.setCursor(0,0);
    lcd.print("Underline Cursor");
    lcd.setCursor(0,1);
    lcd.cursor();
    delay(1000);
    lcd.noCursor();
    lcd.setCursor(0,0);

    lcd.print("Block Cursor    ");
    lcd.setCursor(0,1);
    lcd.blink();
    delay(1000);
    lcd.noBlink();
    lcd.setCursor(0,0);

    lcd.print("No Cursor      ");
    lcd.setCursor(0,1);
    delay(1000);
}

void Count_Numbers()
{
    lcd.clear();
    lcd.print("Count to 255");

    for (int i=0;i<255;i++)
    {
        lcd.setCursor(0,1);

        lcd.print(i,DEC);

        lcd.setCursor(7,1);

        lcd.print(i,BIN);

        // delay(10);
    }
}

void Characters()
{
    int  chartoprint=48;

    lcd.clear();

    for(int i=0 ; i < rows ; i++)
    {
        for(int j=0 ; j < cols ; j++)
        {
            lcd.setCursor(j,i);
            lcd.print(char(chartoprint));
            chartoprint++;
            if(chartoprint == 127)
                return;
        }
    }
}


void Fancy_Clear()
{
    for (int i=0 ; i < rows ; i++)
    {
        for(int j=0 ; j < cols/2 ; j++)
        {
            lcd.setCursor(j,i);
            lcd.print(" ");

            lcd.setCursor(cols - i, j);
            lcd.print(" ");
        }
        //delay(10);
    }
}

void Every_Line(int lines)
{
    lcd.clear();
    for(int i=0 ; i < lines ; i++)
    {
        lcd.setCursor(0,i);
        lcd.print("Line : ");
        lcd.print(i,DEC);
    }
}

void Every_Pos(int lines,int cols)
{
    lcd.clear();

    for(int i=0 ; i < lines ; i++)
    {
        for(int j=0 ; j< cols ; j++)
        {
            lcd.setCursor(j,i);
            lcd.print(i,DEC);
        }
    }
}

void Orientation_test()
{
    lcd.clear();
    lcd.normalHorizontalOrientation();
    lcd.normalVerticalOrientation();
    lcd.print("Normal orientation");
    delay(2000);

    lcd.noDisplay();
    lcd.clear();
    lcd.print("Reverse horizontal orientation");
    lcd.reverseHorizontalOrientation();
    lcd.display();
    delay(2000);

    lcd.noDisplay();
    lcd.clear();
    lcd.print("Normal orientation again");
    lcd.normalHorizontalOrientation();
    lcd.display();
    delay(2000);

    lcd.noDisplay();
    lcd.clear();
    lcd.print("Reverse vertical orientation");
    lcd.reverseVerticalOrientation();
    lcd.display();
    delay(2000);

    lcd.noDisplay();
    lcd.clear();
    lcd.print("Reverse Horizontal and Vertical");
    lcd.reverseHorizontalOrientation();
    lcd.display();
    delay(2000);

    lcd.normalHorizontalOrientation();
    lcd.normalVerticalOrientation();
    lcd.clear();
}


void Autoscroll_test()
{
    lcd.blink();

    lcd.print("noAutoscroll");
    for(char i='A';i<'J';i++)
    {
        lcd.print(i);
        delay(300);
    }

    lcd.autoscroll();


    lcd.print("-A-");

    for(char i='a';i<'z';i++)
    {
        lcd.print(i);
        delay(300);
    }

    for(char i='A';i<'Z';i++)
    {
        lcd.print(i);
        delay(300);
    }

    for(char i='0';i<'9';i++)
    {
        lcd.print(i);
        delay(300);
    }
    delay(2000);

    lcd.clear();
    lcd.noAutoscroll();
    lcd.print("noAutoscroll again");

    for(char i='A';i<'Z';i++)
    {
        lcd.print(i);
        delay(200);
    }

    lcd.noBlink();
}

void ShiftAndCursorMove_test()
{
    lcd.blink();
    lcd.clear();

    lcd.print("Hello again ");

    delay(1000);

    for(int i=0;i<5;i++)
    {
        lcd.scrollDisplayRight();
        delay(300);
    }

    lcd.print("and again");
    delay(1000);

    for(int i=0;i<5;i++)
    {
        lcd.cursorLeft();
        delay(300);
    }

    lcd.print("overwrite");
    delay(1000);

    for(int i=0;i<5;i++)
    {
        lcd.scrollDisplayLeft();
        delay(300);
    }
    delay(1000);

    for(int i=0;i<4;i++)
    {
        lcd.cursorRight();
        delay(300);
    }

    delay(1000);

    lcd.print("end");
    delay(1000);

    lcd.noBlink();

}

void RightAndLeft_test()
{
    lcd.blink();
    lcd.clear();

    for(char i='A';i<'J';i++)
    {
        lcd.print(i);
        delay(500);
    }

    lcd.cursorLeft();
    lcd.rightToLeft();

    for(char i='i';i>='a';i--)
    {
        lcd.print(i);
        delay(500);
    }

    lcd.cursorRight();
    lcd.leftToRight();

    for(char i='A';i<'J';i++)
    {
        lcd.print(i);
        delay(500);
    }




    lcd.noBlink();

}

