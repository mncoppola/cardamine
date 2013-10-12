#include "common.h"
#include "rom.h"
#include "io_regs.h"
#include "timer.h"
#include "audio.h"
#include "video.h"
#include "joypad.h"
#include "serial.h"

int handle_ioregs_read ( unsigned short addr, char *value )
{
    switch ( addr )
    {
        case JOYP:
            *value = BITFIELD(P10_input_right, P11_input_left, P12_input_up, P13_input_down,
                              P14_select_direction_keys, P15_select_button_keys, 0, 0);
            break;

        case SB:
            *value = serial_transfer_data;
            break;

        case SC:
            *value = serial_shift_clock | (serial_transfer_start_flag << 7);
            break;

        case DIV:
            *value = div_reg;
            break;

        case TIMA:
            *value = timer_counter;
            break;

        case TMA:
            *value = timer_modulo;
            break;

        case TAC:
            *value = timer_input_clock_select | (timer_enabled << 2);
            break;

        case NR10:
            *value = channel1_number_of_sweep_shift | (channel1_sweep_increase_decrease << 2) | (channel1_sweep_time << 4);
            break;

        case NR11:
            *value = channel1_wave_pattern_duty << 6;
            break;

        case NR12:
            *value = channel1_number_of_envelope_sweep | (channel1_envelope_direction << 3) |
                     (channel1_initial_volume_of_envelope << 4);
            break;

        case NR13:
            *value = 0;
            break;

        case NR14:
            *value = channel1_counter_consecutive_selection << 6;
            break;

        case NR21:
            *value = channel2_wave_pattern_duty << 6;
            break;

        case NR22:
            *value = channel2_number_of_envelope_sweep | (channel2_envelope_direction << 3) |
                     (channel2_initial_volume_of_envelope << 4);
            break;

        case NR23:
            *value = 0;
            break;

        case NR24:
            *value = channel2_counter_consecutive_selection << 6;
            break;

        case NR30:
            *value = channel3_sound_enabled << 7;
            break;

        case NR31:
            *value = channel3_sound_length;
            break;

        case NR32:
            *value = channel3_select_output_level << 5;
            break;

        case NR33:
            *value = 0;
            break;

        case NR34:
            *value = channel3_counter_consecutive_selection << 6;
            break;

        case NR41:
            *value = channel4_sound_length;
            break;

        case NR42:
            *value = channel4_number_of_envelope_sweep | (channel4_envelope_direction << 3) |
                     (channel4_initial_volume_of_envelope << 4);
            break;

        case NR43:
            *value = channel4_dividing_ratio_of_frequencies | (channel4_counter_step_width << 3) |
                     (channel4_shift_clock_frequency << 4);
            break;

        case NR44:
            *value = channel4_counter_consecutive_selection << 6;
            break;

        case NR50:
            *value = SO1_output_level | (output_vin_to_SO1_terminal << 3) |
                     (SO2_output_level << 4) | (output_vin_to_SO2_terminal << 7);
            break;

        case NR51:
            *value = BITFIELD(output_sound_1_to_SO1_terminal, output_sound_2_to_SO1_terminal,
                              output_sound_3_to_SO1_terminal, output_sound_4_to_SO1_terminal,
                              output_sound_1_to_SO2_terminal, output_sound_2_to_SO2_terminal,
                              output_sound_3_to_SO2_terminal, output_sound_4_to_SO2_terminal);
            break;

        case NR52:
            *value = BITFIELD(sound_1_ON_flag, sound_2_ON_flag, sound_3_ON_flag, sound_4_ON_flag, 0, 0, 0, audio_enabled);
            break;
/*
        case WAVE_PATTERN_RAM:
            break;
*/
        case LCDC:
            *value = BITFIELD(bg_display, sprite_display_enable, sprite_size,
                              bg_tile_map_display_select, bg_window_tile_data_select,
                              window_display_enable, window_tile_map_display_select,
                              lcd_display_enable);
            break;

        case STAT:
            *value = lcd_mode_flag | (lcd_coincidence_flag << 2) | (mode_0_H_Blank_interrupt << 3) |
                     (mode_1_V_Blank_interrupt << 4) | (mode_2_OAM_interrupt << 5) | (LYC_LY_coincidence_interrupt << 6);
            break;

        case SCY:
            *value = lcd_scroll_y;
            break;

        case SCX:
            *value = lcd_scroll_x;
            break;

        case LY:
            *value = lcd_line;
            break;

        case LYC:
            *value = lcd_LY_compare;
            break;

        case DMA:
            /* XXX */
            break;

        case BGP:
            *value = shade_for_color_0 | (shade_for_color_1 << 2) | (shade_for_color_2 << 4) | (shade_for_color_3 << 6);
            break;

        case OBP0:
            *value = (sprite_0_shade_for_color_0 << 2) | (sprite_0_shade_for_color_1 << 4) | (sprite_0_shade_for_color_2 << 6);
            break;

        case OBP1:
            *value = (sprite_1_shade_for_color_0 << 2) | (sprite_1_shade_for_color_1 << 4) | (sprite_1_shade_for_color_2 << 6);
            break;

        case WY:
            *value = window_y_position;
            break;

        case WX:
            *value = window_x_position;
            break;

        case BOOT:
            *value = 0;
            break;

        default:
            return 0;
    }

    return 1;
}

int handle_ioregs_write ( unsigned short addr, char value )
{
    switch ( addr )
    {
        case JOYP:
            P14_select_direction_keys = TEST_BIT(value, 4);
            P15_select_button_keys = TEST_BIT(value, 5);
            break;

        case SB:
            serial_transfer_data = value;
            break;

        case SC:
            serial_shift_clock = TEST_BIT(value, 0);
//            serial_clock_speed = TEST_BIT(value, 1); // GBC only
            serial_transfer_start_flag = TEST_BIT(value, 7);
            break;

        case DIV:
            div_reg = 0;
            break;

        case TIMA:
            timer_counter = value & 0xff;
            break;

        case TMA:
            timer_modulo = value;
            break;

        case TAC:
            timer_enabled = TEST_BIT(value, 2);
            timer_input_clock_select = value & 0x2;
            timer_counter = 0;
            break;

        case NR10:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel1_number_of_sweep_shift = value & 0x7;
                channel1_sweep_increase_decrease = TEST_BIT(value, 3);
                channel1_sweep_time = (value & 0x70) >> 4;
            }
            break;

        case NR11:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel1_sound_length_data = value & 0x3f;
                channel1_wave_pattern_duty = (value & 0xc0) >> 6;
            }
            break;

        case NR12:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel1_number_of_envelope_sweep = value & 0x7;
                channel1_envelope_direction = TEST_BIT(value, 3);
                channel1_initial_volume_of_envelope = (value & 0xf0) >> 4;
            }
            break;

        case NR13:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel1_frequency = (channel1_frequency & 0x700) | value;
            }
            break;

        case NR14:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel1_frequency = (channel1_frequency & 0xff) | ((value & 0x3) << 8);
                channel1_counter_consecutive_selection = TEST_BIT(value, 6);
                channel1_initial = TEST_BIT(value, 7);
            }
            break;

        case NR21:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel2_sound_length_data = value & 0x3f;
                channel2_wave_pattern_duty = (value & 0xc0) >> 6;
            }
            break;

        case NR22:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel2_number_of_envelope_sweep = value & 0x7;
                channel2_envelope_direction = TEST_BIT(value, 3);
                channel2_initial_volume_of_envelope = (value & 0xf0) >> 4;
            }
            break;

        case NR23:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel2_frequency = (channel2_frequency & 0x700) | value;
            }
            break;

        case NR24:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel2_frequency = (channel2_frequency & 0xff) | ((value & 0x3) << 8);
                channel2_counter_consecutive_selection = TEST_BIT(value, 6);
                channel2_initial = TEST_BIT(value, 7);
            }
            break;

        case NR30:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel3_sound_enabled = TEST_BIT(value, 7);
            }
            break;

        case NR31:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel3_sound_length = value;
            }
            break;

        case NR32:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel3_select_output_level = (value & 0x60) >> 5;
            }
            break;

        case NR33:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel3_frequency = (channel3_frequency & 0x700) | value;
            }
            break;

        case NR34:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel3_frequency = (channel3_frequency & 0xff) | ((value & 0x3) << 8);
                channel3_counter_consecutive_selection = TEST_BIT(value, 6);
                channel3_initial = TEST_BIT(value, 7);
            }
            break;

        case NR41:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel4_sound_length = value;
            }
            break;

        case NR42:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel4_number_of_envelope_sweep = value & 0x7;
                channel4_envelope_direction = TEST_BIT(value, 3);
                channel4_initial_volume_of_envelope = (value & 0xf0) >> 4;
            }
            break;

        case NR43:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel4_dividing_ratio_of_frequencies = value & 0x7;
                channel4_counter_step_width = TEST_BIT(value, 3);
                channel4_shift_clock_frequency = (value & 0xf0) >> 4;
            }
            break;

        case NR44:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                channel4_counter_consecutive_selection = TEST_BIT(value, 6);
                channel4_initial = TEST_BIT(value, 7);
            }
            break;

        case NR50:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                SO1_output_level = value & 0x7;
                output_vin_to_SO1_terminal = TEST_BIT(value, 3);
                SO2_output_level = (value & 0x70) >> 4;
                output_vin_to_SO2_terminal = TEST_BIT(value, 7);
            }
            break;

        case NR51:
            if ( TEST_BIT(audio_enabled, 7) )
            {
                output_sound_1_to_SO1_terminal = TEST_BIT(value, 0);
                output_sound_2_to_SO1_terminal = TEST_BIT(value, 1);
                output_sound_3_to_SO1_terminal = TEST_BIT(value, 2);
                output_sound_4_to_SO1_terminal = TEST_BIT(value, 3);
                output_sound_1_to_SO2_terminal = TEST_BIT(value, 4);
                output_sound_2_to_SO2_terminal = TEST_BIT(value, 5);
                output_sound_3_to_SO2_terminal = TEST_BIT(value, 6);
                output_sound_4_to_SO2_terminal = TEST_BIT(value, 7);
            }
            break;

        case NR52:
            audio_enabled = TEST_BIT(value, 7);
            break;
/*
        case WAVE_PATTERN_RAM:
            break;
*/
        case LCDC:
            bg_display = TEST_BIT(value, 0);
            sprite_display_enable = TEST_BIT(value, 1);
            sprite_size = TEST_BIT(value, 2);
            bg_tile_map_display_select = TEST_BIT(value, 3);
            bg_window_tile_data_select = TEST_BIT(value, 4);
            window_display_enable = TEST_BIT(value, 5);
            window_tile_map_display_select = TEST_BIT(value, 6);
            lcd_display_enable = TEST_BIT(value, 7);
            /* XXX: reset LY and LCD cycle? */
            break;

        case STAT:
            mode_0_H_Blank_interrupt = TEST_BIT(value, 3);
            mode_1_V_Blank_interrupt = TEST_BIT(value, 4);
            mode_2_OAM_interrupt = TEST_BIT(value, 5);
            LYC_LY_coincidence_interrupt = TEST_BIT(value, 6);
            break;

        case SCY:
            lcd_scroll_y = value;
            break;

        case SCX:
            lcd_scroll_x = value;
            break;

        case LY:
            lcd_line = 0;
            lcd_mode_flag = 0x2;
            break;

        case LYC:
            lcd_LY_compare = value;
            break;

        case DMA:
            /* XXX */
            break;

        case BGP:
            shade_for_color_0 = value & 0x2;
            shade_for_color_1 = (value & 0xc) >> 2;
            shade_for_color_2 = (value & 0x30) >> 4;
            shade_for_color_3 = (value & 0xc0) >> 6;
            break;

        case OBP0:
            sprite_0_shade_for_color_0 = (value & 0xc) >> 2;
            sprite_0_shade_for_color_1 = (value & 0x30) >> 4;
            sprite_0_shade_for_color_2 = (value & 0xc0) >> 6;
            break;

        case OBP1:
            sprite_1_shade_for_color_0 = (value & 0xc) >> 2;
            sprite_1_shade_for_color_1 = (value & 0x30) >> 4;
            sprite_1_shade_for_color_2 = (value & 0xc0) >> 6;
            break;

        case WY:
            window_y_position = value;
            break;

        case WX:
            window_x_position = value;
            break;

        case BOOT:
            disable_bootROM();
            break;

        default:
            return 0;
    }

    return 1;
}
