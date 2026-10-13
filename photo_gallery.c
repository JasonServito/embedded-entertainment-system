#include <LPC17xx.H>
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include "photos.h"

extern int display_images(int image);

void draw_photo_static() {
    GLCD_Clear(Black);   // simple dark background

    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(Yellow);
    GLCD_DisplayString(0, 0, 1, "   PHOTO VIEWER   ");

    GLCD_SetTextColor(White);
	GLCD_DisplayString(2, 3, 1, "Choose an Image:");

    GLCD_SetTextColor(Yellow);
    GLCD_DisplayString(3, 0, 1, "----------------------");
}

void draw_photo_menu(uint8_t idx) {

    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(White);

    // clear menu region
    GLCD_DisplayString(5, 2, 1, "                ");
    GLCD_DisplayString(6, 2, 1, "                ");
    GLCD_DisplayString(7, 2, 1, "                ");
    GLCD_DisplayString(8, 2, 1, "                ");

    // Image 1
    if (idx == 0) {
        GLCD_DisplayString(5, 2, 1, ">>> Image 1");
    } else {
        GLCD_DisplayString(5, 2, 1, "    Image 1");
    }

    // Image 2
    if (idx == 1) {
        GLCD_DisplayString(6, 2, 1, ">>> Image 2");
    } else {
        GLCD_DisplayString(6, 2, 1, "    Image 2");
    }

    // Image 3
    if (idx == 2) {
        GLCD_DisplayString(7, 2, 1, ">>> Image 3");
    } else {
        GLCD_DisplayString(7, 2, 1, "    Image 3");
    }

    // Exit
    if (idx == 3) {
        GLCD_DisplayString(8, 2, 1, ">>> Exit");
    } else {
        GLCD_DisplayString(8, 2, 1, "    Exit");
    }
}

int begin_photo(void) {
    uint8_t mIdx = 0;     // menu index
    uint8_t jsNow = 0;    // current joystick press
    uint8_t jsPrev = 0;   // last joystick press

    draw_photo_static();
    draw_photo_menu(mIdx);

    SysTick_Config(SystemCoreClock / 100);

    while (1) {

        jsNow = get_button();   // read joystick

        if (jsNow != jsPrev) {  // only act on new inputs

            switch (jsNow) {

                case KBD_DOWN:
                    if (mIdx < 3) {
                        mIdx++;    // move down list
                    }
                    draw_photo_menu(mIdx);
                    break;

                case KBD_UP:
                    if (mIdx > 0) {
                        mIdx--;    // move up list
                    }
                    draw_photo_menu(mIdx);
                    break;

                case KBD_SELECT:
                    if (mIdx == 3) {
                        GLCD_Clear(Black);
                        return 0;   // leave photo viewer
                    }

                    GLCD_Clear(Black);  // clear for photo
                    display_images(mIdx);  // show selected image

                    // re-draw after image closes
                    draw_photo_static();
                    draw_photo_menu(mIdx);
                    break;
            }

            jsPrev = jsNow;  // store last press
        }
    }
}



