//----------------------------------------------------------------------------------------------------------------------------------

#include "fnirsi_1013d_scope.h"
#include "variables.h"

//----------------------------------------------------------------------------------------------------------------------------------
void scope_setup_display_lib(void);
//----------------------------------------------------------------------------------------------------------------------------------
void scope_setup_main_screen(void);

void scope_setup_restore_screen(void);

void scope_setup_view_screen(void);

void scope_setup_usb_screen(void);

void scope_diagnostic_screen(void);
//----------------------------------------------------------------------------------------------------------------------------------
void scope_setup_right_control_menu(void);

void scope_setup_right_file_menu(void);
//----------------------------------------------------------------------------------------------------------------------------------
void scope_setup_bottom_file_menu(int mode);
//----------------------------------------------------------------------------------------------------------------------------------
// Right side bar functions
//----------------------------------------------------------------------------------------------------------------------------------
  
void scope_control_button(int mode);
void scope_run_stop_button(int mode);
void scope_auto_set_button(int mode);
void scope_next_wave_button(int mode);
void scope_previous_wave_button(int mode);
void scope_t_cursor_button(int mode);
void scope_v_cursor_button(int mode);
void scope_measures_button(int mode);
void scope_save_picture_button(int mode);
void scope_save_wave_button(int mode);
void scope_delete_wave_button(int mode);
void scope_50_percent_trigger_button(int mode);
void scope_show_grid_button(int mode);        

void scope_channel_sensitivity_control(PCHANNELSETTINGS settings, int type, int mode);

void scope_return_button(int mode);
void scope_select_all_button(int mode);
void scope_select_button(int mode);
void scope_delete_button(int mode);
void scope_page_up_button(int mode);
void scope_page_down_button(int mode);

//----------------------------------------------------------------------------------------------------------------------------------
// Bitmap control bar functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_bmp_return_button(int mode);
void scope_bmp_delete_button(int mode);
void scope_bmp_previous_button(int mode);
void scope_bmp_next_button(int mode);

//----------------------------------------------------------------------------------------------------------------------------------
// Top bar functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_menu_button(int mode);
void scope_main_return_button(int mode);
void scope_run_stop_text(void);

void scope_channel_settings(PCHANNELSETTINGS settings, int mode);
void scope_acqusition_settings(int mode);
void scope_move_speed(int mode);
void scope_trigger_settings(int mode);
void scope_battery_status(void);

//----------------------------------------------------------------------------------------------------------------------------------
// Menu functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_main_menu(void);
void scope_main_menu_system_settings(int mode);
void scope_main_menu_picture_view(int mode);
void scope_main_menu_waveform_view(int mode);

void scope_main_menu_usb_connection(int mode);

void scope_open_channel_menu(PCHANNELSETTINGS settings);
void scope_channel_enable_select(PCHANNELSETTINGS settings);
void scope_channel_fft_show(PCHANNELSETTINGS settings);
void scope_channel_coupling_select(PCHANNELSETTINGS settings);
void scope_channel_VA_select(PCHANNELSETTINGS settings);
void scope_channel_probe_magnification_select(PCHANNELSETTINGS settings);
void scope_channel_sensitivity_select(PCHANNELSETTINGS settings);

void scope_open_acquisition_menu(void);
void scope_acquisition_speed_select(void);
void scope_acquisition_timeperdiv_select(void);

void scope_open_trigger_menu(void);
void scope_trigger_mode_select(void);
void scope_trigger_edge_select(void);
void scope_trigger_channel_select(void);

void scope_open_system_settings_menu(void);
void scope_system_settings_screen_brightness_item(int mode);
void scope_system_settings_screen_brightness_value(void);
void scope_system_settings_grid_brightness_item(int mode);
void scope_system_settings_grid_brightness_value(void);
void scope_system_settings_trigger_50_item(void);
void scope_system_settings_calibration_item(int mode);
void scope_system_settings_x_y_mode_item(void);
void scope_system_settings_confirmation_item(void);
void scope_system_settings_RTC_settings_item(int mode);
void scope_open_RTC_settings_menu(void);

void scope_system_settings_hour_item(void);
void scope_system_settings_hour_item_value(void);
void scope_system_settings_minute_item(void);
void scope_system_settings_minute_item_value(void);
void scope_system_settings_day_item(void);
void scope_system_settings_day_item_value(void);
void scope_system_settings_month_item(void);
void scope_system_settings_month_item_value(void);
void scope_system_settings_year_item(void);
void scope_system_settings_year_item_value(void);
void scope_system_settings_ONOFF_RTC_item(void);

void scope_open_calibration_start_text(void);
void scope_show_calibrating_text(void);
void scope_show_calibration_done_text(void);
void scope_show_calibration_fail(void);

void scope_show_Input_calibration(void);
void scope_show_calibrating_300mV_text(void);
void scope_show_calibrating_600mV_text(void);
void scope_show_calibrating_1_5V_text(void);
void scope_show_calibrating_3V_text(void);
void scope_show_calibrating_7_5V_text(void);
void scope_show_calibrating_15V_text(void);
void scope_show_Input_calibration_done(void);
void scope_show_Input_calibration_fail(void);

void scope_open_measures_menu(void);
void scope_hide_measures_menu_item(void);
void scope_measures_menu_item(int channel, int item);

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_slider(uint16 xpos, uint16 ypos, uint8 position);
void scope_display_slider(uint16 xpos, uint16 ypos, uint8 position);
int  scope_move_slider(uint16 xpos, uint16 ypos, uint8 *position);

void scope_display_slide_button(uint16 xpos, uint16 ypos, uint8 state);
void scope_display_ok_button(uint16 xpos, uint16 ypos, uint8 mode);

//----------------------------------------------------------------------------------------------------------------------------------
//void scope_adjust_timebase(void);
//----------------------------------------------------------------------------------------------------------------------------------
// Grid and cursor functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_grid(void);
//void scope_draw_pointers(void);
void scope_draw_time_cursors(void);
void scope_draw_volt_cursors(void);
//----------------------------------------------------------------------------------------------------------------------------------