//----------------------------------------------------------------------------------------------------------------------------------
#include "types.h"
#include "menu.h"
#include "test.h"
#include "scope_functions.h"
#include "statemachine.h"
#include "touchpanel.h"
#include "timer.h"
#include "fpga_control.h"
#include "spi_control.h"
#include "sd_card_interface.h"
#include "display_lib.h"
#include "ff.h"
#include "DS3231.h"

#include "usb_interface.h"
#include "variables.h"

#include "sin_cos_math.h"

#include <string.h>
//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_display_lib(void)
{
  display_set_bg_color(BLACK_COLOR);

  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  display_set_dimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_main_screen(void)
{
  //Prepare the screen in a working buffer
//  display_set_screen_buffer(displaybuffer1);

  //Setup for clearing top and right menu bars
  display_set_fg_color(BLACK_COLOR);

  //Top bar for the main, channel and trigger menu and information
  display_fill_rect(0, 0, 730, 48);

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
  scope_channel_settings(&scopesettings.channel1, 0);
  scope_channel_settings(&scopesettings.channel2, 0);

  //Display the current time per div setting
  scope_acqusition_settings(0);

  //Show the user selected move speed
  scope_move_speed(0);

  //Display the trigger menu select button and the settings
  scope_trigger_settings(0);

  //Show the battery charge level and charging indicator
  scope_battery_status();
  
  //Show version information
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_2);
  display_text(VERSION_STRING_XPOS, VERSION_STRING_YPOS, VERSION_STRING);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_restore_screen(void)
{
  //Clear the whole screen
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(0, 0, 800, 480);
  
  //Fill in the screens with a blue color
  display_set_fg_color(DARKBLUE_COLOR);
  display_fill_rect(10, 10, 780, 460);
  
  //Display the text with font 5 and the white color
  display_set_font(&font_5);
  display_set_fg_color(WHITE_COLOR);
  display_text(370, 150, VERSION_STRING);    //Show version information //360
  display_text(270, 200, "Load default settings value");    //260
  
  if (SETTING_SECTOR_VERSION_HIGH == 0x0100)// settingsworkbuffer[2]
    {
    //Display the text with red color
    display_set_fg_color(RED_COLOR);
    display_text(240, 240, "Need input calibration procedure !");  //230
    }
  
  display_set_fg_color(GREEN_COLOR);
  display_text(320, 300, "Touch the screen");  //300
   
  //Scan the touch panel to see if there is user input
  while((havetouch == 0)) tp_i2c_read_status();
  
  //Clear the whole screen
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(0, 0, 800, 480);
   
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_view_screen(void)
{
  //Switch to view mode so disallow saving of settings on power down
  viewactive = VIEW_ACTIVE;

  //Set scope run state to running to have it sample fresh data on exit
  scopesettings.runstate = 0;

  //Only needed for waveform view. Picture viewing does not change the scope settings
  if(viewtype == VIEW_TYPE_WAVEFORM)
  {
    //Save the current settings
    scope_save_setup(&savedscopesettings1);
  }
  
  //Initialize the view mode variables
  //Used for indicating if select all or select button is active
  viewselectmode = 0;

  //Start at first page
  viewpage = 0;

  //Clear the item selected flags
  memset(viewitemselected, VIEW_ITEM_NOT_SELECTED, VIEW_ITEMS_PER_PAGE);

  //Set storage buffer for screen capture under selected signs and messages
  display_set_destination_buffer(displaybuffer2);
  display_set_source_buffer(displaybuffer2);

  //Display the file actions menu on the right side of the screen
  scope_setup_right_file_menu();

  //Load the thumbnail file for the current view type
  if(scope_load_thumbnail_file() != 0)
  {
    //Restore the main screen
    scope_setup_main_screen();
    
    //Loading the thumbnail file failed so no sense in going on
    return;
  }

  //Display the available thumbnails for the current view type
  scope_initialize_and_display_thumbnails();

  //Handle touch for this part of the user interface
  handle_view_mode_touch();

  //This is only needed when an actual waveform has been viewed, but that needs an extra flag
  //Only needed for waveform view. Picture viewing does not change the scope settings
  if(viewtype == VIEW_TYPE_WAVEFORM)
  {
    //Restore the current settings
    scope_restore_setup(&savedscopesettings1);

    //Make sure view mode is normal
    scopesettings.waveviewmode = 0;

    //And resume with auto trigger mode
    //scopesettings.triggermode = 0;

    //Need to restore the original scope data and fpga settings

    //Is also part of startup, so could be done with a function
    //Set the volts per div for each channel based on the loaded scope settings
    fpga_set_channel_voltperdiv(&scopesettings.channel1);
    fpga_set_channel_voltperdiv(&scopesettings.channel2);

    //These are not done in the original code
    //Set the channels AC or DC coupling based on the loaded scope settings
    fpga_set_channel_coupling(&scopesettings.channel1);
    fpga_set_channel_coupling(&scopesettings.channel2);

    //Setup the trigger system in the FPGA based on the loaded scope settings
    fpga_set_sample_rate(scopesettings.samplerate);
    fpga_set_trigger_channel();
    fpga_set_trigger_edge();
    fpga_set_trigger_level();
    fpga_set_trigger_mode();

    //Set channel screen offsets
    fpga_set_channel_offset(&scopesettings.channel1);
    fpga_set_channel_offset(&scopesettings.channel2);
  }

  //Reset the screen to the normal scope screen
  scope_setup_main_screen();
  
  //Display the trace data
  scope_display_trace_data();   //add for single mode screen mismas

  //Back to normal mode so allow saving of settings on power down
  viewactive = VIEW_NOT_ACTIVE;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_usb_screen(void)
{
  uint8   diag_screen = 1;
  //Clear the whole screen
  display_set_fg_color(BLACK_COLOR);
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
  display_set_fg_color(DARKBLUE_COLOR);
  display_fill_rect(477, 125, 235, 163);
  display_fill_rect(88, 210, 163, 112);

  //Draw a dark border around the blue screens
  display_set_fg_color(0x00111111);
  display_draw_rect(477, 125, 235, 163);
  display_draw_rect(88, 210, 163, 112);

  //Display the text with font 5 and the light color
  display_set_font(&font_5);
  display_set_fg_color(WHITE_COLOR);
  display_text(130, 254, "Off USB");    //130 bolo

  //Start the USB interface
  usb_device_enable();

  //Wait for the user to touch the scope Off USB section to quit
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();

    //Check if there is touch
    if(havetouch)
    {
      //Check if touch within the computer screen
      if((xtouch >= 90) && (xtouch <= 250) && (ytouch >= 210) && (ytouch <= 320))
        {
        //Touched the text field so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        break;
        }
      
      //Check if touch within the text field
      if(( diag_screen == 1)&& (xtouch >= 570) && (xtouch <= 610) && (ytouch >= 205) && (ytouch <= 225))// x596 y203
      //if((xtouch >= 550) && (xtouch <= 630) && (ytouch >= 160) && (ytouch <= 240))// x596 y203    
        {  
        //Go diagnostic info
        scope_diagnostic_screen();
        
        //Add the ok button
        scope_display_ok_button(550, 400, 0);
        
        //Touched the text field so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();
        
        //Switch to diag_screen 2
        diag_screen = 2;
        }
      
      //************************
      //Check if touch within the text field
      if(( diag_screen == 2) && (xtouch >= 560) && (xtouch <= 590) && (ytouch >= 415) && (ytouch <= 440))//360 390 415 440
        {
        //Highlight the button
        scope_display_ok_button(550, 400, 1);
          
        //Set default channel Input calibration values
        for(uint8 index=0;index<7;index++)
           {
           //Set default Input calibration values
           scopesettings.channel1.input_calibration[index] = signal_adjusters[index];
           scopesettings.channel2.input_calibration[index] = signal_adjusters[index];
           }
        //Ready to Input calibration
        calibrationfail = 1;
                            
        //Go diagnostic info
        scope_diagnostic_screen();

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();
               
        //Add the ok button
        //scope_display_ok_button(350, 400, 0);
        scope_display_ok_button(550, 400, 0);
                 
        }
      
      
      
      //****************************     
    }
  }

  //Stop the USB interface
  usb_device_disable();

  //Re-sync the system files
  scope_sync_thumbnail_files();
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_diagnostic_screen(void)

{
  const uint16    a=550;
  const uint16    b=650;
  const uint16    c=a+40;

  const uint16    i=40;
  const uint16    j=170;
//const uint16    k=700;

  //Clear the whole screen
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(260, 0, 540, 400);
  
  //Display the text with font 3 and the red color
  display_set_fg_color(RED_COLOR);
  display_set_font(&font_3);
  
  //display_text(435, 414, "Reset calibration values !!!");
  display_text(670, 405, "RESET");
  display_text(630, 425, "calibration values !!!");
  
  //Display the text with font 3 and the white color
  display_set_fg_color(WHITE_COLOR);
  
  display_text(a, i, "Offset ch1");   
  display_decimal(a, i+15, scopesettings.channel1.dc_calibration_offset[0]);
  display_text(c, i+15, "5V");     
  display_decimal(a, i+30, scopesettings.channel1.dc_calibration_offset[1]);
  display_decimal(a, i+45, scopesettings.channel1.dc_calibration_offset[2]);
  display_decimal(a, i+60, scopesettings.channel1.dc_calibration_offset[3]);
  display_decimal(a, i+75, scopesettings.channel1.dc_calibration_offset[4]);
  display_decimal(a, i+90, scopesettings.channel1.dc_calibration_offset[5]);
  display_decimal(a, i+105, scopesettings.channel1.dc_calibration_offset[6]);
  display_text(c, i+105, "50mV");
      
  display_text(b, i, "calibration input ch1");   
  display_decimal(b, i+15, scopesettings.channel1.input_calibration[0]);
  display_decimal(b, i+30, scopesettings.channel1.input_calibration[1]);
  display_decimal(b, i+45, scopesettings.channel1.input_calibration[2]);
  display_decimal(b, i+60, scopesettings.channel1.input_calibration[3]);
  display_decimal(b, i+75, scopesettings.channel1.input_calibration[4]);
  display_decimal(b, i+90, scopesettings.channel1.input_calibration[5]);
  display_decimal(b, i+105, scopesettings.channel1.input_calibration[6]);
      
      
  display_text(a, j, "Offset ch2");  
  display_decimal(a, j+15, scopesettings.channel2.dc_calibration_offset[0]);
  display_text(c, j+15, "5V"); 
  display_decimal(a, j+30, scopesettings.channel2.dc_calibration_offset[1]);
  display_decimal(a, j+45, scopesettings.channel2.dc_calibration_offset[2]);
  display_decimal(a, j+60, scopesettings.channel2.dc_calibration_offset[3]);
  display_decimal(a, j+75, scopesettings.channel2.dc_calibration_offset[4]);
  display_decimal(a, j+90, scopesettings.channel2.dc_calibration_offset[5]);
  display_decimal(a, j+105, scopesettings.channel2.dc_calibration_offset[6]);
  display_text(c, j+105, "50mV");

  display_text(a, j+125, "Xtouch");
  display_decimal(a+60, j+125, xtouch);
  display_text(a, j+140, "Ytouch");
  display_decimal(a+60, j+140, ytouch);
 
  display_text(b, j, "calibration input ch2");    //130 bolo      
  display_decimal(b, j+15, scopesettings.channel2.input_calibration[0]);
  display_decimal(b, j+30, scopesettings.channel2.input_calibration[1]);
  display_decimal(b, j+45, scopesettings.channel2.input_calibration[2]);
  display_decimal(b, j+60, scopesettings.channel2.input_calibration[3]);
  display_decimal(b, j+75, scopesettings.channel2.input_calibration[4]);
  display_decimal(b, j+90, scopesettings.channel2.input_calibration[5]);
  display_decimal(b, j+105, scopesettings.channel2.input_calibration[6]);
      
//Highlight the button
//scope_display_ok_button(350, 400, 0);


}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_right_control_menu(void)
{
  //Setup for clearing right menu bar
  display_set_fg_color(BLACK_COLOR);

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
      scope_previous_wave_button(0);
      scope_next_wave_button(0);
      scope_delete_wave_button(0);
    }
  }
  else
  {
    //Channel sensitivity state
    scope_channel_sensitivity_control(&scopesettings.channel1, 0, 0);
    scope_channel_sensitivity_control(&scopesettings.channel2, 0, 0);

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
  //Set black color for background
  display_set_fg_color(BLACK_COLOR);

  //Clear the right menu bar
  display_fill_rect(730, 0, 70, 480);

  //Add the buttons
  scope_return_button(0);
  scope_select_all_button(0);
  scope_select_button(0);
  scope_delete_button(0);
  scope_page_up_button(0);
  scope_page_down_button(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_setup_bottom_file_menu(int mode)
{
  PTHUMBNAILDATA thumbnaildata;   
    
  //Check if background needs to be saved
  if(mode == VIEW_BOTTON_MENU_INIT)
  {
    //Save the screen rectangle where the menu will be displayed
    display_copy_rect_from_screen(0, 390, 800, 90);
  }

  //Check if it needs to be drawn
  if(mode & VIEW_BOTTON_MENU_SHOW)
  {
    //Draw the background in grey
    display_set_fg_color(DARKGREY_COLOR);
    display_fill_rect(0, 390, 800, 90);

    //Draw the filename in white
    display_set_fg_color(WHITE_COLOR);
    display_set_font(&font_5);
    //display_text(20, 392, viewfilename);
    
      if(onoffRTC) //if RTCon view rtc info
        {
        thumbnaildata = &viewthumbnaildata[viewcurrentindex];
        decodethumbnailfilename(thumbnaildata->filename);
        display_text(20, 392, filenameRTC);
        }   else display_text(20, 392, viewfilename);
    
    //Setup the buttons
    scope_bmp_return_button(0);    
    scope_bmp_delete_button(0);
    scope_bmp_previous_button(0);
    scope_bmp_next_button(0);
    
    //Signal menu is visible
    viewbottommenustate = VIEW_BOTTON_MENU_SHOW | VIEW_BOTTOM_MENU_ACTIVE;
  }
  else
  {
    //Hide the menu bar
    display_copy_rect_to_screen(0, 390, 800, 90);

    //Signal menu is not visible
    viewbottommenustate = VIEW_BOTTON_MENU_HIDE | VIEW_BOTTOM_MENU_ACTIVE;
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
    display_set_fg_color(GREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 5, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(739, 5, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
  //Check if run or stop mode  
  if(scopesettings.runstate == 0) 
    {//Run mode:Green box
    display_set_fg_color(GREEN_COLOR);
    }    
    else 
    {//Stop mode:Red box 
    display_set_fg_color(RED_COLOR);
    }
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 65, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 65, 51, 50, 2);
        
  //Check if inactive or active mode
  if(mode == 0)
  {
      //Check if run or stop mode - Run mode: black text 
    if(scopesettings.runstate == 0) display_set_fg_color(BLACK_COLOR);
          //Stop mode: white text
          else display_set_fg_color(WHITE_COLOR);  
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);  //black?0x00202020
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 125, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 125, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(746, 135, "AUTO");
  display_text(753, 150, "SET");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_next_wave_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 65, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 65, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(749, 75, "next");
  display_text(748, 90, "wave");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_previous_wave_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 125, 51, 50, 2);
  
  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 125, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(749, 135, "prev");
  display_text(748, 150, "wave");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_t_cursor_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 185, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 185, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 245, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 245, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 305, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 305, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 365, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 365, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 425, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 425, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 425, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(752, 436, "50%");
  display_text(749, 449, "TRIG");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_grid_button(int mode)       //treba neexistuje
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(739, 425, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, 425, 51, 50, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(748, 436, "show");
  display_text(752, 449, "grid");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_sensitivity_control(PCHANNELSETTINGS settings, int type, int mode)
{
  uint32 y;

  //Check if V+ is active or inactive
  if((type == 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }

  //Top button
  display_fill_rounded_rect(739, settings->voltdivypos, 51, 50, 2);
  
  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, settings->voltdivypos, 51, 50, 2);

  //Check if V- is active or inactive
  if((type != 0) && (mode != 0))
  {
    //Orange color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //Black color for inactive button
    display_set_fg_color(DARKGREY_COLOR);
  }

  //Y position o f bottom button
  y = settings->voltdivypos + 86;
  
  //Bottom button
  display_fill_rounded_rect(739, y, 51, 50, 2);

  //Draw the edge
  display_set_fg_color(0x00505050);
  display_draw_rounded_rect(739, y, 51, 50, 2);
  
  //Display V+ and V- the text in larger font
  display_set_font(&font_0);
  
  //Check if V+ is active or inactive
  if((type == 0) && (mode != 0))
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  
  //Top button text
  display_text(757, settings->voltdivypos + 18, "V+");

  //Check if V- is active or inactive
  if((type != 0) && (mode != 0))
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  
  //Bottom button text
  display_text(757, settings->voltdivypos + 104, "V-");

  //Display the channel identifier bar with the channel color
  display_set_fg_color(settings->color);
  display_fill_rect(739, settings->voltdivypos + 56, 51, 24);

  //Display the channel identifier in black
  display_set_fg_color(BLACK_COLOR);
  display_set_font(&font_2);
  display_text(754, settings->voltdivypos + 61, settings->buttontext);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_return_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 14, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 14, 63, 58, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(747, 34, "return");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_select_all_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else if(mode == 1)
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //White for enabled button
    display_set_fg_color(WHITE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 93, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 93, 63, 58, 2);
  
  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(746, 106, "select");
  display_text(758, 120, "all");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_select_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else if(mode == 1)
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }
  else
  {
    //White for enabled button
    display_set_fg_color(WHITE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 173, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 173, 63, 58, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(746, 193, "select");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_delete_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 253, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 253, 63, 58, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(746, 273, "delete");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_page_up_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 333, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 333, 63, 58, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(750, 345, "page");
  display_text(758, 360, "up");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_page_down_button(int mode)
{
  //Check if inactive or active mode
  if(mode == 0)
  {
    //Black color for inactive button
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active color for activated button
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Draw the body of the button
  display_fill_rounded_rect(734, 413, 63, 58, 2);

  //Outline the button
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(734, 413, 63, 58, 2);

  //Check if inactive or active mode
  if(mode == 0)
  {
    //White text color for inactive button
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Black text color for activated button
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(750, 425, "page");
  display_text(748, 442, "down");
}

//----------------------------------------------------------------------------------------------------------------------------------
// Bitmap control bar functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_bmp_return_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so black background
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so active color background
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Fill in the body of the button
  display_fill_rounded_rect(40, 425, 120, 50, 3);

  //Draw rounded rectangle as button border in black
  display_set_fg_color(BLACK_COLOR);
  display_draw_rounded_rect(40, 425, 120, 50, 3);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so black foreground and active color background
    display_set_fg_color(LIGHTGREY_COLOR);
    display_set_bg_color(ITEM_ACTIVE_COLOR);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(return_arrow_icon, 79, 436, 41, 27);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_bmp_delete_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so black background
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so active color background
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Fill in the body of the button
  display_fill_rounded_rect(240, 425, 120, 50, 3);

  //Draw rounded rectangle as button border in black
  display_set_fg_color(BLACK_COLOR);
  display_draw_rounded_rect(240, 425, 120, 50, 3);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so black foreground and active color background
    display_set_fg_color(LIGHTGREY_COLOR);
    display_set_bg_color(ITEM_ACTIVE_COLOR);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(waste_bin_icon, 284, 433, 31, 33);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_bmp_previous_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so black background
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so active color background
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Fill in the body of the button
  display_fill_rounded_rect(440, 425, 120, 50, 3);

  //Draw rounded rectangle as button border in black
  display_set_fg_color(BLACK_COLOR);
  display_draw_rounded_rect(440, 425, 120, 50, 3);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so black foreground and active color background
    display_set_fg_color(LIGHTGREY_COLOR);
    display_set_bg_color(ITEM_ACTIVE_COLOR);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(previous_picture_icon, 483, 438, 33, 24);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_bmp_next_button(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so black background
    display_set_fg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so active color background
    display_set_fg_color(ITEM_ACTIVE_COLOR);
  }

  //Fill in the body of the button
  display_fill_rounded_rect(640, 425, 120, 50, 3);

  //Draw rounded rectangle as button border in black
  display_set_fg_color(BLACK_COLOR);
  display_draw_rounded_rect(640, 425, 120, 50, 3);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(LIGHTGREY_COLOR);
  }
  else
  {
    //Active so black foreground and active color background
    display_set_fg_color(LIGHTGREY_COLOR);
    display_set_bg_color(ITEM_ACTIVE_COLOR);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(next_picture_icon, 683, 438, 33, 24);
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
    display_set_fg_color(DARKBLUE_COLOR);//78
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
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active so black foreground
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Active so white background
    display_set_fg_color(WHITE_COLOR);
  }

  //Draw the background
  display_fill_rect(0, 0, 80, 38);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(BLACK_COLOR);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(WHITE_COLOR);
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
    //Run mode. Green box
    if((scopesettings.triggermode == 1)&&(!triggerlong))
        display_set_fg_color(ORANGE_COLOR); 
        else display_set_fg_color(GREEN_COLOR);    
  }
  else
  {
    //Stop mode. Red box
    display_set_fg_color(RED_COLOR);
  }
  
  //Fill the box
  display_fill_rect(RUN_STOP_TEXT_XPOS, RUN_STOP_TEXT_YPOS, RUN_STOP_TEXT_WIDTH, RUN_STOP_TEXT_HEIGHT);

  //Select the font for the text
  display_set_font(&font_3);

  //Check if run or stop mode
  if(scopesettings.runstate == 0)
  {
    //Run mode. Black text 
    display_set_fg_color(BLACK_COLOR);
    if((scopesettings.triggermode==1)&&(!triggerlong))
        display_text(RUN_STOP_TEXT_XPOS + 3, RUN_STOP_TEXT_YPOS + 1, "WAIT");
        else display_text(RUN_STOP_TEXT_XPOS + 5, RUN_STOP_TEXT_YPOS + 1, "RUN");
  }
  else
  {
    //Stop mode. White text
    display_set_fg_color(WHITE_COLOR);
    display_text(RUN_STOP_TEXT_XPOS + 1, RUN_STOP_TEXT_YPOS + 1, "STOP");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_settings(PCHANNELSETTINGS settings, int mode)
{
  int8 **vdtext;

  //Clear the area first
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(settings->buttonxpos, CH_BUTTON_YPOS, CH_BUTTON_BG_WIDTH, CH_BUTTON_BG_HEIGHT);

  //Check if channel is enabled or disabled
  if(settings->enable == 0)
  {
    //Disabled so off colors
    //Check if inactive or active
    if(mode == 0)
    {
      //Inactive, grey menu button
      display_set_fg_color(GREY_COLOR);
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
      display_set_fg_color(settings->color);
    }
    else
    {
      //Active, blue menu button
      display_set_fg_color(settings->touchedcolor);
    }
  }

  //Fill the button
  display_fill_rect(settings->buttonxpos, CH_BUTTON_YPOS, CH_BUTTON_WIDTH, CH_BUTTON_HEIGHT);

  //Select the font for the text
  display_set_font(&font_2);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Active, white text
    display_set_fg_color(WHITE_COLOR);

    //Fill the settings background
    display_fill_rect(settings->buttonxpos + 30, CH_BUTTON_YPOS, CH_BUTTON_BG_WIDTH - 30, CH_BUTTON_BG_HEIGHT);
  }

  //Display the channel identifier text
  display_text(settings->buttonxpos + 5, CH_BUTTON_YPOS + 11, settings->buttontext);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active, black text
    display_set_fg_color(BLACK_COLOR);
  }

  //Check on which coupling is set
  if(settings->coupling == 0)
  {
    //DC coupling
    display_text(settings->buttonxpos + 38, CH_BUTTON_YPOS + 3, "DC");
  }
  else
  {
    //AC coupling
    display_text(settings->buttonxpos + 38, CH_BUTTON_YPOS + 3, "AC");
  }

  //Print the probe magnification factor
  switch(settings->magnification)
  {
    case 0:
      //Times 1 magnification
      display_text(settings->buttonxpos + 63, CH_BUTTON_YPOS + 3, "1X");

      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[0];
      break;

    case 1:
      //Times 10 magnification
      display_text(settings->buttonxpos + 61, CH_BUTTON_YPOS + 3, "10X");

      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[1];
      break;

    default:
      //Times 100 magnification
      display_text(settings->buttonxpos + 59, CH_BUTTON_YPOS + 3, "100X");

      //Set the volts per div text range to be used for this magnification
      vdtext = (int8 **)volt_div_texts[2];
      break;
  }

  //Display the sensitivity when in range
  if(settings->displayvoltperdiv < 7)
  {
    display_text(settings->buttonxpos + 38, CH_BUTTON_YPOS + 19, vdtext[settings->displayvoltperdiv]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_acqusition_settings(int mode)
{
  //Clear the area first
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(ACQ_BUTTON_XPOS, ACQ_BUTTON_YPOS, ACQ_BUTTON_BG_WIDTH, ACQ_BUTTON_BG_HEIGHT);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, short time base green menu button if long time base orange
    if (scopesettings.long_mode) display_set_fg_color(ORANGE_COLOR); else display_set_fg_color(GREEN_COLOR);
  }
  else
  {
    //Active, magenta menu button
    display_set_fg_color(MAGENTA_COLOR);
  }

  //Fill the button
  display_fill_rect(ACQ_BUTTON_XPOS, ACQ_BUTTON_YPOS, ACQ_BUTTON_WIDTH, ACQ_BUTTON_HEIGHT);

  //Select the font for the text
  display_set_font(&font_2);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Active, white text
    display_set_fg_color(WHITE_COLOR);

    //Fill the settings background
    display_fill_rect(ACQ_BUTTON_XPOS + 30, ACQ_BUTTON_YPOS, ACQ_BUTTON_BG_WIDTH - 30, ACQ_BUTTON_BG_HEIGHT);
  }

  //Display the acquisition identifier text
  if (scopesettings.long_mode)
  {
      display_text(ACQ_BUTTON_XPOS + 4, ACQ_BUTTON_YPOS + 4, "ACQ");    //long time base mode
      display_text(ACQ_BUTTON_XPOS + 3, ACQ_BUTTON_YPOS + 20, "Long"); 
  } else display_text(ACQ_BUTTON_XPOS + 4, ACQ_BUTTON_YPOS + 11, "ACQ"); //sort time base mode

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active, black text
    display_set_fg_color(BLACK_COLOR);
  }

  //Only display the text when in range of the text array
  if(scopesettings.samplerate < (sizeof(acquisition_speed_texts) / sizeof(int8 *)))
  {
    //Display the text from the table
    display_text(ACQ_BUTTON_XPOS + 38, ACQ_BUTTON_YPOS + 3, (int8 *)acquisition_speed_texts[scopesettings.samplerate]);
  }

  //Only display the text when in range of the text array
  if(scopesettings.timeperdiv < (sizeof(time_div_texts) / sizeof(int8 *)))
  {
    //Display the text from the table
    display_text(ACQ_BUTTON_XPOS + 38, ACQ_BUTTON_YPOS + 19, (int8 *)time_div_texts[scopesettings.timeperdiv]);
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
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active so black text
    display_set_fg_color(BLACK_COLOR);
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
    
   //-******************************************************************
    /*
   //Load a default set on failure
   scope_reset_config_data();
    
   //Save it to the SD card
   scope_save_configuration_data();
   */
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_settings(int mode)
{
  int8 *modetext = 0;

  //Clear the area first black
  display_set_fg_color(LIGHTGREY_COLOR);    //BLACK_COLOR
    display_fill_rect(560, 5, 80, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, green menu button
    display_set_fg_color(TRIGGER_COLOR);
  }
  else
  {
    //Active, magenta menu button
    display_set_fg_color(MAGENTA_COLOR);
  }

  //Fill the button
  display_fill_rect(560, 5, 31, 35);

  //Select the font for the text
  display_set_font(&font_4);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, black text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Active, white text
    display_set_fg_color(WHITE_COLOR);

    //Fill the settings background
    display_fill_rect(591, 5, 48, 35);
  }

  //Display the channel identifier text
  display_text(572, 15, "T");

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive, white text
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active, black text
    display_set_fg_color(BLACK_COLOR);
  }

  //Check on which trigger mode is set
  switch(scopesettings.triggermode)
  {
    case 0:
      modetext = "  Auto";
      break;

    case 1:
      modetext = " Single";
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
    display_text(596, 7, modetext);
  }

  //Draw the trigger edge symbol
  display_draw_vert_line(632, 27, 38);

  //Draw the arrow based on the selected edge
  if(scopesettings.triggeredge == 0)
  {
    //rising edge
    display_draw_horz_line(27, 632, 635);
    display_draw_horz_line(38, 629, 632);
    display_draw_horz_line(32, 631, 633);
    display_draw_horz_line(33, 630, 634);
  }
  else
  {
    //falling edge
    display_draw_horz_line(27, 629, 632);
    display_draw_horz_line(38, 632, 635);
    display_draw_horz_line(32, 630, 634);
    display_draw_horz_line(33, 631, 633);
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
      display_fill_rect(595, 25, 28, 15);

      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, black text
        display_set_fg_color(BLACK_COLOR);
      }
      else
      {
        //Active, white text
        display_set_fg_color(WHITE_COLOR);
      }

      //Display the text
      display_text(598, 26, "CH1");
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
      display_fill_rect(595, 25, 28, 15);

      //Check if inactive or active
      if(mode == 0)
      {
        //Inactive, black text
        display_set_fg_color(BLACK_COLOR);
      }
      else
      {
        //Active, white text
        display_set_fg_color(WHITE_COLOR);
      }

      //Display the text
      display_text(598, 26, "CH2");
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_battery_status(void)
{   
  uint32    batterychargelevel;
  uint32    batterycharging; 
  //uint32    xpos = 703;   //position text xxx % battery
  
  batterychargelevel = scopesettings.batterychargelevel;
  batterycharging = scopesettings.batterycharging;
  
  //Prepare the battery symbol in a working buffer to avoid flicker
  display_set_screen_buffer(displaybuffertmp);//1

  //Clear the background
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(701, 5, 25, 13);//700 5 26 13

  //Draw an empty battery symbol in white
  display_set_fg_color(WHITE_COLOR);
  display_fill_rect(701, 9, 2, 4);//700
  display_fill_rect(703, 5, 22, 12);//702 5 23 12

  //Check if there is any charge
  if(batterychargelevel)
  {
    //Keep charge level on max if above
    if(batterychargelevel > 20)
    {
      //Max for displaying the level
      batterychargelevel = 20;
    }

    //Check if charge level is low
    if(batterychargelevel < 4)
    {
      //Draw the level indicator in red
      display_set_fg_color(RED_COLOR);
    }
    else
    {
      //Draw the level indicator in dark green
      display_set_fg_color(DARKGREEN_COLOR);
    }

    //Draw the indicator based on the level
    display_fill_rect(724 - batterychargelevel, 6, batterychargelevel, 10);//723
  }

  //Draw the battery charging indicator when plugged in
  if(batterycharging)
  {
#if 0
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
#else
    //Some orange color
    display_set_fg_color(0x00FF6A00);

    //Draw a lightning bolt when charging
    display_draw_horz_line( 7, 715, 716);
    display_draw_horz_line( 8, 713, 716);
    display_draw_horz_line( 9, 711, 715);
    display_draw_horz_line(10, 709, 715);
    display_draw_horz_line(11, 707, 711);
    display_draw_horz_line(12, 705, 709);
    display_draw_horz_line(11, 713, 715);
    display_draw_horz_line(10, 719, 723);
    display_draw_horz_line(11, 717, 721);
    display_draw_horz_line(12, 713, 719);
    display_draw_horz_line(13, 713, 717);
    display_draw_horz_line(14, 712, 715);
    display_draw_horz_line(15, 712, 713);
#endif
  } 
  /*
  else
  {
      
    if (batterychargelevel < 20)
    {
        //Display the text xx capacity in %
        display_set_fg_color(DARKBLUE_COLOR);
        display_set_font(&font_2);
        display_decimal(708, 5, batterychargelevel*5 );//703 5
    //if (batterychargelevel < 20) xpos += 5;// move text right if <100%
    //display_decimal(xpos, 5, batterychargelevel*6 );//703 5
    }
  }
  */
  //Copy it to the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  display_copy_rect_to_screen(701, 5, 25, 13);//700 5 26 13
}

//----------------------------------------------------------------------------------------------------------------------------------
// Menu functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_main_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(2, 46, 147, 233);//0  149

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(2, 46, 147, 233);//0  149

  //Three black lines between the settings
  display_set_fg_color(BLACK_COLOR);
  display_draw_horz_line(104, 9, 140);
  display_draw_horz_line(163, 9, 140);
  display_draw_horz_line(222, 9, 140);

  //Display the menu items
  scope_main_menu_system_settings(0);
  scope_main_menu_picture_view(0);
  scope_main_menu_waveform_view(0);
  scope_main_menu_usb_connection(0);

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(2, 46, 147, 233, 63039);//3  149
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_main_menu_system_settings(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(YELLOW_COLOR);
  }

  //Draw the background
  display_fill_rect(9, 59, 131, 35);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
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
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
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
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(DARKGREY_COLOR);
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
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and white background
    display_set_fg_color(BLACK_COLOR);
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

void scope_open_channel_menu(PCHANNELSETTINGS settings)
{
  uint32 xstart;
  uint32 xend;
  
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(settings->menuxpos, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(settings->menuxpos, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);

  //Line start and end x positions
  xstart = settings->menuxpos + 14;
  xend   = settings->menuxpos + CH_MENU_WIDTH - 4;//-14

  //Three black lines between the settings
  display_set_fg_color(BLACK_COLOR);
  display_draw_horz_line(CH_MENU_YPOS +  62, xstart, xend);
  display_draw_horz_line(CH_MENU_YPOS + 124, xstart, xend);
  display_draw_horz_line(CH_MENU_YPOS + 188, xstart, xend);
  
  //One black lines between the sensitivity
  display_draw_vert_line(settings->menuxpos + 230,CH_MENU_YPOS+4, CH_MENU_YPOS+248);//10 240
  
  //Set channel color
  display_set_fg_color(settings->color);
  //One canal_color lines the settings and sensitivity
  display_draw_vert_line(settings->menuxpos + 171,CH_MENU_YPOS+4, CH_MENU_YPOS+248);

  //Main texts in white
  display_set_fg_color(WHITE_COLOR);

  //Select the font for the texts
  display_set_font(&font_3);

  //Display the texts
  display_text(settings->menuxpos + 15, CH_MENU_YPOS +  10, "open");
  display_text(settings->menuxpos + 22, CH_MENU_YPOS +  29, "CH");
  display_text(settings->menuxpos + 15, CH_MENU_YPOS +  72, "open");
  display_text(settings->menuxpos + 19, CH_MENU_YPOS +  91, "FFT");
  display_text(settings->menuxpos + 15, CH_MENU_YPOS + 136, "coup");
  display_text(settings->menuxpos + 18, CH_MENU_YPOS + 154, "ling");
  display_text(settings->menuxpos + 15, CH_MENU_YPOS + 201, "probe");
  display_text(settings->menuxpos + 15, CH_MENU_YPOS + 219, "mode");
  
  //clear flag for touch 50% button in channel menu
  triger50 = 0;

  //Display the actual settings
  scope_channel_enable_select(settings);
  scope_channel_fft_show(settings);
  scope_channel_coupling_select(settings);
  scope_channel_probe_magnification_select(settings);
  scope_channel_sensitivity_select(settings);

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(settings->menuxpos, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT, 69906);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_enable_select(PCHANNELSETTINGS settings)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(DARKGREY_COLOR);

  //Check if channel is disabled or enabled
  if(settings->enable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 17, 32, 22);//16
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 17, 32, 22);//16
  }

  //Set channel color for the box behind the selected text
  display_set_fg_color(settings->color);

  //Check if channel is disabled or enabled
  if(settings->enable == 0)
  {
    //Disabled so channel color box behind off
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 17, 32, 22);//16
  }
  else
  {
    //Enabled so channel color box behind on
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 17, 32, 22);//16
  }

  //Check if channel is disabled or enabled
  if(settings->enable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the on text
  display_text(settings->menuxpos + 84, CH_MENU_YPOS + 20, "ON");//19

  //Check if channel is disabled or enabled
  if(settings->enable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(WHITE_COLOR);
  }

  //Display the off text
  display_text(settings->menuxpos + 133, CH_MENU_YPOS + 20, "OFF");//19
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_fft_show(PCHANNELSETTINGS settings)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(DARKGREY_COLOR);

  //Check if fft is disabled or enabled
  if(settings->fftenable == 0)
  {
    //Disabled so dark grey box behind on
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 79, 32, 22);//78
  }
  else
  {
    //Enabled so dark grey box behind off
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 79, 32, 22);//78
  }

  //Set channel color for the box behind the selected text
  display_set_fg_color(settings->color);

  //Check if fft is disabled or enabled
  if(settings->fftenable == 0)
  {
    //Disabled so channel color box behind off
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 79, 32, 22);//78
  }
  else
  {
    //Enabled so channel color box behind on
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 79, 32, 22);//78
  }

  //Check if fft is disabled or enabled
  if(settings->fftenable == 0)
  {
    //Disabled so white on text
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Enabled so black on text
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the on text
  display_text(settings->menuxpos + 84, CH_MENU_YPOS + 82, "ON");//81

  //Check if fft is disabled or enabled
  if(settings->fftenable == 0)
  {
    //Disabled so black off text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //Enabled so white off text
    display_set_fg_color(WHITE_COLOR);
  }

  //Display the off text
  display_text(settings->menuxpos + 133, CH_MENU_YPOS + 82, "OFF");//81
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_coupling_select(PCHANNELSETTINGS settings)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the box behind the not selected text
  display_set_fg_color(DARKGREY_COLOR);

  //Check if coupling is dc or ac
  if(settings->coupling == 0)
  {
    //DC so dark grey box behind ac text
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 142, 32, 22);
  }
  else
  {
    //AC so dark grey box behind dc text
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 142, 32, 22);
  }

  //Set channel color for the box behind the selected text
  display_set_fg_color(settings->color);

  //Check if coupling is dc or ac
  if(settings->coupling == 0)
  {
    //DC so channel color box behind dc text
    display_fill_rect(settings->menuxpos + 78, CH_MENU_YPOS + 142, 32, 22);
  }
  else
  {
    //AC so channel color box behind ac text
    display_fill_rect(settings->menuxpos + 130, CH_MENU_YPOS + 142, 32, 22);
  }

  //Check if coupling is dc or ac
  if(settings->coupling == 0)
  {
    //DC so black DC text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //AC so white or grey DC text
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the DC text
  display_text(settings->menuxpos + 85, CH_MENU_YPOS + 145, "DC");//84

  //Check if coupling is DC or AC
  if(settings->coupling == 0)
  {
    //DC so white or grey AC text
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }
  else
  {
    //AC so black AC text
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the off text
  display_text(settings->menuxpos + 137, CH_MENU_YPOS + 145, "AC");//135
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_probe_magnification_select(PCHANNELSETTINGS settings)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(DARKGREY_COLOR);

  //Check magnification 1x or 10x or 100x
  switch(settings->magnification)
  {
    case 0:
      //dark grey times 10 and 100 magnification
      display_fill_rect(settings->menuxpos + 107, CH_MENU_YPOS + 199, 23, 38);//109
      display_fill_rect(settings->menuxpos + 137, CH_MENU_YPOS + 199, 30, 38);//138
      break;

    case 1:
      //dark grey times 1 and 100 magnification
      display_fill_rect(settings->menuxpos +  77, CH_MENU_YPOS + 199, 20, 38);//78
      display_fill_rect(settings->menuxpos + 137, CH_MENU_YPOS + 199, 30, 38);//138
      break;

    default:
      //dark grey times 1 and 10 magnification
      display_fill_rect(settings->menuxpos +  77, CH_MENU_YPOS + 199, 20, 38);//78
      display_fill_rect(settings->menuxpos + 107, CH_MENU_YPOS + 199, 23, 38);//109
      break;
  }

  //Set channel color for the box behind the selected text
  display_set_fg_color(settings->color);

  //Check if which magnification to highlight
  switch(settings->magnification)
  {
    case 0:
      //Highlight times 1 magnification
      display_fill_rect(settings->menuxpos + 77, CH_MENU_YPOS + 199, 20, 38);//78
      break;

    case 1:
      //Highlight times 10 magnification
      display_fill_rect(settings->menuxpos + 107, CH_MENU_YPOS + 199, 23, 38);//109
      break;

    default:
      //Highlight times 100 magnification
      display_fill_rect(settings->menuxpos + 137, CH_MENU_YPOS + 199, 30, 38);//138
      break;
  }

  //Check if magnification is 1x
  if(settings->magnification == 0)
  {
    //Yes so black 1X text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //No so white or grey 1X text
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the 1X text
  display_text(settings->menuxpos + 84, CH_MENU_YPOS + 201, "1");
  display_text(settings->menuxpos + 83, CH_MENU_YPOS + 219, "X");

  //Check if magnification is 10x
  if(settings->magnification == 1)
  {
    //Yes so black 10X text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //No so white or grey 10X text
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the 10X text
  display_text(settings->menuxpos + 112, CH_MENU_YPOS + 201, "10");
  display_text(settings->menuxpos + 115, CH_MENU_YPOS + 219, "X");

  //Check if magnification is 100x
  if(settings->magnification > 1)
  {
    //Yes so black 100X text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //No so white or grey 100X text
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the 100X text
  display_text(settings->menuxpos + 142, CH_MENU_YPOS + 201, "100");
  display_text(settings->menuxpos + 149, CH_MENU_YPOS + 219, "X");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_channel_sensitivity_select(PCHANNELSETTINGS settings)
{  
  //Set channel color for the box behind the selected text
  display_set_fg_color(settings->color);

  //Check if which displayvoltperdiv==5V to highlight
  if (settings->displayvoltperdiv==0)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(settings->menuxpos + 236, CH_MENU_YPOS + 145, 46, 16);
  
  //Check if which displayvoltperdiv==2.5V to highlight
  if (settings->displayvoltperdiv==1)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(settings->menuxpos + 236, CH_MENU_YPOS + 82, 46, 16);

  //Check if which displayvoltperdiv==1V to highlight
  if (settings->displayvoltperdiv==2)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR);   
  display_fill_rect(settings->menuxpos + 236, CH_MENU_YPOS + 20, 46, 16);

  //Check if which displayvoltperdiv==500mV to highlight
  if (settings->displayvoltperdiv==3)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR);    
  display_fill_rect(settings->menuxpos + 178, CH_MENU_YPOS + 209, 46, 16);

  //Check if which displayvoltperdiv==200mV to highlight
  if (settings->displayvoltperdiv==4)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR);   
  display_fill_rect(settings->menuxpos + 178, CH_MENU_YPOS + 145, 46, 16);

  //Check if which displayvoltperdiv==100mV to highlight
  if (settings->displayvoltperdiv==5)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR); 
  display_fill_rect(settings->menuxpos + 178, CH_MENU_YPOS + 82, 46, 16);

  //Check if which displayvoltperdiv==50mV to highlight
  if (settings->displayvoltperdiv==6)display_set_fg_color(settings->color); else display_set_fg_color(DARKGREY_COLOR); 
  display_fill_rect(settings->menuxpos + 178, CH_MENU_YPOS + 20, 46, 16);

  //Select the font for the texts
  display_set_font(&font_3);

  //Check magnification 1x or 10x or 100x and display text
  switch(settings->magnification)
  {
  case 0:
  if (settings->displayvoltperdiv==6)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 185, CH_MENU_YPOS + 20,  "50mV");
  if (settings->displayvoltperdiv==5)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 180, CH_MENU_YPOS + 82,  "100mV");
  if (settings->displayvoltperdiv==4)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 180, CH_MENU_YPOS + 145, "200mV");
  if (settings->displayvoltperdiv==3)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 180, CH_MENU_YPOS + 209, "500mV");
  if (settings->displayvoltperdiv==2)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 253, CH_MENU_YPOS + 20,  "1V");
  if (settings->displayvoltperdiv==1)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 248, CH_MENU_YPOS + 82,  "2.5V");
  if (settings->displayvoltperdiv==0)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 253, CH_MENU_YPOS + 145, "5V"); break;
  
  case 1:
  if (settings->displayvoltperdiv==6)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);    
  display_text(settings->menuxpos + 180, CH_MENU_YPOS + 20,  "500mV");
  if (settings->displayvoltperdiv==5)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 195, CH_MENU_YPOS + 82,  "1V");
  if (settings->displayvoltperdiv==4)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 195, CH_MENU_YPOS + 145, "2V");
  if (settings->displayvoltperdiv==3)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 195, CH_MENU_YPOS + 209, "5V");
  if (settings->displayvoltperdiv==2)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 249, CH_MENU_YPOS + 20,  "10V");
  if (settings->displayvoltperdiv==1)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 249, CH_MENU_YPOS + 82,  "25V");
  if (settings->displayvoltperdiv==0)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 249, CH_MENU_YPOS + 145, "50V"); break;
  
  case 2:
  if (settings->displayvoltperdiv==6)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);    
  display_text(settings->menuxpos + 195, CH_MENU_YPOS + 20,  "5V");
  if (settings->displayvoltperdiv==5)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 190, CH_MENU_YPOS + 82,  "10V");
  if (settings->displayvoltperdiv==4)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 190, CH_MENU_YPOS + 145, "20V");
  if (settings->displayvoltperdiv==3)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 190, CH_MENU_YPOS + 209, "50V");
  if (settings->displayvoltperdiv==2)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 245, CH_MENU_YPOS + 20,  "100V");
  if (settings->displayvoltperdiv==1)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 245, CH_MENU_YPOS + 82,  "250V");
  if (settings->displayvoltperdiv==0)display_set_fg_color(BLACK_COLOR); else display_set_fg_color(WHITE_COLOR);
  display_text(settings->menuxpos + 245, CH_MENU_YPOS + 145, "500V"); break;
  
  default:    break;
  }
  
  if( triger50 )
    {
    //Set channel color for the box behind the selected text
    display_set_fg_color(settings->color);
    display_fill_rect(settings->menuxpos + 236, CH_MENU_YPOS + 209, 45, 16);
    //Texts in black
    display_set_fg_color(BLACK_COLOR);
    display_text(settings->menuxpos + 250, CH_MENU_YPOS + 209, "50%");
    }
    else      
    {
    //Texts in white
    display_set_fg_color(WHITE_COLOR);
    display_text(settings->menuxpos + 250, CH_MENU_YPOS + 209, "50%");
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_acquisition_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(ACQ_MENU_XPOS, ACQ_MENU_YPOS, ACQ_MENU_WIDTH, ACQ_MENU_HEIGHT);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(ACQ_MENU_XPOS, ACQ_MENU_YPOS, ACQ_MENU_WIDTH, ACQ_MENU_HEIGHT);

  //A black line between the settings
  display_set_fg_color(BLACK_COLOR);
  display_draw_horz_line(ACQ_MENU_YPOS +  158, ACQ_MENU_XPOS + 8, ACQ_MENU_XPOS + ACQ_MENU_WIDTH - 8);

  //Main texts in white
  display_set_fg_color(WHITE_COLOR);

  //Select the font for the texts
  display_set_font(&font_3);

  //Display the texts
  display_text(ACQ_MENU_XPOS + 111, ACQ_MENU_YPOS +   8, "Sample Rate");
  display_text(ACQ_MENU_XPOS +  97, ACQ_MENU_YPOS + 166, "Time per Division");

  //Display the actual settings
  scope_acquisition_speed_select();
  scope_acquisition_timeperdiv_select();

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(ACQ_MENU_XPOS, ACQ_MENU_YPOS, ACQ_MENU_WIDTH, ACQ_MENU_HEIGHT, 69906);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_acquisition_speed_select(void)
{
  uint32 i,x,y;

  //Select the font for the texts
  display_set_font(&font_2);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(0x00383838);

  //Clear the boxes for the not selected items
  for(i=0;i<(sizeof(acquisition_speed_texts) / sizeof(int8 *));i++)
  {
    if(i != scopesettings.samplerate)
    {
      x = ((i & 3) * 72) + 10;
      y = ((i >> 2) * 23) + 33;

      display_fill_rect(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, 68, 20);
    }
  }

  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(TRIGGER_COLOR);

  //Get the position of the selected item
  x = ((scopesettings.samplerate & 3) * 72) + 10;
  y = ((scopesettings.samplerate >> 2) * 23) + 33;

  //Highlight the selected item
  display_fill_rect(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, 68, 20);

  for(i=0;i<(sizeof(acquisition_speed_texts) / sizeof(int8 *));i++)
  {
    if(i != scopesettings.samplerate)
    {
      //Check if in stop mode
      if(scopesettings.runstate)
      {
        //When stopped select option is disabled so texts shown in light grey
        display_set_fg_color(0x00606060);
      }
      else
      {
        //When running available not selected texts shown in white
        display_set_fg_color(WHITE_COLOR);
      }
    }
    else
    {
      //Selected texts in black
      display_set_fg_color(BLACK_COLOR);
    }

    //Calculate the position of this text
    x = ((i & 3) * 72) + acquisition_speed_text_x_offsets[i];
    y = ((i >> 2) * 23) + 36;

    //Display the text from the table
    display_text(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, (int8 *)acquisition_speed_texts[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_acquisition_timeperdiv_select(void)
{
  uint32 c,i,x,y;

  //Select the font for the texts
  display_set_font(&font_2);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(0x00383838);

  //Clear the boxes for the not selected items
  for(i=0;i<(sizeof(time_div_texts) / sizeof(int8 *));i++)
  {
    //Settings displayed from smallest to highest value
    c = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1) - i;

    if(c != scopesettings.timeperdiv)
    {
      x = ((i & 3) * 72) + 10;
      y = ((i >> 2) * 23) + 191;

      display_fill_rect(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, 68, 20);
    }
  }

  //Set channel 2 color for the box behind the selected text
  display_set_fg_color(TRIGGER_COLOR);

  //Get the position of the selected item
  c = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1) - scopesettings.timeperdiv;
  x = ((c & 3) * 72) + 10;
  y = ((c >> 2) * 23) + 191;

  //Highlight the selected item
  display_fill_rect(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, 68, 20);

  for(i=0;i<(sizeof(time_div_texts) / sizeof(int8 *));i++)
  {
    //Settings displayed from smallest to highest value
    c = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1) - i;

    //Check if the current text is the selected on
    if(c != scopesettings.timeperdiv)
    {
      //When not check if the current on is a viable candidate for full screen trace display
      if(viable_time_per_div[scopesettings.samplerate][c])
      {
        //Available but viable not selected texts in white
        display_set_fg_color(WHITE_COLOR);
      }
      else
      {
        //Not viable but available not selected texts in grey
        display_set_fg_color(0x00686868);
      }
    }
    else
    {
      //Selected texts in black
      display_set_fg_color(BLACK_COLOR);
    }

    //Calculate the position of this text
    x = ((i & 3) * 72) + time_div_text_x_offsets[c];
    y = ((i >> 2) * 23) + 194;

    //Display the text from the table
    display_text(ACQ_MENU_XPOS + x, ACQ_MENU_YPOS + y, (int8 *)time_div_texts[c]);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_trigger_menu(void)
{
  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(560, 46, 166, 186);//172

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(560, 46, 166, 186);//172

  //Two black lines between the settings
  display_set_fg_color(BLACK_COLOR);
  display_draw_horz_line(107, 570, 716);//722
  display_draw_horz_line(168, 570, 716);//722

  //Main texts in white
  display_set_fg_color(WHITE_COLOR);

  //Select the font for the texts
  display_set_font(&font_3);

  //Display the texts
  display_text(570,  56, "trigger");
  display_text(570,  75, "mode");
  display_text(570, 118, "trigger");
  display_text(570, 137, "edge");
  display_text(570, 182, "trigger");
  display_text(570, 200, "channel");

  //Display the actual settings
  scope_trigger_mode_select();
  scope_trigger_edge_select();
  scope_trigger_channel_select();

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_top_rect_onto_screen(560, 46, 166, 186, 56415);//172
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_trigger_mode_select(void)
{
  //Select the font for the texts
  display_set_font(&font_3);

  //Set dark grey color for the boxes behind the not selected texts
  display_set_fg_color(DARKGREY_COLOR);

  //Check which trigger mode is selected
  switch(scopesettings.triggermode)
  {
    case 0:                                 //AUTO
      //dark grey single and normal
      display_fill_rect(661, 57, 20, 38);
      display_fill_rect(692, 57, 21, 38);
      break;

    case 1:                                 //SINGLE
      //dark grey auto and normal
      display_fill_rect(629, 57, 20, 38);
      display_fill_rect(692, 57, 21, 38);
      break;

    default:                                //NORMAL
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
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //When not selected check if in normal view mode
    if(scopesettings.waveviewmode == 0)
    {
      //White auto text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey auto text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the auto text
  display_text(631, 58, "au");
  display_text(633, 75, "to");

  //Check if trigger mode is single
  if(scopesettings.triggermode == 1)
  {
    //Yes so black single text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //When not selected check if in normal view mode
    if(scopesettings.waveviewmode == 0)
    {
      //White single text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey single text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the single text
  display_text(666, 56, "si");
  display_text(663, 66, "ng");
  display_text(665, 79, "le");

  //Check if trigger mode is normal
  if(scopesettings.triggermode > 1)
  {
    //Yes so black normal text
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //When not selected check if in normal view mode
    if(scopesettings.waveviewmode == 0)
    {
      //White normal text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey normal text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
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
  display_set_fg_color(DARKGREY_COLOR);

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
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //For falling edge trigger check if in normal view mode
    if(scopesettings.waveviewmode == 0)
    {
      //White text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the rising text
  display_text(629, 127, "rising");

  //Check which trigger edge is selected
  if(scopesettings.triggeredge == 0)
  {
    //For rising edge trigger check if in normal view mode
    if(scopesettings.waveviewmode == 0)
    {
      //White text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }
  else
  {
    //Falling so black falling text
    display_set_fg_color(BLACK_COLOR);
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
  display_set_fg_color(DARKGREY_COLOR);

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
    display_set_fg_color(BLACK_COLOR);
  }
  else
  {
    //2 so white or grey CH1 text
    if((scopesettings.waveviewmode == 0) && scopesettings.channel1.enable)
    {
      //White text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }

  //Display the CH1 text
  display_text(635, 191, "CH1");

  //Check if channel is 1 or 2
  if(scopesettings.triggerchannel == 0)
  {
    //1 so white or grey CH2 text
    if((scopesettings.waveviewmode == 0) && scopesettings.channel2.enable)
    {
      //White text if in normal mode
      display_set_fg_color(WHITE_COLOR);
    }
    else
    {
      //Grey text when in waveform view mode
      display_set_fg_color(0x00606060);
    }
  }
  else
  {
    //2 so black CH2 text
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the CH2 text
  display_text(683, 191, "CH2");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_system_settings_menu(void)
{
  int y;

  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(150, 46, 244, 413);//353

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(150, 46, 244, 413);//353

  //Five black lines between the settings
  display_set_fg_color(BLACK_COLOR);

  for(y=104;y<410;y+=59)//350
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
  scope_system_settings_RTC_settings_item(0);

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(150, 46, 244, 413, 63039);//353
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_screen_brightness_item(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(YELLOW_COLOR);
  }

  //Draw the background
  display_fill_rect(159, 59, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
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
  display_set_fg_color(YELLOW_COLOR);
  display_fill_rect(332, 67, 32, 15);

  //Display the number with fixed width font and black color
  display_set_font(&font_0);
  display_set_fg_color(BLACK_COLOR);
  display_decimal(337, 68, scopesettings.screenbrightness);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_grid_brightness_item(int mode)
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(YELLOW_COLOR);
  }

  //Draw the background
  display_fill_rect(159, 116, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
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
  display_set_fg_color(YELLOW_COLOR);
  display_fill_rect(332, 124, 32, 15);
  
   //Display the number with fixed width font and black color
  display_set_font(&font_0);
  display_set_fg_color(BLACK_COLOR);
  display_decimal(337, 125, scopesettings.gridbrightness);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_trigger_50_item(void)
{
  //Set the colors for white foreground and grey background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

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
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(YELLOW_COLOR);
  }

  //Draw the background
  display_fill_rect(159, 235, 226, 36);

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
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
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

  //Display the icon with the set colors
  display_copy_icon_use_colors(x_y_mode_display_icon, 171, 299, 24, 24);

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
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

  //Display the icon with the set colors
  display_copy_icon_use_colors(confirmation_icon, 171, 358, 24, 24);//356

  //Display the text
  display_set_font(&font_3);
  display_text(217, 354, "Notification");
  display_text(213, 370, "confirmation");

  //Show the state
  scope_display_slide_button(326, 358, scopesettings.confirmationmode);
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_system_settings_RTC_settings_item(int mode)//set
{
  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so dark grey background
    display_set_fg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so yellow background
    display_set_fg_color(YELLOW_COLOR);
  }

  //Draw the background
  display_fill_rect(159, 410, 226, 36);//401

  //Check if inactive or active
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
  }

  //Display the icon with the set colors
  display_copy_icon_use_colors(RTC_icon, 171, 417, 24, 24);

  //Display the text
  display_set_font(&font_3);
  display_text(241, 413, "RTC settings");
  display_text(220, 429, "Time & Date settings");
  
  //Read time for RTC
  get_fattime();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_RTC_settings_menu(void)
{
  int y;

  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(395, 106, 200, 353);//150  46 244 353

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(395, 106, 200, 353);//46 413 353

  //Five black lines between the settings
  display_set_fg_color(BLACK_COLOR);

  for(y=163;y<410;y+=59)//104 350
  {
    display_draw_horz_line(y, 404, 585);//159 385 630
  }

  //Display the menu items
  scope_system_settings_hour_item();
  scope_system_settings_minute_item();
  scope_system_settings_day_item();
  scope_system_settings_month_item();
  scope_system_settings_year_item();
  scope_system_settings_ONOFF_RTC_item();

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(395, 106, 200, 353, 63039);//46 244 353
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_hour_item(void)
{
  //Grey color for inactive button
  display_set_fg_color(GREY_COLOR);
  
  //Draw the body of the button < and >
  display_fill_rounded_rect(415, 115, 35, 35, 2);//739 5 51 50
  display_fill_rounded_rect(540, 115, 35, 35, 2);//739 5 51 50

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(415, 115, 35, 35, 2);
  display_draw_rounded_rect(540, 115, 35, 35, 2);

  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(GREY_COLOR);
  
  //Display the icon with the set colors
  display_copy_icon_use_colors(Left_icon, 423, 120, 24, 24);
  display_copy_icon_use_colors(Right_icon, 544, 120, 24, 24);
  
  //Set the colors for white foreground and gray background
   display_set_bg_color(DARKGREY_COLOR);
   
  //Display the text
  display_set_font(&font_3);
  display_text(476, 119, "Hours");//475

  //Show the state
  scope_system_settings_hour_item_value();
}
void scope_system_settings_hour_item_value(void)
{
  //Draw the gray background
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(488, 136, 32, 15);
  
  //Set the colors for white foreground and gray background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

   //Display the text
  display_set_font(&font_3);
  readnameRTC();
  display_decimal(488, 136, hour);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_minute_item(void)
{
  //Grey color for inactive button
  display_set_fg_color(GREY_COLOR);
  
  //Draw the body of the button < and >
  display_fill_rounded_rect(415, 174, 35, 35, 2);//739 5 51 50
  display_fill_rounded_rect(540, 174, 35, 35, 2);//739 5 51 50

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(415, 174, 35, 35, 2);
  display_draw_rounded_rect(540, 174, 35, 35, 2);

  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(GREY_COLOR);
  
  //Display the icon with the set colors
  display_copy_icon_use_colors(Left_icon, 423, 179, 24, 24);
  display_copy_icon_use_colors(Right_icon, 544, 179, 24, 24);
  
  //Set the colors for white foreground and gray background
   display_set_bg_color(DARKGREY_COLOR);
   
  //Display the text
  display_set_font(&font_3);
  display_text(471, 178, "Minutes");//475

  //Show the state
  scope_system_settings_minute_item_value();
}
void scope_system_settings_minute_item_value(void)
{
  //Draw the gray background
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(488, 195, 32, 15);
  
  //Set the colors for white foreground and gray background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

   //Display the text
  display_set_font(&font_3);
  readnameRTC();
  display_decimal(488, 195, minute);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_day_item(void)
{
  //Grey color for inactive button
  display_set_fg_color(GREY_COLOR);
  
  //Draw the body of the button < and >
  display_fill_rounded_rect(415, 233, 35, 35, 2);//739 5 51 50
  display_fill_rounded_rect(540, 233, 35, 35, 2);//739 5 51 50

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(415, 233, 35, 35, 2);
  display_draw_rounded_rect(540, 233, 35, 35, 2);

  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(GREY_COLOR);
  
  //Display the icon with the set colors
  display_copy_icon_use_colors(Left_icon, 423, 238, 24, 24);
  display_copy_icon_use_colors(Right_icon, 544, 238, 24, 24);
  
  //Set the colors for white foreground and gray background
   display_set_bg_color(DARKGREY_COLOR);
   
  //Display the text
  display_set_font(&font_3);
  display_text(483, 237, "Day");//475

  //Show the state
  scope_system_settings_day_item_value();
}
void scope_system_settings_day_item_value(void)
{
  //Draw the gray background
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(488, 254, 32, 15);
  
  //Set the colors for white foreground and gray background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

   //Display the text
  display_set_font(&font_3);
  readnameRTC();
  display_decimal(488, 254, day);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_month_item(void)
{
  //Grey color for inactive button
  display_set_fg_color(GREY_COLOR);
  
  //Draw the body of the button < and >
  display_fill_rounded_rect(415, 291, 35, 35, 2);//739 5 51 50
  display_fill_rounded_rect(540, 291, 35, 35, 2);//739 5 51 50

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(415, 291, 35, 35, 2);
  display_draw_rounded_rect(540, 291, 35, 35, 2);

  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(GREY_COLOR);
  
  //Display the icon with the set colors
  display_copy_icon_use_colors(Left_icon, 423, 296, 24, 24);
  display_copy_icon_use_colors(Right_icon, 544, 296, 24, 24);
  
  //Set the colors for white foreground and gray background
   display_set_bg_color(DARKGREY_COLOR);
 
  //Display the text
  display_set_font(&font_3);
  display_text(475, 295, "Month");//241

  //Show the state
  scope_system_settings_month_item_value();
}
void scope_system_settings_month_item_value(void)
{
  //Draw the gray background
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(488, 312, 32, 15);
  
  //Set the colors for white foreground and gray background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

   //Display the text
  display_set_font(&font_3);
  readnameRTC();
  display_decimal(488, 312, month);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_year_item(void)
{
  //Grey color for inactive button
  display_set_fg_color(GREY_COLOR);
  
  //Draw the body of the button < and >
  display_fill_rounded_rect(415, 350, 35, 35, 2);//739 5 51 50
  display_fill_rounded_rect(540, 350, 35, 35, 2);//739 5 51 50

  //Draw the edge
  display_set_fg_color(0x00606060);
  display_draw_rounded_rect(415, 350, 35, 35, 2);
  display_draw_rounded_rect(540, 350, 35, 35, 2);

  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(GREY_COLOR);
  
  //Display the icon with the set colors
  display_copy_icon_use_colors(Left_icon, 423, 355, 24, 24);
  display_copy_icon_use_colors(Right_icon, 544, 355, 24, 24);
  
  //Set the colors for white foreground and gray background
  display_set_bg_color(DARKGREY_COLOR);

  //Display the text
  display_set_font(&font_3);
  display_text(481, 354, "Year");//241
  display_text(481, 371, "20");
  //Show the state
  scope_system_settings_year_item_value();
}
void scope_system_settings_year_item_value(void)
{
  //char buffer_time_date[]={'2','0','2','3',0}; 
     //Draw the gray background
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(495, 371, 32, 15);
  
  //Set the colors for white foreground and gray background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

   //Display the text
  display_set_font(&font_3);
  readnameRTC();
  display_decimal(496, 371, year);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_system_settings_ONOFF_RTC_item(void)
{
  //Set the colors for white foreground and grey background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(DARKGREY_COLOR);

  //Display the text
  display_set_font(&font_3);
  display_text(410, 420, "RTC On/Off");//241

  //Show the state
  scope_display_slide_button(530, 419, onoffRTC);
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_open_calibration_start_text(void)
{
  //Save the screen under the baseline calibration start text
  display_set_destination_buffer(displaybuffer3);//2
  display_copy_rect_from_screen(395, 222, 199, 59);

  //Setup the text in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in yellow //white
  display_set_fg_color(YELLOW_COLOR);
  display_fill_rect(395, 222, 199, 59);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(RED_COLOR);
  display_set_font(&font_3);
  display_text(409, 227, "Please unplug");
  display_text(410, 243, "the probe and");
  display_text(424, 259, "USB first !");    //409

  //Add the ok button
  scope_display_ok_button(517, 230, 0);

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_left_rect_onto_screen(395, 222, 199, 59, 63039);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibrating_text(void)
{
  //Restore the screen from under the calibration start text to get rid of it
  display_set_source_buffer(displaybuffer3);//2
  display_copy_rect_to_screen(395, 222, 199, 59);

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 109, 58);//110 59

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 110, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(409, 243, "Calibrating...");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibration_done_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 109, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 110, 59);

  //Display the text in green
  display_set_fg_color(GREEN_COLOR);
  display_set_font(&font_3);
  display_text(414, 233, "Calibration");
  display_text(416, 255, "successful");
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibration_fail(void)
{
  //Draw the background in white
  display_set_fg_color(WHITE_COLOR);
  display_fill_rect(395, 222, 198, 58);//199 59

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in red
  display_set_fg_color(RED_COLOR);
  display_set_font(&font_3);
  
  display_text(420, 233, "Offset calibration failed");
  display_text(445, 255, "Check hardware");
}

//-*********************************************************************************
void scope_show_Input_calibration(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);//199 59

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  //display_text(435, 235, "Input");
  display_text(405, 233, "Input CH1 & CH2");
  display_text(421, 255, "calibration");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

//-----------------------------------------------------

void scope_show_calibrating_300mV_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(413, 233, "Set 300mV DC");//100mV/div 425 -12
  display_text(416, 255, "and press OK");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_show_calibrating_600mV_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(413, 233, "Set 600mV DC");//200mV/div 440
  display_text(416, 255, "and press OK");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_show_calibrating_1_5V_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(420, 233, "Set 1.5V DC");//500mV/div 430
  display_text(416, 255, "and press OK");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
void scope_show_calibrating_3V_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(425, 233, "Set 3V DC");  //3V/div
  display_text(416, 255, "and press OK");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_show_calibrating_7_5V_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(420, 233, "Set 7.5V DC");//2.5V/div 235
  display_text(416, 255, "and press OK");//252
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

void scope_show_calibrating_15V_text(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in white
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(422, 233, "Set 15V DC");//5V/div
  display_text(416, 255, "and press OK");
  
  //Add the ok button
  scope_display_ok_button(517, 230, 0);
}

void scope_show_Input_calibration_done(void)
{
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(395, 222, 198, 58);//199 59

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in green
  display_set_fg_color(GREEN_COLOR);
  display_set_font(&font_3);
  
  display_text(425, 233, "Calibration successful");
  display_set_fg_color(RED_COLOR);
  display_text(455, 255, "Please restart");
}

void scope_show_Input_calibration_fail(void)
{
  //Draw the background in white
  display_set_fg_color(WHITE_COLOR);
  display_fill_rect(395, 222, 198, 58);//199 59

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rect(395, 222, 199, 59);

  //Display the text in red
  display_set_fg_color(RED_COLOR);
  display_set_font(&font_3);
  
  display_text(440, 233, "Calibration failed");//435
  //display_text(470, 255, "Try again");//475
  display_text(437, 255, "PLS Reset values !");//475
}

//-*******************************************************************************
//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_measures_menu(void)
{
  int item;
  int channel;

  //Setup the menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(231, 263, 501, 215);//499

  //Draw the edge in black
  //display_set_fg_color(BLACK_COLOR);
  display_set_fg_color(WHITE_COLOR);
  display_draw_rect(231, 263, 501, 215);//499

  //Three horizontal black lines between the settings y, x, long
  display_set_fg_color(BLACK_COLOR);
  //display_set_fg_color(WHITE_COLOR);
  display_draw_horz_line(296, 232, 730);
  display_draw_horz_line(356, 232, 730);
  display_draw_horz_line(416, 232, 730);
  
  //Channel 1 top bar
  display_set_fg_color(CHANNEL1_COLOR);
  display_fill_rect(232, 264, 248, 31);

  //Channel 2 top bar
  display_set_fg_color(CHANNEL2_COLOR);
  display_fill_rect(482, 264, 248, 31);//246
  
  //If hide measures menu item is active change color Hide bar CH1 & CH2
  scope_hide_measures_menu_item();

  //Vertical separator between the channel sections
  //Draw the edge in white
  //display_set_fg_color(ORANGE_COLOR);
  display_set_fg_color(WHITE_COLOR);
  display_draw_vert_line(481, 264, 476);

  //Vertical separators between the items
  //Draw the edge in black
  display_set_fg_color(BLACK_COLOR);
  display_draw_vert_line(294, 264, 476);//294 289 476
  display_draw_vert_line(356, 264, 476);
  display_draw_vert_line(418, 264, 476);
  display_draw_vert_line(544, 264, 476);
  display_draw_vert_line(606, 264, 476);
  display_draw_vert_line(668, 264, 476);

  //Display the channel identifier text in black
  display_set_fg_color(BLACK_COLOR);
  display_set_font(&font_2);
  display_text(240, 273, "CH1");
  display_text(300, 273, "Select All");
  display_text(375, 273, "Clear");  //373
  
  display_text(490, 273, "CH2");
  display_text(550, 273, "Select All");
  display_text(625, 273, "Clear");  //623

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
  display_set_source_buffer(displaybuffertmp);
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Slide the image onto the actual screen. The speed factor makes it start fast and end slow, Smaller value makes it slower.
  display_slide_right_rect_onto_screen(231, 263, 501, 215, 75646);//449
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_hide_measures_menu_item(void)
{
  //Channel 1 HIDE top bar
  if( scopesettings.hide_values_CH1) display_set_fg_color(RED_COLOR); else display_set_fg_color(CHANNEL1_COLOR);
  display_fill_rect(419, 264, 61, 31);

  //Channel 2 HIDE top bar
  if( scopesettings.hide_values_CH2) display_set_fg_color(RED_COLOR); else display_set_fg_color(CHANNEL2_COLOR);
  display_fill_rect(669, 264, 61, 31);   
  
  //Display text Hide in black
  display_set_fg_color(BLACK_COLOR);
  display_set_font(&font_2);
  display_text(440, 273, "Hide");  
  display_text(690, 273, "Hide");     
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
    //Channel 1 is on the left side
    xpos = 232;
  }
  else
  {
    //Channel 2 is on the right side
    xpos = 482;
  }

  //Set the text and the position for the given item
  switch(item)
  {
    case 0:
      text  = "Vmax";
      xpos += 15;
      ypos  = 316;
      break;

    case 1:
      text  = "Vmin";
      xpos += 79;
      ypos  = 316;
      break;

    case 2:
      text  = "Vavg";
      xpos += 141;
      ypos  = 316;
      break;

    case 3:
      text  = "Vrms";
      xpos += 203;
      ypos  = 316;
      break;

    case 4:
      text  = "Vpp";
      xpos += 19;
      ypos  = 376;
      break;

    case 5:
      text  = "Vp";
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
    display_set_fg_color(GREY_COLOR);
  }
  else
  {
    //On so white text
    display_set_fg_color(WHITE_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(xpos, ypos, text);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_open_slider(uint16 xpos, uint16 ypos, uint8 position)
{
  //Save the screen under the screen brightness slider
  display_set_destination_buffer(displaybuffer3);//2
  display_copy_rect_from_screen(xpos, ypos, 331, 58);

  //Setup the slider menu in a separate buffer to be able to slide it onto the screen
  display_set_screen_buffer(displaybuffertmp);

  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);

  //Fill the background
  display_fill_rect(xpos, ypos, 331, 58);

  //Draw the edge in a lighter grey
  display_set_fg_color(LIGHTGREY_COLOR);

  //Draw the edge
  display_draw_rect(xpos, ypos, 331, 58);

  //Display the actual slider
  scope_display_slider(xpos, ypos, position);

  //Set source and target for getting it on the actual screen
  display_set_source_buffer(displaybuffertmp);
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
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(xpos + 8, ypos + 17, 315, 24);

  //Draw the first part of the slider bar in a yellow color
  display_set_fg_color(YELLOW_COLOR);
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
  display_set_fg_color(BLACK_COLOR);
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
  uint32 edgecolor  = GREY_COLOR;
  uint32 fillcolor  = 0x00888888;

  if(state == 1)
  {
    //Displace the lines and button by 19 pixels
    linex   += 19;
    buttonx += 19;

    //Set the enabled colors
    edgecolor  = 0x00008800;
    fillcolor  = GREEN_COLOR;
  }

  //Draw the background
  display_set_fg_color(fillcolor);
  display_fill_rounded_rect(xpos, ypos, 45, 21, 2);

  //Draw the edge
  display_set_fg_color(edgecolor);
  display_draw_rounded_rect(xpos, ypos, 45, 21, 2);

  //Draw button in dark grey
  display_set_fg_color(GREY_COLOR);
  display_fill_rect(buttonx, ypos + 4, 19, 13);

  //Draw lines in black
  display_set_fg_color(BLACK_COLOR);
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
    display_set_fg_color(LIGHTGREY_COLOR);
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
    display_set_fg_color(WHITE_COLOR);
  }
  else
  {
    //Active so black foreground
    display_set_fg_color(BLACK_COLOR);
  }

  //Display the text
  display_set_font(&font_3);
  display_text(xpos + 24, ypos + 14, "OK");
}

//----------------------------------------------------------------------------------------------------------------------------------
/*
void scope_adjust_timebase(void)
{
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
      }
    }
    //Check if touch on the right of the center line
    else if(previousxtouch > 362)
    {
      //Check if not already on the lowest setting (10nS/div)
      if(scopesettings.timeperdiv < ((sizeof(time_div_texts) / sizeof(int8 *)) - 1))
      {
        //Go down in time by adding one to the setting
        scopesettings.timeperdiv++;
      }
    }

    //For time per div set with tapping on the screen the direct relation between the time per div and the sample rate is set
    //but only when the scope is running. Otherwise the sample rate of the acquired buffer still is valid.
    if(scopesettings.runstate == 0)
    {
      //Set the sample rate that belongs to the selected time per div setting
      scopesettings.samplerate = time_per_div_sample_rate[scopesettings.timeperdiv];
    }

    //Set the new setting in the FPGA
    fpga_set_sample_rate(scopesettings.samplerate);

    //Show he new setting on the display
    scope_acqusition_settings(0);
  }
}
*/
//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_grid(void)
{
  uint32 color;
  register uint32 i;
  
 // /*  
  uint32 kurzor_position;
  
  //Set the color for drawing
  display_set_fg_color(WHITE_COLOR);
  //draw stvorec pre posuvanie buffra
  //Draw the edge
  //display_draw_rect(2, 451, 726, 20);//46 404
  display_draw_rect(2, 452, 726, 20);//46 404
  
  
  kurzor_position = (disp_first_sample/4)+5;
  if (kurzor_position > 686) kurzor_position = 686; //600 ma byt
  //Draw the edge
  //display_draw_rect(kurzor_position, 454, 40, 14);
  display_draw_rect(kurzor_position, 455, 40, 14);
  
  //Set the color for drawing
  display_set_fg_color(YELLOW_COLOR);
  
  //Display the text
  display_set_font(&font_3);//3
  
  //Draw the ticks on the x line
    for(i=4;i<722;i+=5)         //722 krok 5
    {
      //display_draw_vert_line(i, 247, 251);
      display_text(i, 453, "~");    //452
    }
    
    //-********************************************************
  
// */
  //Only draw the grid when something will show (not in the original code)
  if(scopesettings.gridbrightness > 3)
  {
    //Calculate a grey shade based on the grid brightness setting
    color = (scopesettings.gridbrightness * 255) / 100;
    color = (color << 16) | (color << 8) | color;

    //Set the color for drawing
    display_set_fg_color(color);

    //Draw the edge
    display_draw_rect(2, 48, 726, 403);//46 404   44-406

    //Draw the center lines
    display_draw_horz_line(249,  2, 726);
    display_draw_vert_line(364, 49, 449);//44-448

    //Draw the ticks on the x line
    for(i=4;i<726;i+=5)
    {
      display_draw_vert_line(i, 247, 251);
    }

    //Draw the ticks on the y line-ok
    for(i=54;i<448;i+=5)//49 448 5
    {
      display_draw_horz_line(i, 362, 366);
    }

    //Draw the horizontal dots-ok
    for(i=99;i<448;i+=50)
    {
      display_draw_horz_dots(i, 4, 726, 5);
    }

    //Draw the vertical dots-ok
    for(i=14;i<726;i+=50)
    {
      display_draw_vert_dots(i, 54, 448, 5);//49
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
/*
void scope_draw_pointers(void)
{
  int32 position;

  //Draw channel 1 pointer when it is enabled
  if(scopesettings.channel1.enable)
  {
    //Set the colors for drawing
    display_set_fg_color(CHANNEL1_COLOR);
    display_set_bg_color(BLACK_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Check if in normal or x-y display mode
    if(scopesettings.xymodedisplay == 0)
    {
      //y position for the channel 1 trace center pointer.
      position = 441 - scopesettings.channel1.traceposition;

      //Limit on the top of the displayable region
      if(position < 44)
      {
        position = 44;
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
      position = 157 + scopesettings.channel1.traceposition;

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
    position = 441 - scopesettings.channel2.traceposition;

    //Limit on the top of the displayable region
    if(position < 44)
    {
      position = 44;
    }
    //Limit on the bottom of the displayable region
    else if(position > 441)
    {
      position = 441;
    }

    //Set the colors for drawing
    display_set_fg_color(CHANNEL2_COLOR);
    display_set_bg_color(BLACK_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_pointer(2, position, '2');
  }

  //Need to think about trigger position in 200mS - 20mS/div settings. Not sure if they work or need to be done in software
  //The original scope does not show them for 50mS and 20mS/div

  //Draw trigger position and level pointer when in normal display mode
  if(scopesettings.xymodedisplay == 0)
  {
    //Set the colors for drawing
    if(disp_have_trigger == 1) display_set_fg_color(RED_COLOR); 
    else display_set_fg_color(TRIGGER_COLOR);
    display_set_bg_color(BLACK_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_3); 
    
    //x position for the trigger position pointer
    //position = scopesettings.triggerhorizontalposition + 2;
    position = scopesettings.triggerhorizontalposition;

    //Limit on the left of the displayable region
    if(position < 3)//3
    { //Out of limit on the left of the displayable region
      display_right_pointer(2, 45, 'H');
    }
    //Limit on the right of the displayable region
    else if(position > 713)
    {//Out of limit on the right of the displayable region
      display_left_pointer(706, 45, 'H');
    }
    else 
    //Draw the pointer
    display_top_pointer(position, 45, 'H');//47
    
    //-----------------------------------------------------
    //y position for the trigger level pointer
    position = 441 - scopesettings.triggerverticalposition;

    //Limit on the top of the displayable region
    if(position < 44)
    {
      position = 44;
    }
    //Limit on the bottom of the displayable region
    else if(position > 441)
    {
      position = 441;
    }

    //Need to reset the fore ground color
    //Select the channel based on the current trigger color
    if(scopesettings.triggerchannel == 0) display_set_fg_color(CHANNEL1_COLOR);
            else display_set_fg_color(CHANNEL2_COLOR);   
    

    //Draw the pointer
    display_right_pointer(707, position, 'T');
  }
}
*/
//----------------------------------------------------------------------------------------------------------------------------------

void scope_draw_time_cursors(void)
{
  //Only draw the lines when enabled
  if(scopesettings.timecursorsenable)
  {
    //Set the color for the dashed lines
    display_set_fg_color(CURSORS_COLOR);

    //Draw the lines
    display_draw_vert_dashes(scopesettings.timecursor1position, 49, 444, 3, 3);//48 448//46 447
    display_draw_vert_dashes(scopesettings.timecursor2position, 49, 444, 3, 3);//48 448
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
    display_draw_horz_dashes(scopesettings.voltcursor1position, 6, 725, 3, 3);//6 726
    display_draw_horz_dashes(scopesettings.voltcursor2position, 6, 725, 3, 3);//6 726
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
