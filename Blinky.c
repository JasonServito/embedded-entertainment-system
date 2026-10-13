#include <LPC17xx.H>
#include <stdint.h>
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"

extern int begin_audio(void);
extern int begin_photo(void);
extern int begin_game(void);

void draw_static_ui(void) {
    GLCD_Clear(Black);   

    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(Yellow);
    GLCD_DisplayString(0, 0, 1, "  Jason Servito   ");

    // Menu label
    GLCD_SetTextColor(White);
    GLCD_DisplayString(2, 3, 1, "MEDIA CENTER:");

}

// Update menu options in UI
void draw_menu_items(uint8_t idx) {
    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(White);

    // Clear previous menu area
    GLCD_DisplayString(5, 2, 1, "              ");
    GLCD_DisplayString(6, 2, 1, "              ");
    GLCD_DisplayString(7, 2, 1, "              ");

    // Item 1
    if (idx == 0) {
        GLCD_DisplayString(5, 2, 1, ">>> Photos");
    } else {
        GLCD_DisplayString(5, 2, 1, "    Photos");
    }

    // Item 2
    if (idx == 1) {
        GLCD_DisplayString(6, 2, 1, ">>> Audio");
    } else {
        GLCD_DisplayString(6, 2, 1, "    Audio");
    }

    // Item 3
    if (idx == 2) {
        GLCD_DisplayString(7, 2, 1, ">>> Game");
    } else {
        GLCD_DisplayString(7, 2, 1, "    Game");
    }
}

int main(void) {  		
    uint8_t mIdx = 0;     // menu index
    uint8_t jsNow = 0;    // current joystick read
    uint8_t jsPrev = 0;   // previous joystick

    KBD_Init();
    LED_Init();
    GLCD_Init();

    SysTick_Config(SystemCoreClock / 100);

    draw_static_ui();       // draw static layout once
    draw_menu_items(mIdx);  // draw menu items

    while (1) {

        jsNow = get_button();   // read joystick

        // Only respond when joystick changes
        if (jsNow != jsPrev) {

            switch (jsNow) {

                case KBD_DOWN:
                    // move selection down
                    mIdx = (mIdx + 1) % 3;
                    draw_menu_items(mIdx);
                    break;

                case KBD_UP:
                    // move selection up
                    mIdx = (mIdx + 2) % 3;
                    draw_menu_items(mIdx);
                    break;

                case KBD_SELECT:
                    // run selected item
                    switch (mIdx) {

                        case 0:
                            begin_photo();
														// UI after returning back to main
                            draw_static_ui();
                            draw_menu_items(mIdx);
                            break;

                        case 1:
                            begin_audio();
														// UI after returning back to main
                            draw_static_ui();
                            draw_menu_items(mIdx);
                            break;

                        case 2:
                            begin_game();
														// UI after returning back to main
                            draw_static_ui();
                            draw_menu_items(mIdx);
                            break;
                    }
                    break;
            }

            jsPrev = jsNow;   // update previous joystick
        }
    }
}



