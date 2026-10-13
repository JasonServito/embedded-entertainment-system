#include <LPC17xx.H> 
#include "string.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"

extern unsigned char DARTGOBLIN_pixel_data[];
extern unsigned char WALLBREAKERS_pixel_data[];
extern unsigned char HOGRIDER_pixel_data[];

int display_images(int image){
    uint8_t current_Joystick = 0;  
    uint8_t previous_Joystick = 0; 

    GLCD_Clear(White);
        
    SysTick_Config(SystemCoreClock/100); 

    while(1){
        current_Joystick = get_button();

        switch(image){
            case 0:
                GLCD_Bitmap(0, 0, 150, 179, DARTGOBLIN_pixel_data);
                break;

            case 1:
                GLCD_Bitmap(0, 0, 180, 229, WALLBREAKERS_pixel_data);
                break;

            case 2:
                GLCD_Bitmap(0, 0, 150, 236, HOGRIDER_pixel_data);
                break;

            default: 
                break;
        }
        
        if(current_Joystick != previous_Joystick && current_Joystick == KBD_LEFT){
            GLCD_Clear(White);
            return 0;					
        }
    }
}

