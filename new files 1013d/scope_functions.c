//----------------------------------------------------------------------------------------------------------------------------------

#include "types.h"
#include "scope_functions.h"
#include "statemachine.h"
#include "touchpanel.h"
#include "timer.h"
#include "fpga_control.h"
#include "spi_control.h"
#include "display_lib.h"
#include "ff.h"

#include "usb_interface.h"
#include "variables.h"

#include "sin_cos_math.h"

#include <string.h>

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_display_lib(void)
{
  display_set_bg_color(0x00000000);
  
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  
  display_set_dimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  disp_xpos = 0;
  
  disp_ch1_y = 0;
  disp_ch2_y = 0;
  
  disp_ch1_prev_y = 0;
  disp_ch2_prev_y = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_main_screen(void)
{
  //Prepare the screen in a working buffer
//  display_set_screen_buffer(displaybuffer1);
  
  //Setup for clearing top and right menu bars
  display_set_fg_color(0x00000000);
  
  //Top bar for the main, channel and trigger menu and information
  display_fill_rect(0, 0, 730, 46);
  
  //Right bar for control buttons
  display_fill_rect(730, 0, 70, 480);
  
  //Setup the menu bar on the right side
  scope_setup_right_control_menu();
  
  //Check if normal or wave view mode
  if(scopesettings.waveviewmode == 0)
  {
    //Normal mode so show menu button
    scope_menu_button(0);
  }
  else
  {
    //Wave view mode so show return button
    scope_main_return_button(0);
  }
 
  //Show the user if the acquisition is running or stopped
  scope_run_stop_text();
  
  //Display the channel menu select buttons and their settings
  scope_channel1_settings(0);
  scope_channel2_settings(0);
  
  //Display the current time per div setting
  scope_time_div_setting();
  
  //Show the user selected move speed
  scope_move_speed(0);
  
  //Display the trigger menu select button and the settings
  scope_trigger_settings(0);
  
  //Show the battery charge level and charging indicator
  scope_battery_status();
  
  //Clear some flags to make the trace part of the screen refresh
  scopesettings.triggerflag1 = 1;
  scopesettings.triggerflag2 = 1;
  
  disp_xpos = 0;  
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_view_screen(void)
{
  //Switch to view mode so disallow saving of settings on power down
  viewactive = VIEW_ACTIVE;
  
  //Set scope run state to running to have it sample fresh data on exit
  scopesettings.runstate = 0;

  //This is different to the original since the thumbnail displaying does not use the settings data anymore
  //Only when picture and waveform display is used the settings are overwritten, since it makes use of the actual waveform functions and main touch handler
  //Created two separate function sets. One for saving and restoring the settings as is, and one for saving to and loading from a file buffer
  
  //Save the current settings
  scope_save_setup(&savedscopesettings1);
  
  //Initialize the view mode variables
  //Used for indicating if select all or select button is active
  viewselectmode = 0;
  
  //Start at first page. In the original code they start with 1 and subtract from the multiplied with 16 to get the correct file number index
  viewpage = 0;
  
  //Clear the item selected flags
  memset(viewitemselected, VIEW_ITEM_NOT_SELECTED, VIEW_ITEMS_PER_PAGE);
  
  //Set storage buffer for screen capture under selected signs and messages
  display_set_destination_buffer(displaybuffer2);
  display_set_source_buffer(displaybuffer2);
  
  //Display the file actions menu on the right side of the screen
  scope_setup_right_file_menu();
  
  //Load the list files for the current view type
  scope_load_list_files();
  
  //Display the available thumbnails for the current view type
  scope_count_and_display_thumbnails();
  
  //Handle touch for this part of the user interface
  handle_view_mode_touch();
  
  //Restore the current settings
  scope_restore_setup(&savedscopesettings1);

  //Make sure view mode is normal
  scopesettings.waveviewmode = 0;

  //And resume with auto trigger mode
  scopesettings.triggermode = 0;

  //Need to restore the original scope data and fpga settings

  //Is also part of startup, so could be done with a function
  //Set the volts per div for each channel based on the loaded scope settings
  fpga_set_channel_1_voltperdiv();
  fpga_set_channel_2_voltperdiv();

  //These are not done in the original code
  //Set the channels AC or DC coupling based on the loaded scope settings
  fpga_set_channel_1_coupling();
  fpga_set_channel_2_coupling();

  //Setup the trigger system in the FPGA based on the loaded scope settings
  fpga_set_trigger_timebase();
  fpga_set_trigger_channel();
  fpga_set_trigger_edge();
  fpga_set_trigger_level();
  fpga_set_trigger_mode();

  //Set channel screen offsets
  fpga_set_channel_1_offset();
  fpga_set_channel_2_offset();



  //Reset the screen to the normal scope screen
  scope_setup_main_screen();

  //Restart the scope at this point in the code
  
          
//        FUN_8000a750();          //Restart the scope function
//        setup_main_screen();          

  //Back to normal mode so allow saving of settings on power down
  viewactive = VIEW_NOT_ACTIVE;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_usb_screen(void)
{
  //Clear the whole screen
  display_set_fg_color(0x00000000);
  display_fill_rect(0, 0, 800, 480);
  
  //Set the light color for the equipment borders
  display_set_fg_color(0x00AAAAAA);
  
  //Draw the computer screen
  display_fill_rounded_rect(470, 115, 250, 190, 2);
  display_fill_rect(580, 305, 30, 20);
  display_fill_rounded_rect(550, 325, 90, 10, 2);
  display_fill_rect(550, 331, 89, 4);

  //Draw the scope
  display_fill_rounded_rect(80, 200, 180, 135, 2);

  //Draw the cable
  display_fill_rect(210, 188, 10, 12);
  display_fill_rect(213, 154, 4, 36);
  display_fill_rect(213, 150, 152, 4);
  display_fill_rect(361, 154, 4, 106);
  display_fill_rect(361, 260, 98, 4);
  display_fill_rect(458, 257, 12, 10);
  
  //Fill in the screens with a blue color
  display_set_fg_color(0x00000055);
  display_fill_rect(477, 125, 235, 163);
  display_fill_rect(88, 210, 163, 112);
  
  //Draw a dark border around the blue screens
  display_set_fg_color(0x00111111);
  display_draw_rect(477, 125, 235, 163);
  display_draw_rect(88, 210, 163, 112);
  
  //Display the text with font 5 and the light color
  display_set_font(&font_5);
  display_set_fg_color(0x00AAAAAA);
  display_text(125, 254, "ON / OFF");

  //Start the USB interface
  usb_device_enable();
  
  //Wait for the user to touch the scope ON / OFF section to quit
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();
    
    //Check if there is touch
    if(havetouch)
    {
      //Check if touch within the text field
      if((xtouch >= 90) && (xtouch <= 250) && (ytouch >= 210) && (ytouch <= 320))
      {
        //Touched the text field so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();
        
        break;
      }
    }
  }
  
  //Stop the USB interface  
  usb_device_disable();
  
  //Re-sync the system files
  scope_sync_list_files();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_right_control_menu(void)
{
  //Setup for clearing right menu bar
  display_set_fg_color(0x00000000);
  
  //Clear the right bar for the control buttons
  display_fill_rect(730, 0, 70, 480);
  
  //Display the control button
  scope_control_button(0);
  
  //Check in which state the right menu is in
  if(scopesettings.rightmenustate == 0)
  {
    //Main control state so draw the always used buttons
    scope_t_cursor_button(0);
    scope_v_cursor_button(0);
    scope_measures_button(0);
    scope_save_picture_button(0);
    
    //Check if in wave view mode
    if(scopesettings.waveviewmode == 0)
    {
      //Main control mode buttons
      scope_run_stop_button(0);
      scope_auto_set_button(0);
      scope_save_wave_button(0);
    }
    else
    {
      //Wave view mode buttons
      scope_page_up_button(0);
      scope_page_down_button(0);
      scope_delete_wave_button(0);
    }
  }
  else
  {
    //Channel sensitivity state
    scope_ch1_sensitivity_control(0,0);
    scope_ch2_sensitivity_control(0,0);
    
    //Check if in wave view mode
    if(scopesettings.waveviewmode == 0)
    {
      //Main control mode
      scope_50_percent_trigger_button(0);
    }
    else
    {
      //Wave view mode
      scope_show_grid_button(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_right_file_menu(void)
{
  int32 y;
  
  //Set black color for background
  display_set_fg_color(0x00000000);

  //Clear the right menu bar
  display_fill_rect(730, 0, 70, 480);

  //Grey color for separators
  display_set_fg_color(0x007F7F7F);
  
  //Draw the button separators
  for(y=80;y<480;y+=80)
  {
    //A 48 pixel long line
    display_draw_horz_line(y, 741, 789);
  }

  //White color for text
  display_set_fg_color(0x00FFFFFF);
  
  //Display the texts with this font
  display_set_font(&font_3);
  
  //Some buttons have two lines of text
  display_text(747, 34, "return");
  
  display_text(746, 104, "select");
  display_text(758, 119, "all");
  
  display_text(746, 191, "select");
  
  display_text(746, 271, "delete");

  display_text(750, 343, "page");
  display_text(758, 358, "up");

  display_text(750, 422, "page");
  display_text(748, 440, "down");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_bottom_file_menu(int mode)
{
  //Check if background needs to be saved
  if(mode == VIEW_BOTTON_MENU_INIT)
  {
    //Save the screen rectangle where the menu will be displayed
    display_copy_rect_from_screen(0, 420, 800, 60);
  }
  
  //Check if it needs to be drawn
  if(mode & VIEW_BOTTON_MENU_SHOW)
  {
    //Draw the background in grey
    display_set_fg_color(0x00202020);
    display_fill_rect(0, 420, 800, 60);
   
    //Draw the three separator lines in black
    display_set_fg_color(0x00000000);
    display_draw_vert_line(200, 423, 477);
    display_draw_vert_line(400, 423, 477);
    display_draw_vert_line(600, 423, 477);

    //Draw the icons in white    
    display_set_fg_color(0x00FFFFFF);
    display_copy_icon_fg_color(return_arrow_icon,      79, 436, 41, 27);
    display_copy_icon_fg_color(waste_bin_icon,        284, 433, 31, 33);
    display_copy_icon_fg_color(previous_picture_icon, 483, 438, 33, 24);
    display_copy_icon_fg_color(next_picture_icon,     683, 438, 33, 24);
    
    //Signal menu is visible
    viewbottommenustate = VIEW_BOTTON_MENU_SHOW;
  }
  else
  {
    //Hide the menu bar
    display_copy_rect_to_screen(0, 420, 800, 60);
    
    //Signal menu is not visible
    viewbottommenustate = VIEW_BOTTON_MENU_HIDE;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
// Right side bar functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_control_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Grey color for inactive button
    display_set_fg_color(0x00333333);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 5, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00444444);
    
    //Draw the edge
    display_draw_rounded_rect(739, 5, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(748, 22, "CTRL");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_run_stop_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 65, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 65, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 75, "RUN/");
  display_text(746, 90, "STOP");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_auto_set_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 125, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 125, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 135, "AUTO");
  display_text(753, 150, "SET");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_page_up_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 65, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 65, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(749, 75, "page");
  display_text(757, 90, "up");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_page_down_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 125, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 125, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(749, 135, "page");
  display_text(748, 150, "down");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_t_cursor_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 185, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 185, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 195, "T");
  display_text(765, 195, "CU");
  display_text(746, 210, "RSOR");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_v_cursor_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 245, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 245, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 255, "V");
  display_text(765, 255, "CU");
  display_text(746, 270, "RSOR");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_measures_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 305, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 305, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 315, "MEAS");
  display_text(746, 330, "URES");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_picture_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 365, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 365, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 375, "SAVE");
  display_text(753, 390, "PIC");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_wave_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 425, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 435, "SAVE");
  display_text(746, 450, "WAVE");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_delete_wave_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 425, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(745, 436, "delete");
  display_text(748, 449, "wave");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_50_percent_trigger_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 425, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(752, 436, "50%");
  display_text(749, 449, "TRIG");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_grid_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Edge color for the inactive button
    display_set_fg_color(0x00222222);
    
    //Draw the edge
    display_draw_rounded_rect(739, 425, 51, 50, 2);
  }
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(748, 436, "show");
  display_text(752, 449, "grid");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_ch1_sensitivity_control(int type,int mode)
{
  //Check if V+ is active or inactive
  if((type == 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  
  //Top button
  display_fill_rect(735, 78, 60, 60);

  //Check if V- is active or inactive
  if((type != 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }

  //Bottom button
  display_fill_rect(735, 159, 60, 60);
  
  //Display V+ and V- the text in larger font and white color
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_0);
  display_text(757, 99, "V+");
  display_text(757, 185, "V-");
  
  //Display the grey bar for the channel identifier text
  display_set_fg_color(0x00404040);
  display_fill_rect(735, 139, 60, 24);
  
  //Display the channel identifier in black
  display_set_fg_color(0x00000000);
  display_set_font(&font_2);
  display_text(756, 144, "CH1");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_ch2_sensitivity_control(int type,int mode)
{
  //Check if V+ is active or inactive
  if((type == 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  
  //Top button
  display_fill_rect(735, 258, 60, 60);

  //Check if V- is active or inactive
  if((type != 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }

  //Bottom button
  display_fill_rect(735, 339, 60, 60);
  
  //Display V+ and V- the text in larger font and white color
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_0);
  display_text(757, 279, "V+");
  display_text(757, 365, "V-");
  
  //Display the grey bar for the channel identifier text
  display_set_fg_color(0x00404040);
  display_fill_rect(735, 319, 60, 24);
  
  //Display the channel identifier in black
  display_set_fg_color(0x00000000);
  display_set_font(&font_2);
  display_text(756, 324, "CH2");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_select_all_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //White for activated button
    display_set_fg_color(0x00FFFFFF);
  }

  //Draw the body of the button
  display_fill_rect(737, 100, 55, 40);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 104, "select");
  display_text(758, 119, "all");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_select_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(0x00000000);
  }
  else
  {
    //White for activated button
    display_set_fg_color(0x00FFFFFF);
  }

  //Draw the body of the button
  display_fill_rect(737, 180, 55, 40);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(746, 191, "select");
}

//----------------------------------------------------------------------------------------------------------------------------------
// Top bar function
//----------------------------------------------------------------------------------------------------------------------------------

void scope_menu_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark blue background
    display_set_fg_color(0x00000078);
  }
  else
  {
    //Active so pale yellow background
    display_set_fg_color(0x00FFFF80);
  }
  
  //Draw the background
  display_fill_rect(0, 0, 80, 38);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active so black foreground
    display_set_fg_color(0x00000000);
  }

  //Draw the menu symbol
  display_fill_rect(6,  11, 7, 7);
  display_fill_rect(15, 11, 7, 7);
  display_fill_rect(6,  20, 7, 7);
  display_fill_rect(15, 20, 7, 7);
  
  //Display the text
  display_set_font(&font_3);
  display_text(32, 11, "MENU");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_return_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so black background
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Active so white background
    display_set_fg_color(0x00FFFFFF);
  }
  
  //Draw the background
  display_fill_rect(0, 0, 80, 38);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00000000);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00FFFFFF);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(return_arrow_icon, 20, 5, 41, 27);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_run_stop_text(void)
{
  //Check if run or stop mode
  if(scopesettings.runstate == 0)
  {
    //Run mode. Black box
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Stop mode. Red box
    display_set_fg_color(0x00FF0000);
  }
  
  //Fill the box
  display_fill_rect(102, 12, 38, 18);

  //Select the font for the text
  display_set_font(&font_3);
  
  //Check if run or stop mode
  if(scopesettings.runstate == 0)
  {
    //Run mode. White text
    display_set_fg_color(0x00FFFFFF);
    display_text(106, 13, "RUN");
  }
  else
  {
    //Stop mode. Black text
    display_set_fg_color(0x00000000);
    display_text(103, 13, "STOP");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel1_settings(int mode)
{
  int8 **vdtext;
  
  //Clear the area first
  display_set_fg_color(0x00000000);
  display_fill_rect(161, 5, 99, 35);
  
  //Check if channel is enabled or disabled
  if(scopesettings.channel1.enable == 0)
  {
    //Disabled so off colors
    //Check if inactive or active
    if(mode == 0)
    {
      //Inactive, grey menu button
      display_set_fg_color(0x00444444);
    }
    else
    {
      //Active, light grey menu button
      display_set_fg_color(0x00BBBBBB);
    }
  }
  else
  {
    //Enabled so on colors
    //Check if inactive or active
    if(mode == 0)
    {
      //Inactive, channel 1 color menu button
      display_set_fg_color(CHANNEL1_COLOR);
    }
    else
    {
      //Active, blue menu button
      display_set_fg_color(0x000000FF);
    }
  }
  
  //Fill the button
  display_fill_rect(161, 5, 30, 35);

  //Select the font for the text
  display_set_font(&font_2);
  
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Active, white text
    display_set_fg_color(0x00FFFFFF);
    
    //Fill the settings background
    display_fill_rect(191, 5, 69, 35);
  }
  
  //Display the channel identifier text
  display_text(166, 16, "CH1");

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active, black text
    display_set_fg_color(0x00000000);
  }
  
  //Check on which coupling is set
  if(scopesettings.channel1.coupling == 0)
  {
    //DC coupling
    display_text(199, 8, "DC");
  }
  else
  {
    //AC coupling
    display_text(199, 8, "AC");
  }
  
  //Print the probe magnification factor
  switch(scopesettings.channel1.magnification)
  {
    case 0:
      //Times 1 magnification
      display_text(224, 8, "1X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[0];
      break;
      
    case 1:
      //Times 10 magnification
      display_text(222, 8, "10X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[1];
      break;
      
    default:
      //Times 100 magnification
      display_text(220, 8, "100X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[2];
      break;
  }
  
  //Display the sensitivity when in range
  if(scopesettings.channel1.voltperdiv < 7)
  {
    display_text(199, 24, vdtext[scopesettings.channel1.voltperdiv]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel2_settings(int mode)
{
  int8 **vdtext;
  
  //Clear the area first
  display_set_fg_color(0x00000000);
  display_fill_rect(288, 5, 99, 35);
  
  //Check if channel is enabled or disabled
  if(scopesettings.channel2.enable == 0)
  {
    //Disabled so on colors
    //Check if inactive or active
    if(mode == 0)
    {
      //Inactive, grey menu button
      display_set_fg_color(0x00444444);
    }
    else
    {
      //Active, light grey menu button
      display_set_fg_color(0x00BBBBBB);
    }
  }
  else
  {
    //Enabled so on colors
    //Check if inactive or active
    if(mode == 0)
    {
      //Inactive, cyan menu button
      display_set_fg_color(CHANNEL2_COLOR);
    }
    else
    {
      //Active, red menu button
      display_set_fg_color(0x00FF0000);
    }
  }
  
  //Fill the button
  display_fill_rect(288, 5, 30, 35);

  //Select the font for the text
  display_set_font(&font_2);
  
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Active, white text
    display_set_fg_color(0x00FFFFFF);
    
    //Fill the settings background
    display_fill_rect(318, 5, 69, 35);
  }
  
  //Display the channel identifier text
  display_text(293, 16, "CH2");

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active, black text
    display_set_fg_color(0x00000000);
  }
  
  //Check on which coupling is set
  if(scopesettings.channel2.coupling == 0)
  {
    //DC coupling
    display_text(326, 8, "DC");
  }
  else
  {
    //AC coupling
    display_text(326, 8, "AC");
  }
  
  //Print the probe magnification factor
  switch(scopesettings.channel2.magnification)
  {
    case 0:
      //Times 1 magnification
      display_text(351, 8, "1X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[0];
      break;
      
    case 1:
      //Times 10 magnification
      display_text(349, 8, "10X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[1];
      break;
      
    default:
      //Times 10 magnification
      display_text(347, 8, "100X");
      
      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[2];
      break;
  }
  
  //Display the sensitivity when in range
  if(scopesettings.channel2.voltperdiv < 7)
  {
    display_text(326, 24, vdtext[scopesettings.channel2.voltperdiv]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

const int8 *time_div_texts[30] =
{
    "50S/div",   "20S/div",   "10S/div",
     "5S/div",    "2S/div",    "1S/div",
  "500mS/div", "200mS/div", "100mS/div",
   "50mS/div",  "20mS/div",  "10mS/div",
    "5mS/div",   "2mS/div",   "1mS/div",
  "500uS/div", "200uS/div", "100uS/div",
   "50uS/div",  "20uS/div",  "10uS/div",
    "5uS/div",   "2uS/div",   "1uS/div",
  "500nS/div", "250nS/div", "100nS/div",
   "50nS/div",  "25nS/div",  "10nS/div",
};

//----------------------------------------------------------------------------------------------------------------------------------

void scope_time_div_setting(void)
{
  //Clear the area first
  display_set_fg_color(0x00000000);
  display_fill_rect(395, 5, 93, 35);
  
  //Only display the text when in range
  if(scopesettings.timeperdiv < 30)
  {
    //White text
    display_set_fg_color(0x00FFFFFF);

    //Select the font for the text
    display_set_font(&font_0);

    //Display the text from the table
    display_text(402, 16, (int8 *)time_div_texts[scopesettings.timeperdiv]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_move_speed(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark blue background
    display_set_fg_color(0x00000078);
  }
  else
  {
    //Active so pale yellow background
    display_set_fg_color(0x00FFFF80);
  }
  
  //Draw the background
  display_fill_rect(493, 5, 40, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active so black text
    display_set_fg_color(0x00000000);
  }

  //Select the font for the text
  display_set_font(&font_3);
  
  //Display the common text
  display_text(496, 6, "move");
  
  //Check on which speed is set
  if(scopesettings.movespeed == 0)
  {
    display_text(501, 21, "fast");
  }
  else
  {
    display_text(499, 21, "slow");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_settings(int mode)
{
  int8 *modetext = 0;
  
  //Clear the area first
  display_set_fg_color(0x00000000);
  display_fill_rect(570, 5, 130, 35);
  
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, green menu button
    display_set_fg_color(TRIGGER_COLOR);
  }
  else
  {
    //Active, magenta menu button
    display_set_fg_color(0x00FF00FF);
  }
  
  //Fill the button
  display_fill_rect(570, 5, 31, 35);

  //Select the font for the text
  display_set_font(&font_4);
  
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Active, white text
    display_set_fg_color(0x00FFFFFF);
    
    //Fill the settings background
    display_fill_rect(601, 5, 88, 35);
  }
  
  //Display the channel identifier text
  display_text(582, 15, "T");

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active, black text
    display_set_fg_color(0x00000000);
  }
  
  //Check on which trigger mode is set
  switch(scopesettings.triggermode)
  {
    case 0:
      modetext = "Auto";
      break;

    case 1:
      modetext = "Single";
      break;

    case 2:
      modetext = "Normal";
      break;
  }

  //Select the font for the texts
  display_set_font(&font_2);
  
  //Check if valid setting
  if(modetext)
  {
    //Display the selected text if so
    display_text(606, 7, modetext);
  }

  //Draw the trigger edge symbol
  display_draw_vert_line(642, 27, 38);
  
  //Draw the arrow based on the selected edge
  if(scopesettings.triggeredge == 0)
  {
    //rising edge
    display_draw_horz_line(27, 642, 645);
    display_draw_horz_line(38, 639, 642);
    display_draw_horz_line(32, 641, 643);
    display_draw_horz_line(33, 640, 644);
  }
  else
  {
    //falling edge
    display_draw_horz_line(27, 639, 642);
    display_draw_horz_line(38, 642, 645);
    display_draw_horz_line(32, 640, 644);
    display_draw_horz_line(33, 641, 643);
  }
  
  //Check on which channel is used for triggering
  switch(scopesettings.triggerchannel)
  {
    //Channel 1
    case 0:
      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, dark channel 1 trigger color box
        display_set_fg_color(CHANNEL1_TRIG_COLOR);
      }
      else
      {
        //Active, some blue box
        display_set_fg_color(0x003333FF);
      }
      
      //Fill the channel background
      display_fill_rect(605, 25, 28, 15);
      
      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, black text
        display_set_fg_color(0x00000000);
      }
      else
      {
        //Active, white text
        display_set_fg_color(0x00FFFFFF);
      }
      
      //Display the text
      display_text(608, 26, "CH1");
      break;
    
    //Channel 2
    case 1:
      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, dark cyan box
        display_set_fg_color(CHANNEL2_TRIG_COLOR);
      }
      else
      {
        //Active, some red box
        display_set_fg_color(0x00FF3333);
      }
      
      //Fill the channel background
      display_fill_rect(605, 25, 28, 15);
      
      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, black text
        display_set_fg_color(0x00000000);
      }
      else
      {
        //Active, white text
        display_set_fg_color(0x00FFFFFF);
      }
      
      //Display the text
      display_text(608, 26, "CH2");
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_battery_status(void)
{
  //Prepare the battery symbol in a working buffer to avoid flicker
  display_set_screen_buffer(displaybuffer1);
  
  //Clear the background
  display_set_fg_color(0x00000000);
  display_fill_rect(701, 5, 25, 13);
  
  //Draw an empty battery symbol in white
  display_set_fg_color(0x00FFFFFF);
  display_fill_rect(701, 9, 3, 5);
  display_fill_rect(703, 5, 23, 13);
  
  //Check if there is any charge
  if(scopesettings.batterychargelevel)
  {
    //Keep charge level on max if above
    if(scopesettings.batterychargelevel > 20)
    {
      //Max for displaying the level
      scopesettings.batterychargelevel = 20;
    }

    //Check if charge level is low
    if(scopesettings.batterychargelevel < 4)
    {
      //Draw the level indicator in red
      display_set_fg_color(0x00FF0000);
    }
    else
    {
      //Draw the level indicator in dark green
      display_set_fg_color(0x0000BB00);
    }
    
    //Draw the indicator based on the level
    display_fill_rect(724 - scopesettings.batterychargelevel, 6, scopesettings.batterychargelevel + 1, 11);
  }  
  
  //Draw the battery charging indicator when plugged in
  if(scopesettings.batterycharging)
  {
    //Some light blue color
    display_set_fg_color(0x002222FF);

    //Draw an arrow when charging
    display_draw_horz_line(10, 708, 718);
    display_draw_horz_line(11, 708, 718);
    display_draw_horz_line(12, 708, 718);
    display_draw_vert_line(719, 8, 14);
    display_draw_vert_line(720, 9, 13);
    display_draw_vert_line(721, 10, 12);
    display_draw_vert_line(722, 11, 11);
  }
  
  //Copy it to the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  display_copy_rect_to_screen(701, 5, 25, 13);
}

//----------------------------------------------------------------------------------------------------------------------------------
// Menu functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_main_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(0, 46, 149, 233);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(0, 46, 149, 233);
  
  //Three black lines between the settings
  display_set_fg_color(0x00000000);
  display_draw_horz_line(104, 9, 140);
  display_draw_horz_line(163, 9, 140);
  display_draw_horz_line(222, 9, 140);

  //Display the menu items  
  scope_main_menu_system_settings(0);
  scope_main_menu_picture_view(0);
  scope_main_menu_waveform_view(0);
  scope_main_menu_usb_connection(0);
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(0, 46, 149, 233, 63039);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_menu_system_settings(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(0x00FFFF00);
  }
  
  //Draw the background
  display_fill_rect(9, 59, 131, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00FFFF00);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(system_settings_icon, 21, 63, 15, 25);
  
  //Display the text
  display_set_font(&font_3);
  display_text(69, 60, "System");
  display_text(68, 76, "settings");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_menu_picture_view(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so white background
    display_set_fg_color(0x00CCCCCC);
  }
  
  //Draw the background
  display_fill_rect(9, 116, 131, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00CCCCCC);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(picture_view_icon, 17, 122, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(73, 119, "Picture");
  display_text(79, 135, "view");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_menu_waveform_view(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so white background
    display_set_fg_color(0x00CCCCCC);
  }
  
  //Draw the background
  display_fill_rect(9, 175, 131, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00CCCCCC);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(waveform_view_icon, 17, 181, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(62, 178, "Waveform");
  display_text(79, 194, "view");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_menu_usb_connection(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so white background
    display_set_fg_color(0x00CCCCCC);
  }
  
  //Draw the background
  display_fill_rect(9, 235, 131, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00CCCCCC);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(usb_icon, 20, 239, 18, 25);
  
  //Display the text
  display_set_font(&font_3);
  display_text(80, 237, "USB");
  display_text(60, 253, "connection");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_channel1_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(161, 46, 183, 252);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(161, 46, 183, 252);
  
  //Three black lines between the settings
  display_set_fg_color(0x00000000);
  display_draw_horz_line(108, 175, 330);
  display_draw_horz_line(170, 175, 330);
  display_draw_horz_line(234, 175, 330);
  
  //Main texts in white  
  display_set_fg_color(0x00FFFFFF);
  
  //Select the font for the texts
  display_set_font(&font_3);
  
  //Display the texts
  display_text(176,  56, "open");
  display_text(183,  75, "CH");
  display_text(176, 118, "open");
  display_text(180, 137, "FFT");
  display_text(176, 182, "coup");
  display_text(179, 200, "ling");
  display_text(176, 247, "probe");
  display_text(176, 265, "mode");

  //Display the actual settings
  scope_channel1_enable_select();
  scope_channel1_fft_show();
  scope_channel1_coupling_select();
  scope_channel1_probe_magnification_select();
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(161, 46, 183, 252, 69906);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel1_enable_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if channel is disabled or enabled
  if(scopesettings.channel1.enable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(239, 62, 32, 22);
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(291, 62, 32, 22);
  }
  
  //Set channel 1 color for the box behind the selected text
  display_set_fg_color(CHANNEL1_COLOR);
  
  //Check if channel is disabled or enabled
  if(scopesettings.channel1.enable == 0)
  {
    //Disabled so channel 1 color box behind off
    display_fill_rect(291, 62, 32, 22);
  }
  else
  {
    //Enabled so channel 1 color box behind on
    display_fill_rect(239, 62, 32, 22);
  }

  //Check if channel is disabled or enabled
  if(scopesettings.channel1.enable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(0x00000000);
  }

  //Display the on text
  display_text(245, 65, "ON");

  //Check if channel is disabled or enabled
  if(scopesettings.channel1.enable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the off text
  display_text(294, 65, "OFF");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel1_fft_show(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if fft is disabled or enabled
  if(scopesettings.channel1.fftenable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(239, 124, 32, 22);
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(291, 124, 32, 22);
  }
  
  //Set channel 1 color for the box behind the selected text
  display_set_fg_color(CHANNEL1_COLOR);
  
  //Check if fft is disabled or enabled
  if(scopesettings.channel1.fftenable == 0)
  {
    //Disabled so channel 1 color box behind off
    display_fill_rect(291, 124, 32, 22);
  }
  else
  {
    //Enabled so channel 1 color box behind on
    display_fill_rect(239, 124, 32, 22);
  }

  //Check if fft is disabled or enabled
  if(scopesettings.channel1.fftenable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(0x00000000);
  }

  //Display the on text
  display_text(245, 127, "ON");

  //Check if fft is disabled or enabled
  if(scopesettings.channel1.fftenable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the off text
  display_text(294, 127, "OFF");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel1_coupling_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if coupling is dc or ac
  if(scopesettings.channel1.coupling == 0)
  {
    //DC so dark grey box behind ac text
    display_fill_rect(291, 188, 32, 22);
  }
  else
  {
    //AC so dark grey box behind dc text
    display_fill_rect(239, 188, 32, 22);
  }

  //Set channel 1 color for the box behind the selected text
  display_set_fg_color(CHANNEL1_COLOR);
  
  //Check if coupling is dc or ac
  if(scopesettings.channel1.coupling == 0)
  {
    //DC so channel 1 color box behind dc text
    display_fill_rect(239, 188, 32, 22);
  }
  else
  {
    //AC so channel 1 color box behind ac text
    display_fill_rect(291, 188, 32, 22);
  }

  //Check if coupling is dc or ac
  if(scopesettings.channel1.coupling == 0)
  {
    //DC so black dc text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //AC so white dc text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the dc text
  display_text(245, 191, "DC");

  //Check if coupling is dc or ac
  if(scopesettings.channel1.coupling == 0)
  {
    //DC so white ac text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //AC so black ac text
    display_set_fg_color(0x00000000);
  }

  //Display the off text
  display_text(296, 191, "AC");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel1_probe_magnification_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(0x00181818);
  
  //Check if coupling is dc or ac
  switch(scopesettings.channel1.magnification)
  {
    case 0:
      //dark grey times 10 and 100 magnification
      display_fill_rect(270, 245, 23, 38);
      display_fill_rect(299, 245, 30, 38);
      break;
      
    case 1:
      //dark grey times 1 and 100 magnification
      display_fill_rect(239, 245, 20, 38);
      display_fill_rect(299, 245, 30, 38);
      break;
      
    default:
      //dark grey times 1 and 10 magnification
      display_fill_rect(239, 245, 20, 38);
      display_fill_rect(270, 245, 23, 38);
      break;
  }
  
  //Set channel 1 color for the box behind the selected text
  display_set_fg_color(CHANNEL1_COLOR);
  
  //Check if which magnification to highlight
  switch(scopesettings.channel1.magnification)
  {
    case 0:
      //Highlight times 1 magnification
      display_fill_rect(239, 245, 20, 38);
      break;
      
    case 1:
      //Highlight times 10 magnification
      display_fill_rect(270, 245, 23, 38);
      break;
      
    default:
      //Highlight times 100 magnification
      display_fill_rect(299, 245, 30, 38);
      break;
  }

  //Check if magnification is 1x
  if(scopesettings.channel1.magnification == 0)
  {
    //Yes so black 1X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 1X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 1X text
  display_text(245, 247, "1");
  display_text(244, 265, "X");

  //Check if magnification is 10x
  if(scopesettings.channel1.magnification == 1)
  {
    //Yes so black 10X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 10X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 10X text
  display_text(274, 247, "10");
  display_text(276, 265, "X");

  //Check if magnification is 100x
  if(scopesettings.channel1.magnification > 1)
  {
    //Yes so black 100X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 100X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 100X text
  display_text(303, 247, "100");
  display_text(310, 265, "X");
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_open_channel2_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(288, 46, 183, 252);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(288, 46, 183, 252);
  
  //Three black lines between the settings
  display_set_fg_color(0x00000000);
  display_draw_horz_line(108, 302, 457);
  display_draw_horz_line(170, 302, 457);
  display_draw_horz_line(234, 302, 457);
  
  //Main texts in white  
  display_set_fg_color(0x00FFFFFF);
  
  //Select the font for the texts
  display_set_font(&font_3);
  
  //Display the texts
  display_text(303,  56, "open");
  display_text(310,  75, "CH");
  display_text(303, 118, "open");
  display_text(307, 137, "FFT");
  display_text(303, 182, "coup");
  display_text(306, 200, "ling");
  display_text(303, 247, "probe");
  display_text(303, 265, "mode");

  //Display the actual settings
  scope_channel2_enable_select();
  scope_channel2_fft_show();
  scope_channel2_coupling_select();
  scope_channel2_probe_magnification_select();
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(288, 46, 183, 252, 69906);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel2_enable_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if channel is disabled or enabled
  if(scopesettings.channel2.enable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(366, 62, 32, 22);
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(418, 62, 32, 22);
  }
  
  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(CHANNEL2_COLOR);
  
  //Check if channel is disabled or enabled
  if(scopesettings.channel2.enable == 0)
  {
    //Disabled so channel 2 color box behind off
    display_fill_rect(418, 62, 32, 22);
  }
  else
  {
    //Enabled so channel 2 color box behind on
    display_fill_rect(366, 62, 32, 22);
  }

  //Check if channel is disabled or enabled
  if(scopesettings.channel2.enable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(0x00000000);
  }

  //Display the on text
  display_text(372, 65, "ON");

  //Check if channel is disabled or enabled
  if(scopesettings.channel2.enable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the off text
  display_text(421, 65, "OFF");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel2_fft_show(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if fft is disabled or enabled
  if(scopesettings.channel2.fftenable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(366, 124, 32, 22);
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(418, 124, 32, 22);
  }
  
  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(CHANNEL2_COLOR);
  
  //Check if fft is disabled or enabled
  if(scopesettings.channel2.fftenable == 0)
  {
    //Disabled so channel 2 color box behind off
    display_fill_rect(418, 124, 32, 22);
  }
  else
  {
    //Enabled so channel 2 color box behind on
    display_fill_rect(366, 124, 32, 22);
  }

  //Check if fft is disabled or enabled
  if(scopesettings.channel2.fftenable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(0x00000000);
  }

  //Display the on text
  display_text(372, 127, "ON");

  //Check if fft is disabled or enabled
  if(scopesettings.channel2.fftenable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the off text
  display_text(421, 127, "OFF");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel2_coupling_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if coupling is dc or ac
  if(scopesettings.channel2.coupling == 0)
  {
    //DC so dark grey box behind ac text
    display_fill_rect(418, 188, 32, 22);
  }
  else
  {
    //AC so dark grey box behind dc text
    display_fill_rect(366, 188, 32, 22);
  }

  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(CHANNEL2_COLOR);
  
  //Check if coupling is dc or ac
  if(scopesettings.channel2.coupling == 0)
  {
    //DC so channel 2 color box behind dc text
    display_fill_rect(366, 188, 32, 22);
  }
  else
  {
    //AC so channel 2 color box behind ac text
    display_fill_rect(418, 188, 32, 22);
  }

  //Check if coupling is dc or ac
  if(scopesettings.channel2.coupling == 0)
  {
    //DC so black dc text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //AC so white dc text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the dc text
  display_text(372, 191, "DC");

  //Check if coupling is dc or ac
  if(scopesettings.channel2.coupling == 0)
  {
    //DC so white ac text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //AC so black ac text
    display_set_fg_color(0x00000000);
  }

  //Display the off text
  display_text(423, 191, "AC");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel2_probe_magnification_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(0x00181818);
  
  //Check if coupling is dc or ac
  switch(scopesettings.channel2.magnification)
  {
    case 0:
      //dark grey times 10 and 100 magnification
      display_fill_rect(397, 245, 23, 38);
      display_fill_rect(426, 245, 30, 38);
      break;
      
    case 1:
      //dark grey times 1 and 100 magnification
      display_fill_rect(366, 245, 20, 38);
      display_fill_rect(426, 245, 30, 38);
      break;
      
    default:
      //dark grey times 1 and 10 magnification
      display_fill_rect(366, 245, 20, 38);
      display_fill_rect(397, 245, 23, 38);
      break;
  }
  
  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(CHANNEL2_COLOR);
  
  //Check if which magnification to highlight
  switch(scopesettings.channel2.magnification)
  {
    case 0:
      //Highlight times 1 magnification
      display_fill_rect(366, 245, 20, 38);
      break;
      
    case 1:
      //Highlight times 10 magnification
      display_fill_rect(397, 245, 23, 38);
      break;
      
    default:
      //Highlight times 100 magnification
      display_fill_rect(426, 245, 30, 38);
      break;
  }

  //Check if magnification is 1x
  if(scopesettings.channel2.magnification == 0)
  {
    //Yes so black 1X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 1X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 1X text
  display_text(372, 247, "1");
  display_text(371, 265, "X");

  //Check if magnification is 10x
  if(scopesettings.channel2.magnification == 1)
  {
    //Yes so black 10X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 10X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 10X text
  display_text(401, 247, "10");
  display_text(403, 265, "X");

  //Check if magnification is 100x
  if(scopesettings.channel2.magnification > 1)
  {
    //Yes so black 100X text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white 100X text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the 100X text
  display_text(430, 247, "100");
  display_text(437, 265, "X");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_trigger_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(560, 46, 172, 186);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(560, 46, 172, 186);
  
  //Three black lines between the settings
  display_set_fg_color(0x00000000);
  display_draw_horz_line(107, 570, 722);
  display_draw_horz_line(168, 570, 722);
  
  //Main texts in white  
  display_set_fg_color(0x00FFFFFF);
  
  //Select the font for the texts
  display_set_font(&font_3);
  
  //Display the texts
  display_text(571,  56, "trig");
  display_text(571,  75, "mode");
  display_text(571, 118, "trig");
  display_text(571, 137, "edge");
  display_text(571, 182, "trig");
  display_text(573, 200, "CH");

  //Display the actual settings
  scope_trigger_mode_select();
  scope_trigger_edge_select();
  scope_trigger_channel_select();
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(560, 46, 172, 186, 56415);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_mode_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(0x00181818);
  
  //Check which trigger mode is selected
  switch(scopesettings.triggermode)
  {
    case 0:
      //dark grey single and normal
      display_fill_rect(661, 57, 20, 38);
      display_fill_rect(692, 57, 21, 38);
      break;
      
    case 1:
      //dark grey auto and normal
      display_fill_rect(629, 57, 20, 38);
      display_fill_rect(692, 57, 21, 38);
      break;
      
    default:
      //dark grey auto and single
      display_fill_rect(629, 57, 20, 38);
      display_fill_rect(661, 57, 20, 38);
      break;
  }
  
  //Set trigger color for the box behind the selected text
  display_set_fg_color(TRIGGER_COLOR);
  
  //Check if which trigger mode to highlight
  switch(scopesettings.triggermode)
  {
    case 0:
      //Highlight auto mode
      display_fill_rect(629, 57, 20, 38);
      break;
      
    case 1:
      //Highlight single mode
      display_fill_rect(661, 57, 20, 38);
      break;
      
    default:
      //Highlight normal mode
      display_fill_rect(692, 57, 21, 38);
      break;
  }

  //Check if trigger mode is auto
  if(scopesettings.triggermode == 0)
  {
    //Yes so black auto text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white auto text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the auto text
  display_text(631, 58, "au");
  display_text(633, 75, "to");

  //Check if trigger mode is single
  if(scopesettings.triggermode == 1)
  {
    //Yes so black single text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white single text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the single text
  display_text(666, 56, "si");
  display_text(663, 66, "ng");
  display_text(665, 79, "le");

  //Check if trigger mode is normal
  if(scopesettings.triggermode > 1)
  {
    //Yes so black normal text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //No so white normal text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the normal text
  display_text(695, 56, "no");
  display_text(694, 66, "rm");
  display_text(696, 79, "al");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_edge_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check which trigger edge is selected
  if(scopesettings.triggeredge == 0)
  {
    //Rising so dark grey box behind falling
    display_fill_rect(671, 125, 45, 22);
  }
  else
  {
    //Falling so dark grey box behind rising
    display_fill_rect(626, 125, 40, 22);
  }
  
  //Set trigger color for the box behind the selected text
  display_set_fg_color(TRIGGER_COLOR);
  
  //Check which trigger edge is selected
  if(scopesettings.triggeredge == 0)
  {
    //Rising so trigger color box behind rising
    display_fill_rect(626, 125, 40, 22);
  }
  else
  {
    //Falling so trigger color box behind falling
    display_fill_rect(671, 125, 45, 22);
  }

  //Check which trigger edge is selected
  if(scopesettings.triggeredge == 0)
  {
    //Rising so black rising text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //Falling so white rising text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the rising text
  display_text(629, 127, "rising");

  //Check which trigger edge is selected
  if(scopesettings.triggeredge == 0)
  {
    //Rising so white falling text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Falling so black falling text
    display_set_fg_color(0x00000000);
  }

  //Display the falling text
  display_text(674, 127, "falling");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_channel_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(0x00181818);
  
  //Check if channel is 1 or 2
  if(scopesettings.triggerchannel == 0)
  {
    //1 so dark grey box behind CH2 text
    display_fill_rect(680, 188, 32, 22);
  }
  else
  {
    //2 so dark grey box behind CH1 text
    display_fill_rect(632, 188, 32, 22);
  }

  //Set trigger color for the box behind the selected text
  display_set_fg_color(TRIGGER_COLOR);
  
  //Check if channel is 1 or 2
  if(scopesettings.triggerchannel == 0)
  {
    //1 so trigger color box behind CH1 text
    display_fill_rect(632, 188, 32, 22);
  }
  else
  {
    //2 so trigger color box behind CH2 text
    display_fill_rect(680, 188, 32, 22);
  }

  //Check if channel is 1 or 2
  if(scopesettings.triggerchannel == 0)
  {
    //1 so black CH1 text
    display_set_fg_color(0x00000000);
  }
  else
  {
    //2 so white CH1 text
    display_set_fg_color(0x00FFFFFF);
  }

  //Display the CH1 text
  display_text(635, 191, "CH1");

  //Check if channel is 1 or 2
  if(scopesettings.triggerchannel == 0)
  {
    //1 so white CH2 text
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //2 so black CH2 text
    display_set_fg_color(0x00000000);
  }

  //Display the CH2 text
  display_text(683, 191, "CH2");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_system_settings_menu(void)
{
  int y;
  
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(150, 46, 244, 353);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(150, 46, 244, 353);
  
  //Five black lines between the settings
  display_set_fg_color(0x00000000);
  
  for(y=104;y<350;y+=59)
  {
    display_draw_horz_line(y, 159, 385);
  }
  
  //Display the menu items  
  scope_system_settings_screen_brightness_item(0);
  scope_system_settings_grid_brightness_item(0);
  scope_system_settings_trigger_50_item();
  scope_system_settings_calibration_item(0);
  scope_system_settings_x_y_mode_item();
  scope_system_settings_confirmation_item();
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(150, 46, 244, 353, 63039);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_screen_brightness_item(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(0x00FFFF00);
  }
  
  //Draw the background
  display_fill_rect(159, 59, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00FFFF00);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(screen_brightness_icon, 171, 63, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(231, 60, "Screen");
  display_text(220, 76, "brightness");
  
  //Show the actual setting
  scope_system_settings_screen_brightness_value();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_screen_brightness_value(void)
{
  //Draw the yellow background
  display_set_fg_color(0x00FFFF00);
  display_fill_rect(332, 67, 32, 15);
  
  //Display the number with fixed width font and black color
  display_set_font(&font_0);
  display_set_fg_color(0x00000000);
  display_decimal(337, 68, scopesettings.screenbrightness);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_grid_brightness_item(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(0x00FFFF00);
  }
  
  //Draw the background
  display_fill_rect(159, 116, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00FFFF00);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(grid_brightness_icon, 171, 122, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(240, 119, "Grid");
  display_text(220, 135, "brightness");
  
  //Show the actual setting
  scope_system_settings_grid_brightness_value();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_grid_brightness_value(void)
{
  //Draw the yellow background
  display_set_fg_color(0x00FFFF00);
  display_fill_rect(332, 124, 32, 15);
  
  //Display the number with fixed width font and black color
  display_set_font(&font_0);
  display_set_fg_color(0x00000000);
  display_decimal(337, 125, scopesettings.gridbrightness);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_trigger_50_item(void)
{
  //Set the colors for white foreground and grey background
  display_set_fg_color(0x00FFFFFF);
  display_set_bg_color(0x00181818);

  //Display the icon with the set colors
  display_copy_icon_use_colors(trigger_50_percent_icon, 171, 181, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(229, 178, "Always");
  display_text(217, 194, "trigger 50%");
  
  //Show the state
  scope_display_slide_button(326, 183, scopesettings.alwaystrigger50);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_calibration_item(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00181818);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(0x00FFFF00);
  }
  
  //Draw the background
  display_fill_rect(159, 235, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(0x00FFFFFF);
    display_set_bg_color(0x00181818);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(0x00000000);
    display_set_bg_color(0x00FFFF00);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(baseline_calibration_icon, 171, 239, 24, 25);
  
  //Display the text
  display_set_font(&font_3);
  display_text(225, 237, "Baseline");
  display_text(219, 253, "calibration");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_x_y_mode_item(void)
{
  //Set the colors for white foreground and grey background
  display_set_fg_color(0x00FFFFFF);
  display_set_bg_color(0x00181818);

  //Display the icon with the set colors
  display_copy_icon_use_colors(x_y_mode_display_icon, 171, 297, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(223, 295, "X-Y mode");
  display_text(231, 311, "display");
  
  //Show the state
  scope_display_slide_button(326, 299, scopesettings.xymodedisplay);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_confirmation_item(void)
{
  //Set the colors for white foreground and grey background
  display_set_fg_color(0x00FFFFFF);
  display_set_bg_color(0x00181818);

  //Display the icon with the set colors
  display_copy_icon_use_colors(confirmation_icon, 171, 356, 24, 24);
  
  //Display the text
  display_set_font(&font_3);
  display_text(217, 354, "Notification");
  display_text(213, 370, "confirmation");
  
  //Show the state
  scope_display_slide_button(326, 358, scopesettings.confirmationmode);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_calibration_start_text(void)
{
  //Save the screen under the baseline calibration start text
  display_set_destination_buffer(displaybuffer2);
  display_copy_rect_from_screen(395, 222, 199, 59);

  //Setup the text in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);

  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  display_fill_rect(395, 222, 199, 59);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_3);
  display_text(409, 227, "Please unplug");
  display_text(409, 243, "the probe and");
  display_text(409, 259, "USB first !");

  //Add the ok button
  scope_display_ok_button(517, 230, 0);
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(395, 222, 199, 59, 63039);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibrating_text(void)
{
  //Restore the screen from under the calibration start text to get rid of it
  display_set_source_buffer(displaybuffer2);
  display_copy_rect_to_screen(395, 222, 199, 59);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  display_fill_rect(395, 222, 110, 59);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  display_draw_rect(395, 222, 110, 59);

  //Display the text in white
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_3);
  display_text(409, 243, "Calibrating...");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibration_done_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  display_fill_rect(395, 222, 110, 59);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  display_draw_rect(395, 222, 110, 59);

  //Display the text in white
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_3);
  display_text(414, 235, "Calibration");
  display_text(416, 251, "successful");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_measures_menu(void)
{
  int item;
  int channel;
  
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);
  
  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  display_fill_rect(231, 263, 499, 214);

  //Draw the edge in black
  display_set_fg_color(0x00000000);
  display_draw_rect(231, 263, 499, 214);
  
  //Three horizontal black lines between the settings
  display_draw_horz_line(288, 232, 729);
  display_draw_horz_line(350, 232, 729);
  display_draw_horz_line(412, 232, 729);

  //Vertical separator between the channel sections
  display_draw_vert_line(481, 264, 476);
  
  //Vertical separators between the items
  display_draw_vert_line(294, 289, 476);
  display_draw_vert_line(356, 289, 476);
  display_draw_vert_line(418, 289, 476);
  display_draw_vert_line(544, 289, 476);
  display_draw_vert_line(606, 289, 476);
  display_draw_vert_line(668, 289, 476);
  
  //Channel 1 top bar
  display_set_fg_color(CHANNEL1_COLOR);
  display_fill_rect(482, 264, 247, 23);
  
  //Channel 2 top bar
  display_set_fg_color(CHANNEL2_COLOR);
  display_fill_rect(232, 264, 248, 23);

  //Display the channel identifier text in black
  display_set_fg_color(0x00000000);
  display_set_font(&font_2);
  display_text(490, 269, "CH1");
  display_text(240, 269, "CH2");
  
  //Display the menu items
  for(channel=0;channel<2;channel++)
  {
    //For each channel 12 items
    for(item=0;item<12;item++)
    {
      //Draw the separate items
      scope_measures_menu_item(channel, item);
    }
  }
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_right_rect_onto_screen(231, 263, 499, 214, 75646);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_measures_menu_item(int channel, int item)
{
  uint16  xpos;
  uint16  ypos;
  char   *text;
  
  //Set the x position offset for the given channel
  if(channel == 0)
  {
    //Channel 1 is on the right side
    xpos = 482;
  }
  else
  {
    //Channel 2 is on the left side
    xpos = 232;
  }
  
  //Set the text and the position for the given item 
  switch(item)
  {
    case 0:
      text  = "Vmax";
      xpos += 15;
      ypos  = 312;
      break;

    case 1:
      text  = "Vmin";
      xpos += 79;
      ypos  = 312;
      break;

    case 2:
      text  = "Vavg";
      xpos += 141;
      ypos  = 312;
      break;

    case 3:
      text  = "Vrms";
      xpos += 203;
      ypos  = 312;
      break;

    case 4:
      text  = "VPP";
      xpos += 19;
      ypos  = 376;
      break;

    case 5:
      text  = "VP";
      xpos += 86;
      ypos  = 376;
      break;

    case 6:
      text  = "Freq";
      xpos += 143;
      ypos  = 376;
      break;

    case 7:
      text  = "Cycle";
      xpos += 201;
      ypos  = 376;
      break;

    case 8:
      text  = "Tim+";
      xpos += 17;
      ypos  = 437;
      break;

    case 9:
      text  = "Tim-";
      xpos += 80;
      ypos  = 437;
      break;

    case 10:
      text  = "Duty+";
      xpos += 138;
      ypos  = 437;
      break;

    case 11:
      text  = "Duty-";
      xpos += 202;
      ypos  = 437;
      break;
      
    default:
      return;
  }
  
  //Check if item is on or off
  if(scopesettings.measuresstate[channel][item] == 0)
  {
    //Off so some dark grey text
    display_set_fg_color(0x00444444);
  }
  else
  {
    //On so white text
    display_set_fg_color(0x00FFFFFF);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(xpos, ypos, text);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_slider(uint16 xpos, uint16 ypos, uint8 position)
{
  //Save the screen under the screen brightness slider
  display_set_destination_buffer(displaybuffer2);
  display_copy_rect_from_screen(xpos, ypos, 331, 58);

  //Setup the slider menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffer1);

  //Draw the background in dark grey
  display_set_fg_color(0x00181818);
  
  //Fill the background
  display_fill_rect(xpos, ypos, 331, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(0x00333333);
  
  //Draw the edge
  display_draw_rect(xpos, ypos, 331, 58);
  
  //Display the actual slider
  scope_display_slider(xpos, ypos, position);
  
  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffer1);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(xpos, ypos, 331, 58, 63039);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_slider(uint16 xpos, uint16 ypos, uint8 position)
{
  uint16 x = xpos + 20;
  uint16 y = ypos + 24;
  uint16 w = (291 * position) / 100;
  uint16 ys = ypos + 23;
  uint16 ye = ypos + 35;

  //Clear the background first
  display_set_fg_color(0x00181818);
  display_fill_rect(xpos + 8, ypos + 17, 315, 24);
  
  //Draw the first part of the slider bar in a yellow color
  display_set_fg_color(0x00FFFF00);
  display_fill_rounded_rect(x, y, w, 10, 2);
  
  //Adjust positions for the grey part
  x += w;
  w  = 291 - w;
  
  //Draw the last part of the slider bar in a light grey color
  display_set_fg_color(0x00666666);
  display_fill_rounded_rect(x, y, w, 10, 2);
  
  //Adjust positions for drawing the knob
  x -= 11;
  y -= 6;
  
  //Draw the knob
  display_set_fg_color(0x00AAAAAA);
  display_fill_rounded_rect(x, y, 22, 22, 2);
  
  //Draw the black lines on the knob
  display_set_fg_color(0x00000000);
  display_draw_vert_line(x +  6, ys, ye);
  display_draw_vert_line(x + 11, ys, ye);
  display_draw_vert_line(x + 16, ys, ye);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_move_slider(uint16 xpos, uint16 ypos, uint8 *position)
{
  uint16 xs = xpos + 20;
  uint16 value;
  int16 filter = xtouch - prevxtouch;
  
  //Check if update needed
  if((filter > -3) && (filter < 3))
  {
    //When change in movement less the absolute 3 don't process
    return(0);
  }
  
  //Save for next filter check
  prevxtouch = xtouch;
  
  //Make sure it stays in allowed range
  if(xtouch <= xs)
  {
    //Below slider keep it on 0
    value = 0;
  }
  else if(xtouch >= (xpos + 311))
  {
    //Above slider keep it on max
    value = 100;
  }
  else
  {
    //Based on xtouch position calculate a new position from 0 to 100
    value = ((xtouch - xs) * 100) / 291;
  }
  
  //Update the position variable
  *position = value;
  
  //Show the new position on screen
  scope_display_slider(xpos, ypos, value);
  
  //Signal there is change
  return(1);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_slide_button(uint16 xpos, uint16 ypos, uint8 state)
{
  uint16 linex      = xpos + 8;
  uint16 lineystart = ypos + 6;
  uint16 lineyend   = ypos + 15;
  uint16 buttonx    = xpos + 4;
  uint32 edgecolor  = 0x00444444;
  uint32 fillcolor  = 0x00888888;
  
  if(state == 1)
  {
    //Displace the lines and button by 19 pixels
    linex   += 19;
    buttonx += 19;

    //Set the enabled colors
    edgecolor  = 0x00008800;
    fillcolor  = 0x0000FF00;
  }
  
  //Draw the background
  display_set_fg_color(fillcolor);
  display_fill_rounded_rect(xpos, ypos, 45, 21, 2);

  //Draw the edge
  display_set_fg_color(edgecolor);
  display_draw_rounded_rect(xpos, ypos, 45, 21, 2);

  //Draw button in dark grey
  display_set_fg_color(0x00444444);
  display_fill_rect(buttonx, ypos + 4, 19, 13);
  
  //Draw lines in black
  display_set_fg_color(0x00000000);  
  display_draw_vert_line(linex,     lineystart, lineyend);
  display_draw_vert_line(linex + 3, lineystart, lineyend);
  display_draw_vert_line(linex + 6, lineystart, lineyend);
  display_draw_vert_line(linex + 9, lineystart, lineyend);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_ok_button(uint16 xpos, uint16 ypos, uint8 mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(0x00333333);
  }
  else
  {
    //Active so light grey background
    display_set_fg_color(0x00CCCCCC);
  }
  
  //Draw the background
  display_fill_rect(xpos, ypos, 66, 44);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground
    display_set_fg_color(0x00FFFFFF);
  }
  else
  {
    //Active so black foreground
    display_set_fg_color(0x00000000);
  }
  
  //Display the text
  display_set_font(&font_3);
  display_text(xpos + 24, ypos + 14, "OK");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_adjust_timebase(void)
{
  int32 diff;
  
  //Check if touch within the trace display region
  if((previousxtouch > 2) && (previousxtouch < 720) && (previousytouch > 50) && (previousytouch < 470))
  {
    //Check if touch on the left of the center line
    if(previousxtouch < 358)
    {
      //Check if not already on the highest setting (50S/div)
      if(scopesettings.timeperdiv > 0)
      {
        //Go up in time by taking one of the setting
        scopesettings.timeperdiv--;
        
        //18-11-2021 Temp for saving sample files
        saved_sample_buffers_count = 0;
        
      }
    }
    //Check if touch on the right of the center line
    else if(previousxtouch > 362)
    {
      //Check if not already on the lowest setting (10nS/div)
      if(scopesettings.timeperdiv < 29)
      {
        //Go down in time by adding one to the setting
        scopesettings.timeperdiv++;
        
        //18-11-2021 Temp for saving sample files
        saved_sample_buffers_count = 0;
        
      }
    }
    
    //Check if in stopped state to limit on selectable range
    if(scopesettings.runstate)    
    {
      //Check if new setting above 10mS/div. (below 11)
      if(scopesettings.timeperdiv < 11)
      {
        //Keep it on 10mS/div if so
        scopesettings.timeperdiv = 11;
      }

      //Check if new setting within allowed range from backed up setting, which is time base of last sample data
      //Calculate the step difference between the two settings
      diff = scopesettings.timeperdiv - scopesettings.timeperdivbackup;

      //Limit it on 3 steps either side
      if(diff > 3)
      {
        //Add three if the new setting is above the old
        scopesettings.timeperdiv = scopesettings.timeperdivbackup + 3;

        //Limit on max for table index
        diff = 3;
      }
      else if(diff < -3)
      {
        //Subtract three if the new setting is below the old
        scopesettings.timeperdiv = scopesettings.timeperdivbackup - 3;

        //Limit on min for table index
        diff = -3;
      }

      //Set sample buffer position select for zoom in stop mode. This also selects an up sampling function as needed
      zoom_select = zoom_select_settings[scopesettings.timeperdivbackup % 3][diff + 3];
    }

    //For trigger modes single and normal the time base setting can't go up beyond 50mS/div
    if((scopesettings.triggermode) && (scopesettings.timeperdiv < 9))
    {
      //In that case limit it on 50mS/div
      scopesettings.timeperdiv = 9;
    }
    
    //When in run state save the new setting in the backup
    if(scopesettings.runstate == 0)
    {
      //Save the setting
      scopesettings.timeperdivbackup = scopesettings.timeperdiv;
    }
    
    //Set the new setting in the FPGA
    fpga_set_trigger_timebase();
    
    //Show he new setting on the display
    scope_time_div_setting();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_grid(void)
{
  uint32 color;
  register uint32 i;
  
  //Only draw the grid when something will show (not in the original code)
  if(scopesettings.gridbrightness > 3)
  {
    //Calculate a grey shade based on the grid brightness setting
    color = (scopesettings.gridbrightness * 255) / 100;
    color = (color << 16) | (color << 8) | color;

    //Set the color for drawing
    display_set_fg_color(color);
    
    //Draw the edge
    display_draw_rect(2, 46, 726, 404);

    //Draw the center lines
    display_draw_horz_line(249,  2, 726);
    display_draw_vert_line(364, 46, 448);
    
    //Draw the ticks on the x line
    for(i=4;i<726;i+=5)
    {
      display_draw_vert_line(i, 247, 251);
    }

    //Draw the ticks on the y line
    for(i=49;i<448;i+=5)
    {
      display_draw_horz_line(i, 362, 366);
    }

    //Draw the horizontal dots
    for(i=99;i<448;i+=50)
    {
      display_draw_horz_dots(i, 4, 726, 5);
    }

    //Draw the vertical dots
    for(i=14;i<726;i+=50)
    {
      display_draw_vert_dots(i, 49, 448, 5);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_pointers(void)
{
  uint32 position;
  
  //Draw channel 1 pointer when it is enabled
  if(scopesettings.channel1.enable)
  {
    //Set the colors for drawing
    display_set_fg_color(CHANNEL1_COLOR);
    display_set_bg_color(0x00000000);

    //Select the font for this pointer id
    display_set_font(&font_0);
    
    //Check if in normal or x-y display mode
    if(scopesettings.xymodedisplay == 0)
    {
      //y position for the channel 1 trace center pointer.
      position = 441 - scopesettings.channel1.traceoffset;

      //Limit on the top of the displayable region
      if(position < 46)
      {
        position = 46;
      }
      //Limit on the bottom of the displayable region
      else if(position > 441)
      {
        position = 441;
      }

      //Draw the pointer
      display_left_pointer(2, position, '1');
    }
    else
    {
      //y position for the channel 1 trace center pointer.
      position = 154 + scopesettings.channel1.traceoffset;

      //Limit on the left of the active range
      if(position < 166)
      {
        position = 166;
      }
      //Limit on the right of the active range
      else if(position > 548)
      {
        position = 548;
      }

      //Draw the pointer
      display_top_pointer(position, 47, '1');
    }
  }
  
  //Draw channel 2 pointer when it is enabled
  if(scopesettings.channel2.enable)
  {
    //y position for the channel 2 trace center pointer
    position = 441 - scopesettings.channel2.traceoffset;
    
    //Limit on the top of the displayable region
    if(position < 46)
    {
      position = 46;
    }
    //Limit on the bottom of the displayable region
    else if(position > 441)
    {
      position = 441;
    }
    
    //Set the colors for drawing
    display_set_fg_color(CHANNEL2_COLOR);
    display_set_bg_color(0x00000000);
    
    //Select the font for this pointer id
    display_set_font(&font_0);
    
    //Draw the pointer
    display_left_pointer(2, position, '2');
  }
  
  //Draw trigger position and level pointer when time base in range of 10mS/div - 10nS/div and in normal display mode
  if((scopesettings.timeperdiv > 10) && (scopesettings.xymodedisplay == 0))
  {
    //x position for the trigger position pointer
    position = scopesettings.triggerposition + 2;
    
    //Limit on the left of the displayable region
    if(position < 2)
    {
      position = 2;
    }
    //Limit on the right of the displayable region
    else if(position > 712)
    {
      position = 712;
    }

    //Set the colors for drawing
    display_set_fg_color(TRIGGER_COLOR);
    display_set_bg_color(0x00000000);

    //Select the font for this pointer id
    display_set_font(&font_3);
    
    //Draw the pointer
    display_top_pointer(position, 47, 'H');

    //y position for the trigger level pointer
    position = 448 - scopesettings.triggeroffset;
    
    //Limit on the top of the displayable region
    if(position < 46)
    {
      position = 46;
    }
    //Limit on the bottom of the displayable region
    else if(position > 441)
    {
      position = 441;
    }

    //Need to reset the fore ground color
    display_set_fg_color(TRIGGER_COLOR);
    
    //Draw the pointer
    display_right_pointer(707, position, 'T');
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_time_cursors(void)
{
  //Only draw the lines when enabled
  if(scopesettings.timecursorsenable)
  {
    //Set the color for the dashed lines
    display_set_fg_color(CURSORS_COLOR);
   
    //Draw the lines
    display_draw_vert_dashes(scopesettings.timecursor1position, 48, 448, 3, 3);
    display_draw_vert_dashes(scopesettings.timecursor2position, 48, 448, 3, 3);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_volt_cursors(void)
{
  //Only draw the lines when enabled
  if(scopesettings.voltcursorsenable)
  {
    //Set the color for the dashed lines
    display_set_fg_color(CURSORS_COLOR);
   
    //Draw the lines
    display_draw_horz_dashes(scopesettings.voltcursor1position, 5, 726, 3, 3);
    display_draw_horz_dashes(scopesettings.voltcursor2position, 5, 726, 3, 3);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_process_trace_data(void)
{
  //Based on the time base setting different actions are needed for getting the trace data
  //Check on time base setting if range is between 50S/div and 100ms/div
  if(scopesettings.timeperdiv < 9)
  {
    //Reads an average of ten data points for each channel, active or not
    scope_get_long_timebase_data();
    
    //Point needs to be drawn twice to be on the correct time base speed
    //So called here as well as in the main loop
    scope_display_trace_data();
  }
  else
  {
    //Check if running and not in a trace or cursor displacement state
    if((scopesettings.runstate == 0) && (touchstate == 0))
    {
      //Read trace data for active channels
      scope_get_short_timebase_data();
      
      //scope_process_short_timebase_data();
    }
    else
    {
      //Makes not a lot of sense since the scope is not running or being touched and the next function is also called in the main loop directly after this one
      scope_display_trace_data();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_get_long_timebase_data(void)
{
  //Default timeout for 50S/div
  uint32 timeout = 2000;
  uint32 curticks;
  uint32 signaladjust;
  uint32 temp1, temp2;
  uint16 *ptr;
  
  //Send the time base command for the longer settings
  fpga_set_long_timebase();
  
  //Get the delay setting based on the time base
  switch(scopesettings.timeperdiv)
  {
    //20S/div
    case 1:
      timeout = 1000;
      break;

    //10S/div
    case 2:
      timeout = 400;
      break;

    //5S/div
    case 3:
      timeout = 200;
      break;

    //2S/div
    case 4:
      timeout = 80;
      break;

    //1S/div
    case 5:
      timeout = 40;
      break;

    //500mS/div
    case 6:
      timeout = 20;
      break;

    //200mS/div
    case 7:
      timeout = 8;
      break;

    //100mS/div
    case 8:
      timeout = 4;
      break;
  }
  
  //Make the timeout timer tick related by adding it to the previous capture
  timeout += scopesettings.previoustimerticks;
  
  //For smaller timeouts (500mS/div, 200mS/div and 100mS/div) stay in the wait loop even if there is touch
  while((scopesettings.timeperdiv > 5) || (havetouch == 0))
  {
    //Get the current ticks
    curticks = timer0_get_ticks();
  
    //Check if there is a timeout
    //While technically prone to error on timer ticks overflow the scope has to be running for >49 days before it occurs
    if(curticks >= timeout)
    {
      //Save the current ticks for next timeout and bail out the loop
      scopesettings.previoustimerticks = curticks;
      goto skip_delay;
    }
    
    //Scan the touch panel to see if there is user input
    tp_i2c_read_status();
  }
    
  //Wait an extra 40 milliseconds when there was touch
  timer0_delay(40);
  
skip_delay:
  //Some mode select command for the FPGA (0x01 for long time base)
  fpga_write_cmd(0x28);
  fpga_write_byte(0x01);
  
  //Read, accumulate and average 10 bytes of channel 1 trace data
  channel1tracebuffer1[0] = fpga_average_trace_data(0x24);
  
  //Read, accumulate and average 10 bytes of channel 2 trace data
  channel2tracebuffer1[0] = fpga_average_trace_data(0x26);
  
  //Need insight in the code that displays the data to get an understanding of the next bit of code
  //It is a more or less straight conversion from what Ghidra shows
  
  //Some fractional scaling on the signal to fit it on screen???
  //Adjust the channel 1 signal based on the volts per div setting
  signaladjust = channel1tracebuffer1[0] * signal_adjusters[scopesettings.channel1.voltperdiv];
  temp1 = ((0xA3D7 * signaladjust) + 0xA3D7) >> 0x16;
  temp2 = signaladjust + (((int64)((int64)signaladjust * (int64)0x51EB851F) >> 0x25) * -100);
  
  //If above half the pixel up to next one?????
  if(temp2 > 50)
  {
    temp1++;
  }
  
  //Store it somewhere
  channel1tracebuffer3[0] = temp1;                    //At address 0x801A916A in original code

  //Check if data needs to be doubled
  //This is missing in the original code  
  if(scopesettings.channel1.voltperdiv == 6)
  {
    //Only on highest sensitivity
    temp1 <<= 1;
    
    //Check if the data is smaller then the offset
    if(temp1 < scopesettings.channel1.traceoffset)
    {
      //If so limit to top of the screen
      temp1 = 0;
    }
    else
    {
      //Else take of the offset
      temp1 = temp1 - scopesettings.channel1.traceoffset;
    }
  }
  
  //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
  ptr = (uint16 *)channel1tracebuffer4;
  
  //Check if outside displayable range??
  if(temp1 > 401)
  {
    //Keep it on max allowed
    *ptr = 401;                    //At address 0x801AC04A in original code
  }
  else
  {
    //Else store it again in an other location
    *ptr = temp1;
  }
  
  //Some fractional scaling on the signal to fit it on screen???
  //Adjust the channel 2 signal based on the volts per div setting
  signaladjust = channel2tracebuffer1[0] * signal_adjusters[scopesettings.channel2.voltperdiv];
  temp1 = ((0xA3D7 * signaladjust) + 0xA3D7) >> 0x16;
  temp2 = signaladjust + (((int64)((int64)signaladjust * (int64)0x51EB851F) >> 0x25) * -100);
  
  //If above half the pixel up to next one?????
  if(temp2 > 50)
  {
    temp1++;
  }
  
  //Store it somewhere
  channel2tracebuffer3[0] = temp1;               //At address 0x801AA8DA in original code

  //Check if data needs to be doubled
  //This is missing in the original code  
  if(scopesettings.channel2.voltperdiv == 6)
  {
    //Only on highest sensitivity
    temp1 <<= 1;
    
    //Check if the data is smaller then the offset
    if(temp1 < scopesettings.channel2.traceoffset)
    {
      //If so limit to top of the screen
      temp1 = 0;
    }
    else
    {
      //Else take of the offset
      temp1 = temp1 - scopesettings.channel2.traceoffset;
    }
  }
  
  //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
  ptr = (uint16 *)channel2tracebuffer4;
  
  //Check if outside displayable range??
  if(temp1 > 401)
  {
    //Keep it on max allowed
    *ptr = 401;               //At address 0x801AD7BA in original code
  }
  else
  {
    //Else store it again in an other location
    *ptr = temp1;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_get_short_timebase_data(void)
{
  uint32 data;
  uint32 count;
  uint32 command;
  uint32 triggered = 0;
  uint32 signaladjust;
  
  //Check if time base in range 50mS/div - 50nS/div or trigger mode is single or normal
  if((scopesettings.timeperdiv < 28) || scopesettings.triggermode)
  {
    //Set the trigger level if so
    fpga_set_trigger_level();
  }
  
  //Write the short time base setting to the FPGA
  fpga_set_short_timebase();
  
  //Some bounds setting. Zero here, One after trigger???
  fpga_write_cmd(0x0F);
  fpga_write_byte(0x00);

  //Some mode select command for the FPGA (0x00 for short time base)
  fpga_write_cmd(0x28);
  fpga_write_byte(0x00);

  //Some start command??
  fpga_write_cmd(0x01);
  fpga_write_byte(0x01);
  
  //Send check on ready command
  fpga_write_cmd(0x05);
  
  //Wait for the flag to become 1
  while((fpga_read_byte() & 1) == 0);
  
  //Test again to make sure it was no glitch?????
  while((fpga_read_byte() & 1) == 0);
  
  //Some stop command??
  fpga_write_cmd(0x01);
  fpga_write_byte(0x00);
  
  //Backup the settings
  scopesettings.timeperdivbackup = scopesettings.timeperdiv;
  scopesettings.channel1.voltperdivbackup = scopesettings.channel1.voltperdiv;
  scopesettings.channel2.voltperdivbackup = scopesettings.channel2.voltperdiv;
  
  //Check if screen needs to be redrawn
  if(scopesettings.updatescreen)
  {
    //Reset the flag so only done once until new request
    scopesettings.updatescreen = 0;

    //Use a separate buffer to clear the screen
    display_set_screen_buffer(displaybuffer1);
      
    //Clear the trace portion of the screen
    display_set_fg_color(0x00000000);
    display_fill_rect(2, 46, 728, 434);
    
    //Draw the grid lines and dots based on the grid brightness setting
    scope_draw_grid();
    
    //Draw the signal center, trigger level and trigger position pointers
    scope_draw_pointers();

    //Copy it to the actual screen buffer
    display_set_source_buffer(displaybuffer1);
    display_set_screen_buffer((uint16 *)maindisplaybuffer);
    display_copy_rect_to_screen(2, 46, 728, 434);
  }
  
  //Send check on triggered command to the FPGA
  fpga_write_cmd(0x0A);
  
  //Wait for the FPGA to signal triggered or touch panel is touched or signals or cursors are being moved
  while(((fpga_read_byte() & 1) == 0) && (havetouch == 0) && (touchstate == 0))
  {
    //Scan the touch panel
    tp_i2c_read_status();
  }
  
  //Some bounds setting. Zero before, one here after trigger???
  fpga_write_cmd(0x0F);
  fpga_write_byte(0x01);
  
  //Check on trigger mode not being auto
  if(scopesettings.triggermode)
  {
    //Check if there is touch
    if(havetouch)
    {
      //Signal this in some state variables
      scopesettings.triggerflag2 = 1;
      
      //Check if single mode
      if(scopesettings.triggermode == 1)
      {
        //Signal this in the flags
        scopesettings.triggerflag1 = 0;
        scopesettings.triggerflag3 = 0;
      }
      else
      {
        //In normal mode
        if(scopesettings.triggerflag3 == 0)
        {
          scopesettings.triggerflag1 = 0;
        }
        else
        {
          scopesettings.triggerflag1 = 1;
        }
      }
      
      return;
    }
    
    //Check if in single mode
    if(scopesettings.triggermode == 1)
    {
      //Switch to stopped
      scopesettings.runstate = 1;
      
      //Show this on the screen
      scope_run_stop_text();
      
      //Backup the settings
      scopesettings.channel1.voltperdivbackup = scopesettings.channel1.voltperdiv;
      scopesettings.channel2.voltperdivbackup = scopesettings.channel2.voltperdiv;
    }
    
    //Switch some mode
    scopesettings.triggerflag3 = 1;
  }

  //Not sure if this name is correct. Sends command 0x14 and formats and translates returned data
  //Later on used to send to the FPGA with command 0x1F
  data = fpga_prepare_for_transfer();

  //Handle the returned data based on the time base setting
  //For the range 50mS/div - 500nS/div (50S/div - 100mS/div are handled in the long time base function)
  if(scopesettings.timeperdiv < 25)
  {
    //Check if range is 50mS/div - 20mS/div or 10mS/div - 500nS/div
    if(scopesettings.timeperdiv < 11)
    {
      //For 50mS/div and 20mS/div use 10
      data = 10;
    }
    else
    {
      //For 10mS/div - 500nS/div add or subtract data based on from FPGA returned value
      if(data < 750)
      {
        //Less then 750 make it bigger
        data = data + 3345;
      }
      else
      {
        //More then 750 make it smaller
        data = data - 750;
      }
    }
  }
  else
  {
    //Get correction value for the time base range 250nS/div - 10nS/div
    signaladjust = timebase_adjusters[scopesettings.timeperdiv - 25];
    
    //Check if need to add or subtract
    if(data < signaladjust)
    {
      //Perform some other adjustment
      data = 4095 - (signaladjust - data);
    }
    else
    {
      //Take adjuster of
      data = data - signaladjust;
    }
  }
  
  //Check if channel 1 is enabled
  if(scopesettings.channel1.enable)
  {
    //Send command 0x1F to the FPGA followed by the translated data returned from command 0x14
    fpga_write_cmd(0x1F);
    fpga_write_short(data);
    
    //Determine the number of bytes to read based on the time base setting
    if(scopesettings.timeperdiv < 11)
    {
      //For 50mS/div and 20mS/div only 750 bytes
      count = 750;
    }
    else
    {
      //For 10mS/div - 10nS/div 1500 bytes
      count = 1500;
    }
    
    //It seems it actually needs this. Using a fixed command 0x20 with or without the call to the special chip makes the trace garbage
    //Get the FPGA command to read from based on the trigger channel
    command = fpga_read_parameter_ic(0x0C, scopesettings.triggerchannel);
  
    //Read the bytes into a trace buffer
    fpga_read_trace_data(command, channel1tracebuffer1, count);

#if 1
    //Prepare FPGA for reading again
    //Send command 0x1F to the FPGA followed by the translated data returned from command 0x14
    fpga_write_cmd(0x1F);
    fpga_write_short(data);

    //Read the bytes into a trace buffer
    fpga_read_trace_data(0x21, channel1tracebuffer2, count);

    
    //Merge the samples from the two ADC's into the first trace buffer
    scope_interleave_samples(channel1tracebuffer1, channel1tracebuffer2, &channel1adc2calibration);
#endif

#if 0    
    //Check if data needs to be written to file
    if(saved_sample_buffers_count == 50)
    {
      int8 filename[100];
      int8 *ptr;
      
      memcpy(filename, "fnirsi_samples_", 15);
      
      ptr = scope_print_decimal(&filename[15], scopesettings.timeperdiv, 0);
      
      memcpy(ptr, ".bin", 5);
      
      //Create a file for the touch panel configuration. Fails if it already exists
      if(f_open(&viewfp, filename, FA_CREATE_NEW | FA_WRITE | FA_READ) == FR_OK)
      {
        //Write the sample data to the sd card  
        f_write(&viewfp, viewthumbnaildata, sizeof(viewthumbnaildata), 0);

        //Close the file to finish the write
        f_close(&viewfp);

        //Show the saved successful message
        scope_display_file_status_message(MESSAGE_SAVE_SUCCESSFUL, 1);
      }

      //Only do it once
      saved_sample_buffers_count++;
    }
    else if(saved_sample_buffers_count < 50)
    {
      //Copy the sample buffers for writing to file
      uint16 *dptr = (uint16 *)(viewthumbnaildata + (saved_sample_buffers_count * 6080));

      //Copy first set of samples
//      memcpy(dptr, channel1tracebuffer1, 6000);
      memcpy(dptr, channel1tracebuffer1, 3000);

      //Point to next location
      dptr += 1520;

      //Copy second set of samples
      memcpy(dptr, channel1tracebuffer2, 3000);

      //One set of buffers done
      saved_sample_buffers_count++;
    }
#endif        
    
#if 0
    //Check if triggered on this channel
    if(scopesettings.triggerchannel == 0)
    {
      triggered = scope_process_trigger();
    }
    
    //Pre process data based on time base setting
    switch(scopesettings.timeperdiv)
    {
      //250nS/div
      case 25:
        //Call up sampling function for it
        scope_up_sample_x_2(channel1tracebuffer1, 1500);
        break;

      //100nS/div
      case 26:
        //Call up sampling function for it
        scope_up_sample_x_5(channel1tracebuffer1, 1500);
        break;

      //50nS/div
      case 27:
        //Call up sampling function for it
        scope_up_sample_x_10(channel1tracebuffer1, 1500);
        break;

      //25nS/div and 10nS/div
      case 28:
      case 29:
        //Prepare FPGA for reading again
        //Send command 0x1F to the FPGA followed by the translated data returned from command 0x14
        fpga_write_cmd(0x1F);
        fpga_write_short(data);
        
        //Read the bytes into a trace buffer
        fpga_read_trace_data(0x21, channel1tracebuffer2, 1500);
        
        //Merge the samples from the two ADC's into the first trace buffer
        scope_interleave_samples(channel1tracebuffer1, channel1tracebuffer2, &channel1adc2calibration);
        
#if 0        
        //Check if data needs to be written to file
        if(saved_sample_buffers_count == 50)
        {
          //Create a file for the touch panel configuration. Fails if it already exists
          if(f_open(&viewfp, "fnirsi_25ns_samples.bin", FA_CREATE_NEW | FA_WRITE | FA_READ) == FR_OK)
          {
            //Write the touch panel configuration to the sd card  
            f_write(&viewfp, viewthumbnaildata, sizeof(viewthumbnaildata), 0);

            //Close the file to finish the write
            f_close(&viewfp);
            
            //Show the saved successful message
            scope_display_file_status_message(MESSAGE_SAVE_SUCCESSFUL, 1);
          }
          
          //Only do this once
          saved_sample_buffers_count++;
        }
        else if(saved_sample_buffers_count < 50)
        {
          //Copy the sample buffers for writing to file
          uint16 *dptr = (uint16 *)((uint32)viewthumbnaildata + (saved_sample_buffers_count * 6048));
          
          //Copy first set of samples
          memcpy(dptr, channel1tracebuffer1, 6000);
          
          //Point to next location
//          dptr += 1504;
          
          //Copy second set of samples
//          memcpy(dptr, channel1tracebuffer2, 3000);
          
          //One set of buffers done
          saved_sample_buffers_count++;
        }
#endif        
        
        //No special signal generation when input frequency is high
        //For now just do the correct up sampling for either 25nS/div or 10nS/div
        if(scopesettings.timeperdiv == 28)
        {
          //For 25nS/div need to up sample by 10
          scope_up_sample_x_10(channel1tracebuffer1, 1500);
        }
        else
        {
          //For 10nS/div need to up sample by 25
          scope_up_sample_x_25(channel1tracebuffer1, 1500);
        }
        break;
    }
#endif
    
    //Set sample count based on time base setting
    if(scopesettings.timeperdiv >= 25)
    {
      //For settings 250nS/div - 10nS/div. This would mean that for 250nS/div, 100nS/div and 50nS/div samples are interpolated???
      count = 2500;
    }
    else if(scopesettings.timeperdiv >= 11)  //In original code this is 9, but below that is handled in long time base function
    {                                        //And based on the code before I guessed it to be 11. Needs to be confirmed
      //For settings 10mS/div - 250nS/div
      count = 1500;
    }
    else
    {
      //For settings 50mS/div and 20mS/div
      count = 750;
    }

    //Adjust the data into another buffer
    scope_adjust_data(channel1tracebuffer3, channel1tracebuffer1, count, scopesettings.channel1.voltperdiv);
    
    //Copy the buffer to another buffer
    memcpy((uint8*)channel1tracebuffer4, (uint8*)channel1tracebuffer3, count);
    
    //Check if on highest sensitivity (50mV/div on 1x probe setting)
    if(scopesettings.channel1.voltperdiv == 6)
    {
      //Adjust data in buffer 4 to be twice the magnitude
      scope_double_data((uint16 *)channel1tracebuffer4, count);
      
      //Adjust the data for trace offset???
      scope_offset_data((uint16 *)channel1tracebuffer4, count, scopesettings.channel1.traceoffset);
    }

    //Limit the data to max range
    scope_limit_data((uint16 *)channel1tracebuffer4, count);   //In the original they process 2500 samples no matter what
    
    //Do additional handling for time base range 50mS/div - 500nS/div
    if(scopesettings.timeperdiv < 25)
    {
      //Filter the data
      scope_filter_data((uint16 *)channel1tracebuffer4, count - 2);    //In the original they do 2998 samples no matter what
    }
    
    //Calculate some of the basic measurements like min, max, average, peak peak and another one (max + (min >> 1))???
    scope_calculate_min_max_avg((uint16 *)channel1tracebuffer4, &channel1measurements);
    
    //Check on signal being large enough and otherwise clear it with some noise
    //scope_evaluate_trace_data((uint16 *)channel1tracebuffer4, &channel1measurements, scopesettings.channel1.voltperdiv, scopesettings.channel1.traceoffset);
    
    
    //Based on scopesettings.alwaystrigger50 the trigger level needs to be adjusted
    
    
    //SKip for now
    //And if some variable is 0 call another one
    //Basically only turned off in perform auto set
    //Is this frequency determination??????
    //if (pcVar2[0x48] == '\0')
    //{
    //  FUN_80003ec8();
    //}
  }
  
  //Handle channel 2 here
  
  
  
  
  //Handle some trigger stuff here
  
  
  
  
  //Set flags based on being triggered
  if(triggered)
  {
    //Triggered????
    scopesettings.triggerflag4 = 1;
    scopesettings.triggerflag1 = 0;
    scopesettings.triggerflag2 = 0;
  }
  else
  {
    //Not triggered????
    scopesettings.triggerflag4 = 0;
    scopesettings.triggerflag1 = 1;
    scopesettings.triggerflag2 = 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_adjust_data(uint16 *destination, uint16 *source, uint32 count, uint8 voltperdiv)
{
  uint32 signaladjust;
  uint32 temp1, temp2, temp3;

  //Translate this channel volts per div setting
  signaladjust = fpga_read_parameter_ic(0x0B, voltperdiv) & 0x0000FFFF;
  
  //Process the samples
  while(count)
  {
    //Some fractional scaling on the signal to fit it on screen???
    //Adjust the channel signal based on the volts per div setting
    temp1 = *source * signaladjust;
    temp2 = ((0xA3D7 * temp1) + 0xA3D7) >> 0x16;
    temp3 = temp1 + (((int64)(temp1 * 0x51EB851F) >> 0x25) * -100);

    //If above half the pixel up to next one?????
    if(temp3 > 50)
    {
      temp2++;
    }

    //Store it for further handling
    *destination = temp2;

    //Point to next source and destination
    source++;
    destination++;

    //One more sample done
    count--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_offset_data(uint16 *buffer, uint32 count, uint32 offset)
{
  //Process the samples
  while(count)
  {
    //Check if the data is smaller then the offset
    if(*buffer < offset)
    {
      //If so limit to top of the screen
      *buffer = 0;
    }
    else
    {
      //Else take of the offset
      *buffer = *buffer - offset;
    }

    //Point to next sample
    buffer++;

    //One more sample done
    count--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_limit_data(uint16 *buffer, uint32 count)
{
  //Process the samples
  while(count)
  {
    //Check if the data is bigger then max allowed
    if(*buffer > 401)
    {
      //If so limit to bottom of the screen
      *buffer = 401;
    }

    //Point to next sample
    buffer++;

    //One more sample done
    count--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_filter_data(uint16 *buffer, uint32 count)
{
  uint32 sample1, sample2, sample3;
  uint32 midlow, midhigh;
  
  //Process the samples
  while(count)
  {
    //Get three consecutive samples
    sample1 = buffer[0];
    sample2 = buffer[1];
    sample3 = buffer[2];
    
    //Set filter ranges
    midlow  = sample2 - 6;
    midhigh = sample2 + 6;
    
    //Check if first and last sample not near the middle sample and at the same side of it. (both below or above)
    if(((sample1 < midlow) && (sample3 < midlow)) || ((midhigh < sample1) && (midhigh < sample3)))
    {
      //If so set the middle sample to the average of the other two
      buffer[1] = (sample1 + sample3) >> 1;
    }

    //Point to next sample
    buffer++;

    //One more sample done
    count--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_double_data(uint16 *buffer, uint32 count)
{
  //Process the samples
  while(count)
  {
    //Multiply the sample by two
    *buffer <<= 1;

    //Point to next sample
    buffer++;

    //One more sample done
    count--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 scope_process_trigger(void)
{
  uint16 *buffer;
  uint32  index;
  uint32  level = scopesettings.triggerlevel;
  uint32  havetrigger = 1;
  uint32  lower  = 0;
  uint32  higher = 0;
  
  //Process based on time base setting and trigger mode
  //Skip for 50mS/div, 20mS/div, 250nS/div, 100nS/div, 50nS/div, 25nS/div and 10nS/div or single and normal trigger modes
  if((scopesettings.timeperdiv > 10) && (scopesettings.timeperdiv < 25) && (scopesettings.triggermode = 0))
  {
    //Select the trace buffer to process based on the trigger channel
    if(scopesettings.triggerchannel == 0)
    {
      //Channel 1 buffer
      buffer = channel1tracebuffer1;
    }
    else
    {
      //Channel 2 buffer
      buffer = channel2tracebuffer1;
    }

    //Set a starting point for checking on trigger
    index = 745;
    
    //Check on which edge to trigger
    if(scopesettings.triggeredge == 0)
    {
      //Rising edge
      //Only check around center point of the buffer
      while(index < 755)
      {
        //Check if previous sample lower then level and next sample equal or higher then level
        if((buffer[index - 1] < level) && (level <= buffer[index + 1]))
        {
          //Is this a triggered signal or not???
          havetrigger = 0;
          break;
        }
        
        //One sample done
        index++;
      }
    }
    else
    {
      //Falling edge
      //Only check around center point of the buffer
      while(index < 755)
      {
        //Check if previous sample higher then level and next sample equal or lower then level
        if((level < buffer[index - 1]) && (buffer[index + 1] <= level))
        {
          //Is this a triggered signal or not???
          havetrigger = 0;
          break;
        }
        
        //One sample done
        index++;
      }
    }
    
    //Set a new starting point for checking
    index = 100;
    
    //Process the buffer from sample 100 to sample 1400
    while(index < 1400)
    {
      //Check if sample lower then level
      if(buffer[index] < level)
      {
        //Signal had sample lower then level
        lower = 1;
      }
      //Check if sample higher then level
      else if(buffer[index] > level)
      {
        //Signal had sample higher then level
        higher = 1;
      }
        
      //One sample done
      index++;
    }
    
    //Check if all samples where on trigger level
    if((lower == 0) || (higher == 0))
    {
      //Return zero if so
      return(0);
    }
    
    //Otherwise return the flag from edge scan
    return(havetrigger);  
  }
  
  //No valid time base or trigger setting just return 0
  return(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_up_sample_x_2(uint16 *buffer, uint32 count)
{
  register uint32  cnt;
  register uint16 *sptr;
  register uint16 *dptr;
  register uint32  sample1, sample2;
  
  //Only do half the samples
  cnt = count / 2;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Store the average of the two samples
    *dptr-- = (sample1 + sample2) / 2;
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//This is a special case where there is a need for two different interpolations.
//Between 1st and 2nd sample two new samples and between 2nd and 3rd sample only one new sample making 5 samples for every two samples
//The original code uses four loops for this. The first loop to copy every other sample to the 1st, 6th, 11th, etc. position
//A second loop to copy every other sample starting from the second sample to the 4th, 9th, 14th, etc. position
//And a third loop to do the interpolation between the samples. At last a fourth loop to copy the data back to the input buffer

void scope_up_sample_x_2_5(uint16 *buffer, uint32 count)
{
  register uint32  cnt;
  register uint16 *sptr;
  register uint16 *dptr;
  register uint32  sample1, sample2, sample3;
  register int32   delta;
  
  //Only do two and a half the samples
  cnt = (count * 2) / 5;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Since the loop processes two samples per loop do half the count
  cnt /= 2;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Store the average of the two samples
    *dptr-- = (sample1 + sample2) / 2;
    
    //Get the third sample
    sample3 = *sptr--;
    
    //Fill in the in between samples
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample2 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample2 - (sample3 << 22)) / 3;

    //Calculate the next sample with fixed point calculation
    //Since the direction is from last sample to first sample the step needs to be taken off
    sample2 -= delta;

    //Store the decimal part of it
    *dptr-- = sample2 >> 22;
    
    //Repeat this
    sample2 -= delta;

    //Store the decimal part of it
    *dptr-- = sample2 >> 22;

    //Save the third sample as the first sample
    sample1 = sample3;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//The up sample code can be reduced in size by creating a function for the basic up sampling with the times as input (4, 5, 10, 25)
//A second function for the interpolating with averaging with the interpolation factor as input (5, 6, 10, 12, 25)

void scope_up_sample_x_4(uint16 *buffer, uint32 count)
{
  register uint32  cnt, idx;
  register uint16 *sptr;
  register uint16 *dptr;
  register int32   sample1, sample2;
  register int32   delta;
  
  //Only do one fourth of the samples
  cnt = count / 4;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 4;
    
    for(idx=0;idx<3;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the decimal part of it
      *dptr-- = sample1 >> 22;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_up_sample_x_5(uint16 *buffer, uint32 count)
{
  register uint32  cnt, idx;
  register uint16 *sptr;
  register uint16 *dptr;
  register int32   sample1, sample2;
  register int32   delta;
  
  //Only do one fifth of the samples
  cnt = count / 5;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 5;
    
    for(idx=0;idx<4;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the decimal part of it
      *dptr-- = sample1 >> 22;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_up_sample_x_10(uint16 * buffer, uint32 count)
{
  register uint32  cnt, idx;
  register uint16 *sptr;
  register uint16 *dptr;
  register int32   sample1, sample2;
  register int32   delta;
  
  //Only do one tenth of the samples
  cnt = count / 10;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 10;
    
    for(idx=0;idx<9;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the decimal part of it
      *dptr-- = sample1 >> 22;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
  
  //Create a second interpolated set of data based on the initially interpolated samples and average the two sets
  //Only take one tenth of the samples, minus one since the data is shifted, as input
  cnt = (count / 10) - 1;

  //For the source point to the last sample to use
  sptr = &buffer[count - 5];
  
  //For the destination point to the last result sample
  dptr = &buffer[count - 5];
  
  //Get the first sample to use and skip ten samples
  sample1 = *sptr;
  sptr -= 10;
  
  //Process all the needed samples
  while(cnt)
  {
    //Average and store the first sample
    *dptr = (*dptr + sample1) / 2;
    
    //Point to the previous sample
    dptr--;
    
    //Get the second sample and skip ten samples
    sample2 = *sptr;
    sptr -= 10;
    
    //Fill in the in between samples
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 10;
    
    for(idx=0;idx<9;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Average and store the decimal part of it
      *dptr = (*dptr + (sample1 >> 22)) / 2;
      
      //Point to the previous sample
      dptr--;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
  
  //Create a third interpolated set of data based on the previous interpolated and averaged samples and also average these two sets
  //Only take one fifth of the samples, minus one since the data is shifted, as input
  cnt = (count / 5) - 1;

  //For the source point to the last sample to use
  //Takes the second sample as last input
  sptr = &buffer[count - 8];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use and skip five samples
  sample1 = *sptr;
  sptr -= 5;
  
  //Process all the needed samples
  while(cnt)
  {
    //Average and store the first sample
    *dptr = (*dptr + sample1) / 2;
    
    //Point to the previous sample
    dptr--;
    
    //Get the second sample and skip ten samples
    sample2 = *sptr;
    sptr -= 5;
    
    //Fill in the in between samples
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 5;
    
    for(idx=0;idx<4;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Average and store the decimal part of it
      *dptr = (*dptr + (sample1 >> 22)) / 2;
      
      //Point to the previous sample
      dptr--;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_up_sample_x_25(uint16 *buffer, uint32 count)
{
  register uint32  cnt, idx;
  register uint16 *sptr;
  register uint16 *dptr;
  register int32   sample1, sample2;
  register int32   delta;
  
  //Only do one twenty fifth of the samples
  cnt = count / 25;
  
  //For the source point to the last sample to use
  sptr = &buffer[cnt];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use
  sample1 = *sptr--;
  
  //Process all the needed samples
  while(cnt)
  {
    //Store the first sample
    *dptr-- = sample1;
    
    //Get the second sample
    sample2 = *sptr--;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 25;
    
    for(idx=0;idx<24;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the decimal part of it
      *dptr-- = sample1 >> 22;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
  
  //Create a second interpolated set of data based on the interpolated samples
  //Only do one twenty fifth of the samples minus one since the data is shifted
  cnt = (count / 25) - 1;

  //For the source point to the last sample to use
  sptr = &buffer[count - 13];
  
  //For the destination point to the last result sample
  //Also on shifted sample to end where needed
  dptr = &buffer[count - 13];
  
  //Get the first sample to use and skip twenty five samples
  sample1 = *sptr;
  sptr -= 25;
  
  //Process all the needed samples
  while(cnt)
  {
    //Average and store the first sample
    *dptr = (*dptr + sample1) / 2;
      
    //Point to the previous sample
    dptr--;
    
    //Get the second sample and skip twenty five samples
    sample2 = *sptr;
    sptr -= 25;
    
    //Fill in the in between samples
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 25;
    
    for(idx=0;idx<24;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the averaged decimal part of it
      *dptr = (*dptr + (sample1 >> 22)) / 2;
      
      //Point to the previous sample
      dptr--;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
  
  //The next step is a 12 step interpolating of the processed data and averaging that with the processed data
  //Only do one twelfth of the samples
  cnt = (count / 12);
  
  //For the source point to the last sample to use
  //Takes the second sample as last input
  sptr = &buffer[count];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use and skip twelve samples
  sample1 = *sptr;
  sptr -= 12;
  
  //Process all the needed samples
  while(cnt)
  {
    //Average and store the first sample
    *dptr = (*dptr + sample1) / 2;
      
    //Point to the previous sample
    dptr--;
    
    //Get the second sample and skip twelve samples
    sample2 = *sptr;
    sptr -= 12;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 12;
    
    for(idx=0;idx<11;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the averaged decimal part of it
      *dptr = (*dptr + (sample1 >> 22)) / 2;
      
      //Point to the previous sample
      dptr--;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
  
  //The next step is a 6 step interpolating of the processed data and averaging that with the processed data
  //Only do one sixth of the samples
  cnt = (count / 6);
  
  //For the source point to the last sample to use
  //Takes the second sample as last input
  sptr = &buffer[count];
  
  //For the destination point to the last result sample
  dptr = &buffer[count];
  
  //Get the first sample to use and skip 6 samples
  sample1 = *sptr;
  sptr -= 6;
  
  //Process all the needed samples
  while(cnt)
  {
    //Average and store the first sample
    *dptr = (*dptr + sample1) / 2;
      
    //Point to the previous sample
    dptr--;
    
    //Get the second sample and skip 6 samples
    sample2 = *sptr;
    sptr -= 6;
    
    //Fill in the in between samples
    //The original code uses a different approach
    //Get the samples shifted up for fractional calculations 10.22 bits
    sample1 <<= 22;
    
    //Calculate a delta step between the samples
    delta = (sample1 - (sample2 << 22)) / 6;
    
    for(idx=0;idx<5;idx++)
    {
      //Calculate the next sample with fixed point calculation
      //Since the direction is from last sample to first sample the step needs to be taken off
      sample1 -= delta;
      
      //Store the averaged decimal part of it
      *dptr = (*dptr + (sample1 >> 22)) / 2;
      
      //Point to the previous sample
      dptr--;
    }
    
    //Save the second sample as the first sample
    sample1 = sample2;
    
    //One set of samples done
    cnt--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_interleave_samples(uint16 *buffer1, uint16 *buffer2, PADC2CALIBRATIONDATA calibration)
{
  //Fixed number of samples to do
  //In the original code it is 1490, but the read from the FPGA is for 1500, so all data done here
  register uint32 count = 1500;
  
  //Buffers have a fixed size of 3000 samples
  //Destination pointer on the last entry in the first buffer
  register  uint16 *dptr = &buffer1[2999];
  
  //Source pointers on the last sample in each buffer
  register uint16 *sptr1 = &buffer1[1499];
  register uint16 *sptr2 = &buffer2[1499];
  
  //Get the compensation into a register for better performance
  register uint32 compensation = calibration->compensation;
  
  register uint32 sample;
  
  //Check on what to do with the calibration compensation
  if(calibration->flag == 0)
  {
    //On zero subtract from second buffer samples
    
    //Loop until all the samples are done
    while(count)
    {
      //Copy the sample of the first buffer to the upper entries
      *dptr-- = *sptr1--;
      
      //Get the sample from the second ADC
      sample = *sptr2--;
      
      //Check if there is data to compensate
      if(sample > compensation)
      {
        //Store the compensated sample to the lower entries
        *dptr-- = sample - compensation;
      }
      else
      {
        //Zero the sample if to small
        *dptr-- = 0;
      }
      
      //One set of samples done
      count--;
    }
  }
  else
  {
    //On one add to second buffer samples
    
    //Loop until all the samples are done
    while(count)
    {
      //Copy the sample of the first buffer to the upper entries
      *dptr-- = *sptr1--;
      
      //Get the sample from the second ADC
      sample = *sptr2--;
      
      //Compensate and store the sample to the lower entries
      *dptr-- = sample + compensation;
      
      //One set of samples done
      count--;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_calculate_min_max_avg(uint16 *buffer, PMEASUREMENTS measurements)
{
  uint32 start;
  uint32 end;
  uint16 *ptr;
  uint32  min = 0xFFFF;
  uint32  max = 0;
  uint32  avg = 0;
  uint32  cnt1, cnt2;
  uint32  sample;
  
  //Check on time base setting for start and end points
  if(scopesettings.timeperdiv < 11)
  {
    //For 50mS/div and 20mS/div start at 100 and end at 600
    start = 100;
    end   = 600;
  }
  else
  {
    //For 10mS/div - 10nS/div start at 200 and end at 1200
    start =  200;
    end   = 1200;
  }
  
  //Point to the start of the data
  ptr = &buffer[start];
  
  //Calculate the number of samples to do
  cnt1 = end - start;
  cnt2 = cnt1;
  
  //Process the samples
  while(cnt2)
  {
    //Get the current sample and point to the next one
    sample = *ptr++;
    
    //Get the minimum value of the samples
    if(sample < min)
    {
      //Keep the lowest
      min = sample;
    }
    
    //Get the maximum value of the samples
    if(sample > max)
    {
      //Keep the highest
      max = sample;
    }
    
    //Add the samples for average calculation
    avg += sample;
    
    //One more sample done
    cnt2--;
  }
  
  //Save the results
  measurements->min = min;
  measurements->max = max;
  measurements->avg = avg / cnt1;
  measurements->peakpeak = max - min;
  measurements->center = (max + min) >> 1;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_evaluate_trace_data(uint16 *buffer, PMEASUREMENTS measurements, uint32 voltperdiv, uint32 screenoffset)
{
  uint32 minsignalneeded;
  uint32 cnt1, cnt2;
  uint32 sample;
  uint32 index;
  
  //Check if on highest sensitivity (lowest volt/div setting)
  if(voltperdiv == 6)
  {
    //Set the compensation factor for this
    minsignalneeded = 35;
  }
  else
  {
    //Otherwise use a lower compensation
    minsignalneeded = 20;
  }
  
  //Check on time base setting for the number of samples to process
  if(scopesettings.timeperdiv < 25)
  {
    //For 50mS/div - 500nS/div use 1500 samples
    cnt1 = 1500;
  }
  else
  {
    //For 250nS/div - 10nS/div use 2500 samples
    cnt1 = 2500;
  }
  
  //Check if peak peak measurement of the signal is less then the minimum signal needed
  if(measurements->peakpeak < minsignalneeded)
  {
    //Check if the center of the signal is outside the minimal signal band around the center line
    if((measurements->avg < (screenoffset - minsignalneeded)) || (measurements->avg > (screenoffset + minsignalneeded)))
    {
      //Remove the small signal from the buffer by using the average
      while(cnt1)
      {
        //Count is either 1500 or 2500 so even and therfore ok to handle two samples at once
        //Set the current sample with average value
        *buffer++ = measurements->avg;
        *buffer++ = measurements->avg;
        
        //Two samples done
        cnt1 -= 2;
      }
    }
    else
    {
      //signal center within the minimal signal band around the center line
      index = 0;
      cnt2 = 0;
      
      //Process the samples
      while(cnt1)
      {
        //Get a sample
        sample = buffer[index];

        //Overwrite it with the screen offset
        buffer[index] = screenoffset;
        
        //Check if it is above the average
        if(sample > measurements->avg)
        {
          //Every so many counts introduce some noise?????
          if(cnt2 >= 21)
          {
            //No idea on the why of this
            cnt2 = 0;
            buffer[index] += 1;
          }
        }
        else
        {
          //Check is sample on the average
          if(sample == measurements->avg)
          {
            //Every so many samples a bit of noise is introduced???
            cnt2++;
          }
          else if(cnt2 >= 21)
          {
            //No idea on the why of this
            cnt2 = 0;
            buffer[index] -= 1;
          }
        }
        
        //One sample done
        cnt1--;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_determine_sample_buffer_indexes(void)
{
  //Needs to set these based on the zoom_select variable
  //disp_x_start = 0;
  //disp_sample_count = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 scope_do_baseline_calibration(void)
{
  uint32 flag = 1;
  
  //Save the current settings
  scope_save_setup(&savedscopesettings1);

  //Calibrate the trace offsets
  flag &= scope_do_trace_offset_calibration();
  
  //Determine the interleaving ADC compensation values
  flag &= scope_do_adc1_adc2_difference_calibration();
  
  //Restore the settings from before calibration
  scope_restore_setup(&savedscopesettings1);
  
  //Load the settings back into the FPGA
  fpga_set_channel_1_voltperdiv();
  fpga_set_channel_1_offset();

  fpga_set_channel_2_voltperdiv();
  fpga_set_channel_2_offset();
  
  fpga_set_trigger_timebase();
  fpga_set_trigger_level();
  
  return(flag);
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 scope_do_trace_offset_calibration(void)
{
  int32 offset;
  int32 voltperdiv;
  int32 signaladjust5v;
  int32 signaladjust;
  int32 testbit;
  int32 channel1_done = 0;
  int32 channel2_done = 0;
  int32 channel1_top_offset     = 200;
  int32 channel1_top_average    = 380;
  int32 channel1_bottom_offset  = 1500;
  int32 channel1_bottom_average = 20;
  int32 channel2_top_offset     = 200;
  int32 channel2_top_average    = 380;
  int32 channel2_bottom_offset  = 1500;
  int32 channel2_bottom_average = 20;
  
  //Initialize the FPGA for calibration
  fpga_setup_for_calibration();
  
  //Find the top offset value by going from bottom 200 to top 1500 in increments of 50
  for(offset=200;offset<1500;offset+=50)
  {
    //Write the offset for both channels to the FPGA
    fpga_set_channel_1_trace_offset(offset);
    fpga_set_channel_2_trace_offset(offset);
  
    //Wait 50ms to settle
    timer0_delay(50);
    
    //Do a conversion run and wait until it is done
    fpga_do_conversion();
    
    //Only do this channel when not already done
    if(channel1_done == 0)
    {
      //Average the samples from the first ADC for channel 1. 0x0C is the parameter for the special IC
      channel1_top_average = fpga_process_channel_adc1_samples(0x0C, scopesettings.channel1.voltperdiv);
      
      //Check if the value is below a required value
      if(channel1_top_average < 381)
      {
        //If so signal this channel as done
        channel1_done = 1;
        
        //And save the offset value it happened on
        channel1_top_offset = offset;
      }
    }

    //Only do this channel when not already done
    if(channel2_done == 0)
    {
      //Average the samples from the first ADC for channel 1. 0x0D is the parameter for the special IC
      channel2_top_average = fpga_process_channel_adc1_samples(0x0D, scopesettings.channel2.voltperdiv);
      
      //Check if the value is below a required value
      if(channel2_top_average < 381)
      {
        //If so signal this channel as done
        channel2_done = 1;
        
        //And save the offset value it happened on
        channel2_top_offset = offset;
      }
    }
    
    //Check if both channels done
    if((channel1_done) && (channel2_done))
    {
      //Quit the loop if so
      break;
    }
  }  

  //Reset for bottom offset detection
  channel1_done = 0;
  channel2_done = 0;
  
  //Find the bottom offset value by going from top 1500 to bottom 200 in decrements of 50
  for(offset=1500;offset>200;offset-=50)
  {
    //Write the offset for both channels to the FPGA
    fpga_set_channel_1_trace_offset(offset);
    fpga_set_channel_2_trace_offset(offset);
  
    //Wait 50ms to settle
    timer0_delay(50);
    
    //Do a conversion run and wait until it is done
    fpga_do_conversion();
    
    //Only do this channel when not already done
    if(channel1_done == 0)
    {
      //Average the samples from the first ADC for channel 1. 0x0C is the parameter for the special IC
      channel1_bottom_average = fpga_process_channel_adc1_samples(0x0C, scopesettings.channel1.voltperdiv);
      
      //Check if the value is above a required value
      if(channel1_bottom_average > 19)
      {
        //If so signal this channel as done
        channel1_done = 1;
        
        //And save the offset value it happened on
        channel1_bottom_offset = offset;
      }
    }

    //Only do this channel when not already done
    if(channel2_done == 0)
    {
      //Average the samples from the first ADC for channel 1. 0x0D is the parameter for the special IC
      channel2_bottom_average = fpga_process_channel_adc1_samples(0x0D, scopesettings.channel2.voltperdiv);
      
      //Check if the value is above a required value
      if(channel2_bottom_average > 19)
      {
        //If so signal this channel as done
        channel2_done = 1;
        
        //And save the offset value it happened on
        channel2_bottom_offset = offset;
      }
    }
    
    //Check if both channels done
    if((channel1_done) && (channel2_done))
    {
      //Quit the loop if so
      break;
    }
  }  
  
  //Determine the channel 1 calibration factor and first calibration value based on the found data
  //Use the special IC to get some value, based on the offsets found
  offset = fpga_read_parameter_ic(0x12, (channel1_top_offset & 0x0000FFFF) | (channel1_bottom_offset << 16));
  
  //Divide the result by the difference of the two averages
  offset = offset / (channel1_top_average - channel1_bottom_average);
  
  //Save the new channel1 calibration factor
  channel1_calibration_factor = offset;
  
  //Calculate the calibration data for 5V/div setting
  channel1_calibration_data[0] = channel1_bottom_offset + ((int64)((int64)0x51EB851F * (int64)offset * (int64)channel1_bottom_average) >> 37);
  
  //Do the same for channel 2
  //Use the special IC to get some value, based on the offsets found
  offset = fpga_read_parameter_ic(0x12, (channel2_top_offset & 0x0000FFFF) | (channel2_bottom_offset << 16));
  
  //Divide the result by the difference of the two averages
  offset = offset / (channel2_top_average - channel2_bottom_average);
  
  //Save the new channel1 calibration factor
  channel2_calibration_factor = offset;
  
  //Calculate the calibration data for 5V/div setting
  channel2_calibration_data[0] = channel2_bottom_offset + ((int64)((int64)0x51EB851F * (int64)offset * (int64)channel2_bottom_average) >> 37);
  
  //Determine the calibration values for the other sensitivity settings
  //Set the trace offsets in the FPGA based on the new found 5V/div calibration
  fpga_set_channel_1_offset();
  fpga_set_channel_2_offset();

  //Get the 5V/div signal adjuster
  signaladjust5v = fpga_read_parameter_ic(0x0B, 0);
  
  //Loop through the settings
  //Setting 0 is already done and the last setting (6) is the same as the fore last setting
  for(voltperdiv=1;voltperdiv<6;voltperdiv++)
  {
    //Set the current sensitivity in the scope settings
    scopesettings.channel1.voltperdiv = voltperdiv;
    scopesettings.channel2.voltperdiv = voltperdiv;
    
    //Write them to the FPGA
    fpga_set_channel_1_voltperdiv();
    fpga_set_channel_2_voltperdiv();

    //Wait 100ms to settle
    timer0_delay(100);

    //Do a conversion run and wait until it is done
    fpga_do_conversion();

    //Get the signal adjuster for this setting
    signaladjust = fpga_read_parameter_ic(0x0B, voltperdiv);
    
    //Average the samples from the first ADC for channel 1. 0x0C is the parameter for the special IC
    channel1_bottom_average = fpga_process_channel_adc1_samples(0x0C, scopesettings.channel1.voltperdiv) + 1;

    //Based on the level of the found average some different calculation is done
    if(channel1_bottom_average < 200)
    {
      //Some fixed point fractional calculations
      channel1_calibration_data[voltperdiv] = channel1_calibration_data[0] - ((int64)((int64)channel1_calibration_factor * (int64)200 * (int64)0x51EB851F) >> 37) - ((signaladjust5v * ((int64)((int64)0x51EB851F * (int64)channel1_calibration_factor * (int64)channel1_bottom_average) >> 37)) / signaladjust);
    }
    else
    {
      //Some fixed point fractional calculations
      channel1_calibration_data[voltperdiv] = ((int64)((int64)0x51EB851F * (int64)channel1_calibration_factor * (int64)channel1_bottom_average) >> 37) - (((int64)((int64)signaladjust * (int64)channel1_calibration_factor * (int64)200 * (int64)0x51EB851F) >> 37) / signaladjust5v) + channel1_calibration_data[0];
    }
    
    //Average the samples from the first ADC for channel 2. 0x0D is the parameter for the special IC
    channel2_bottom_average = fpga_process_channel_adc1_samples(0x0D, scopesettings.channel2.voltperdiv) + 1;

    //Based on the level of the found average some different calculation is done
    if(channel2_bottom_average < 200)
    {
      //Some fixed point fractional calculations
      channel2_calibration_data[voltperdiv] = channel2_calibration_data[0] - ((int64)((int64)channel2_calibration_factor * (int64)200 * (int64)0x51EB851F) >> 37) - ((signaladjust5v * ((int64)((int64)0x51EB851F * (int64)channel2_calibration_factor * (int64)channel2_bottom_average) >> 37)) / signaladjust);
    }
    else
    {
      //Some fixed point fractional calculations
      channel2_calibration_data[voltperdiv] = ((int64)((int64)0x51EB851F * (int64)channel2_calibration_factor * (int64)channel2_bottom_average) >> 37) - (((int64)((int64)signaladjust * (int64)channel2_calibration_factor * (int64)200 * (int64)0x51EB851F) >> 37) / signaladjust5v) + channel2_calibration_data[0];
    }
  }

  //Test the found settings
  //Initialize the FPGA for calibration again
  fpga_setup_for_calibration();
  
  //Reset for fault detection
  channel1_done = 0;
  channel2_done = 0;
  
  //Test on 5V/div, 500mV/div and 200mV/div
  for(voltperdiv=0,testbit=1;testbit<8;testbit<<=1)
  {
    //Set the current sensitivity in the scope settings
    scopesettings.channel1.voltperdiv = voltperdiv;
    scopesettings.channel2.voltperdiv = voltperdiv;
    
    //Write them to the FPGA
    fpga_set_channel_1_voltperdiv();
    fpga_set_channel_2_voltperdiv();

    //Set the adjusted offsets in the FPGA
    fpga_set_channel_1_offset();
    fpga_set_channel_2_offset();
    
    //Wait 100ms to settle
    timer0_delay(100);
    
    //Do a conversion run and wait until it is done
    fpga_do_conversion();
    
    //Average the samples from the first ADC for channel 1. 0x0C is the parameter for the special IC
    channel1_bottom_average = fpga_process_channel_adc1_samples(0x0C, scopesettings.channel1.voltperdiv);
    
    //Check if the averaged value is in range
    if((channel1_bottom_average - 193) < 15)
    {
      //If so flag this test as passed
      channel1_done |= testbit;
    }
    
    //Average the samples from the first ADC for channel 1. 0x0C is the parameter for the special IC
    channel2_bottom_average = fpga_process_channel_adc1_samples(0x0D, scopesettings.channel2.voltperdiv);
    
    //Check if the averaged value is in range
    if((channel2_bottom_average - 193) < 15)
    {
      //If so flag this test as passed
      channel2_done |= testbit;
    }
    
    //Select the next test sensitivity based on the current one
    if(voltperdiv == 0)
    {
      voltperdiv = 3;
    }
    else if(voltperdiv == 3)
    {
      voltperdiv = 4;
    }
  }
  
  //Return the result of the tests. True if all tests passed
  return((channel1_done == 7) && (channel2_done == 7));
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 scope_do_adc1_adc2_difference_calibration(void)
{
  int32 adc1_avg = 0;
  int32 adc2_avg = 0;
  
  //Initialize the FPGA for calibration again
  fpga_setup_for_calibration();

  //Switch to 10ns/div
  scopesettings.timeperdiv = 29;
  
  //Set the new time base setting in the FPGA
  fpga_set_trigger_timebase();

  //Do a conversion run and wait until it is done
  fpga_do_conversion();
  
  //Get the channel 1 averages
  adc1_avg = fpga_average_adc1_samples(0x0C);
  adc2_avg = fpga_average_adc2_samples(0x21);
  
  //Set the channel 1 compensation value based on which is higher
  if(adc1_avg < adc2_avg)
  {
    //Negative compensation
    channel1adc2calibration.flag = 0;
    channel1adc2calibration.compensation = adc2_avg - adc1_avg;
  }
  else
  {
    //Positive compensation
    channel1adc2calibration.flag = 1;
    channel1adc2calibration.compensation = adc1_avg - adc2_avg;
  }
  
  //Get the channel 2 averages
  adc1_avg = fpga_average_adc1_samples(0x0D);
  adc2_avg = fpga_average_adc2_samples(0x23);
  
  //Set the channel 2 compensation value based on which is higher
  if(adc1_avg < adc2_avg)
  {
    //Negative compensation
    channel2adc2calibration.flag = 0;
    channel2adc2calibration.compensation = adc2_avg - adc1_avg;
  }
  else
  {
    //Positive compensation
    channel2adc2calibration.flag = 1;
    channel2adc2calibration.compensation = adc1_avg - adc2_avg;
  }
  
  //Signal ok when the differences are less then 26
  return((channel1adc2calibration.compensation < 26) && (channel2adc2calibration.compensation < 26));
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_do_50_percent_trigger_setup(void)
{
  uint32 command;
  uint32 voltperdiv;
  
  //Check which channel is the active trigger channel
  if(scopesettings.triggerchannel == 0)
  {
    //FPGA command for getting channel 1 samples
    command = 0x24;
    voltperdiv = scopesettings.channel1.voltperdiv;
  }
  else
  {
    //FPGA command for getting channel 2 samples
    command = 0x26;
    voltperdiv = scopesettings.channel2.voltperdiv;
  }
  
  //Need min and max values from 1000 readings
  //Samples need to be adjusted as normal and limited on 401
  scopesettings.triggeroffset = fpga_get_center_level(command, voltperdiv);
  
  //Set the new level in the FPGA
  fpga_set_trigger_level();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_do_auto_setup(void)
{
  uint32 flags = 0;
  uint32 voltperdiv;
  uint32 minvpplevel;
  uint32 center1;
  uint32 center2;
  uint32 offset;
  
  //No need to do auto setup if no channel is enabled
  if((scopesettings.channel1.enable == 0) && (scopesettings.channel2.enable == 0))
  {
    return;
  }
  
  //Check on which bottom check level needs to be used
  if((scopesettings.channel1.enable) && (scopesettings.channel2.enable))
  {
    //Both channels enabled use lower value??
    minvpplevel = 70;
    
    center1 = 300;
    center2 = 100;
  }
  else
  {
    //Only one channel enabled use this value??
    minvpplevel = 110;
    
    center1 = 200;
    center2 = 200;
  }
  
  //Setup for channel 1 if it is enabled
  if(scopesettings.channel1.enable)
  {
    //Trace offset on low end
    scopesettings.channel1.traceoffset = 200;
    fpga_set_channel_1_offset();
    
    //Signal channel is enabled and needs to be done
    flags |= 0x01;
  }

  //Setup for channel 2 if it is enabled
  if(scopesettings.channel2.enable)
  {
    //Trace offset on low end
    scopesettings.channel2.traceoffset = 200;
    fpga_set_channel_2_offset();
    
    //Signal channel is enabled and needs to be done
    flags |= 0x02;
  }

  //Walk through the sensitivity settings until enabled channels are done
  for(voltperdiv=0;voltperdiv<6 && flags!=0;voltperdiv++)
  {
    //Check if channel still needs to be done
    if(flags & 0x01)
    {
      //Set the current sensitivity
      scopesettings.channel1.voltperdiv = voltperdiv;
      fpga_set_channel_1_voltperdiv();
    }
    
    //Check if channel still needs to be done
    if(flags & 0x02)
    {
      //Set the current sensitivity
      scopesettings.channel2.voltperdiv = voltperdiv;
      fpga_set_channel_2_voltperdiv();
    }
    
    fpga_get_auto_set_values(flags);
  
    //Check the channel data if not yet done
    if(flags & 0x01)
    {
      //Check if signal in good range 
      if((channel1_max >= 301) || (channel1_min < 100) || (channel1_vpp > minvpplevel))
      {
        //When signal in range stop scanning the channel
        flags &= 0x2;
      }
    }    

    //Check the channel data if not yet done
    if(flags & 0x02)
    {
      //Check if signal in good range 
      if((channel2_max >= 301) || (channel2_min < 100) || (channel2_vpp > minvpplevel))
      {
        //When signal in range stop scanning the channel
        flags &= 0x1;
      }
    }    
  }
  
  //Need a check if channel is not done and switch to lowest sensitivity if so
/*
  if(flags & 0x02)
  {
    scopesettings.channel2.voltperdiv = 6;
    fpga_set_channel_2_voltperdiv();
  }
  */
  
  //Check if channel enabled
  if(scopesettings.channel1.enable)
  {
    //Check if on fore last sensitivity still not done
    if(flags & 0x01)
    {
      //Switch to last sensitivity if so
      scopesettings.channel1.voltperdiv = 6;
      fpga_set_channel_1_voltperdiv();
    }

    //Adjust the center line based on targeted center
    if(channel1_center < center1)
    {
      //Calculate a new trace offset
      scopesettings.channel1.traceoffset += (center1 - channel1_center);
      
      //Make sure it is not out of range
      if(scopesettings.channel1.traceoffset > 385)
      {
        scopesettings.channel1.traceoffset = 385;
      }
    }
    else
    {
      //Calculate the needed offset
      offset = channel1_center - center1;
       
      //Check if current setting large enough to subtract the offset
      if(scopesettings.channel1.traceoffset > offset)
      {
        //Take of the found offset
        scopesettings.channel1.traceoffset -= offset;
      }
      else
      {
        //Limit on zero
        scopesettings.channel1.traceoffset = 0;
      }
    }
    
    //Write the new offset to the FPGA
    fpga_set_channel_1_offset();
    
    //Display the new channel setting
    scope_channel1_settings(0);
  }
  
  
  //Check if channel enabled
  if(scopesettings.channel2.enable)
  {
    //Check if on fore last sensitivity still not done
    if(flags & 0x02)
    {
      //Switch to last sensitivity if so
      scopesettings.channel2.voltperdiv = 6;
      fpga_set_channel_2_voltperdiv();
    }

    //Adjust the center line based on targeted center
    if(channel2_center < center2)
    {
      //Calculate a new trace offset
      scopesettings.channel2.traceoffset += (center2 - channel2_center);
      
      //Make sure it is not out of range
      if(scopesettings.channel2.traceoffset > 385)
      {
        scopesettings.channel2.traceoffset = 385;
      }
    }
    else
    {
      //Calculate the needed offset
      offset = channel2_center - center2;
       
      //Check if current setting large enough to subtract the offset
      if(scopesettings.channel2.traceoffset > offset)
      {
        //Take of the found offset
        scopesettings.channel2.traceoffset -= offset;
      }
      else
      {
        //Limit on zero
        scopesettings.channel2.traceoffset = 0;
      }
    }
    
    //Write the new offset to the FPGA
    fpga_set_channel_2_offset();
    
    //Display the new channel setting
    scope_channel2_settings(0);
  }
  
  //Set the trigger level based on the trigger channel
  if(scopesettings.triggerchannel == 0)
  {
    //Use the channel 1 trace offset for 50% trigger setting
    scopesettings.triggeroffset = scopesettings.channel1.traceoffset;
  }
  else
  {
    //Use the channel 2 trace offset for 50% trigger setting
    scopesettings.triggeroffset = scopesettings.channel2.traceoffset;
  }

  //Set the level in the FPGA
  fpga_set_trigger_level();
  
  
  //To determine the needed time base setting the scope needs a vpp reading of 24 on one of the channels.
  //Best to make is so that it switches to the one that has this signal if the trigger channel is below
  
  
  //Need to determine an average period time by finding the zero crossings in the samples
  //Can this be done while sampling
  
  //For this version of the code use 
  
  
  
  
  
  
  //If both channels lack enough signal depending on enabled of course switch to 20us/div
  
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_trace_data(void)
{
  uint32 xpos1;
  uint32 xpos2;
  uint32 dy;
  uint16 *ptr;
 
  //Reset some flags if display touched for trace and cursor movement and stopped or in auto or normal mode
  if((touchstate) && ((scopesettings.triggermode != 1) || (scopesettings.runstate)))
  {
    //Set some flags
    scopesettings.triggerflag1 = 1;
    scopesettings.triggerflag2 = 1;
  }
  
  //Depending on the time base setting use different methods
  //Check if time base on 50mS/div - 10nS/div
  if(scopesettings.timeperdiv > 8)
  {
    //Use a separate display buffer
    display_set_screen_buffer(displaybuffer1);

    //Check if screen needs to be redrawn
    if(scopesettings.triggerflag2)
    {
      //Clear the trace portion of the screen
      display_set_fg_color(0x00000000);
      display_fill_rect(2, 46, 728, 434);

      //Draw the grid lines and dots based on the grid brightness setting
      scope_draw_grid();
    }
    
    //Check on another flag if this next bit needs to be done
    if(scopesettings.triggerflag1)
    {
      //Get the sample buffer indexes based on the sample_index_select variable
      scope_determine_sample_buffer_indexes();
      
      //Check if channel 1 is enabled
      if(scopesettings.channel1.enable)
      {
        //Check if running and not moving traces, cursors or pointers
        if((scopesettings.runstate == 0) && (touchstate == 0))
        {
          //Set some variable here
          //*(undefined2 *)(puVar8 + 0x12) = 400; 
        }
        
        //Call the zoom processing function
        //FUN_8000410c();
        
        //Check if auto or normal trigger mode and the time base in range of 50nS/div - 10nS/div and (trigger channel is channel 2 or some variable is set)
        if(((scopesettings.triggermode == 0) || (scopesettings.triggermode == 2)) &&  (scopesettings.timeperdiv > 26) && ((scopesettings.triggerchannel == 1) || channel_1_process_anyway))
        {
          //Process channel 1 trigger???
          //FUN_8000583c();
        }
        
        //Temporarily set to fixed values for testing
        disp_x_start = 3;
        disp_sample_count = 720;
        
        //Check if in normal display mode
        if(scopesettings.xymodedisplay == 0)
        {
          //Draw the trace on the screen
          scope_display_channel_trace((uint16 *)channel1tracebuffer4, disp_x_start, disp_sample_count, CHANNEL1_COLOR);
        }
      }
      

      //Do channel 2 here


      //Check on channel 1 FFT here
      
      
      //Check on channel 2 FFT here

    }
    
    
    //Check if screen needs to be redrawn
    if(scopesettings.triggerflag2)
    {
      //Draw the cursors with their measurement displays
      scope_draw_time_cursors();
      scope_draw_volt_cursors();
      scope_display_cursor_measurements();    //Still needs implementing

      //Draw the signal center, trigger level and trigger position pointers
      scope_draw_pointers();
      
      //Show the enabled measurements on the screen
      scope_display_measurements();    //Still needs implementing

      //Copy it to the actual screen buffer
      display_set_source_buffer(displaybuffer1);
      display_set_screen_buffer((uint16 *)maindisplaybuffer);
      display_copy_rect_to_screen(2, 46, 728, 434);
    }
  }
  else
  {
    //Long time base data 50S/div - 100mS/div
    //Check if not running
    if(scopesettings.runstate != 0)
    {
      //Done for now
      return;
    }
    
    //Based on touchstate in move trace or cursor lines use either the previous x position or reset it
    if(touchstate)
    {
      disp_xpos = 0;
    }
    
    //Check if back on start of screen
    if(disp_xpos == 0)
    {
      //Use a separate buffer to clear the screen
      display_set_screen_buffer(displaybuffer1);

      //Clear the trace portion of the screen
      display_set_fg_color(0x00000000);
      display_fill_rect(2, 46, 728, 434);

      //Draw the grid lines and dots based on the grid brightness setting
      scope_draw_grid();
      
      //Draw the signal center, trigger level and trigger position pointers
      scope_draw_pointers();

      //Copy it to the actual screen buffer
      display_set_source_buffer(displaybuffer1);
      display_set_screen_buffer((uint16 *)maindisplaybuffer);
      display_copy_rect_to_screen(2, 46, 728, 434);
    }

    //Draw the traces directly to the screen
    display_set_screen_buffer((uint16 *)maindisplaybuffer);
    
    //Check if channel 1 is enabled
    if(scopesettings.channel1.enable)
    {
      //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
      ptr = (uint16 *)channel1tracebuffer4;

      //Get the current sample for channel 1 coming up from the bottom of the screen
      disp_ch1_y = 449 - *ptr;
      
      //Check if it is within displayable region
      if(disp_ch1_y < 47)
      {
        //Limit on the top of the screen
        disp_ch1_y = 47;
      }
      else if(disp_ch1_y > 448)
      {
        //Limit on the bottom of the screen
        disp_ch1_y = 448;
      }
      
      //Skip drawing if in x-y mode
      if(scopesettings.xymodedisplay == 0)
      {
        //On start of screen need to start with fresh previous
        if(disp_xpos == 0)
        {
          //Make previous the current
          disp_ch1_prev_y = disp_ch1_y;
        }

        //Set x-y mode display trace color
        display_set_fg_color(CHANNEL1_COLOR);

        //Current x positions for drawing the trace lines
        xpos1 = disp_xpos + 3;
        xpos2 = disp_xpos + 4;

        //Check on rise speed of the signal
        if(disp_ch1_y < disp_ch1_prev_y)
        {
          //previous bigger then current so subtract from it to get positive delta
          dy = disp_ch1_prev_y - disp_ch1_y;
        }
        else
        {
          //current bigger then previous so subtract from it to get positive delta
          dy = disp_ch1_y - disp_ch1_prev_y;
        }
        
        //Take action based on the delta
        if(dy < 15)
        {
          //Less then 15 apart slow the trace by stopping on the average of the two points
          disp_ch1_y = (disp_ch1_y + disp_ch1_prev_y) >> 1;
        }
        else if(dy > 20)
        {
          //Else if delta bigger then 20 draw on a single x position 
          xpos2--;
        }

        //Draw the lines. Needs improvement
        display_draw_line(xpos1, disp_ch1_prev_y, xpos2, disp_ch1_y);
        display_draw_line(xpos1, disp_ch1_prev_y + 1, xpos2, disp_ch1_y + 1);

        //Copy the new points to the previous one
        disp_ch1_prev_y = disp_ch1_y;

        //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
        ptr = (uint16 *)channel1ypoints;
        
        //Save in a point array for picture and waveform save
        ptr[disp_xpos] = disp_ch1_y;
      }
    }

    //Check if channel 2 is enabled
    if(scopesettings.channel2.enable)
    {
      //Source buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
      ptr = (uint16 *)channel2tracebuffer4;

      //Get the current sample for channel 1 coming up from the bottom of the screen
      disp_ch2_y = 449 - *ptr;
      
      //Check if it is within displayable region
      if(disp_ch2_y < 47)
      {
        //Limit on the top of the screen
        disp_ch2_y = 47;
      }
      else if(disp_ch2_y > 448)
      {
        //Limit on the bottom of the screen
        disp_ch2_y = 448;
      }
      
      //Skip drawing if in x-y mode
      if(scopesettings.xymodedisplay == 0)
      {
        //On start of screen need to start with fresh previous
        if(disp_xpos == 0)
        {
          //Make previous the current
          disp_ch2_prev_y = disp_ch2_y;
        }

        //Set x-y mode display trace color
        display_set_fg_color(CHANNEL2_COLOR);

        //Current x positions for drawing the trace lines
        xpos1 = disp_xpos + 3;
        xpos2 = disp_xpos + 4;

        //Check on rise speed of the signal
        if(disp_ch2_y < disp_ch2_prev_y)
        {
          //previous bigger then current so subtract from it to get positive delta
          dy = disp_ch2_prev_y - disp_ch2_y;
        }
        else
        {
          //current bigger then previous so subtract from it to get positive delta
          dy = disp_ch2_y - disp_ch2_prev_y;
        }
        
        //Take action based on the delta
        if(dy < 15)
        {
          //Less then 15 apart slow the trace by stopping on the average of the two points
          disp_ch2_y = (disp_ch2_y + disp_ch2_prev_y) >> 1;
        }
        else if(dy > 20)
        {
          //Else if delta bigger then 20 draw on a single x position 
          xpos2--;
        }

        //Draw the lines
        display_draw_line(xpos1, disp_ch2_prev_y, xpos2, disp_ch2_y);
        display_draw_line(xpos1, disp_ch2_prev_y + 1, xpos2, disp_ch2_y + 1);

        //Copy the new points to the previous one
        disp_ch2_prev_y = disp_ch2_y;

        //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
        ptr = (uint16 *)channel2ypoints;
        
        //Save in a point array for picture and waveform save
        ptr[disp_xpos] = disp_ch2_y;
      }
    }

    //Check if in x-y mode
    if(scopesettings.xymodedisplay)
    {
      //On start of screen need to start with fresh previous
      if(disp_xpos == 0)
      {
        //Make previous the current
        disp_ch1_prev_y = disp_ch1_y;
        disp_ch2_prev_y = disp_ch2_y;
      }
    
      //Set x-y mode display trace color
      display_set_fg_color(XYMODE_COLOR);
      
      //Draw some lines two dots wide
      //The offsets do not put it on center screen so needs adjusting
      display_draw_line(disp_ch1_prev_y + 150, disp_ch2_prev_y, disp_ch1_y + 150, disp_ch2_y);
      display_draw_line(disp_ch1_prev_y + 151, disp_ch2_prev_y, disp_ch1_y + 151, disp_ch2_y);

      //Copy the new points to the previous one
      disp_ch1_prev_y = disp_ch1_y;
      disp_ch2_prev_y = disp_ch2_y;

      //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
      ptr = (uint16 *)channel1ypoints;
      
      //Save in a point array for picture and waveform save
      ptr[disp_xpos] = disp_ch1_y;
      
      //Destination buffer is declared as uint32 to be able to use it with file functions, so need to cast it to uint16 pointer here
      ptr = (uint16 *)channel2ypoints;
      
      //Save in a point array for picture and waveform save
      ptr[disp_xpos] = disp_ch2_y;
    }

    //Redraw the signal center, trigger level and trigger position pointers to be on top of the signals
    scope_draw_pointers();
    
    //Point to next x position
    disp_xpos++;
    
    //Check if past the end of displayable region
    if(disp_xpos > 719)
    {
      //Reset the x position
      disp_xpos = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_channel_trace(uint16 *buffer, uint16 xpos, uint16 count, uint32 color)
{
  register uint32 index = 0;
  register uint32 sample1, sample2;

  display_set_fg_color(color);
  
  sample1 = 449 - buffer[index++];
  
  while(index < count)
  {
    sample2 = 449 - buffer[index++];

    display_draw_line(xpos, sample1, xpos + 1, sample2);
    
    xpos++;
    
    sample1 = sample2;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_cursor_measurements(void)
{
  uint32 height = 5;
  uint32 ch1ypos = 52;
  uint32 ch2ypos = 52;
  uint32 delta;
  char   displaytext[10];
  
  //Check if need to do anything here
  if(scopesettings.timecursorsenable || (scopesettings.voltcursorsenable && (scopesettings.channel1.enable || scopesettings.channel2.enable)))
  {
    //Check if time cursor is enabled
    if(scopesettings.timecursorsenable)
    {
      //Add height for two text lines
      height += 32;
      
      //Shift the voltage text positions down
      ch1ypos += 32;
      ch2ypos += 32;
    }
    
    //Check if volt cursor is enabled
    if(scopesettings.voltcursorsenable)
    {
      //Check if channel 1 is enabled
      if(scopesettings.channel1.enable)
      {
        //Add height for one text line
        height += 16;
        
        //Shift the channel 2 voltage text down
        ch2ypos += 16;
      }
      
      //Check if channel 2 is enabled
      if(scopesettings.channel2.enable)
      {
        //Add height for one text line
        height += 16;
      }
    }
  
    //Set gray background for the cursor measurements
    display_set_fg_color(0x00404040);

    //Draw rounded rectangle depending on what is enabled.
    display_fill_rounded_rect(5, 49, 102, height, 2);

    //Use white text and font_0
    display_set_fg_color(0x00FFFFFF);
    display_set_font(&font_0);
    
    //Check if time cursor is enabled
    if(scopesettings.timecursorsenable)
    {
      //Time texts are always on the top two lines

      //Get the time delta based on the cursor positions
      delta = scopesettings.timecursor2position - scopesettings.timecursor1position;
      
      //Get the time calculation data for this time base setting. Only for the short time bases so take of the first 9
      PTIMECALCDATA tcd = (PTIMECALCDATA)&time_calc_data[scopesettings.timeperdiv - 9];
      
      //For the time multiply with the scaling factor and display based on the time scale
      delta *= tcd->mul_factor;
      
      //Format the time for displaying
      scope_print_value(displaytext, delta, tcd->time_scale, "T ", "S");
      display_text(10, 52, displaytext);
      
      //Calculate the frequency for this time. Need to adjust for stay within 32 bits
      delta /= 10;
      delta = 1000000000/ delta;
      
      //Format the frequency for displaying
      scope_print_value(displaytext, delta, tcd->freq_scale, "F ", "Hz");
      display_text(10, 68, displaytext);
    }
    
    //Check if volt cursor is enabled
    if(scopesettings.voltcursorsenable)
    {
      PVOLTCALCDATA vcd;
      uint32        volts;
      
      //Get the volts delta based on the cursor positions
      delta = scopesettings.voltcursor2position - scopesettings.voltcursor1position;
      
      //Check if channel 1 is enabled
      if(scopesettings.channel1.enable)
      {
        //Calculate the voltage based on the channel 1 settings
        vcd = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel1.magnification][scopesettings.channel1.voltperdiv];
        
        //Multiply with the scaling factor for the channel 1 settings
        volts = delta * vcd->mul_factor;
        
        //Channel 1 text has a variable position
        //Format the voltage for displaying
        scope_print_value(displaytext, volts, vcd->volt_scale, "V1 ", "V");
        display_text(10, ch1ypos, displaytext);
      }
      
      //Check if channel 2 is enabled
      if(scopesettings.channel2.enable)
      {
        //Calculate the voltage based on the channel 2 settings
        vcd = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel2.magnification][scopesettings.channel2.voltperdiv];
        
        //Multiply with the scaling factor for the channel 2 settings
        volts = delta * vcd->mul_factor;
        
        //Channel 2 text has a variable position
        //Format the voltage for displaying
        scope_print_value(displaytext, volts, vcd->volt_scale, "V2 ", "V");
        display_text(10, ch2ypos, displaytext);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_measurements(void)
{
  
}


//----------------------------------------------------------------------------------------------------------------------------------
//Simple non optimized function for string copy that returns the position of the terminator
//----------------------------------------------------------------------------------------------------------------------------------
char *strcpy(char *dst, const char *src)
{
  while(*src)
  {
    *dst++ = *src++;
  }
  
  //Terminate the copy
  *dst = 0;
  
  return(dst);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_print_value(char *buffer, uint32 value, uint32 scale, char *header, char *sign)
{
  //Copy the header into the string buffer
  buffer = strcpy(buffer, header);

  //Need to find the magnitude scale for the input
  //The calculations are based on fixed point
  while(value >= 100000)
  {
    //Skip to the next magnitude
    scale++;
    
    //Bring the value in range
    value /= 1000;
  }

  //Format the remainder for displaying. Only 3 digits are allowed to be displayed
  if(value < 1000)
  {
    //Less then 1000 means x.yy
    buffer = scope_print_decimal(buffer, value, 2);
  }
  else if(value < 10000)
  {
    //More then 1000 but less then 10000 means xx.y
    value /= 10;
    buffer = scope_print_decimal(buffer, value, 1);
  }
  else
  {
    //More then 10000 and less then 100000 means xxx
    value /= 100;
    buffer = scope_print_decimal(buffer, value, 0);
  }

  //Make sure scale is not out of range
  if(scale > 7)
  {
    scale = 7;
  }
  
  //Add the magnitude scaler
  buffer = strcpy(buffer, magnitude_scaler[scale]);
  
  //Add the type of measurement sign
  strcpy(buffer, sign);
}

//----------------------------------------------------------------------------------------------------------------------------------

char *scope_print_decimal(char *buffer, uint32 value, uint32 decimals)
{
  char    b[12];
  uint32  i = 12;   //Start beyond the array since the index is pre decremented
  uint32  s;

  //For value 0 no need to do the work
  if(value == 0)
  {
    //Value is zero so just set 0 character
    b[--i] = '0';
  }
  else
  {
    //Process the digits
    while(value)
    {
      //Set current digit to decreased index
      b[--i] = (value % 10) + '0';

      //Check if decimal point needs to be placed
      if(i == 12 - decimals)
      {
        //If so put it in
        b[--i] = '.';
      }
      
      //Take of the current digit
      value /= 10;
    }
  }

  //Determine the size of the string
  s = 12 - i;
  
  //Copy to the buffer
  memcpy(buffer, &b[i], s);
  
  //terminate the string
  buffer[s] = 0;
  
  //Return the position of the terminator to allow appending
  return(&buffer[s]);
}

//----------------------------------------------------------------------------------------------------------------------------------
// File display functions
//----------------------------------------------------------------------------------------------------------------------------------
//Simplest setup here is to put all important data in a struct and make it such that a pointer is used to point to the active one
//This way no memory needs to be copied
//Needs a bit of a re write but might improve things a bit
//Depends on how the pointer setup effects the main code

void scope_save_setup(PSCOPESETTINGS settings)
{
  //For now just copy the settings to the given struct
  memcpy(settings, &scopesettings, sizeof(SCOPESETTINGS));
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_setup(PSCOPESETTINGS settings)
{
  //For now just copy the settings from the given struct
  memcpy(&scopesettings, settings, sizeof(SCOPESETTINGS));
}

//----------------------------------------------------------------------------------------------------------------------------------
//These functions are for handling the settings to and from file

void scope_prepare_setup_for_file(void)
{
  uint16 *ptr = (uint16 *)viewfilesetupdata;
  uint32 index;
  uint32 channel;
  
  //No idea yet why this is done and what triggerflag4 is for
  if(scopesettings.triggerflag4)
  {
    //But these flags ensure the display is redrawn on trace displaying
    scopesettings.triggerflag2 = 1;
    scopesettings.triggerflag1 = 1;
  }
  
  //Best to clear the buffer first since not all bytes are used
  memset((uint8 *)viewfilesetupdata, 0, sizeof(viewfilesetupdata));
  
  //Copy the different settings to the file
  ptr[0]  = scopesettings.runstate;
  
  ptr[1]  = scopesettings.channel1.enable;
  ptr[2]  = scopesettings.channel1.voltperdiv;
  ptr[3]  = scopesettings.channel1.fftenable;
  ptr[4]  = scopesettings.channel1.coupling;
  ptr[5]  = scopesettings.channel1.magnification;
  
  ptr[6]  = scopesettings.channel2.enable;
  ptr[7]  = scopesettings.channel2.voltperdiv;
  ptr[8]  = scopesettings.channel2.fftenable;
  ptr[9]  = scopesettings.channel2.coupling;
  ptr[10] = scopesettings.channel2.magnification;

  ptr[11] = scopesettings.timeperdiv;
  
  ptr[12] = scopesettings.movespeed;
  
  ptr[13] = scopesettings.triggermode;
  ptr[14] = scopesettings.triggeredge;
  ptr[15] = scopesettings.triggerchannel;
  
  ptr[16] = scopesettings.batterychargelevel;
  
  ptr[17] = scopesettings.rightmenustate;

  ptr[18] = scopesettings.triggerflag2;
  ptr[19] = scopesettings.triggerflag1;

  ptr[20] = disp_x_start;

  //For time base settings 50S - 100mS per div disp_xpos is used as sample count
  if(scopesettings.timeperdiv < 9)
  {
    ptr[21] = disp_xpos;
  }
  else
  {
    ptr[21] = disp_sample_count;
  }

  //These are still unknown variables so set to zero for now
  ptr[22] = 0;
  ptr[23] = 0;
  ptr[24] = 0;
  ptr[25] = 0;
  
  ptr[26] = scopesettings.timeperdivbackup;
  
  ptr[27] = scopesettings.waveviewmode;
  
  ptr[40] = scopesettings.triggerposition;
  ptr[41] = scopesettings.triggeroffset;       //Is this one the correct one or should it be triggerlevel
  
  ptr[42] = scopesettings.channel1.traceoffset;
  ptr[43] = scopesettings.channel2.traceoffset;

  ptr[60] = scopesettings.screenbrightness;
  ptr[61] = scopesettings.gridbrightness;
  ptr[62] = scopesettings.alwaystrigger50;
  ptr[63] = scopesettings.xymodedisplay;
  
  //Point to the location in the buffer where the measurement states are set
  //Since the buffer is declared as uint32 the index will be based on that and the cast is needed for the assignment to the pointer
  //Need to verify this
  ptr = (uint16 *)&viewfilesetupdata[40];
  
  //Save the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(index=0;index<12;index++)
    {
      //Copy the current measurement state and point to the next one
      *ptr++ = scopesettings.measuresstate[channel][index];
    }
  }
  
  //The measurement values need to be copied in, but these seem to be 32 bits. With the buffer being 32 bit variables this is easy to do,
  //but I don't have these variables yet so skipped for now
  
  //Then the list of measurement numbers for display is copied in. The plan is to use a different setup in my code so also skipped
  
  
  //Point to the location in the buffer where the remaining parameters are copied
  //Since the buffer is declared as uint32 the index will be based on that and the cast is needed for the assignment to the pointer
  //Need to verify this
  ptr = (uint16 *)&viewfilesetupdata[100];
  
  ptr[0] = scopesettings.batterycharging;
  ptr[1] = scopesettings.batterychargelevel;   //Already copied in earlier so not really needed
  
  //Not sure what these two are for so set to zero for now.
  ptr[2] = 0;
  ptr[3] = 0;

  //No idea what this is for, but it is done in the original code
  fpga_read_parameter_ic(0x15, 0x18);
  
  //In the original code the sample buffers are saved here
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_setup_from_file(void)
{
  uint16 *ptr = (uint16 *)viewfilesetupdata;
  uint32 index;
  uint32 channel;
  
  //Copy the different settings to the file
  scopesettings.runstate = ptr[0];
  
  scopesettings.channel1.enable        = ptr[1];
  scopesettings.channel1.voltperdiv    = ptr[2];
  scopesettings.channel1.fftenable     = ptr[3];
  scopesettings.channel1.coupling      = ptr[4];
  scopesettings.channel1.magnification = ptr[5];
  
  scopesettings.channel2.enable        = ptr[6];
  scopesettings.channel2.voltperdiv    = ptr[7];
  scopesettings.channel2.fftenable     = ptr[8];
  scopesettings.channel2.coupling      = ptr[9];
  scopesettings.channel2.magnification = ptr[10];

  scopesettings.timeperdiv = ptr[11];
  
  scopesettings.movespeed = ptr[12];
  
  scopesettings.triggermode    = ptr[13];
  scopesettings.triggeredge    = ptr[14];
  scopesettings.triggerchannel = ptr[15];
  
  scopesettings.batterychargelevel = ptr[16];
  
  scopesettings.rightmenustate = ptr[17];

  scopesettings.triggerflag2 = ptr[18];
  scopesettings.triggerflag1 = ptr[19];

  disp_x_start = ptr[20];

  //For time base settings 50S - 100mS per div disp_xpos is used as sample count
  if(scopesettings.timeperdiv < 9)
  {
    disp_xpos = ptr[21];
  }
  else
  {
    disp_sample_count = ptr[21];
  }

  //These are still unknown variables so set to zero for now
  //ptr[22] = 0;
  //ptr[23] = 0;
  //ptr[24] = 0;
  //ptr[25] = 0;
  
  scopesettings.timeperdivbackup = ptr[26];
  
  scopesettings.waveviewmode = ptr[27];
  
  scopesettings.triggerposition = ptr[40];
  scopesettings.triggeroffset   = ptr[41];       //Is this one the correct one or should it be triggerlevel
  
  scopesettings.channel1.traceoffset = ptr[42];
  scopesettings.channel2.traceoffset = ptr[43];

  scopesettings.screenbrightness = ptr[60];
  scopesettings.gridbrightness   = ptr[61];
  scopesettings.alwaystrigger50  = ptr[62];
  scopesettings.xymodedisplay    = ptr[63];
  
  //Point to the location in the buffer where the measurement states are set
  //Since the buffer is declared as uint32 the index will be based on that and the cast is needed for the assignment to the pointer
  //Need to verify this
  ptr = (uint16 *)&viewfilesetupdata[40];
  
  //Save the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(index=0;index<12;index++)
    {
      //Copy the current measurement state and point to the next one
      scopesettings.measuresstate[channel][index] = *ptr++;
    }
  }
  
  //The measurement values need to be copied in, but these seem to be 32 bits. With the buffer being 32 bit variables this is easy to do,
  //but I don't have these variables yet so skipped for now
  
  //Then the list of measurement numbers for display is copied in. The plan is to use a different setup in my code so also skipped
  
  
  //Point to the location in the buffer where the remaining parameters are copied
  //Since the buffer is declared as uint32 the index will be based on that and the cast is needed for the assignment to the pointer
  //Need to verify this
  ptr = (uint16 *)&viewfilesetupdata[100];
  
  scopesettings.batterycharging    = ptr[0];
  scopesettings.batterychargelevel = ptr[1];   //Already copied in earlier so not really needed
  
  //Not sure what these two are for so set to zero for now.
  //ptr[2] = 0;
  //ptr[3] = 0;

  //No idea what this is for, but it is done in the original code
  //In the original code there might be an error, since for waveform view the r1 register (parameter input) is not set in this function
  fpga_read_parameter_ic(0x15, 0x18);
  
  //In the original code the sample buffers are restored here
}

//----------------------------------------------------------------------------------------------------------------------------------

const char view_file_extension[2][5] =
{
  { ".bmp" },
  { ".wav" }
};

//----------------------------------------------------------------------------------------------------------------------------------
//In the original code they setup a list with all the available file names and index into that list when a file is used
//Here the file name is made on the fly when needed

void scope_print_file_name(uint32 filenumber)
{
  char    b[12];
  uint32  i = 12;   //Start beyond the array since the index is pre decremented
  uint32  s;

  //For file number 0 no need to do the work
  if(filenumber == 0)
  {
    //Value is zero so just set a 0 character
    b[--i] = '0';
  }
  else
  {
    //Process the digits
    while(filenumber)
    {
      //Set current digit to decreased index
      b[--i] = (filenumber % 10) + '0';

      //Take of the current digit
      filenumber /= 10;
    }
  }

  //Determine the size of the decimal part
  s = 12 - i;
  
  //Copy in the decimal file number
  memcpy(viewfilename, &b[i], s);
  
  //Add the extension
  memcpy(&viewfilename[s], view_file_extension[viewtype & VIEW_TYPE_MASK], 5);
}

//----------------------------------------------------------------------------------------------------------------------------------

const char list_file_name[2][13] =
{
  { "piclist.sys" },
  { "wavelist.sys" }
};

const char system_file_name[2][16] =
{
  { "pic_system.sys" },
  { "wave_system.sys" }
};

//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_list_files(void)
{
  int32 result;
  
  //The original code is crap, so improved on it here
  //It also uses two separate functions for loading the lists
  //This code still needs more error handling with user feedback, but it will do for now
  
  //Try to open the thumbnail file for this view type
  result = f_open(&viewfp, list_file_name[viewtype & VIEW_TYPE_MASK], FA_READ);

  //Check the result
  if(result == FR_OK)
  {
    //Opened ok, so read the thumbnail data
    f_read(&viewfp, viewthumbnaildata, VIEW_THUMBNAIL_DATA_SIZE, 0);
    
    //Close the file
    f_close(&viewfp);
  }
  //Failure then check if file does not exist
  else if(result == FR_NO_FILE)
  {
    //Clear the thumbnail memory for a fresh file
    memset(viewthumbnaildata, 0, VIEW_THUMBNAIL_DATA_SIZE);
    
    //Need the file so create it
    result = f_open(&viewfp, list_file_name[viewtype & VIEW_TYPE_MASK], FA_CREATE_NEW | FA_WRITE | FA_READ);
    
    //Check if file is created ok
    if(result == FR_OK)
    {
      //Write the empty list to the file
      f_write(&viewfp, viewthumbnaildata, VIEW_THUMBNAIL_DATA_SIZE, 0);
    
      //Close the file
      f_close(&viewfp);
    }
  }
  
  //Try to open the file number file for this view type
  result = f_open(&viewfp, system_file_name[viewtype & VIEW_TYPE_MASK], FA_READ);
  
  //Check the result
  if(result == FR_OK)
  {
    //Opened ok, so read the file number data
    f_read(&viewfp, viewfilenumberdata, VIEW_FILE_NUMBER_DATA_SIZE, 0);

    //Close the file
    f_close(&viewfp);
  }
  //Failure then check if file does not exist
  else if(result == FR_NO_FILE)
  {
    //Clear the file number memory for a fresh file
    memset(viewfilenumberdata, 0, VIEW_FILE_NUMBER_DATA_SIZE);
    
    //Need the file so create it
    result = f_open(&viewfp, system_file_name[viewtype & VIEW_TYPE_MASK], FA_CREATE_NEW | FA_WRITE | FA_READ);
    
    //Check if file is created ok
    if(result == FR_OK)
    {
      //Write the empty list to the file
      f_write(&viewfp, viewfilenumberdata, VIEW_FILE_NUMBER_DATA_SIZE, 0);
    
      //Close the file
      f_close(&viewfp);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_list_files(void)
{
  int32 result;
  
  //The original code uses two separate functions for loading the lists
  //In the original code they try to create the file if open fails, but that is already done in the load function
  //and that is always called before a save is done, and the USB functionality is only operational when started via the menu
  //so the file can't be deleted in the meantime
  
  //Try to open the thumbnail file for this view type
  result = f_open(&viewfp, list_file_name[viewtype & VIEW_TYPE_MASK], FA_CREATE_ALWAYS | FA_WRITE);
  
  //Only if the file is opened write to it
  if(result == FR_OK)
  {
    //Write the list to the file
    f_write(&viewfp, viewthumbnaildata, VIEW_THUMBNAIL_DATA_SIZE, 0);

    //Close the file
    f_close(&viewfp);
  }
  else
  {
    //Show an error on the screen
  }
  
  //Try to open the file number file for this view type
  result = f_open(&viewfp, system_file_name[viewtype & VIEW_TYPE_MASK], FA_CREATE_ALWAYS | FA_WRITE);
  
  //Only if the file is opened write to it
  if(result == FR_OK)
  {
    //Write the list to the file
    f_write(&viewfp, viewfilenumberdata, VIEW_FILE_NUMBER_DATA_SIZE, 0);

    //Close the file
    f_close(&viewfp);
  }
  else
  {
    //Show an error on the screen
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//In the original code there are two separate functions for saving a picture or a waveform
//Since they are very similar in the basis I only made one function that handles all the tasks
//including restoring the data when a picture was made in waveform view mode.
//Another change is how a file number is obtained. Instead of probing the file system with every possible file until one is found that does not exist
//I use the file number list to get the first free number.

void scope_save_view_item_file(int32 type)
{
  uint32  newnumber;
  uint32  filenumber;
  uint32  result;
  uint16 *dptr;
  uint16 *sptr;
  
  //Point to the thumbnail data
  PTHUMBNAILDATA thumbnaildata = (PTHUMBNAILDATA)viewthumbnaildata;
  
  //Point to the file numbers
  uint16 *fnptr = (uint16 *)viewfilenumberdata;

  //Set the end pointer  
  uint16 *eptr = &fnptr[VIEW_MAX_ITEMS];
  
  //Save the current view type to be able to determine if the list files need to be reloaded
  uint32 currentviewtype = viewtype;
  
  //Switch to the given type
  viewtype = type;
  
  //Load the list files for this type. Needed for finding the file name and to add the thumbnail
  scope_load_list_files();

  //Find the first free file number
  //Most likely a more efficient solution for this problem exists, but this beats the original code where they try if a file number is free on the SD card with f_open
  for(newnumber=1;newnumber<VIEW_MAX_ITEMS;newnumber++)
  {
    //Start at the beginning of the list
    fnptr = (uint16 *)viewfilenumberdata;
    
    //Go through the list to see if the current number is in the list
    while((*fnptr) && (fnptr < eptr))
    {
      //Check if this number is in the list
      if(*fnptr == newnumber)
      {
        //Found it, so quit the loop
        break;
      }
      
      //Select the next number entry
      fnptr++;
    }
    
    //Check if not found
    if(*fnptr != newnumber)
    {
      //Can use this number since it is not in the list
      break;
    }
  }

  //Source is the entry before the last valid
  sptr = eptr - 2;

  //Destination is the last valid
  dptr = eptr - 1;
  
  //Bump all the entries in the list up
  //Compiler converts this to memmove
  while(dptr > (uint16 *)viewfilenumberdata)
  {
    *dptr-- = *sptr--;
  }
  
  //Fill in the new number
  *dptr = newnumber;
  
  //Find a free entry in the thumbnail list
  while(thumbnaildata < (PTHUMBNAILDATA)(((uint8 *)viewthumbnaildata) + VIEW_THUMBNAIL_DATA_SIZE))
  {
    //Get the file number from the current thumbnail
    filenumber = (thumbnaildata->filenumbermsb << 8) | thumbnaildata->filenumberlsb;
    
    //Check if the file number of this thumbnail is zero (not used) or matches the new file number
    if((filenumber == 0) || (filenumber == newnumber))
    {
      //If so create the thumbnail and quit the loop
      //This means that if there is no free slot or the new number is not found there will not be a thumbnail for it.
      //My version signals this.
      scope_create_thumbnail(newnumber, thumbnaildata);
      break;
    }

    //Select the next thumbnail set
    thumbnaildata++;
  }
  
  //save the amended lists
  scope_save_list_files();

  //Setup the filename
  scope_print_file_name(newnumber);
  
  //Open the new file. On failure signal this and quit
  result = f_open(&viewfp, viewfilename, FA_CREATE_NEW | FA_WRITE);

  //Check if file opened without problems
  if(result == FR_OK)
  {
    //For pictures the bitmap header needs to be written
    if(type == VIEW_TYPE_PICTURE)
    {
      //Write the bitmap header
      result = f_write(&viewfp, bmpheader, sizeof(bmpheader), 0);
    }

    //Check if still ok to proceed
    if(result == FR_OK)
    {
      //Save the settings for the trace portion of the data and write them to the file
      scope_prepare_setup_for_file();

      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 sample data
        if((result = f_write(&viewfp, (uint8 *)channel1tracebuffer4, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 sample data
          if((result = f_write(&viewfp, (uint8 *)channel2tracebuffer4, 3000, 0)) == FR_OK)
          {
            //Save the channel 1 display data
            if((result = f_write(&viewfp, (uint8 *)channel1ypoints, 1500, 0)) == FR_OK)
            {
              //Save the channel 2 display data
              if((result = f_write(&viewfp, (uint8 *)channel2ypoints, 1500, 0)) == FR_OK)
              {
                //Finish with an empty blob of data to reach the needed file size
                //Clear part of the thumbnail data for this. Is reloaded anyway so does not matter
                memset((uint8 *)viewthumbnaildata, 0, 5000);
                
                //Save it to the file
                if((result = f_write(&viewfp, (uint8 *)viewthumbnaildata, 5000, 0)) == FR_OK)
                {
                  //For pictures extra code is needed to write the pixel data to the file
                  if(type == VIEW_TYPE_PICTURE)
                  {
                    //Write the pixel data
                    result = f_write(&viewfp, (uint8 *)maindisplaybuffer, PICTURE_DATA_SIZE, 0);
                  }
                }
              }
            }
          }
        }
      }
    }

    //Close the file
    f_close(&viewfp);

    //Check if all went well
    if(result == FR_OK)
    {
      //Show the saved successful message
      scope_display_file_status_message(MESSAGE_SAVE_SUCCESSFUL, 0);
    }
    else
    {
      //Signal unable to write to the file
      scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);
    }
  }
  else
  {
    //Signal unable to create the file
    scope_display_file_status_message(MESSAGE_FILE_OPEN_FAILED, 0);
  }
  
  //When a picture is saved while viewing a waveform, reload the waveform lists
  if((type == VIEW_TYPE_PICTURE) && (currentviewtype == VIEW_TYPE_WAVEFORM) && (scopesettings.waveviewmode == 1))
  {
    //Restore the previous viewtype
    viewtype = currentviewtype;
    
    //Load the lists
    scope_load_list_files();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_remove_item_from_lists(uint32 delete)
{
  //Point to the file numbers
  uint16 *fnptr = (uint16 *)viewfilenumberdata;
  uint16 *dptr = &fnptr[viewcurrentindex];
  uint16 *sptr = &fnptr[viewcurrentindex + 1];
  uint16 *eptr = &fnptr[VIEW_MAX_ITEMS];
  
  //Get the file number for matching the item in the thumbnail list
  uint16 filenumber = fnptr[viewcurrentindex];
  
  //Point to the thumbnail data
  PTHUMBNAILDATA thumbnaildata = (PTHUMBNAILDATA)viewthumbnaildata;
  
  //The original code does this two at a time, which requires a check to see if there is an odd number of items to copy
  //Another method can be with memcpy and calculate the number of bytes that need to be moved
  //This solution only does the needed items, and with a max of 1000 items it does not really matter that much
  //Optimized compiling uses a memmove for this
  
  //Remove the item by shifting the others down
  while((sptr < eptr) && (*sptr))
  {
    //Copy the next item down
    *dptr++ = *sptr++;
  }
  
  //Clear the last entry
  *dptr = 0;
  
  //Locate the thumbnail data for the current file number
  while(thumbnaildata < (PTHUMBNAILDATA)(((uint8 *)viewthumbnaildata) + VIEW_THUMBNAIL_DATA_SIZE))
  {
    //Check if the file number of this thumbnail matches the current file number
    if(((thumbnaildata->filenumbermsb << 8) | thumbnaildata->filenumberlsb) == filenumber)
    {
      //If found, clear the file number
      thumbnaildata->filenumbermsb = 0;
      thumbnaildata->filenumberlsb = 0;

      //Done so quit the loop
      break;
    }

    //Select the next thumbnail set
    thumbnaildata++;
  }

  //Setup the file name for this file
  scope_print_file_name(filenumber);
  
  //Only delete when requested
  if(delete)
  {
    //Delete the file from the SD card
    f_unlink(viewfilename);                   //Need error handling with message here
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//This is used for both waveforms as well as pictures

int32 scope_load_trace_data(void)
{
  //Point to the file numbers
  uint16 *fnptr = (uint16 *)viewfilenumberdata;
  uint32 result;

  //Setup the file name for this view item  
  scope_print_file_name(fnptr[viewcurrentindex]);
  
  //Try to open the file for reading
  result = f_open(&viewfp, viewfilename, FA_READ);
  
  //Check if file opened ok
  if(result == FR_OK)
  {
    //For pictures the bitmap header needs to be skipped
    if(viewtype == VIEW_TYPE_PICTURE)
    {
      //SKip the first 70 bytes, which form the bitmap header
      result = f_lseek(&viewfp, 70);
    }
    
    //Check if still ok to proceed
    if(result == FR_OK)
    {
      //Checks on correct number of bytes read might be needed
      //Load the setup data to the file setup data buffer
      if((result = f_read(&viewfp, (uint8 *)viewfilesetupdata, 1000, 0)) == FR_OK)
      {
        //Copy the loaded data to the settings
        scope_restore_setup_from_file();

        //Load the channel 1 sample data
        if((result = f_read(&viewfp, (uint8 *)channel1tracebuffer4, 3000, 0)) == FR_OK)
        {
          //Load the channel 2 sample data
          if((result = f_read(&viewfp, (uint8 *)channel2tracebuffer4, 3000, 0)) == FR_OK)
          {
            //Load the channel 1 display data
            if((result = f_read(&viewfp, (uint8 *)channel1ypoints, 1500, 0)) == FR_OK)
            {
              //Load the channel 2 display data
              if((result = f_read(&viewfp, (uint8 *)channel2ypoints, 1500, 0)) == FR_OK)
              {
                //For waveform type view some variables need to be set to force the normal display in waveform view
                if(viewtype == VIEW_TYPE_WAVEFORM)
                {
                  scopesettings.triggerflag1 = 1;
                  scopesettings.triggerflag2 = 1;
                  scopesettings.runstate = 1;
                  scopesettings.waveviewmode = 1;
                }

                //Show the normal scope screen
                scope_setup_main_screen();

    //display the trace data

                //For now just a clean screen
                //Clear the trace portion of the screen
                display_set_fg_color(0x00000000);
                display_fill_rect(2, 46, 728, 434);
              }
            }
          }
        }
      }
    }
    else
    {
      //Signal unable to seek the file
      scope_display_file_status_message(MESSAGE_FILE_SEEK_FAILED, 0);
    }
    
    //Done with the file so close it
    f_close(&viewfp);
    
    //Check if one of the reads failed
    if(result != FR_OK)
    {
      //Signal unable to write to the file
      scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);
    }
  }
  else
  {
    //Signal unable to create the file
    scope_display_file_status_message(MESSAGE_FILE_OPEN_FAILED, 0);
  }

  //Check if all went well
  if(result == FR_OK)
  {
    //Tell it to the caller
    return(VIEW_TRACE_LOAD_OK);
  }

  //Remove the current item from the lists and delete the item from disk
  scope_remove_item_from_lists(1);
  
  //Save the list files
  scope_save_list_files();
  
  return(VIEW_TRACE_LOAD_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_sync_list_files(void)
{
  uint32  t;
  uint32  save = 0;
  uint16 *fnptr;
  uint16 *eptr;
  
  //Handle the two types of list files
  for(t=0;t<VIEW_MAX_TYPES;t++)
  {
    //Set the current type to handle
    viewtype = t;
    
    //Load the files for this type
    scope_load_list_files();
    
    //Point to the file numbers
    fnptr = (uint16 *)viewfilenumberdata;

    //Set the end pointer  
    eptr = &fnptr[VIEW_MAX_ITEMS];
    
    //Go through the items in the system file and check if the files still exist on the SD card
    while((*fnptr) && (fnptr < eptr))
    {
      //Setup the filename
      scope_print_file_name(*fnptr);

      //Try to open the file. On failure remove it from the lists
      if(f_open(&viewfp, viewfilename, FA_READ) == FR_NO_FILE)
      {
        //Set the file index
//        viewcurrentindex = ((uint32)fnptr - (uint32)viewfilenumberdata) / sizeof(uint16);
        viewcurrentindex = (fnptr - (uint16 *)viewfilenumberdata) / sizeof(uint16);
        
        //Remove the current item from the lists without delete, since it is already removed from the SD card 
        scope_remove_item_from_lists(0);
        
        //Signal saving of the list files is needed
        save = 1;
      }
      else
      {
        //File exists so close it
        f_close(&viewfp);
      }
      
      //Select the next file number
      fnptr++;
    }
    
    //Check if there was a change
    if(save)
    {
      //Save the files if so
      scope_save_list_files();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_count_and_display_thumbnails(void)
{
  uint16 *ptr;

  //Count how many items are available
  //The file contains shorts
  ptr = (uint16 *)viewfilenumberdata;

  //No items available yet
  viewavailableitems = 0;
  viewpages = 0;
  
  //A zero indicates no more entries. Make sure not to check beyond buffer size
  while(*ptr && (viewavailableitems < VIEW_MAX_ITEMS))
  {
    //One more item available
    viewavailableitems++;

    //Point to next entry
    ptr++;
  }

  //Calculate the number of pages available, based on number of items per page. 0 means 1 page
  //available items starts with 1 and with 16 items it would result in pages being 1, so need to subtract 1 before dividing
  if(viewavailableitems)
  {
    viewpages = (viewavailableitems - 1) / VIEW_ITEMS_PER_PAGE;
  }
  
  //Need to check if the current page is still valid
  if(viewpage > viewpages)
  {
    //Page no longer valid then use last page
    viewpage = viewpages;
  }
  
  //Display the thumbnails
  scope_display_thumbnails();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_thumbnails(void)
{
  //Point to the file numbers
  uint16 *fnptr = (uint16 *)viewfilenumberdata;

  //Point to the thumbnail data
  PTHUMBNAILDATA thumbnaildata = (PTHUMBNAILDATA)viewthumbnaildata;
  
  //Flag for signaling thumbnail found
  uint32 found = 0;
    
  //Determine the first index based on the current page
  uint32 index = viewpage * VIEW_ITEMS_PER_PAGE;

  //Set black color for background
  display_set_fg_color(0x00000000);

  //Clear the thumbnail display area
  display_fill_rect(0, 0, 730, 480);
  
  //Set grey color for menu separator
  display_set_fg_color(0x00202020);
  
  //Separate the thumbnails from the menu bar
  display_draw_vert_line(730, 0, 479);
  
  //Check if there are items to display
  if(viewavailableitems)
  {
    //Determine the available items for the current page
    if(viewpage < viewpages)
    {
      //Not on the last page so full set available
      viewitemsonpage = VIEW_ITEMS_PER_PAGE;
    }
    else
    {
      //Get the remainder of items for the last page
      uint32 nofitems = viewavailableitems % VIEW_ITEMS_PER_PAGE;
      
      //See if a fraction of the max items per page is available
      if(nofitems)
      {
        //If so only display these
        viewitemsonpage = nofitems;
      }
      else
      {
        //If the remainder is zero there are max number of items on the last page
        viewitemsonpage = VIEW_ITEMS_PER_PAGE;
      }
    }

    //Determine the last index based on the available items on the current page
    uint32 lastindex = index + viewitemsonpage;

    //Start with first item for drawing
    uint32 xpos = VIEW_ITEM_XSTART;
    uint32 ypos = VIEW_ITEM_YSTART;

    //Draw the available items on the screen
    while(index < lastindex)
    {
      //Examining the original code makes believe there can be a mismatch between the two files so it is necessary to search for
      //the thumbnail for this file number. The original code uses a function for this and copies the data to buffers.
      //Here it is done directly from the source data

      //Get the thumbnail data for the current file
      while(thumbnaildata < (PTHUMBNAILDATA)(((uint8 *)viewthumbnaildata) + VIEW_THUMBNAIL_DATA_SIZE))
      {
        //Check if the file number of this thumbnail matches the current file number
        if(((thumbnaildata->filenumbermsb << 8) | thumbnaildata->filenumberlsb) == fnptr[index])
        {
          //If so signal found
          found = 1;
          break;
        }

        //Select the next thumbnail set
        thumbnaildata++;
      }

      //Check if thumbnail found
      if(found)
      {
        //Display the thumbnail
        //Need to make a distinction between normal display and xy display mode
        if(thumbnaildata->xydisplaymode == 0)
        {
          //Normal mode
          //Set the x start position based on trace position and skip three pixels.
          //Trace position can be > 0 when zoomed in stop mode 
          uint32 x = xpos + thumbnaildata->traceposition + 3;

          //Check if channel 1 is enabled
          if(thumbnaildata->channel1enable)
          {
            scope_display_thumbnail_data(x, ypos, thumbnaildata, 0);
          }

          //Check if channel 2 is enabled
          if(thumbnaildata->channel2enable)
          {
            scope_display_thumbnail_data(x, ypos, thumbnaildata, 1);
          }
        }
        else
        {
          //xy display mode so set the trace color for it
          display_set_fg_color(XYMODE_COLOR);

          //A minimum of 2 would also do, but the original code uses 5.
          //It does not check on a maximum of samples, which is needed because channel 1 is limited to 180 samples and channel 2 to 200 samples
          //Check on a minimum of 5 samples
          if(thumbnaildata->tracesamples < 5)
          {
            //If not set to zero for a single line drawing
            thumbnaildata->tracesamples = 0;
          }
          else
          {
            //If more then 5 take 5 samples of
            thumbnaildata->tracesamples -= 5;
          }

          //Check on maximum samples allowed
          if(thumbnaildata->tracesamples > 176)
          {
            thumbnaildata->tracesamples = 176;
          }

          //Point to the data of the two channels
          uint8 *channel1data = thumbnaildata->channel1data;
          uint8 *channel2data = thumbnaildata->channel2data;

          //Start with first sample
          uint32 sample = 0;

          //Center the xy display
          uint32 x = xpos + 29;

          //Keep the samples in registers
          register uint32 x1, x2, y1, y2;

          //Load the first samples
          x1 = *channel1data + x;
          y1 = *channel2data + ypos;

          //Point to the next samples
          channel1data++;
          channel2data++;

          //Draw the trace
          while(sample < thumbnaildata->tracesamples)
          {
            //Get second samples
            x2 = *channel1data + x;
            y2 = *channel2data + ypos;

            //Draw all the lines
            display_draw_line(x1, y1, x2, y2);

            //Swap the samples
            x1 = x2;
            y1 = y2;

            //Point to the next samples
            channel1data++;
            channel2data++;

            //One sample done
            sample++;
          }
        }
      }
      else
      {
        //Display thumbnail not found
        //This is not in the original code
        //Display the message in red and with font_0
        display_set_fg_color(0x00FF0000);
        display_set_font(&font_0);
        display_text(xpos + 15, ypos + 50, "THUMBNAIL NOT FOUND");
      }

      //Set grey color for item border
      display_set_fg_color(0x00202020);

      //Draw the border
      display_draw_rect(xpos, ypos, VIEW_ITEM_WIDTH, VIEW_ITEM_HEIGHT);

      //Skip to next coordinates
      xpos += VIEW_ITEM_XNEXT;

      //Check if next row needs to be used
      if(xpos > VIEW_ITEM_XLAST)
      {
        //Reset x position to beginning of row
        xpos = VIEW_ITEM_XSTART;

        //Bump y position to next row
        ypos += VIEW_ITEM_YNEXT;
      }

      //Select next index
      index++;

      //Reset the thumbnail data pointer and found flag
      thumbnaildata = (PTHUMBNAILDATA)viewthumbnaildata;
      found = 0;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//The original code uses the display_trace function for this but that needs preprocessing of the data, so separate function used here

void scope_display_thumbnail_data(uint32 xpos, uint32 ypos, PTHUMBNAILDATA thumbnaildata, uint32 channel)
{
  register uint32 index = 0;
  register uint32 sample1, sample2;
  register uint32 count = thumbnaildata->tracesamples - 4;
  register uint8 *channeldata;
  
  //Check if count not out of range
  if(count > 176)
  {
    //Limit on the number of displayable pixels
    count = 176;
  }
  
  //Initialize based on the channel
  if(channel == 0)
  {
    //Use channel 1 trace color
    display_set_fg_color(CHANNEL1_COLOR);
    
    //Point to the data for this trace
    channeldata = thumbnaildata->channel1data;
  }
  else
  {
    //Use channel 2 trace color
    display_set_fg_color(CHANNEL2_COLOR);
    
    //Point to the data for this trace
    channeldata = thumbnaildata->channel2data;
  }
  
  //Get the first sample
  sample1 = channeldata[index++] + ypos;
  
  //Do while the samples last
  while(index < count)
  {
    //Get the second sample
    sample2 = channeldata[index++] + ypos;

    //Draw the line for these samples
    display_draw_line(xpos, sample1, xpos + 1, sample2);
    
    //Skip to next position
    xpos++;
    
    //Swap the samples
    sample1 = sample2;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_create_thumbnail(uint32 filenumber, PTHUMBNAILDATA thumbnaildata)
{
  uint32  count;
  uint8  *dptr;
  uint16 *sptr;
  
  //The number of samples needs to be reduced by a factor 4
  //The original code does this in a stupid way, without filtering the data. For now I will use similar method without doing everything 4 times
  
  //Fill in the file number
  thumbnaildata->filenumberlsb =  filenumber       & 0xFF;
  thumbnaildata->filenumbermsb = (filenumber >> 8) & 0xFF;
  
  //Set the parameters for channel 1
  thumbnaildata->channel1enable      = scopesettings.channel1.enable;
  thumbnaildata->channel1traceoffset = (((scopesettings.channel1.traceoffset - 46) * 10000) / 36166) + 5;

  //Set the parameters for channel 2
  thumbnaildata->channel2enable      = scopesettings.channel2.enable;
  thumbnaildata->channel2traceoffset = (((scopesettings.channel2.traceoffset - 46) * 10000) / 36166) + 5;
  
  //Set trigger information
  thumbnaildata->triggerlevelscreenoffset = ((scopesettings.triggeroffset * 10000) / 36166) + 5;
  thumbnaildata->triggerpositiononscreen  = scopesettings.triggerposition / 4;
  
  //Set the xy display mode
  thumbnaildata->xydisplaymode = scopesettings.xymodedisplay;
  
  //For long timebase (50S/div - 100mS/div) use a different start and number of samples count
  if(scopesettings.timeperdiv < 9)
  {
    //Always start at the beginning
    thumbnaildata->traceposition = 0;
    
    //Use the current x position divided by 4 as sample count
    thumbnaildata->tracesamples = disp_xpos / 4;
  }
  else
  {
    //Start at where the current trace starts on screen divided by 4
    thumbnaildata->traceposition = disp_x_start / 4;
    
    //Use the display sample count divided by 4 as sample count
    thumbnaildata->tracesamples = disp_sample_count / 4;
  }
  
  //Do channel 1 data if it is enabled
  if(scopesettings.channel1.enable)
  {
    //Only take 176 samples
    count = 176;
    dptr = thumbnaildata->channel1data;
    sptr = (uint16 *)channel1ypoints + 2;
    
    //Do all the samples
    while(count)
    {
      //Reduce the magnitude of the sample to fit the thumbnail rectangle
      *dptr = (uint8)(((*sptr - 46) * 10000) / 36166) + 5;
      
      //Point to next samples
      dptr++;
      
      //Only take every fourth sample
      sptr += 4;
      
      //One sample done
      count--;
    }
  }
  else
  {
    //Not enabled then just clear the data
    memset(thumbnaildata->channel1data, 5, sizeof(thumbnaildata->channel1data));
  }
  
  //Do channel 2 data if it is enabled
  if(scopesettings.channel2.enable)
  {
    //Only take 176 samples
    count = 176;
    dptr = thumbnaildata->channel2data;
    sptr = (uint16 *)channel2ypoints + 2;
    
    //Do all the samples
    while(count)
    {
      //Reduce the magnitude of the sample to fit the thumbnail rectangle
      *dptr = (uint8)(((*sptr - 46) * 10000) / 36166) + 5;
      
      //Point to next samples
      dptr++;
      
      //Only take every fourth sample
      sptr += 4;
      
      //One sample done
      count--;
    }
  }
  else
  {
    //Not enabled then just clear the data
    memset(thumbnaildata->channel2data, 5, sizeof(thumbnaildata->channel2data));
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_display_picture_item(void)
{
  //Display the new item
  if(scope_load_trace_data() == VIEW_TRACE_LOAD_ERROR)
  {
    //Return on an error
    return(VIEW_TRACE_LOAD_ERROR);
  }

  //And draw the bottom menu bar with a save of the background
  scope_setup_bottom_file_menu(VIEW_BOTTON_MENU_INIT);
  
  return(VIEW_TRACE_LOAD_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_selected_signs(void)
{
  uint32 index = 0;
  uint32 xpos = VIEW_ITEM_SELECTED_XSTART;
  uint32 ypos = VIEW_ITEM_SELECTED_YSTART;

  //Set the colors for displaying the selected sign. White sign on blue background
  display_set_fg_color(0x00FFFFFF);
  display_set_bg_color(0x000000FF);
  
  //Can't have more selects than items on the page  
  while(index < viewitemsonpage)
  {
    //Handle the current item based on its state
    switch(viewitemselected[index])
    {
      case VIEW_ITEM_SELECTED_NOT_DISPLAYED:
        //Make a copy of the screen under the selected sign location
        display_copy_rect_from_screen(xpos, ypos, 30, 30);
        
        //Display the selected sign
        display_copy_icon_use_colors(select_sign_icon, xpos, ypos, 30, 30);
        
        //Switch to displayed state
        viewitemselected[index] = VIEW_ITEM_SELECTED_DISPLAYED;
        break;

      case VIEW_ITEM_NOT_SELECTED_DISPLAYED:
        //Restore the screen on the selected sign location
        display_copy_rect_to_screen(xpos, ypos, 30, 30);
        
        //Switch to not selected state
        viewitemselected[index] = VIEW_ITEM_NOT_SELECTED;
        break;
    }
    
    //Skip to next coordinates
    xpos += VIEW_ITEM_XNEXT;
    
    //Check if next row needs to be used
    if(xpos > VIEW_ITEM_XLAST)
    {
      //Reset x position to beginning of selected row
      xpos = VIEW_ITEM_SELECTED_XSTART;
      
      //Bump y position to next row
      ypos += VIEW_ITEM_YNEXT;
    }
    
    //Select next index
    index++;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_file_status_message(int32 msgid, int32 alwayswait)
{
  uint32 checkconfirmation = scopesettings.confirmationmode;
  
  //Check if need to wait is requested
  if(alwayswait)
  {
    //If so override the setting
    checkconfirmation = 1;
  }
  
  //Need to save the screen buffer pointer and set it to the actual screen
  //When displaying trace data to avoid flickering data is drawn in a different screen buffer
  display_save_screen_buffer();

  //Save the screen rectangle where the message will be displayed
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  display_set_destination_buffer(displaybuffer2);
  display_copy_rect_from_screen(310, 210, 180, 60);
  
  //Draw the background in grey
  display_set_fg_color(0x00202020);
  display_fill_rect(310, 210, 180, 60);
  
  //Draw the border in a lighter grey
  display_set_fg_color(0x00303030);
  display_draw_rect(310, 210, 180, 60);

  //White color for text and use font_3
  display_set_fg_color(0x00FFFFFF);
  display_set_font(&font_3);
  
  switch(msgid)
  {
    case MESSAGE_SAVE_SUCCESSFUL:
      display_text(320, 220, "File saved successfully");
     
      //Don't wait for confirmation in case of success, unless requested
      checkconfirmation = alwayswait;
      break;

    case MESSAGE_FILE_CREATE_FAILED:
      display_text(320, 220, "Failed to create file");
      break;

    case MESSAGE_FILE_OPEN_FAILED:
      display_text(320, 220, "Failed to open file");
      break;

    case MESSAGE_FILE_WRITE_FAILED:
      display_text(320, 220, "Failed to write to file");
      break;

    case MESSAGE_FILE_READ_FAILED:
      display_text(320, 220, "Failed to read from file");
      break;

    case MESSAGE_FILE_SEEK_FAILED:
      display_text(320, 220, "Failed to seek in file");
      break;
  }

  //Display the file name in question
  display_text(320, 245, viewfilename);
  
  //Maybe wait for touch to continue in case of an error message
  if(checkconfirmation)
  {
    //wait for touch
    while(1)
    {
      //Read the touch panel status
      tp_i2c_read_status();

      //Check if the panel is touched
      if(havetouch)
      {
        //Done so quit the loop
        break;
      }
    }
  
    //Need to wait for touch to release before returning
    tp_i2c_wait_for_touch_release();
  }
  else
  {
    //Wait for half a second
    timer0_delay(500);
  }
  
  //Restore the original screen
  display_set_source_buffer(displaybuffer2);
  display_copy_rect_to_screen(310, 210, 180, 60);
  
  //Need to restore the screen buffer pointer
  display_restore_screen_buffer();
}

//----------------------------------------------------------------------------------------------------------------------------------
// Configuration data functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_configuration_data(void)
{
  //Get the settings data form the flash memory
  sys_spi_flash_read(0x001FD000, (uint8 *)settingsworkbuffer, sizeof(settingsworkbuffer));
  
  //Check if configuration data is ok
  if(settingsworkbuffer[200] == 0x1432)
  {
    //If they are ok, copy them to the actual settings
    scope_restore_config_data();
  }
  else
  {
    //Not ok, so reset to a default configuration
    scope_reset_config_data();
    
    //Get the settings in the working buffer and write them to the flash
    scope_save_configuration_data();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_configuration_data(void)
{
  //Save the settings for writing to the flash
  scope_save_config_data();

  //Write it to the flash
  sys_spi_flash_write(0x001FD000, (uint8 *)settingsworkbuffer, sizeof(settingsworkbuffer));
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_reset_config_data(void)
{
  //Load a default configuration in case of settings in flash being corrupted
  
  //Enable channel 1, dc coupling, 1x magnification, 50V/div, fft disabled and trace line in top part of the screen
  scopesettings.channel1.enable        = 1;
  scopesettings.channel1.coupling      = 0;
  scopesettings.channel1.magnification = 0;
  scopesettings.channel1.voltperdiv    = 0;
  scopesettings.channel1.fftenable     = 0;
  scopesettings.channel1.traceoffset   = 300;
  
  //Enable channel 2, dc coupling, 1x magnification, 50V/div, fft disabled and trace line in bottom part of the screen
  scopesettings.channel2.enable        = 1;
  scopesettings.channel2.coupling      = 0;
  scopesettings.channel2.magnification = 0;
  scopesettings.channel2.voltperdiv    = 0;
  scopesettings.channel2.fftenable     = 0;
  scopesettings.channel2.traceoffset   = 100;
  
  //Set trigger mode to auto, trigger edge to rising, trigger channel to channel 1, trigger position and trigger screen offset to center of the screen
  scopesettings.triggermode     = 0;
  scopesettings.triggeredge     = 0;
  scopesettings.triggerchannel  = 0;
  scopesettings.triggerposition = 362;
  scopesettings.triggeroffset   = 200;
  
  //Set move speed to fast
  scopesettings.movespeed = 0;
  
  //Set time base to 20uS/div
  scopesettings.timeperdiv = 19;
  
  //Enable some default measurements
  //Not yet implemented display wise and am thinking of a different way of doing it so left for later
  
  //Turn time cursor off and set some default positions
  scopesettings.timecursorsenable   = 0;
  scopesettings.timecursor1position = 183;
  scopesettings.timecursor2position = 547;
  
  //Turn volt cursor of and set some default positions
  scopesettings.voltcursorsenable   = 0;
  scopesettings.voltcursor1position = 167;
  scopesettings.voltcursor2position = 328;
  
  //Set right menu to normal state
  scopesettings.rightmenustate = 0;
  
  //Trigger flags to start mode
  scopesettings.triggerflag1 = 1;
  scopesettings.triggerflag2 = 1;

  //Set screen brightness to max, grid brightness to low, always 50% trigger on and x y mode display off
  scopesettings.screenbrightness = 100;
  scopesettings.gridbrightness   = 15;
  scopesettings.alwaystrigger50  = 1;
  scopesettings.xymodedisplay    = 0;
  
  //Set the settings integrity check flag
  system_ok = 0x1432;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_config_data(void)
{
  uint32  channel;
  uint32  index;
  uint16 *ptr;
  
  //Save the channel 1 settings
  settingsworkbuffer[0]  = scopesettings.channel1.enable;
  settingsworkbuffer[3]  = scopesettings.channel1.coupling;
  settingsworkbuffer[4]  = scopesettings.channel1.magnification;
  settingsworkbuffer[1]  = scopesettings.channel1.voltperdiv;
  settingsworkbuffer[2]  = scopesettings.channel1.fftenable;
  settingsworkbuffer[42] = scopesettings.channel1.traceoffset;

  //Save the channel 2 settings
  settingsworkbuffer[5]  = scopesettings.channel2.enable;
  settingsworkbuffer[8]  = scopesettings.channel2.coupling;
  settingsworkbuffer[9]  = scopesettings.channel2.magnification;
  settingsworkbuffer[6]  = scopesettings.channel2.voltperdiv;
  settingsworkbuffer[7]  = scopesettings.channel2.fftenable;
  settingsworkbuffer[43] = scopesettings.channel2.traceoffset;
  
  //Save trigger settings
  settingsworkbuffer[12] = scopesettings.triggermode;
  settingsworkbuffer[13] = scopesettings.triggeredge;
  settingsworkbuffer[14] = scopesettings.triggerchannel;
  settingsworkbuffer[40] = scopesettings.triggerposition;
  settingsworkbuffer[41] = scopesettings.triggeroffset;
  
  //Save the time base
  settingsworkbuffer[10] = scopesettings.timeperdiv;

  //Save the move speed
  settingsworkbuffer[11] = scopesettings.movespeed;
  
  //Save the right menu state
  settingsworkbuffer[15] = scopesettings.rightmenustate;

  //Save the confirmation mode (not in the original code)
  settingsworkbuffer[16] = scopesettings.confirmationmode;
  
  //Save screen brightness, grid brightness, always 50% trigger and xy display mode
  settingsworkbuffer[60] = scopesettings.screenbrightness;
  settingsworkbuffer[61] = scopesettings.gridbrightness;
  settingsworkbuffer[62] = scopesettings.alwaystrigger50;
  settingsworkbuffer[63] = scopesettings.xymodedisplay;
  
  //Save the time cursor settings
  settingsworkbuffer[161] = scopesettings.timecursorsenable;
  settingsworkbuffer[162] = scopesettings.timecursor1position;
  settingsworkbuffer[163] = scopesettings.timecursor2position;
  
  //Save the volt cursor settings
  settingsworkbuffer[164] = scopesettings.voltcursorsenable;
  settingsworkbuffer[165] = scopesettings.voltcursor1position;
  settingsworkbuffer[166] = scopesettings.voltcursor2position;

  //Point to the first measurement enable setting
  ptr = &settingsworkbuffer[80];
  
  //Save the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(index=0;index<12;index++)
    {
      //Copy the current measurement state and point to the next one
      *ptr++ = scopesettings.measuresstate[channel][index];
    }
  }
  
  //Save the calibration data
  settingsworkbuffer[120] = channel1_calibration_factor;
  settingsworkbuffer[127] = channel2_calibration_factor;

  //Point to the first channel calibration value
  ptr = &settingsworkbuffer[121];
  
  //Copy the working set values to the saved values
  for(index=0;index<6;index++,ptr++)
  {
    //Copy the data for both channels
    ptr[0] = channel1_calibration_data[index];
    ptr[7] = channel2_calibration_data[index];
  }

  //Restore the ADC2 calibration
  settingsworkbuffer[134] = channel1adc2calibration.flag;
  settingsworkbuffer[135] = channel1adc2calibration.compensation;
  settingsworkbuffer[136] = channel2adc2calibration.flag;
  settingsworkbuffer[137] = channel2adc2calibration.compensation;
  
  //Save the system ok flag
  settingsworkbuffer[200] = system_ok;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_config_data(void)
{
  uint32  channel;
  uint32  index;
  uint16 *ptr;
  
  //Restore the channel 1 settings
  scopesettings.channel1.enable        = settingsworkbuffer[0];
  scopesettings.channel1.coupling      = settingsworkbuffer[3];
  scopesettings.channel1.magnification = settingsworkbuffer[4];
  scopesettings.channel1.voltperdiv    = settingsworkbuffer[1];
  scopesettings.channel1.fftenable     = settingsworkbuffer[2];
  scopesettings.channel1.traceoffset   = settingsworkbuffer[42];

  //Restore the channel 2 settings
  scopesettings.channel2.enable        = settingsworkbuffer[5];
  scopesettings.channel2.coupling      = settingsworkbuffer[8];
  scopesettings.channel2.magnification = settingsworkbuffer[9];
  scopesettings.channel2.voltperdiv    = settingsworkbuffer[6];
  scopesettings.channel2.fftenable     = settingsworkbuffer[7];
  scopesettings.channel2.traceoffset   = settingsworkbuffer[43];
  
  //Restore trigger settings
  scopesettings.triggermode     = settingsworkbuffer[12];
  scopesettings.triggeredge     = settingsworkbuffer[13];
  scopesettings.triggerchannel  = settingsworkbuffer[14];
  scopesettings.triggerposition = settingsworkbuffer[40];
  scopesettings.triggeroffset   = settingsworkbuffer[41];
  
  //Restore the time base
  scopesettings.timeperdiv = settingsworkbuffer[10];

  //Restore the move speed
  scopesettings.movespeed = settingsworkbuffer[11];
  
  //Restore the right menu state
  scopesettings.rightmenustate = settingsworkbuffer[15];
  
  //Restore the confirmation mode (not in the original code)
  scopesettings.confirmationmode = settingsworkbuffer[16];
  
  //Since it is added to the code need to make sure the loaded value is in range.
  //Was not the case on my system, even though the data position is not used in the code checked so far
  if(scopesettings.confirmationmode > 1)
  {
    scopesettings.confirmationmode = 1;
  }
  
  //Restore screen brightness, grid brightness, always 50% trigger and xy display mode
  scopesettings.screenbrightness = settingsworkbuffer[60];
  scopesettings.gridbrightness   = settingsworkbuffer[61];
  scopesettings.alwaystrigger50  = settingsworkbuffer[62];
  scopesettings.xymodedisplay    = settingsworkbuffer[63];
  
  //Restore the time cursor settings
  scopesettings.timecursorsenable   = settingsworkbuffer[161];
  scopesettings.timecursor1position = settingsworkbuffer[162];
  scopesettings.timecursor2position = settingsworkbuffer[163];
  
  //Restore the volt cursor settings
  scopesettings.voltcursorsenable   = settingsworkbuffer[164];
  scopesettings.voltcursor1position = settingsworkbuffer[165];
  scopesettings.voltcursor2position = settingsworkbuffer[166];

  //Point to the first measurement enable setting
  ptr = &settingsworkbuffer[80];
  
  //Restore the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(index=0;index<12;index++)
    {
      //Copy the current measurement state and point to the next one
      scopesettings.measuresstate[channel][index] = *ptr++;
    }
  }
  
  //Restore the calibration data
  channel1_calibration_factor = settingsworkbuffer[120];
  channel2_calibration_factor = settingsworkbuffer[127];

  //Point to the first channel calibration value
  ptr = &settingsworkbuffer[121];
  
  //Copy the saved values to the working set
  for(index=0;index<6;index++,ptr++)
  {
    //Copy the data for both channels
    channel1_calibration_data[index] = ptr[0];
    channel2_calibration_data[index] = ptr[7];
  }

  //The last entry is a copy of the fore last value
  //Different from the original code where they use a switch statement instead of an array index for getting the data
  channel1_calibration_data[6] = settingsworkbuffer[126];
  channel2_calibration_data[6] = settingsworkbuffer[133];

  //Restore the ADC2 calibration
  channel1adc2calibration.flag         = settingsworkbuffer[134];
  channel1adc2calibration.compensation = settingsworkbuffer[135];
  channel2adc2calibration.flag         = settingsworkbuffer[136];
  channel2adc2calibration.compensation = settingsworkbuffer[137];
  
  //Restore the system ok flag. This is a bit off bullshit. Should be a CRC over all the settings to really verify the integrity
  system_ok = settingsworkbuffer[200];
}

//----------------------------------------------------------------------------------------------------------------------------------

#ifndef USE_TP_CONFIG
void save_tp_config(void)
{
  //Create a file for the touch panel configuration. Fails if it already exists
  if(f_open(&viewfp, "FNIRSI_1013D_tp_config.bin", FA_CREATE_NEW | FA_WRITE | FA_READ) == FR_OK)
  {
    //Write the touch panel configuration to the sd card  
    f_write(&viewfp, tp_config_data, sizeof(tp_config_data), 0);
  
    //Close the file to finish the write
    f_close(&viewfp);
  }
}
#endif  

//----------------------------------------------------------------------------------------------------------------------------------
