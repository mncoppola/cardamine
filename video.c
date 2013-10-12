#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "interrupt.h"
#include "video.h"

unsigned char bg_display;
unsigned char sprite_display_enable;
unsigned char sprite_size;
unsigned char bg_tile_map_display_select;
unsigned char bg_window_tile_data_select;
unsigned char window_display_enable;
unsigned char window_tile_map_display_select;
unsigned char lcd_display_enable;
unsigned char lcd_line;
unsigned char lcd_coincidence_flag;
unsigned char mode_0_H_Blank_interrupt;
unsigned char mode_1_V_Blank_interrupt;
unsigned char mode_2_OAM_interrupt;
unsigned char LYC_LY_coincidence_interrupt;
unsigned char lcd_scroll_y;
unsigned char lcd_scroll_x;
unsigned char lcd_mode_flag;
unsigned char lcd_LY_compare;
unsigned char window_y_pos;
unsigned char shade_for_color_0;
unsigned char shade_for_color_1;
unsigned char shade_for_color_2;
unsigned char shade_for_color_3;
unsigned char sprite_0_shade_for_color_0;
unsigned char sprite_0_shade_for_color_1;
unsigned char sprite_0_shade_for_color_2;
unsigned char sprite_1_shade_for_color_0;
unsigned char sprite_1_shade_for_color_1;
unsigned char sprite_1_shade_for_color_2;
unsigned char window_y_position;
unsigned char window_x_position;
unsigned int lcd_cycles;

void check_coincidence ( void )
{
    if ( lcd_line == lcd_LY_compare )
    {
        lcd_coincidence_flag = 1;
        if ( LYC_LY_coincidence_interrupt )
            INTERRUPT(LCD_STAT);
    }
    else
    {
        lcd_coincidence_flag = 0;
    }
}

void update_STAT ( void )
{
    lcd_cycles += cpu_cycles;
    switch ( lcd_mode_flag )
    {
        case DURING_H_BLANK:
            if ( lcd_cycles >= 204 )
            {
                lcd_line++;
                check_coincidence();
                if ( lcd_line == 144 )
                {
                    lcd_mode_flag = DURING_V_BLANK;
                    //render_to_screen();
                    if ( mode_1_V_Blank_interrupt )
                        INTERRUPT(LCD_STAT);
                    INTERRUPT(V_BLANK);
                    /* XXX: Handle RTC counter */
                }
                else
                {
                    lcd_mode_flag = DURING_SEARCHING_OAM_RAM;
                    if ( mode_2_OAM_interrupt )
                        INTERRUPT(LCD_STAT);
                }
                lcd_cycles -= 204;
            }
            break;

        case DURING_V_BLANK:
            if ( lcd_cycles >= 456 )
            {
                lcd_line++;
                check_coincidence();
                if ( lcd_line == 154 )
                {
                    lcd_line = 0;
                }
                else if ( lcd_line == 1 )
                {
                    lcd_line = 0;
                    lcd_mode_flag = DURING_SEARCHING_OAM_RAM;
                    check_coincidence();
                    if ( mode_2_OAM_interrupt )
                        INTERRUPT(LCD_STAT);
                }

                lcd_cycles -= 456;
            }
            break;

        case DURING_SEARCHING_OAM_RAM:
            if ( lcd_cycles >= 80 )
            {
                lcd_mode_flag = DURING_TRANSFER_DATA_TO_LCD;
                lcd_cycles -= 80;
            }
            break;

        case DURING_TRANSFER_DATA_TO_LCD:
            if ( lcd_cycles >= 172 )
            {
            //    scanline();
                lcd_mode_flag = DURING_H_BLANK;
                if ( mode_0_H_Blank_interrupt )
                    INTERRUPT(LCD_STAT);
                lcd_cycles -= 172;
            }
            break;
    }
}

void init_video ( void )
{
    lcd_cycles = 0;
}

// Timing stolen from zid's gameboy-emulator until
// a better timing subsystem is determined
void cycle_video ( void )
{
    update_STAT();
}
