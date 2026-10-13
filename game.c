#include <LPC17xx.H>
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include "tic_tac_toe.h"

extern int play_tictactoe(void);

void draw_game_static() {

    GLCD_Clear(Black);   // simple dark background

    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(Yellow);
    GLCD_DisplayString(0, 0, 1, "   GAME CENTER   ");

    GLCD_SetTextColor(White);
    GLCD_DisplayString(2, 3, 1, "Choose a Game");

    GLCD_SetTextColor(Yellow);
    GLCD_DisplayString(3, 0, 1, "----------------------");
}

void draw_game_menu(uint8_t idx) {

    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(White);

    // clear previous menu items
    GLCD_DisplayString(5, 2, 1, "                 ");
    GLCD_DisplayString(6, 2, 1, "                 ");

    // option 1
    if (idx == 0) {
        GLCD_DisplayString(5, 2, 1, ">>> Tic Tac Toe");
    } else {
        GLCD_DisplayString(5, 2, 1, "    Tic Tac Toe");
    }

    // option 2
    if (idx == 1) {
        GLCD_DisplayString(6, 2, 1, ">>> Exit");
    } else {
        GLCD_DisplayString(6, 2, 1, "    Exit");
    }
}

int begin_game(void) {

    uint8_t mIdx = 0;     // menu index
    uint8_t jsNow = 0;    // joystick input
    uint8_t jsPrev = 0;   // previous input

    draw_game_static();
    draw_game_menu(mIdx);

    SysTick_Config(SystemCoreClock / 100);

    while (1) {

        jsNow = get_button();   // read joystick

        if (jsNow != jsPrev) {  // act only on new press

            switch (jsNow) {

                case KBD_DOWN:
                    // cycle down menu
                    mIdx = (mIdx + 1) % 2;
                    draw_game_menu(mIdx);
                    break;

                case KBD_UP:
                    // cycle up menu (same wrap since only 2 items)
                    mIdx = (mIdx + 1) % 2;
                    draw_game_menu(mIdx);
                    break;

                case KBD_SELECT:
                    if (mIdx == 0) {
                        play_tictactoe();     // start game

                        // rebuild menu after exit
                        draw_game_static();
                        draw_game_menu(mIdx);
                    }
                    else if (mIdx == 1) {
                        GLCD_Clear(Black);
                        return 0;            // exit
                    }
                    break;
            }

            jsPrev = jsNow;   // store last press
        }
    }
}



