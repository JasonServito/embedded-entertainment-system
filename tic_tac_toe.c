#include <LPC17xx.H>
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include "game.h"

static const int CELL_X[3] = {5, 9, 13};
static const int CELL_Y[3] = {1, 4, 7};

int curX = 0;    
int curY = 0;
int turn = 0;   
int moves = 0;

int board[3][3]; 

int P1 = 0;
int P2 = 0;

void draw_board(void) {
    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(White);

    GLCD_DisplayString(0, 0, 1, "P1:");
    GLCD_DisplayString(0, 15, 1, "P2:");

    GLCD_DisplayString(0, 4, 1, "   |   |   ");
    GLCD_DisplayString(1, 4, 1, "           ");
    GLCD_DisplayString(2, 4, 1, "_ _|_ _|_ _");
    GLCD_DisplayString(3, 4, 1, "   |   |   ");
    GLCD_DisplayString(4, 4, 1, "           ");
    GLCD_DisplayString(5, 4, 1, "_ _|_ _|_ _");
    GLCD_DisplayString(6, 4, 1, "   |   |   ");
    GLCD_DisplayString(7, 4, 1, "           ");
    GLCD_DisplayString(8, 4, 1, "   |   |   ");
}

void draw_cursor(void) {
    int px = CELL_X[curX];
    int py = CELL_Y[curY];
    GLCD_DisplayChar(py + 1, px, 1, 0x85);
}

void clear_cursor(void) {
    int px = CELL_X[curX];
    int py = CELL_Y[curY];
    GLCD_DisplayChar(py + 1, px, 1, ' ');
}

void switch_turn(void) {
    moves++;
    turn ^= 1;  
}

int check_win(int player) {
		int i;
    for ( i = 0; i < 3; i++){
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return 1;
			}

    for ( i = 0; i < 3; i++){
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
            return 1;
			}

    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return 1;

    if (board[2][0] == player && board[1][1] == player && board[0][2] == player) return 1;

    return 0;
}

void place_symbol(void) {
    int px = CELL_X[curX];
    int py = CELL_Y[curY];

    if (board[curY][curX] != 0)
        return; 

    if (turn == 0) {
        GLCD_DisplayChar(py, px, 1, 0x58);  
        board[curY][curX] = 1;
    } else {
        GLCD_DisplayChar(py, px, 1, 0x80);  
        board[curY][curX] = 2;
    }
    switch_turn();
}

int play_tictactoe(void) {
		uint8_t current_Joystick = 0;
		uint8_t previous_Joystick = 0;
	  char s1[10]; 
		char s2[10];

    moves = 0;
    turn = 0;
    curX = curY = 0;

		memset(board, 0, sizeof(board));
	
		GLCD_Clear(Black);
				
    draw_board();

    sprintf(s1, "%d", P1);
    sprintf(s2, "%d", P2);

    GLCD_DisplayString(0, 3, 1, (unsigned char*)s1);
    GLCD_DisplayString(0, 18, 1, (unsigned char*)s2);

    SysTick_Config(SystemCoreClock / 100);



    draw_cursor();

    while (1) {
        current_Joystick = get_button();
        if (current_Joystick != previous_Joystick) {

            clear_cursor();

            if (current_Joystick == KBD_UP && curY > 0)       curY--;
            if (current_Joystick == KBD_DOWN && curY < 2)     curY++;
            if (current_Joystick == KBD_LEFT && curX > 0)     curX--;
            if (current_Joystick == KBD_RIGHT && curX < 2)    curX++;

            if (current_Joystick == KBD_SELECT) {
                place_symbol();

                if (check_win(1)) {
                    P1++;
                    GLCD_Clear(Black);
                    LED_Out(0);
                    GLCD_DisplayString(3, 0, 1, "P1 Win");
                    return 0;
                }

                if (check_win(2)) {
                    P2++;
                    GLCD_Clear(Black);
                    LED_Out(0);
                    GLCD_DisplayString(3, 0, 1, "P2 Win");
                    return 0;
                }

                if (moves == 9) {
                    GLCD_Clear(Black);
                    GLCD_DisplayString(3, 0, 1, "Tie");
                    return 0;
                }
            }

            draw_cursor();
            previous_Joystick = current_Joystick;
        }
    }
}
