//----------------------------------------------------------------------------------------------------------------------------------

#ifndef SCOPE_FUNCTIONS_H
#define SCOPE_FUNCTIONS_H

//----------------------------------------------------------------------------------------------------------------------------------
#include "fnirsi_1013d_scope.h"
#include "variables.h"

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Menu functions
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//void scope_adjust_timebase(void);
void scope_adjust_timebase_and_voltdiv (void);
void scope_set_timebase(void);
void scope_preset_values(void);
//----------------------------------------------------------------------------------------------------------------------------------
// Grid and cursor functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_pointers(void);

//----------------------------------------------------------------------------------------------------------------------------------
// Signal data processing functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_calculate_trigger_vertical_position();

//void scope_start_acquire_trace_data(void);
void scope_acquire_trace_data(void);
//void scope_acquire_trace_data_long(void);

void scope_process_trigger(uint32 count);   

uint32 scope_do_baseline_calibration(void);
uint32 scope_do_channel_calibration(void);
void scope_input_calibration(void);

void scope_do_50_percent_trigger_setup(void);

void scope_do_auto_setup(void);
void scope_do_auto_setup_old(void);
void scope_do_auto_setup_new(void);

//----------------------------------------------------------------------------------------------------------------------------------
// Signal data display functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_trace_data(void);

int32 scope_get_x_sample(PCHANNELSETTINGS settings, int32 index);
int32 scope_get_y_sample(PCHANNELSETTINGS settings, int32 index);

void scope_display_channel_sinx_trace(PCHANNELSETTINGS settings);
void scope_display_channel_linear_trace(PCHANNELSETTINGS settings);

void scope_display_cursor_measurements(void);

void scope_display_measurements(void);
void scope_display_channel_measurements(PCHANNELSETTINGS settings, uint8 *measuresstate, uint32 xstart, uint32 color);

void scope_display_vmax(PCHANNELSETTINGS settings);
void scope_display_vmin(PCHANNELSETTINGS settings);
void scope_display_vavg(PCHANNELSETTINGS settings);
void scope_display_vrms(PCHANNELSETTINGS settings);
void scope_display_vpp(PCHANNELSETTINGS settings);
void scope_display_vp(PCHANNELSETTINGS settings);
void scope_display_freq(PCHANNELSETTINGS settings);
void scope_display_cycle(PCHANNELSETTINGS settings);
void scope_display_time_plus(PCHANNELSETTINGS settings);
void scope_display_time_min(PCHANNELSETTINGS settings);
void scope_display_duty_plus(PCHANNELSETTINGS settings);
void scope_display_duty_min(PCHANNELSETTINGS settings);

void scope_display_voltage(PCHANNELSETTINGS settings, int32 value);

void scope_print_value(char *buffer, int32 value, uint32 scale, char *header, char *sign);

char *scope_print_decimal(char *buffer, int32 value, uint32 decimalplace, uint32 negative);

//----------------------------------------------------------------------------------------------------------------------------------
// File display functions
//----------------------------------------------------------------------------------------------------------------------------------

//These two functions are for save guarding the operational settings when switched to waveform view mode
void scope_save_setup(PSCOPESETTINGS settings);
void scope_restore_setup(PSCOPESETTINGS settings);

//These two functions are for the system settings, preparing for and restoring from file
void scope_prepare_setup_for_file(void);
void scope_restore_setup_from_file(void);
int32 scope_check_waveform_file(void);

void scope_print_file_name(uint32 filenumber);

int32 scope_load_thumbnail_file(void);
int32 scope_save_thumbnail_file(void);

void scope_save_view_item_file(int32 type);

void scope_remove_item_from_thumbnails(uint32 delete);

int32 scope_load_trace_data(void);

int32 scope_load_bitmap_data(void);
int32 load_picture(const char* filename);

void scope_sync_thumbnail_files(void);

void scope_initialize_and_display_thumbnails(void);

void scope_display_thumbnails(void);

void scope_display_thumbnail_data(uint32 xstart, uint32 xend, uint32 ypos, uint32 color, uint8 *buffer);

void scope_create_thumbnail(PTHUMBNAILDATA thumbnaildata);

void scope_thumbnail_set_trace_data(PCHANNELSETTINGS settings, uint8 *buffer);
void scope_thumbnail_calculate_trace_data(int32 xstart, int32 ystart, int32 xend, int32 yend);

void scope_thumbnail_draw_pointer(uint32 xpos, uint32 ypos, uint32 direction, uint32 color);

int32 scope_display_picture_item(void);

void scope_display_selected_signs(void);

void scope_display_file_status_message(int32 msgid, int32 alwayswait);
//void scope_display_file_status_message(int32 msgid, int32 index, int32 alwayswait);

//----------------------------------------------------------------------------------------------------------------------------------
// Configuration data functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_configuration_data(void);
void scope_save_configuration_data(void);

void scope_load_input_calibration_data(void);
void scope_save_input_calibration_data(void);

void scope_reset_config_data(void);

void scope_save_config_data(void);
void scope_restore_config_data(void);

//----------------------------------------------------------------------------------------------------------------------------------

#ifndef USE_TP_CONFIG
#ifdef SAVE_TP_CONFIG
void save_tp_config(void);
#endif
#endif

//----------------------------------------------------------------------------------------------------------------------------------

#endif /* SCOPE_FUNCTIONS_H */
