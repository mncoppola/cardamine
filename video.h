void init_video(void);
void check_coincidence(void);
void update_STAT(void);
void cycle_video(void);

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

#define DURING_H_BLANK              0
#define DURING_V_BLANK              1
#define DURING_SEARCHING_OAM_RAM    2
#define DURING_TRANSFER_DATA_TO_LCD 3
