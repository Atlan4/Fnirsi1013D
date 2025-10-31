//----------------------------------------------------------------------------------------------------------------------------------

//Scan the touch panel

//Main state is displaying the scope data

//On touch the first action is to see where the touch is, so an array of boxes might be needed
//Difficulty lies in when a trace or cursor is selected.

//----------------------------------------------------------------------------------------------------------------------------------

#include "statemachine.h"
#include "touchpanel.h"
#include "timer.h"
#include "fpga_control.h"
#include "sd_card_interface.h"
#include "scope_functions.h"
#include "power_and_battery.h"
#include "display_lib.h"
#include "DS3231.h"
#include "menu.h"
#include "test.h"

#include  "ref_and_math.h"

#include "variables.h"

//----------------------------------------------------------------------------------------------------------------------------------
//touch handler

void touch_handler(void)
{
  //Check on touch state
  if(touchstate == TOUCH_STATE_INACTIVE)
  {
    //No touch yet so scan for it
    scan_for_touch();

    //When movement of object initiated previous settings need to be saved for updating
    if(touchstate)
    {
      //Check if the trigger point position can be moved
      if(touchstate & TOUCH_STATE_MOVE_TRIGGER_POINT)
      {
        //Save the current position for it
        previous_trigger_point_position = scopesettings.triggerhorizontalposition;
      }

      //Save the data for the selected object
      switch(touchstate & TOUCH_STATE_MASK)
      {
        case TOUCH_STATE_X_Y_MODE:
          previous_channel_1_offset = scopesettings.channel1.traceposition;
          previous_channel_2_offset = scopesettings.channel2.traceposition;
          break;

        case TOUCH_STATE_MOVE_CHANNEL_1:
          previous_channel_1_offset = scopesettings.channel1.traceposition;

          //Check if trigger on this channel and if so save it's current offset
          if(scopesettings.triggerchannel == 0)
          {
            previous_trigger_level_offset = scopesettings.triggerverticalposition;
          }
          break;

        case TOUCH_STATE_MOVE_CHANNEL_2:
          previous_channel_2_offset = scopesettings.channel2.traceposition;

          //Check if trigger on this channel and if so save it's current offset
          if(scopesettings.triggerchannel)
          {
            previous_trigger_level_offset = scopesettings.triggerverticalposition;
          }
          break;

        case TOUCH_STATE_MOVE_TRIGGER_LEVEL:
          previous_trigger_level_offset = scopesettings.triggerverticalposition;
          break;

        case TOUCH_STATE_MOVE_TIME_CURSOR_LEFT:
          previous_left_time_cursor_position = scopesettings.timecursor1position;
          //for lock move cursors
          if(scopesettings.lockcursors) previous_right_time_cursor_position = scopesettings.timecursor2position;
          break;

        case TOUCH_STATE_MOVE_TIME_CURSOR_RIGHT:
          previous_right_time_cursor_position = scopesettings.timecursor2position;
          break;

        case TOUCH_STATE_MOVE_VOLT_CURSOR_TOP:
          previous_top_volt_cursor_position = scopesettings.voltcursor1position;
          //for lock move cursors
          //if(scopesettings.lockcursors) previous_bottom_volt_cursor_position = scopesettings.voltcursor2position; 
          break;

        case TOUCH_STATE_MOVE_VOLT_CURSOR_BOTTOM:
          previous_bottom_volt_cursor_position = scopesettings.voltcursor2position;
          //for lock move cursors
          if(scopesettings.lockcursors) previous_top_volt_cursor_position = scopesettings.voltcursor1position;
          break;
          
        case TOUCH_STATE_MOVE_REF1:
          previous_ch_ref1_position = scopesettings.ch_ref1.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF2:
          previous_ch_ref2_position = scopesettings.ch_ref2.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF3:
          previous_ch_ref3_position = scopesettings.ch_ref3.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF4:
          previous_ch_ref4_position = scopesettings.ch_ref4.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF5:
          previous_ch_ref5_position = scopesettings.ch_ref5.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF6:
          previous_ch_ref6_position = scopesettings.ch_ref6.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF7:
          previous_ch_ref7_position = scopesettings.ch_ref7.traceposition;
          break;
          
        case TOUCH_STATE_MOVE_REF8:
          previous_ch_ref8_position = scopesettings.ch_ref8.traceposition;
          break;
      }
    }
  }
  else
  {
    //At this point the movement of the traces, cursors and pointers is handled
    //Read the touch panel status to get a new position or be done
    tp_i2c_read_status();
    
    //When no longer touched reset and quit
    if(havetouch == 0)
    {
      //Signal done with moving
      touchstate = TOUCH_STATE_INACTIVE;

      //That's it for now
      return;
    }

    //Slow things down a bit. This is not a proper solution but will do for now
    //timer0_delay(50);  //before 0.26v4

    //Check if the trigger point position can be moved
    if(touchstate & TOUCH_STATE_MOVE_TRIGGER_POINT)
    {
      //Move the trigger point position
      move_trigger_point_position();
    }

    //Handle the selected object
    switch(touchstate & TOUCH_STATE_MASK)
    {
      case TOUCH_STATE_X_Y_MODE:
        change_channel_1_offset();
        change_channel_2_offset();
        break;

      case TOUCH_STATE_MOVE_CHANNEL_1:
        if(scopesettings.channel1.enable) change_channel_1_offset();

        //Check if trigger on this channel and if so move it accordingly
        if(scopesettings.triggerchannel == 0)
        {
          change_trigger_level_offset();
        }
        break;

      case TOUCH_STATE_MOVE_CHANNEL_2:
        if(scopesettings.channel2.enable) change_channel_2_offset();

        //Check if trigger on this channel and if so move it accordingly
        if(scopesettings.triggerchannel)
        {
          change_trigger_level_offset();
        }
        break;

      case TOUCH_STATE_MOVE_TRIGGER_LEVEL:
        change_trigger_level_offset();
        break;

      case TOUCH_STATE_MOVE_TIME_CURSOR_LEFT:
        move_left_time_cursor_position();
        //for lock cursors
        if(scopesettings.lockcursors) move_right_time_cursor_position();
        break;

      case TOUCH_STATE_MOVE_TIME_CURSOR_RIGHT:
        move_right_time_cursor_position();
        break;

      case TOUCH_STATE_MOVE_VOLT_CURSOR_TOP:
        move_top_volt_cursor_position();
        //for lock cursors
        //if(scopesettings.lockcursors) move_bottom_volt_cursor_position();
        break;

      case TOUCH_STATE_MOVE_VOLT_CURSOR_BOTTOM:
        move_bottom_volt_cursor_position();
        //for lock cursors
        if(scopesettings.lockcursors) move_top_volt_cursor_position();
        break;
        
      case TOUCH_STATE_MOVE_REF1:
        if(scopesettings.ref1) move_ref1_position();
        break;  
        
      case TOUCH_STATE_MOVE_REF2:
        if(scopesettings.ref2) move_ref2_position();
        break;
        
      case TOUCH_STATE_MOVE_REF3:
        if(scopesettings.ref3) move_ref3_position();
        break;
        
      case TOUCH_STATE_MOVE_REF4:
        if(scopesettings.ref4) move_ref4_position();
        break;
        
      case TOUCH_STATE_MOVE_REF5:
        if(scopesettings.ref5) move_ref5_position();
        break;
        
      case TOUCH_STATE_MOVE_REF6:
        if(scopesettings.ref6) move_ref6_position();
        break;
        
      case TOUCH_STATE_MOVE_REF7:
        if(scopesettings.ref7) move_ref7_position();
        break;
        
      case TOUCH_STATE_MOVE_REF8:
        if(scopesettings.ref8) move_ref8_position();
        break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//Top menu bar ranges from 0,0 to 730,46        //44
//Right menu bar ranges from 730,0 to 800,480

void scan_for_touch(void)
{
  uint32 offset;

  //Read the touch panel status
  tp_i2c_read_status();
  
  //Check if the panel is not touched
  if(havetouch == 0)
  {
    //Quit if so
    return;
  }
  
  //scope_test_TP();
  //xtouch_tmp=xtouch;
  //ytouch_tmp=ytouch;

  //Draw directly to screen
  display_set_screen_buffer((uint16 *)maindisplaybuffer);

  //Scan for where the touch is applied
  if((xtouch <= 730) && (ytouch <= 44))//46
  {
    //Top menu bar, so handle that
    //Check if in main menu button range
    if(xtouch <= 80)
    {
      //Check if in normal state
      if(!scopesettings.waveviewmode)
      {
        //Set the button active
        scope_menu_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Set the button inactive
        scope_menu_button(0);

        //Save the screen rectangle where the menu will be displayed
        display_set_destination_buffer(displaybuffer1);//2 0.026p z,menene
        display_copy_rect_from_screen(MAIN_MENU_XPOS, MAIN_MENU_YPOS, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT);//0  149 233

        //Go and setup the channel 1 menu
        scope_open_main_menu();

        //Go and handle the menu touch
        handle_main_menu_touch();

        //Restore the screen when done
        display_set_source_buffer(displaybuffer1);//2
        display_copy_rect_to_screen(MAIN_MENU_XPOS, MAIN_MENU_YPOS, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT);//0  149 233
        
        //set other values and clear view area
        if(scopesettings.long_mode) scope_preset_values();
            //scope_update_cursor_possition(); else scope_preset_values();
      }
      else
      {
        //Set the button active
        scope_main_return_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Switch back to the main menu button and set it inactive
        scope_menu_button(0);

        //Reset the wave view mode back to normal state
        scopesettings.waveviewmode = 0;
      }
    }
    //Check if in channel 1 settings button range
    else if((xtouch >= CH1_BUTTON_XPOS) && (xtouch <= CH1_BUTTON_XPOS + CH_BUTTON_BG_WIDTH))
    {
      //Set the button active
      scope_channel_settings(&scopesettings.channel1, 1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Set the button inactive
      scope_channel_settings(&scopesettings.channel1, 0);

      //Save the screen rectangle where the menu will be displayed
      display_set_destination_buffer(displaybuffer1);//2
      display_copy_rect_from_screen(CH1_MENU_XPOS, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);

      //Go and setup the channel 1 menu
      scope_open_channel_menu(&scopesettings.channel1);
      
      //Signal the channel menu is opened
      channelmenuopen = 1;

      //Go and handle the menu touch
      handle_channel_menu_touch(&scopesettings.channel1);
      
      //Signal the channel menu is close
      //channelmenuopen = 0;

      //Restore the screen when done
      display_set_source_buffer(displaybuffer1);//2
      display_copy_rect_to_screen(CH1_MENU_XPOS, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);
    }
    //Check if in channel 2 settings button range
    else if((xtouch >= CH2_BUTTON_XPOS) && (xtouch <= CH2_BUTTON_XPOS + CH_BUTTON_BG_WIDTH))
    {
      //Set the button active
      scope_channel_settings(&scopesettings.channel2, 1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Set the button inactive
      scope_channel_settings(&scopesettings.channel2, 0);

      //Save the screen rectangle where the menu will be displayed
      display_set_destination_buffer(displaybuffer1);
      display_copy_rect_from_screen(CH2_MENU_XPOS, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);

      //Go and setup the channel 1 menu
      scope_open_channel_menu(&scopesettings.channel2);
      
      //Signal the channel menu is opened
      channelmenuopen = 1;
      
      //Go and handle the menu touch
      handle_channel_menu_touch(&scopesettings.channel2);
      
      //Signal the channel menu is close
      //channelmenuopen = 0;

      //Restore the screen when done
      display_set_source_buffer(displaybuffer1);
      display_copy_rect_to_screen(CH2_MENU_XPOS, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);
    }
    //Check if in acquisition button range & set short time base (long_mode = 0)
    else if(((xtouch >= ACQ_BUTTON_XPOS) && (xtouch <= ACQ_BUTTON_XPOS + ACQ_BUTTON_BG_WIDTH)))// && (!long_mode) )
    {
      //Set the button active
      scope_acqusition_settings(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Set the button inactive
      scope_acqusition_settings(0);

      //Save the screen rectangle where the menu will be displayed
      display_set_destination_buffer(displaybuffer1);
      display_copy_rect_from_screen(ACQ_MENU_XPOS, ACQ_MENU_YPOS, ACQ_MENU_WIDTH, ACQ_MENU_HEIGHT);

      //Go and setup the channel 1 menu
      scope_open_acquisition_menu();

      //Go and handle the menu touch
      handle_acquisition_menu_touch();

      //Restore the screen when done
      display_set_source_buffer(displaybuffer1);
      display_copy_rect_to_screen(ACQ_MENU_XPOS, ACQ_MENU_YPOS, ACQ_MENU_WIDTH, ACQ_MENU_HEIGHT);
    }
    //Check if in move speed button range
    else if((xtouch >= MOVE_FAST_BUTTON_XPOS) && (xtouch <= MOVE_FAST_BUTTON_XPOS+MOVE_FAST_BUTTON_WIDTH))//487 527
    {
      //Set the button active
      scope_move_speed(1);
      
      //if (dev_mode) watchdog_reset_now();


      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Toggle the speed
      scopesettings.movespeed ^= 1;

      //Set the button inactive and show the new state
      scope_move_speed(0);
    }
    //Check if in maximum lignt button range
    else if((xtouch >= MAXLIGHT_BUTTON_XPOS) && (xtouch <= MAXLIGHT_BUTTON_XPOS+MAXLIGHT_BUTTON_WIDTH)) //535 545
    {
      //Set the button active
      scope_maxlight_item(2);
    
      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Toggle the max backlight brightness
      scopesettings.maxlight ^= 1;

      if(scopesettings.maxlight == 0)
      {
        //Set the icon to white sun
        scope_maxlight_item(0);
        //Set backlight brightness to user set value
        scope_set_maxlight(0);
      }
      else
      {  
        //Set the icon to yellow sun
        scope_maxlight_item(1);
        //Set backlight brightness to max
        scope_set_maxlight(1);
      }
    }
    
    //Check if in trigger settings button range
    //else if((xtouch >= 570) && (xtouch <= 700))
    else if((xtouch >= TRIGGER_BUTTON_XPOS) && (xtouch <= TRIGGER_BUTTON_XPOS+TRIGGER_BUTTON_BG_WIDTH))    
    {
      //Set the button active
      scope_trigger_settings(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Set the button inactive
      scope_trigger_settings(0);

      //Save the screen rectangle where the menu will be displayed
      display_set_destination_buffer(displaybuffer1);
      //display_copy_rect_from_screen(560, 46, 172, 246);//186
      display_copy_rect_from_screen(TRIGGER_MENU_XPOS, TRIGGER_MENU_YPOS, TRIGGER_MENU_WIDTH, TRIGGER_MENU_HEIGHT);

      //Go and setup the channel 1 menu
      scope_open_trigger_menu();

      //Go and handle the menu touch
      handle_trigger_menu_touch();

      //Restore the screen when done
      display_set_source_buffer(displaybuffer1);
      //display_copy_rect_to_screen(560, 46, 172, 246);//186
      display_copy_rect_to_screen(TRIGGER_MENU_XPOS, TRIGGER_MENU_YPOS, TRIGGER_MENU_WIDTH, TRIGGER_MENU_HEIGHT);
      
      //Preset screen and values to long time mod
      if(scopesettings.long_mode) scope_preset_values();  
    }
  }
  else if(xtouch > 730)
  {
    //Check if control button is touched
    if((ytouch >= 3) && (ytouch <= 57))
    {
      //Highlight the button if touched
      scope_control_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_control_button(0);

      //Toggle the right menu state
      //scopesettings.rightmenustate ^= 1;
      
      if ((dc_shift_cal)&&(scopesettings.rightmenustate < 2))
      {   scopesettings.rightmenustate = 2; 
      
          //frst run,reset
          if ((scopesettings.channel1.dc_shift_center<1)||(scopesettings.channel1.dc_shift_center>250))
          {
            scopesettings.channel1.dc_shift_center = 174;
            scopesettings.channel1.dc_shift_size   = 220;
            scopesettings.channel1.dc_shift_value  = 385;
    
            scopesettings.channel2.dc_shift_center = 174;
            scopesettings.channel2.dc_shift_size   = 220;
            scopesettings.channel2.dc_shift_value  = 385;
          }
      
      }    
      else {if (scopesettings.rightmenustate < 2) scopesettings.rightmenustate ^= 1; 
                else scopesettings.rightmenustate = 0;}
      
      //if dcshift menu and pres ctrl switch to normal scope menu
      //if (scopesettings.rightmenustate > 1) scopesettings.rightmenustate = 0;
      
      
      //if (scopesettings.rightmenustate > 0) scopesettings.rightmenustate = 0;
        //else scopesettings.rightmenustate = 1;

      //Display the changed state
      scope_setup_right_control_menu();
    }
    //Not then handle the menus based on the selected menu state
    else
    {
      //Right menu bar, so handle it according to the menu state
      if(scopesettings.rightmenustate == 0)
      {
        //Basic control state
        handle_right_basic_menu_touch();
      }
      else
      {
        //Volts per div adjust state
        handle_right_volts_div_menu_touch();
      }
    }
  }
  else
  {
    //Main screen part, so handle that
    //Get timer ticks to detect touch duration
    previoustimerticks = timer0_get_ticks();

    //Set first touch position for movement detection
    previousxtouch = xtouch;
    previousytouch = ytouch;

    //Wait for touch to end and detect movement within the trace display section
    while(havetouch)
    {
      //Scan the touch panel for touch
      tp_i2c_read_status();

      //Check if there still is touch
      if(havetouch)
      {
        //Calculate the absolute x displacement
        if(xtouch < previousxtouch)
        {
          xtouchdisplacement = previousxtouch - xtouch;
        }
        else
        {
          xtouchdisplacement = xtouch - previousxtouch;
        }

        //Calculate the absolute y displacement
        if(ytouch < previousytouch)
        {
          ytouchdisplacement = previousytouch - ytouch;
        }
        else
        {
          ytouchdisplacement = ytouch - previousytouch;
        }

        //Check which of the two is the biggest
        if(xtouchdisplacement > ytouchdisplacement)
        {
          //Use the x displacement
          maxdisplacement = xtouchdisplacement;
        }
        else
        {
          //Use the y displacement
          maxdisplacement = ytouchdisplacement;
        }

        //Check if moved more than 2 pixels
        if(maxdisplacement > 2)
        {
          //Signal a movement of touch
          touchstate = TOUCH_STATE_HAVE_DISPLACEMENT;

          //Go and handle it
          break;
        }
      }
    }

    //Check if no big displacement detected. If there was, touchstate will not be zero
    if(touchstate == 0)
    {
      //See if it was short touch for time base change. Needs to be less then 200mS for that
      if((timer0_get_ticks() - previoustimerticks) < 200)
      {
        //Change the time base setting
        //scope_adjust_timebase();
        scope_adjust_timebase_and_voltdiv();
      }

      //Done for now.
      return;
    }

    //Check if there is displacement that needs to be processed for trace and cursor movement
    if(touchstate)
    {
      //scope_test_TP();
      //Check if display in normal mode
      if(scopesettings.xymodedisplay == 0)
      {
        //Calculate absolute distances for the active traces and cursors
        
        //Check on x higher 60 & below 660 to decide early on which trace to move
        //if(((xtouch > 60)&&(xtouch < 310))||((xtouch > 410)&&(xtouch < 660)))
        if (((scopesettings.voltcursorsenable == 1) && ((xtouch > 60 && xtouch < 310) || (xtouch > 410 && xtouch < 660))) ||
              (scopesettings.voltcursorsenable == 0 && (xtouch > 60 && xtouch < 660))) 
        {
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.channel1.traceposition;

          //Check if touch point below the trace
          if(ytouch < offset)
          {
            //If so take the touch point of the offset for the distance
            distance_channel_1 = offset - ytouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_channel_1 =  ytouch - offset;
          }
        
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.channel2.traceposition;

          //Check if touch point below the trace
          if(ytouch < offset)
          {
            //If so take the touch point of the offset for the distance
            distance_channel_2 = offset - ytouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_channel_2 =  ytouch - offset;
          }
        
          //Check if touch closer to channel 1 center then channel 2 center
          if((distance_channel_1 < 30) && (distance_channel_1 < distance_channel_2))
          {
            //Signal channel 1 trace is being moved and that the trigger point position can also be moved
            touchstate = TOUCH_STATE_MOVE_CHANNEL_1 | TOUCH_STATE_MOVE_TRIGGER_POINT;

            //Go and handle it
            return;
          }
          //Not close to channel 1 or closer to channel 2 so check if close to channel 2
          else if(distance_channel_2 < 30)
          {
            //Signal channel 2 trace is being moved and that the trigger point position can also be moved
            if(scopesettings.channel2.enable) touchstate = TOUCH_STATE_MOVE_CHANNEL_2 | TOUCH_STATE_MOVE_TRIGGER_POINT; 
                else touchstate = TOUCH_STATE_MOVE_CHANNEL_1 | TOUCH_STATE_MOVE_TRIGGER_POINT;

            //Go and handle it
            return;
          }
        }
        //--------------------------------
        //Check on x below 60 to decide early on which trace to move
        if(xtouch < 60)
        {
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref1.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_1 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_1 =  ytouch - offset;
          } 
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref2.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_2 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_2 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref3.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_3 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_3 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref4.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_4 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_4 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref5.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_5 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_5 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref6.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_6 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_6 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref7.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
          distance_ch_ref_7 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
          distance_ch_ref_7 =  ytouch - offset;
          }
          //Need to make the trace offset in the same orientation as the touch
          offset = 449 - scopesettings.ch_ref8.traceposition;
          //Check if touch point below the trace
          if(ytouch < offset)
          { //If so take the touch point of the offset for the distance
            distance_ch_ref_8 = offset - ytouch;
          }else
          {  //Otherwise take the offset of the touch point for the distance
            distance_ch_ref_8 =  ytouch - offset;
          }

               if((distance_ch_ref_1 < 30)&&(scopesettings.ref1)) { touchstate = TOUCH_STATE_MOVE_REF1; return; }
          else if((distance_ch_ref_2 < 30)&&(scopesettings.ref2)) { touchstate = TOUCH_STATE_MOVE_REF2; return; }
          else if((distance_ch_ref_3 < 30)&&(scopesettings.ref3)) { touchstate = TOUCH_STATE_MOVE_REF3; return; }
          else if((distance_ch_ref_4 < 30)&&(scopesettings.ref4)) { touchstate = TOUCH_STATE_MOVE_REF4; return; }
          else if((distance_ch_ref_5 < 30)&&(scopesettings.ref5)) { touchstate = TOUCH_STATE_MOVE_REF5; return; }
          else if((distance_ch_ref_6 < 30)&&(scopesettings.ref6)) { touchstate = TOUCH_STATE_MOVE_REF6; return; }
          else if((distance_ch_ref_7 < 30)&&(scopesettings.ref7)) { touchstate = TOUCH_STATE_MOVE_REF7; return; }
          else if((distance_ch_ref_8 < 30)&&(scopesettings.ref8)) { touchstate = TOUCH_STATE_MOVE_REF8; return; }
        }

        //This bit needs to be skipped for time base 200mS/div - 20mS/div
        //Hardware trigger seems to be available only from 10mS/div - 10nS/div

        //Check on x above 660 to decide early on move of trigger level offset
        if(xtouch > 660)
        {
          //Need to make the trigger level offset in the same orientation as the touch
          offset = 448 - scopesettings.triggerverticalposition;

          //Check if touch point below the trace
          if(ytouch < offset)
          {
            //If so take the touch point of the offset for the distance
            distance_trigger_level = offset - ytouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_trigger_level =  ytouch - offset;
          }

          //Check if close to trigger level pointer
          if(distance_trigger_level < 30)
          {
            //Signal trigger level is being moved
            touchstate = TOUCH_STATE_MOVE_TRIGGER_LEVEL;

            //Go and handle it
            return;
          }
        }

        //Check if time cursor is enabled
        if(scopesettings.timecursorsenable)
        {
          //Check if touch point left of the left cursor
          if(xtouch < scopesettings.timecursor1position)
          {
            //If so take the touch point of the offset for the distance
            distance_time_cursor_left = scopesettings.timecursor1position - xtouch;
            //for lock move cursor
            if(scopesettings.lockcursors) distance_time_cursor_right = scopesettings.timecursor2position - xtouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_time_cursor_left =  xtouch - scopesettings.timecursor1position;
            //for lock move cursor
            if(scopesettings.lockcursors) distance_time_cursor_right =  xtouch - scopesettings.timecursor2position;
          }

          //Check if touch point left of the right cursor
          if(xtouch < scopesettings.timecursor2position)
          {
            //If so take the touch point of the offset for the distance
            distance_time_cursor_right = scopesettings.timecursor2position - xtouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_time_cursor_right =  xtouch - scopesettings.timecursor2position;
          }

          //Check if touch closer to left cursor then right cursor
          if((distance_time_cursor_left < 30) && (distance_time_cursor_left < distance_time_cursor_right))
          {
            //Signal left time cursor is being moved
            touchstate = TOUCH_STATE_MOVE_TIME_CURSOR_LEFT;
            
            //Go and handle it
            return;
          }
          //Not close to left cursor or closer to right cursor so check if close to right cursor
          else if(distance_time_cursor_right < 30)
          {
            //Signal right time cursor is being moved
            touchstate = TOUCH_STATE_MOVE_TIME_CURSOR_RIGHT;
            
            //Go and handle it
            return;
          }
        }

        //Check if volt cursor is enabled
        if((scopesettings.voltcursorsenable)&&(xtouch > 60))
        {
          //Check if touch point under of the bottom cursor
          //if(previousytouch < scopesettings.voltcursor2position)
          if(ytouch < scopesettings.voltcursor2position)
          {
            //If so take the touch point of the offset for the distance
            distance_volt_cursor_bottom = scopesettings.voltcursor2position - ytouch;
            //for lock move cursor
            if(scopesettings.lockcursors) distance_volt_cursor_top = scopesettings.voltcursor1position - ytouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_volt_cursor_bottom =  ytouch - scopesettings.voltcursor2position;
            //for lock move cursor
            if(scopesettings.lockcursors) distance_volt_cursor_top =  ytouch - scopesettings.voltcursor1position;
          }
          
          //Check if touch point under of the top cursor
          if(ytouch < scopesettings.voltcursor1position)
          {
            //If so take the touch point of the offset for the distance
            distance_volt_cursor_top = scopesettings.voltcursor1position - ytouch;
            //for lock move cursor
            //if(scopesettings.lockcursors) distance_volt_cursor_bottom = scopesettings.voltcursor2position - ytouch;
          }
          else
          {
            //Otherwise take the offset of the touch point for the distance
            distance_volt_cursor_top =  ytouch - scopesettings.voltcursor1position;
            //for lock move cursor
            //if(scopesettings.lockcursors) distance_volt_cursor_bottom =  ytouch - scopesettings.voltcursor2position;
          }

          //Check if touch closer to top cursor then bottom cursor
          if((distance_volt_cursor_top < 30) && (distance_volt_cursor_top < distance_volt_cursor_bottom))
          {
            //Signal top volt cursor is being moved
            touchstate = TOUCH_STATE_MOVE_VOLT_CURSOR_TOP;

            //Go and handle it
            return;
          }
          //Not close to top cursor or closer to bottom cursor so check if close to bottom cursor
          else if(distance_volt_cursor_bottom < 30)//30
          {
            //Signal bottom volt cursor is being moved
            touchstate = TOUCH_STATE_MOVE_VOLT_CURSOR_BOTTOM;

            //Go and handle it
            return;
          }
        }

        //Down to here for the skip

        //No other object selected so far then check again on the signal traces
        //When channel 1 or channel2 or only x movement signal that the trigger point can also be moved.
        touchstate = TOUCH_STATE_MOVE_TRIGGER_POINT;
        /*
        //Check if touch closer to channel 1 center then channel 2 center
        if((distance_channel_1 < 30) && (distance_channel_1 < distance_channel_2))
        {
          //Signal channel 1 trace is being moved
          touchstate |= TOUCH_STATE_MOVE_CHANNEL_1;
        }
        //Not close to channel 1 or closer to channel 2 so check if close to channel 2
        else if(distance_channel_2 < 30)
        {
          //Signal channel 2 trace is being moved
          touchstate |= TOUCH_STATE_MOVE_CHANNEL_2;
        }
        */
      }
      else
      {
        //Display in x-y mode, so signal this in touchstate
        touchstate = TOUCH_STATE_X_Y_MODE;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------------------

void handle_main_menu_touch(void)
{
  uint8 tmp;                            //for set boot mode
  
  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();

    //Check if there is touch
    if(havetouch)
    {
      //Check if touch within the menu field
      if((xtouch >= MAIN_MENU_XPOS) && (xtouch <= MAIN_MENU_XPOS + MAIN_MENU_WIDTH) && (ytouch >= MAIN_MENU_YPOS) && (ytouch <= MAIN_MENU_YPOS + MAIN_MENU_HEIGHT))
      {
        //Check if on system settings
        if((ytouch >= 46) && (ytouch <= 105))
        {
          //Check if already open
          if(systemsettingsmenuopen == 0)
          {
            //Set the button active
            scope_main_menu_system_settings(1);
            //Set the button inactive
            //scope_main_menu_diagnostic_view(0);
            

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();

            //Save the screen under the menu
            display_set_destination_buffer(displaybuffer2);
            display_copy_rect_from_screen(SYSTEM_MENU_XPOS, SYSTEM_MENU_YPOS, SYSTEM_MENU_WIDTH, SYSTEM_MENU_HEIGHT);//353

            //Show the system settings menu
            scope_open_system_settings_menu();

            //Signal the system settings menu is opened
            systemsettingsmenuopen = 1;
          }
        }
        //Check if on picture view
        else if((ytouch >= 107) && (ytouch <= 164))
        {
          //Close any other open menu
          close_open_menus(1);

          //Set the button active
          scope_main_menu_picture_view(1);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();

          //Signal viewing of pictures
          viewtype = VIEW_TYPE_PICTURE;

          //Switch to picture view screen and stay there until return is pressed
          scope_setup_view_screen();

          //Need to exit the loop here
          return;
        }
        //Check if on waveform view
        else if((ytouch >= 166) && (ytouch <= 223))//+57
        {
          //Close any other open menu
          close_open_menus(1);

          //Set the button active
          scope_main_menu_waveform_view(1);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();

          //Signal viewing of pictures
          viewtype = VIEW_TYPE_WAVEFORM;

          //Switch to waveform view screen and stay there until return is pressed
          scope_setup_view_screen();

          //Need to exit the loop here
          return;
        }
        //Check if on diagnostic_screen
        else if((ytouch >= 225) && (ytouch <= 282))//278
        {
          //Close any other open menu
          close_open_menus(1);
            
          //Signal the system settings menu is closed
          //systemsettingsmenuopen = 0;
          
          //Set the button active
          scope_main_menu_diagnostic_view(1);
          //Set the button inactive
          //scope_main_menu_system_settings(0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
 
          //Restore the main screen before entering the diagnostic_screen
          display_set_source_buffer(displaybuffer1);//2 0.26p
          //display_copy_rect_to_screen(2, 46, 149, 292);//149 233
          display_copy_rect_to_screen(MAIN_MENU_XPOS, MAIN_MENU_YPOS, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT);//149 233

          //And make a copy of it to return to
          display_set_destination_buffer(displaybuffer2);
          display_copy_rect_from_screen(0, 0, 800, 480);

          //Show the scope diagnostic screen
          scope_open_diagnostic_view();
            
          handle_diagnostic_view_touch();
            
          //Copy back the original screen to continue were left of
          display_set_source_buffer(displaybuffer2);
          display_copy_rect_to_screen(0, 0, 800, 480);
  
          //Need to exit the loop here
          return;  
          
        }
        //Check if on USB connection
        else if((ytouch >= 284) && (ytouch <= 341))
        {
          //Close any other open menu
          close_open_menus(1);

          //Set the button active
          scope_main_menu_usb_connection(1);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();

          //Restore the main screen before entering the USB screen
          display_set_source_buffer(displaybuffer1);//2 0.026p
          //display_copy_rect_to_screen(2, 46, 149, 292);//149 233
          display_copy_rect_to_screen(MAIN_MENU_XPOS, MAIN_MENU_YPOS, MAIN_MENU_WIDTH, MAIN_MENU_HEIGHT);//149 233


          //And make a copy of it to return to
          display_set_destination_buffer(displaybuffer2);
          display_copy_rect_from_screen(0, 0, 800, 480);

          //Show and handle the usb connection here
          scope_setup_usb_screen();

          //Copy back the original screen to continue were left of
          display_set_source_buffer(displaybuffer2);
          display_copy_rect_to_screen(0, 0, 800, 480);

          //Need to exit the loop here
          return;
        }
      }
      
      //**--------------------------------------------------------
      
      //Check on system settings menu opened and being touched
      else if(systemsettingsmenuopen && (xtouch >= 150) && (xtouch <= 394) && (ytouch >= 46) && (ytouch <= 470))//399 //458
      {
        //Check if on screen brightness
        if((ytouch >= 47) && (ytouch <= 103))
        {
          //Check if already open
          if(screenbrightnessopen == 0)
          {
            //Close any of the sub menus if open
            close_open_menus(0);

            //Set this item active
            scope_system_settings_screen_brightness_item(1);

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();

            //Show the screen brightness slider
            scope_open_slider(395, 46, scopesettings.screenbrightness);

            //Signal the screen brightness slider is opened
            screenbrightnessopen = 1;
          }
        }
        //Check if on grid brightness
        else if((ytouch >= 105) && (ytouch <= 162))
        {
          //Check if already open
          if(gridbrightnessopen == 0)
          {
            //Close any of the sub menus if open
            close_open_menus(0);

            //Set this item active
            scope_system_settings_grid_brightness_item(1);

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();

            //Show the screen brightness slider
            scope_open_slider(395, 104, scopesettings.gridbrightness);

            //Signal the screen brightness slider is opened
            gridbrightnessopen = 1;
          }
        }
        //Check if on OTHER SETINGS
        else if((ytouch >= 164) && (ytouch <= 221))
        {
          //Close any of the sub menus if open
          //close_open_menus(0);

          //Wait until touch is released
          //tp_i2c_wait_for_touch_release();

          //Check if already open
          if(othersettingsopen == 0)
          {
            //Close any of the sub menus if open
            close_open_menus(0);
            
            //Set this item active
            scope_system_settings_other_settings_item(1);
 
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Save the screen under the menu
            display_set_destination_buffer(displaybuffer3);
            display_copy_rect_from_screen(OTHER_MENU_XPOS, OTHER_MENU_YPOS, OTHER_MENU_WIDTH, OTHER_MENU_HEIGHT);
            
            //Show the other settings menu
            scope_system_settings_open_other_settings_menu();

            //Signal the menu other_settings is opened
            othersettingsopen = 1;
          }
        }
        //Check if on baseline calibration
        else if((ytouch >= 223) && (ytouch <= 280))
        {
          //Check if already open
          if(calibrationopen == 0)
          {
            //Close any of the sub menus if open
            close_open_menus(0);

            //Set this item active
            scope_system_settings_calibration_item(1);

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();

            //Show the start text
            scope_open_calibration_start_text();

            //Signal the calibration text is opened
            calibrationopen = 1;
          }
        }
        //Check if on x-y mode display
        else if((ytouch >= 282) && (ytouch <= 339))
        {
          //Close any of the sub menus if open
          close_open_menus(0);

          //Toggle the x-y mode display state
          scopesettings.xymodedisplay ^= 1;

          //Show the state
          scope_system_settings_x_y_mode_item(1);
          //scope_display_slide_button(326, 299, scopesettings.xymodedisplay, MAGENTA_COLOR);
         
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        }
        //check on notification confirmation
        else if((ytouch >= 341) && (ytouch <= 398))
        {
          //Close any of the sub menus if open
          close_open_menus(0);

          //Toggle the notification confirmation mode
          scopesettings.confirmationmode ^= 1;

          //Show the state
          scope_system_settings_confirmation_item(1);
          //scope_display_slide_button(326, 358, scopesettings.confirmationmode, GREEN_COLOR);
          
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        }
        //check on RTC settings
        else if((ytouch >= 405) && (ytouch <= 470))
        {
          //Check if already open
          if(RTCsettingsopen == 0)
          {
            //Close any of the sub menus if open
            close_open_menus(0);
            
            //Set this item active
            scope_system_settings_RTC_settings_item(1);

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Save the screen under the menu
            display_set_destination_buffer(displaybuffer3);
            display_copy_rect_from_screen(RTC_MENU_XPOS, RTC_MENU_YPOS, RTC_MENU_WIDTH, RTC_MENU_HEIGHT);
            
            //Show the RTC settings menu
            scope_open_RTC_settings_menu();

            //Signal the menu RTC_settings is opened
            RTCsettingsopen = 1;
          }
        }
      }
      //****----------------------------------------------------------------------      
            //****----------------------------------------------------------------------      
      else if(othersettingsopen && (xtouch >= OTHER_MENU_XPOS) && (xtouch <= OTHER_MENU_XPOS + OTHER_MENU_WIDTH) && (ytouch >= OTHER_MENU_YPOS) && (ytouch <= OTHER_MENU_YPOS + OTHER_MENU_HEIGHT+15))
      {
        //Check if touch button Boot menu
        if((ytouch >= OTHER_MENU_YPOS) && (ytouch <= OTHER_MENU_YPOS + 46))//+59
        {
          //Toggle the Boot_menu
          tmp = (boot_menu_start & 0x04)>>2;
          tmp ^= 1;
          boot_menu_start &= 0x03;
          boot_menu_start |= tmp<<2;
            
          //Set this item active
          scope_system_settings_other_settings_boot_menu_item();
          //scope_system_settings_other_settings_default_start_item(0);
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch default start
        else if((ytouch >= OTHER_MENU_YPOS + 46) && (ytouch <= OTHER_MENU_YPOS + 92))//59-118
        {    
          //Toggle the Default start
          tmp = (boot_menu_start & 0x03);
          if (tmp < 2) tmp++; else tmp = 0;  
          boot_menu_start &= 0x04;
          boot_menu_start |= tmp;
                  
          //Set this item active
          scope_system_settings_other_settings_default_start_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        } 
        //Check if touch button NEWAUTOSETUP 
        else if((ytouch >= OTHER_MENU_YPOS + 92) && (ytouch <= OTHER_MENU_YPOS + 138))//118-177
        {    
          //Toggle the new_autosetup
          scopesettings.new_autosetup ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_new_autosetup_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }    
        //Check if touch button lock cursors
        else if((ytouch >= OTHER_MENU_YPOS + 138) && (ytouch <= OTHER_MENU_YPOS + 184))
        {
          //Toggle the lock cursors
          scopesettings.lockcursors ^= 1;
                  
          //Set this item active
          scope_system_settings_other_settings_lock_cursor_item();
          
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        }
        //Check if touch button DC shift calibration
        else if((ytouch >= OTHER_MENU_YPOS + 184) && (ytouch <= OTHER_MENU_YPOS + 230))
        {    
          //Toggle the shift_cal
          dc_shift_cal ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_dc_shift_cal_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch button USB CH340 mode
        else if((ytouch >= OTHER_MENU_YPOS + 230) && (ytouch <= OTHER_MENU_YPOS + 276))
        {    
          //Toggle the USB_CH340
          USB_CH340 ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_CH340_mode_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch button developer mode
        else if((ytouch >= OTHER_MENU_YPOS + 276) && (ytouch <= OTHER_MENU_YPOS + 322))
        {    
          //Toggle the dev_mode
          dev_mode ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_dev_mode_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch button Add a tag to BMP
        else if((ytouch >= OTHER_MENU_YPOS + 322) && (ytouch <= OTHER_MENU_YPOS + 368))
        {    
          //Toggle the tag_in_BMP
          tag_in_BMP ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_BMP_mark_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch button ref_on_start 
        else if((ytouch >= OTHER_MENU_YPOS + 368) && (ytouch <= OTHER_MENU_YPOS + 425))//413
        {    
          //Toggle the ref_on_start
          scopesettings.ref_on_startup ^= 1;
          
          //Set this item active
          scope_system_settings_other_settings_ref_on_start_item();
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
        }
      }
        //------------------------------------------------------------------------------------------------
      else if(RTCsettingsopen && (xtouch >= 395) && (xtouch <= 595) && (ytouch >= 106) && (ytouch <= 470))
      {
        //Check if on RTC is pushed
        if((ytouch >= 405) && (ytouch <= 470))
        {
          //Toggle the ON/OFF RTC state
          onoffRTC ^= 1;
              
          if (onoffRTC)
          { //Read time for RTC
            get_fattime();    
            //Show the state
            scope_system_settings_hour_item_value(); 
            scope_system_settings_minute_item_value(); 
            scope_system_settings_day_item_value(); 
            scope_system_settings_month_item_value(); 
            scope_system_settings_year_item_value(); 
          } else       
              {//Clear TIME on display
                display_set_fg_color(BLACK_COLOR);
                //display_fill_rect(647, 5, 50, 13);
                display_fill_rect(TIME_TEXT_XPOS, TIME_TEXT_YPOS, TIME_TEXT_WIDTH, TIME_TEXT_HEIGHT);
              }

          //Show the state
          scope_display_slide_button(530, 419, onoffRTC, GREEN_COLOR); 
        }
        
        //Check if year (<) being touched
        if((xtouch >= 420) && (xtouch <= 450) && (ytouch >= 354) && (ytouch <= 378))
        {       
          //Decrement year and sent to RTC
          yearDown();

          //Show the state
          scope_system_settings_year_item_value(); 
        }
        
        //Check if year (>) being touched
        if((xtouch >= 540) && (xtouch <= 572) && (ytouch >= 354) && (ytouch <= 378))
        {
          //Increment year and sent to RTC
          yearUp();
                    
          //Show the state
          scope_system_settings_year_item_value();
        }
        
        //Check if month (<) being touched
        if((xtouch >= 420) && (xtouch <= 450) && (ytouch >= 295) && (ytouch <= 319))
        {
          //Decrement month and sent to RTC
          monthDown();

          //Show the state
          scope_system_settings_month_item_value(); 
        }
        
        //Check if month (>) being touched
        if((xtouch >= 540) && (xtouch <= 572) && (ytouch >= 295) && (ytouch <= 319))
        {
          //Increment month and sent to RTC
          monthUp();
                    
          //Show the state
          scope_system_settings_month_item_value();
        }
        //Check if day (<) being touched
        if((xtouch >= 420) && (xtouch <= 450) && (ytouch >= 237) && (ytouch <= 261))
        {
          //Decrement day and sent to RTC
          dayDown();

          //Show the state
          scope_system_settings_day_item_value(); 
        }
        
        //Check if day (>) being touched
        if((xtouch >= 540) && (xtouch <= 572) && (ytouch >= 237) && (ytouch <= 261))
        {
          //Increment day and sent to RTC
          dayUp();
                    
          //Show the state
          scope_system_settings_day_item_value();
        }
        
        //Check if minutes (<) being touched
        if((xtouch >= 420) && (xtouch <= 450) && (ytouch >= 178) && (ytouch <= 202))
        {
          //Decrement minutes and sent to RTC
          minuteDown();

          //Show the state
          scope_system_settings_minute_item_value(); 
        }
        
        //Check if minutes (>) being touched
        if((xtouch >= 540) && (xtouch <= 572) && (ytouch >= 178) && (ytouch <= 202))
        {
          //Increment minutes and sent to RTC
          minuteUp();
                    
          //Show the state
          scope_system_settings_minute_item_value();
        }
        
        //Check if hours (<) being touched
        if((xtouch >= 420) && (xtouch <= 450) && (ytouch >= 119) && (ytouch <= 143))
        {
          //Decrement hours and sent to RTC
          hourDown();

          //Show the state
          scope_system_settings_hour_item_value(); 
        }
        
        //Check if hours (>) being touched
        if((xtouch >= 540) && (xtouch <= 572) && (ytouch >= 119) && (ytouch <= 143))
        {
          //Increment hours and sent to RTC
          hourUp();
                    
          //Show the state
          scope_system_settings_hour_item_value();
        }
        
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();
      }
        
       //*********************************************************************************************************
      //--------------------------------------------------------------------------------------------------------
      //Check on screen brightness slider opened and being touched
      else if(screenbrightnessopen && (xtouch >= 395) && (xtouch <= 726) && (ytouch >= 46) && (ytouch <= 104))
      {
        //Move the slider to a new position and check if there was a change in position
        if(scope_move_slider(395, 46, &scopesettings.screenbrightness))
        {
          //Update the setting in the system settings menu
          scope_system_settings_screen_brightness_value();

          //Update the actual screen brightness
          fpga_set_translated_brightness();
        }
      }
      //Check on grid brightness slider opened and being touched
      else if(gridbrightnessopen && (xtouch >= 395) && (xtouch <= 726) && (ytouch >= 105) && (ytouch <= 163))
      {
        //Move the slider to a new position and check if there was a change in position
        if(scope_move_slider(395, 104, &scopesettings.gridbrightness))
        {
          //Update the setting in the system settings menu
          scope_system_settings_grid_brightness_value();

          //Update the actual grid brightness if needed. There is no direct update of the background!
        }
      }
      //Check on calibration start text opened and being touched
      else if((calibrationopen == 1) && (xtouch >= 395) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
      {
        //Check if touch is on the button
        if((xtouch >= 517) && (xtouch <= 583) && (ytouch >= 230) && (ytouch <= 272))
        {
          //Highlight the button
          scope_display_ok_button(517, 230, 1);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();

          //Show the baseline calibration active text
          scope_show_calibrating_text();
          ///*
          //Start the calibration process
          if(scope_do_baseline_calibration() == 0)//docasne vypnute
            {
            //if (dev_mode) display_decimal(500, 400, failflag);
            //Show the Offset calibration FAILED text
            scope_show_calibration_fail();
            //Cancel Input calibration
            calibrationfail = 0;
            timer0_delay(1500);
            } 
            else
          //*/  //Show the calibration successful text if all went well
            scope_show_calibration_done_text();   

          //Signal calibration has been done
          calibrationopen = 2;
                    
          //Close menu if confirmation mode is inactive & calibration is OK, delay 700ms
          if((!scopesettings.confirmationmode)&&(!calibrationfail)) {timer0_delay(700);close_open_menus(1);return;}
          
          timer0_delay(1000);   //700
          //Show the Input calibration text 
          if (calibrationfail) scope_show_Input_calibration();  
        }   
      }
      //-********************************************************************************************************
      //Check on Input_calibration text opened and OK touched
      else if((calibrationfail) && (calibrationopen == 2) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
      {
          //Highlight the button
          scope_display_ok_button(517, 230, 1);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
            
          //Show the calibration text 300mV
          scope_show_calibrating_300mV_text();

          //Signal calibration 300mV
          calibrationopen = 3; 
      }
      
      //Check on calibrating_300mV text opened and OK touched
      else if((calibrationopen == 3) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
      { 
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //GO calibration 300mV for settings calibrationopen = 3;
            scope_input_calibration();

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Show the calibration text 300mV
            if (!calibrationfail) scope_show_calibrating_600mV_text();
      }
      
      //Check on calibrating_600mV text opened and OK touched
      else if((!calibrationfail) && (calibrationopen == 4) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
            {
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //if no fail GO calibration 600mV for settings calibrationopen = 4;
            scope_input_calibration();

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Show the calibration text 300mV
            if (!calibrationfail) scope_show_calibrating_1_5V_text();
            }
      
      //Check on calibrating_1.5V text opened and OK touched
      else if((!calibrationfail) && (calibrationopen == 5) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
            {
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //if no fail GO calibration 1.5V for settings calibrationopen = 5;
            scope_input_calibration();

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Show the calibration text 300mV
            if (!calibrationfail) scope_show_calibrating_3V_text();
            }
      
      //Check on calibrating 3V text opened and OK touched
      else if((!calibrationfail) && (calibrationopen == 6) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
            {
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //if no fail GO calibration 3V for settings calibrationopen = 6;
            scope_input_calibration();

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Show the calibration text 300mV
            if (!calibrationfail) scope_show_calibrating_7_5V_text();
            }
      
      //Check on calibrating 7.5V text opened and OK touched
      else if((!calibrationfail) && (calibrationopen == 7) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
            {
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //if no fail GO calibration 7.5V for settings calibrationopen = 7;
            scope_input_calibration();

            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            //Show the calibration text 15V
            if (!calibrationfail) scope_show_calibrating_15V_text();
            }
      
      //Check on calibrating 15V text opened and OK touched
      else if((!calibrationfail) && (calibrationopen == 8) && (xtouch >= 495) && (xtouch <= 594) && (ytouch >= 223) && (ytouch <= 280))
            {
            //Highlight the button
            scope_display_ok_button(517, 230, 1);
            
            //if no fail GO calibration 15V for settings calibrationopen = 8;
            scope_input_calibration();
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
            
            if (calibrationfail)
                //Show the calibration FAILED text
                scope_show_Input_calibration_fail();
            else
                {
                //Save input_calibration data do SDcart
                scope_save_input_calibration_data();
                //Show the calibration successful text
                scope_show_Input_calibration_done();  
                }
            } 
      
      //-**************************************************************************************************************
      //Check on calibration done text opened and being touched
      else if((calibrationopen == 9) && (xtouch >= 395) && (xtouch <= 580) && (ytouch >= 223) && (ytouch <= 280))  //505    
        {
        //Nothing to do here so wait until touch is released
        tp_i2c_wait_for_touch_release();
        }
      else
      {
        //Close any of the menus if open
        close_open_menus(1);

        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();
        
        return;
      }
    } 
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_channel_menu_touch(PCHANNELSETTINGS settings)
{
  uint32 i,j,x,y;
  
  //If ref function is enable automatic open ref menu
  //if (settings->ref1||settings->ref2||settings->ref3||settings->ref4)  
  //if((scopesettings.ch_ref1.enable)||(scopesettings.ch_ref2.enable)||(scopesettings.ch_ref3.enable)||(scopesettings.ch_ref4.enable))
  if(((settings->color == CHANNEL1_COLOR)&&((scopesettings.ref1)||(scopesettings.ref2)||(scopesettings.ref3)||(scopesettings.ref4)))||
    ((settings->color == CHANNEL2_COLOR)&&((scopesettings.ref5)||(scopesettings.ref6)||(scopesettings.ref7)||(scopesettings.ref8))))
    { 
      refmenuopen = 1; 
      //Set this item active
      scope_channel_ref_menu_item(settings, 1);
      //Show the screen ref menu
      scope_channel_open_ref_menu(settings);
    } 
    //If math function is enable automatic open math menu
    else if (mathmode) 
    { 
      mathmenuopen = 1; 
      //Set this item active
      scope_channel_math_menu_item(settings, 1);
      //Show the screen math menu
      scope_channel_open_math_menu(settings);
    } 
    
  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();

    //Check if there is touch
    if(havetouch)
    {
 /*          
  xtouch_tmp=xtouch-settings->menuxpos;
  ytouch_tmp=ytouch-CH_MENU_YPOS;   
  scope_test_TP();
   */     
      //Check if touch within the menu field
      if((xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + CH_MENU_WIDTH) && (ytouch >= CH_MENU_YPOS) && (ytouch <= CH_MENU_YPOS + CH_MENU_HEIGHT))
      {
        //Check on channel enable or disable
        if((ytouch >= CH_MENU_YPOS + 15) && (ytouch <= CH_MENU_YPOS + 50))  //15 45 
        {
          //Check on enable
          if((xtouch >= settings->menuxpos + 80) && (xtouch <= settings->menuxpos + 155))//78 162
          {
            //Toggle the channel enable
            settings->enable ^= 1;

            //Update the trigger channel selection in the FPGA as needed
            fpga_swap_trigger_channel();

            //Set the trigger vertical position to match the trigger channel position
            scope_calculate_trigger_vertical_position();
            
            //Display this
            scope_channel_enable_select(settings);
            scope_channel_settings(settings, 0);
            scope_trigger_settings(0);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
        } 
        //Check on fft enable or disable
        else if((ytouch >= CH_MENU_YPOS + 75) && (ytouch <= CH_MENU_YPOS + 110))    //78-100
        {
          //Check on enable
          if((xtouch >= settings->menuxpos + 80) && (xtouch <= settings->menuxpos + 155))   //
          {
            //Toggle the FFTenable
            settings->fftenable ^= 1;

            //Display this
            scope_channel_fft_show(settings);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
        }
        //Check on invert channel
        else if((ytouch >= CH_MENU_YPOS + 125) && (ytouch <= CH_MENU_YPOS + 165))// && (scopesettings.waveviewmode == 0)) //140 170
        {
          //Check on invert channel
          if((xtouch >= settings->menuxpos + 80) && (xtouch <= settings->menuxpos + 155))//78 162
          {
            //Toggle the channel enable
            settings->invert ^= 1;
            
            //Display this
            //scope_channel_enable_select(settings);
            scope_channel_invert_select(settings);
            scope_channel_settings(settings, 0);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
        }

        //Check on coupling DC or AD, and in normal view mode
        else if((ytouch >= CH_MENU_YPOS + 180) && (ytouch <= CH_MENU_YPOS + 220))// && (scopesettings.waveviewmode == 0)) //140 170
        {
          //Check on DC coupling
          if((xtouch >= settings->menuxpos + 78) && (xtouch <= settings->menuxpos + 110) && (!scopesettings.waveviewmode))
          {
            //Set the channel to DC coupling
            settings->coupling = 0;

            //Update the FPGA
            fpga_set_channel_coupling(settings);
            fpga_set_channel_offset(settings);

            //Display this
            scope_channel_coupling_select(settings);
            scope_channel_settings(settings, 0);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
          //Check on AC coupling
          else if((xtouch >= settings->menuxpos + 130) && (xtouch <= settings->menuxpos + 162) && (!scopesettings.waveviewmode))
          {
            //Set the channel to AC coupling
            settings->coupling = 1;
            
            //Set the channel to dcoffset = 0
            settings->dcoffset = 0;

            //Update the FPGA
            fpga_set_channel_coupling(settings);
            fpga_set_channel_offset(settings);

            //Display this
            scope_channel_coupling_select(settings);
            scope_channel_settings(settings, 0);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
        //}
        
         /*          
        //Check on 50%
         if((xtouch >= settings->menuxpos + 270 ) && (xtouch <= settings->menuxpos + 320 ))//6232-282//4
            {
            //Check if channel is 1 (0) or 2 (1) and trigger channel 1 or channel 2 and long mode with trigger or short mode
            //if(((scopesettings.triggerchannel == 0) && (settings->color == YELLOW_COLOR )&&((!scopesettings.long_mode)||(scopesettings.triggermode)))
             //  ||((scopesettings.triggerchannel == 1) && (settings->color == BLUE_COLOR )&&((!scopesettings.long_mode)||(scopesettings.triggermode))))
              if((((!scopesettings.long_mode)||(scopesettings.triggermode))))
                {
              
                  if(settings->color == CHANNEL1_COLOR) scopesettings.triggerchannel = 0; else scopesettings.triggerchannel = 1;


                  //Use the channel center value as trigger level
                  scope_do_50_percent_trigger_setup();

                  //flag - button 50% is touched
                  trigger50 = 1;

                  //Display this
                  scope_channel_sensitivity_select(settings);

                  //Display this on trigger button
                  scope_trigger_settings(0);

                  //Wait until touch is released
                  tp_i2c_wait_for_touch_release();
                }
            }
          */
        }//end else if coupling DC or AD, and in normal view mode
           
          //------------------
        
        //Check on V_A mode setting, and in normal view mode
        else if((ytouch >= CH_MENU_YPOS + 240) && (ytouch <= CH_MENU_YPOS + 275))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //Check on V
          if((xtouch >= settings->menuxpos + 75) && (xtouch <= settings->menuxpos + 105) && (!scopesettings.waveviewmode))
          {
            //Switch to V
            settings->V_A = 0;

            //Display this
            //scope_channel_probe_magnification_select(settings);
            scope_channel_VA_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }
          //Check on A
          else if((xtouch >= settings->menuxpos + 130) && (xtouch <= settings->menuxpos + 150) && (!scopesettings.waveviewmode))
          {
            //Switch to A
            settings->V_A = 1;

            //Display this
            //scope_channel_probe_magnification_select(settings);
            scope_channel_VA_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
            
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
          }       
        }
        //Check on probe magnification setting, and in normal view mode
        else if((ytouch >= CH_MENU_YPOS + 300) && (ytouch <= CH_MENU_YPOS + 335))// && (scopesettings.waveviewmode == 0))//260-290
        {
          //Check on 0.5x
          if((xtouch >= settings->menuxpos + 70) && (xtouch <= settings->menuxpos + 90) && (!scopesettings.waveviewmode))
          {
            //Enable the channel
            settings->magnification = 0;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 1x
          else if((xtouch >= settings->menuxpos + 100) && (xtouch <= settings->menuxpos + 120) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 1;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 10x
          else if((xtouch >= settings->menuxpos + 130) && (xtouch <= settings->menuxpos + 150) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 2;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 20x
          else if((xtouch >= settings->menuxpos + 160) && (xtouch <= settings->menuxpos + 180) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 3;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 50x
          else if((xtouch >= settings->menuxpos + 190) && (xtouch <= settings->menuxpos + 210) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 4;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 100x
          else if((xtouch >= settings->menuxpos + 220) && (xtouch <= settings->menuxpos + 240) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 5;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          //Check on 1000x
          else if((xtouch >= settings->menuxpos + 250) && (xtouch <= settings->menuxpos + 270) && (!scopesettings.waveviewmode))
          {
            //Disable the channel
            settings->magnification = 6;

            //Display this
            scope_channel_probe_magnification_select(settings);
            scope_channel_settings(settings, 0);
            scope_channel_sensitivity_select(settings);
          }
          
         //Check on 50%
          else if((xtouch >= settings->menuxpos + 280 ) && (xtouch <= settings->menuxpos + 330 ))//270-320//6232-282//4
          {
          //Check if channel is 1 (0) or 2 (1) and trigger channel 1 or channel 2 and long mode with trigger or short mode
          //if(((scopesettings.triggerchannel == 0) && (settings->color == YELLOW_COLOR )&&((!scopesettings.long_mode)||(scopesettings.triggermode)))
          //  ||((scopesettings.triggerchannel == 1) && (settings->color == BLUE_COLOR )&&((!scopesettings.long_mode)||(scopesettings.triggermode))))
            if((((!scopesettings.long_mode)||(scopesettings.triggermode))))
              {    
                //switch (select) trigger according to current channel
                if(settings->color == CHANNEL1_COLOR) scopesettings.triggerchannel = 0; else scopesettings.triggerchannel = 1;

                //Use the channel center value as trigger level
                scope_do_50_percent_trigger_setup();

                //flag - button 50% is touched
                trigger50 = 1;

                //Display this
                scope_channel_sensitivity_select(settings);

                //Display this on trigger button
                scope_trigger_settings(0);

                //Wait until touch is released
                //tp_i2c_wait_for_touch_release();
              }
          }
          
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        
        }
              //************************************************************************************************
          
        //Check on sensitivity set
        if((ytouch >= CH_MENU_YPOS) && (ytouch <= CH_MENU_YPOS + 280) && (xtouch >= settings->menuxpos + 190) && (xtouch <= settings->menuxpos + 330))//skontrolovat 190 az 330
        {
            //------------------ acquisition_speed_texts start ------------------
            //Check on the different bounding boxes for the separate settings
            //Clear the boxes for the not selected items
            for(i=0;i<7;i++)//12 //7
            {
                if (i<6) {x = 178;j=i;} else {x = 261;j=i-6;}//178 260 -6//<4 177*258
                y = (j * 46) + 6;    //3 *56+8
    
                //Calculate the coordinates for this setting
                x = x + settings->menuxpos;
                y = y + CH_MENU_YPOS;

                //Check if touch within this bounding box
                if((xtouch >= x) && (xtouch <= x + 76) && (ytouch >= y) && (ytouch <= y + 40))
                {                  
                  //Set voltperdiv 
                  settings->displayvoltperdiv = 6-i;

                  //Set voltperdiv in FPGA Only update the FPGA in run mode
                  //For waveform view mode the stop state is forced and can't be changed
                  if(scopesettings.runstate) match_volt_per_div_settings(settings);//ok

                  //Display this
                  scope_channel_settings(settings, 0);
                  scope_channel_sensitivity_select(settings);
                  break;
                }
            }
            //Wait until touch is released
            tp_i2c_wait_for_touch_release();
        }
                
        //************************************************************************************************
     
        //Check on 
        if(channelmenuopen && (xtouch >= settings->menuxpos + 350) && (xtouch <= settings->menuxpos + CH_MENU_WIDTH) && (!scopesettings.waveviewmode))
        { 
          //Check on REF menu button, and in normal view mode
          if((ytouch >= CH_MENU_YPOS + 6) && (ytouch <= CH_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
          {
            //Check if already open
            if(refmenuopen == 0)
            {
              //Close any of the sub menus if open
              close_open_channel_menus(settings, 0);
              
              //Set this item active
              scope_channel_ref_menu_item(settings, 1);
              
              //Wait until touch is released
              tp_i2c_wait_for_touch_release();
              
              //Save the screen under the menu
              display_set_destination_buffer(displaybuffer2);
              display_copy_rect_from_screen(settings->menuxpos + CH_REF_MENU_XPOS, CH_REF_MENU_YPOS, CH_REF_MENU_WIDTH, CH_REF_MENU_HEIGHT);
            
              //Show the screen ref menu
              scope_channel_open_ref_menu(settings);
              
              //Signal the REF menu is opened
              refmenuopen = 1;         
            }
          }
          //Check on MATH menu button, and in normal view mode
          else if((ytouch >= CH_MENU_YPOS + 52) && (ytouch <= CH_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
          {
            //Check if already open
            if(mathmenuopen == 0)
            {
              //Close any of the sub menus if open
              close_open_channel_menus(settings, 0);
              
              //Set this item active
              scope_channel_math_menu_item(settings, 1);
              
              //Wait until touch is released
              tp_i2c_wait_for_touch_release();
              
              //Save the screen under the menu
              display_set_destination_buffer(displaybuffer2);
              display_copy_rect_from_screen(settings->menuxpos + CH_MATH_MENU_XPOS, CH_MATH_MENU_YPOS, CH_MATH_MENU_WIDTH, CH_MATH_MENU_HEIGHT);
            
              //Show the screen math menu
              scope_channel_open_math_menu(settings);
              
              //Signal the REF menu is opened
              mathmenuopen = 1;    
            }   
          }
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();
          //---------------------------
        } //else close_open_channel_menus(settings, 0); //Close any of the sub menus if open (ref menu or math menu)
      } //tu konci X a Y plocha menu kanalov uvedene v prvom IF
         //************************************************************************************************
       
      //Check on  menu
      //else if(mathmenuopen && (xtouch >= settings->menuxpos + CH_MENU_WIDTH) && (xtouch <= settings->menuxpos + CH_MENU_WIDTH + CH_MATH_MENU_WIDTH) && (!scopesettings.waveviewmode))
      else if(refmenuopen && (xtouch >= settings->menuxpos + CH_REF_MENU_XPOS) && (xtouch <= (settings->menuxpos + CH_REF_MENU_XPOS + 70))
          &&(ytouch >= CH_REF_MENU_YPOS) && (ytouch <= CH_REF_MENU_YPOS + CH_REF_MENU_HEIGHT))
          //if(mathmenuopen && (xtouch >= settings->menuxpos+380) && (xtouch <= (settings->menuxpos + 500)))// && (!scopesettings.waveviewmode))
        //if(mathmenuopen && (xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + 550))// && (!scopesettings.waveviewmode))
      { 
        //Check on REF1 button, and in normal view mode
        if((ytouch >= CH_REF_MENU_YPOS + 6) && (ytouch <= CH_REF_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //Toggle the ref1

          //scopesettings.ch_ref1.enable ^= 1;
          if(settings->color == CHANNEL1_COLOR)  scopesettings.ref1 ^= 1;
            else scopesettings.ref5 ^= 1;
                
          //Display this
          scope_channel_ref_select(settings);
          //scope_channel_ref_select(settings, &scopesettings.ch1_ref1);//&scopesettings.channel1
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
          //Check on REF2 button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 52) && (ytouch <= CH_REF_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //Toggle the ref2
          if(settings->color == CHANNEL1_COLOR)  scopesettings.ref2 ^= 1;
            else scopesettings.ref6 ^= 1;
                          
          //Display this
          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on REF3 button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 98) && (ytouch <= CH_REF_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //Toggle the ref3
          if(settings->color == CHANNEL1_COLOR)  scopesettings.ref3 ^= 1;
            else scopesettings.ref7 ^= 1;
       
          //Display this
          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on REF4 button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 144) && (ytouch <= CH_REF_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //Toggle the ref4
          if(settings->color == CHANNEL1_COLOR)  scopesettings.ref4 ^= 1;
            else scopesettings.ref8 ^= 1;
                         
          //Display this
          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on OFF ALL button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 190) && (ytouch <= CH_REF_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set All REF to OFF
          if(settings->color == CHANNEL1_COLOR) {scopesettings.ref1 = 0; scopesettings.ref2 = 0; scopesettings.ref3 = 0; scopesettings.ref4 = 0;}
          else { scopesettings.ref5 = 0; scopesettings.ref6 = 0; scopesettings.ref7 = 0; scopesettings.ref8 = 0; }

          //Display this
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_COPYtoREF1_item(settings, 0);
          scope_channel_COPYtoREF2_item(settings, 0);
          scope_channel_COPYtoREF3_item(settings, 0);
          scope_channel_COPYtoREF4_item(settings, 0);
          
          scope_channel_LOADtoREF1_item(settings, 0);
          scope_channel_LOADtoREF2_item(settings, 0);
          scope_channel_LOADtoREF3_item(settings, 0);
          scope_channel_LOADtoREF4_item(settings, 0);

          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
          
          //---------------------------
      }
  
      else if(refmenuopen && (xtouch >= settings->menuxpos + CH_REF_MENU_XPOS + 70) && (xtouch <= (settings->menuxpos + CH_REF_MENU_XPOS + 140))
          &&(ytouch >= CH_REF_MENU_YPOS) && (ytouch <= CH_REF_MENU_YPOS + CH_REF_MENU_HEIGHT))
          //if(mathmenuopen && (xtouch >= settings->menuxpos+380) && (xtouch <= (settings->menuxpos + 500)))// && (!scopesettings.waveviewmode))
        //if(mathmenuopen && (xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + 550))// && (!scopesettings.waveviewmode))
      { 
        //Check on CHx copy to REF1, and in normal view mode
        if((ytouch >= CH_REF_MENU_YPOS + 6) && (ytouch <= CH_REF_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          if(settings->color == CHANNEL1_COLOR) { copy_ch_to_ref(&scopesettings.ch_ref1, settings, ref1_tracebuffer); scopesettings.ref1 = 1; }
            else { copy_ch_to_ref(&scopesettings.ch_ref5, settings, ref5_tracebuffer); scopesettings.ref5 = 1; }
                
          //Display this
          scope_channel_COPYtoREF1_item(settings, 1);
          
          //Display this
          scope_channel_ref_select(settings);
                   
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_LOADtoREF1_item(settings, 0);
          scope_channel_COPYtoREF1_item(settings, 2);
        }
          //Check on CHx copy to REF2, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 52) && (ytouch <= CH_REF_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          if(settings->color == CHANNEL1_COLOR) { copy_ch_to_ref(&scopesettings.ch_ref2, settings, ref2_tracebuffer); scopesettings.ref2 = 1; }
            else { copy_ch_to_ref(&scopesettings.ch_ref6, settings, ref6_tracebuffer); scopesettings.ref6 = 1; }
                
          //Display this
          scope_channel_COPYtoREF2_item(settings, 1);
          
          //Display this
          scope_channel_ref_select(settings);
         
          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_LOADtoREF2_item(settings, 0);
          scope_channel_COPYtoREF2_item(settings, 2);
        }
        //Check on CHx copy to REF3, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 98) && (ytouch <= CH_REF_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          if(settings->color == CHANNEL1_COLOR) { copy_ch_to_ref(&scopesettings.ch_ref3, settings, ref3_tracebuffer); scopesettings.ref3 = 1; }
            else { copy_ch_to_ref(&scopesettings.ch_ref7, settings, ref7_tracebuffer); scopesettings.ref7 = 1; }
                
          //Display this
          scope_channel_COPYtoREF3_item(settings, 1);
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_LOADtoREF3_item(settings, 0);
          scope_channel_COPYtoREF3_item(settings, 2);
        }
        //Check on CHx copy to REF4, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 144) && (ytouch <= CH_REF_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          if(settings->color == CHANNEL1_COLOR) { copy_ch_to_ref(&scopesettings.ch_ref4, settings, ref4_tracebuffer); scopesettings.ref4 = 1; }
            else { copy_ch_to_ref(&scopesettings.ch_ref8, settings, ref8_tracebuffer); scopesettings.ref8 = 1; }
                         
          //Display this
          scope_channel_COPYtoREF4_item(settings, 1);
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_LOADtoREF4_item(settings, 0);
          scope_channel_COPYtoREF4_item(settings, 2);
          
        }
        //Check on CLEAR ALL ref button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 190) && (ytouch <= CH_REF_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          if(settings->color == CHANNEL1_COLOR) 
          {
            //For now clear buffers REF1 to REF4
            //memset(ref1_tracebuffer, 128, sizeof(ref1_tracebuffer));
            memset(scopesettings.ch_ref1.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref2.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref3.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref4.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            
            scopesettings.ch_ref1.traceposition = 225;
            scopesettings.ch_ref2.traceposition = 250;
            scopesettings.ch_ref3.traceposition = 300;
            scopesettings.ch_ref4.traceposition = 350;
            
            scopesettings.ref1 = 0; scopesettings.ref2 = 0; scopesettings.ref3 = 0; scopesettings.ref4 = 0; 
          }
          else
          {
            //For now clear buffers REF5 to REF8
            memset(scopesettings.ch_ref5.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref6.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref7.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE);
            memset(scopesettings.ch_ref8.tracebuffer, 128, MAX_SAMPLE_BUFFER_SIZE); 
            
            scopesettings.ch_ref5.traceposition = 25;
            scopesettings.ch_ref6.traceposition = 50;
            scopesettings.ch_ref7.traceposition = 100;
            scopesettings.ch_ref8.traceposition = 150;
            
            scopesettings.ref5 = 0; scopesettings.ref6 = 0; scopesettings.ref7 = 0; scopesettings.ref8 = 0;       
          }
                
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 1);
          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
                    
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF1_item(settings, 0);
          scope_channel_COPYtoREF2_item(settings, 0);
          scope_channel_COPYtoREF3_item(settings, 0);
          scope_channel_COPYtoREF4_item(settings, 0);
          
          scope_channel_LOADtoREF1_item(settings, 0);
          scope_channel_LOADtoREF2_item(settings, 0);
          scope_channel_LOADtoREF3_item(settings, 0);
          scope_channel_LOADtoREF4_item(settings, 0);
          
          scope_channel_CLEAR_REF1_4_item(settings, 2);
        } 
          
          //---------------------------
      }
      
      else if(refmenuopen && (xtouch >= settings->menuxpos + CH_REF_MENU_XPOS + 140) && (xtouch <= (settings->menuxpos + CH_REF_MENU_XPOS + 210))
          &&(ytouch >= CH_REF_MENU_YPOS) && (ytouch <= CH_REF_MENU_YPOS + CH_REF_MENU_HEIGHT))
          //if(mathmenuopen && (xtouch >= settings->menuxpos+380) && (xtouch <= (settings->menuxpos + 500)))// && (!scopesettings.waveviewmode))
        //if(mathmenuopen && (xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + 550))// && (!scopesettings.waveviewmode))
      { 
        //Check on CHx copy to REF1, and in normal view mode
        if((ytouch >= CH_REF_MENU_YPOS + 6) && (ytouch <= CH_REF_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {                
          //Display this
          scope_channel_LOADtoREF1_item(settings, 1);
          
          //Load data from file 1.ref to buffer ref1
          if(settings->color == CHANNEL1_COLOR) { scope_load_REFx_item_file(1); scopesettings.ref1 = 1; }
            else { scope_load_REFx_item_file(5); scopesettings.ref5 = 1; }
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF1_item(settings, 0);
          scope_channel_LOADtoREF1_item(settings, 2);
        }
          //Check on CHx copy to REF2, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 52) && (ytouch <= CH_REF_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {            
          //Display this
          scope_channel_LOADtoREF2_item(settings, 1);
          
          //Load data from file 2.ref to buffer ref2
          if(settings->color == CHANNEL1_COLOR) { scope_load_REFx_item_file(2); scopesettings.ref2 = 1; }
            else { scope_load_REFx_item_file(6); scopesettings.ref6 = 1; }
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF2_item(settings, 0);
          scope_channel_LOADtoREF2_item(settings, 2);
        }
        //Check on CHx copy to REF3, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 98) && (ytouch <= CH_REF_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {                
          //Display this
          scope_channel_LOADtoREF3_item(settings, 1);
          
          //Load data from file 3.ref to buffer ref3
          if(settings->color == CHANNEL1_COLOR) { scope_load_REFx_item_file(3); scopesettings.ref3 = 1; }
            else { scope_load_REFx_item_file(7); scopesettings.ref7 = 1; }
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF3_item(settings, 0);
          scope_channel_LOADtoREF3_item(settings, 2);
        }
        //Check on CHx copy to REF4, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 144) && (ytouch <= CH_REF_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {           
          //Display this
          scope_channel_LOADtoREF4_item(settings, 1);
          //scope_channel_ref_select(settings);
          
          //Load data from file 4.ref to buffer ref4
          if(settings->color == CHANNEL1_COLOR) { scope_load_REFx_item_file(4); scopesettings.ref4 = 1; }
            else { scope_load_REFx_item_file(8); scopesettings.ref8 = 1; }
          
          //Display this
          scope_channel_ref_select(settings);
          scope_channel_SAVE_REF1_4_item(settings, 0);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
          //Display this
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF4_item(settings, 0);
          scope_channel_LOADtoREF4_item(settings, 2);
        }
        //Check on Load all ref button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 190) && (ytouch <= CH_REF_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          if(settings->color == CHANNEL1_COLOR)
          {
            //Load data from file x.ref to buffer refx (1-4)
            scope_load_REFx_item_file(1);
            scope_load_REFx_item_file(2);
            scope_load_REFx_item_file(3);
            scope_load_REFx_item_file(4);
            scopesettings.ref1 = 1;
            scopesettings.ref2 = 1;
            scopesettings.ref3 = 1;
            scopesettings.ref4 = 1;
          }
          else
          {
            //Load data from file x.ref to buffer refx (5-8)
            scope_load_REFx_item_file(5);
            scope_load_REFx_item_file(6);
            scope_load_REFx_item_file(7);
            scope_load_REFx_item_file(8);
            scopesettings.ref5 = 1;
            scopesettings.ref6 = 1;
            scopesettings.ref7 = 1;
            scopesettings.ref8 = 1;
          }
            
          //settings->ref1 = 1; settings->ref2 = 1; settings->ref3 = 1; settings->ref4 = 1;
          //scopesettings.ref1 = 1; scopesettings.ref2 = 1; scopesettings.ref3 = 1; scopesettings.ref4 = 1;
          //scopesettings.ch_ref1.enable = 1; scopesettings.ch_ref2.enable = 1; scopesettings.ch_ref3.enable = 1; scopesettings.ch_ref4.enable = 1;
                
          //Display this
          scope_channel_LOAD_REF1_4_item(settings, 1);
          
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_SAVE_REF1_4_item(settings, 0);
          
          scope_channel_COPYtoREF1_item(settings, 0);
          scope_channel_COPYtoREF2_item(settings, 0);
          scope_channel_COPYtoREF3_item(settings, 0);
          scope_channel_COPYtoREF4_item(settings, 0);
                    
          scope_channel_LOADtoREF1_item(settings, 0);
          scope_channel_LOADtoREF2_item(settings, 0);
          scope_channel_LOADtoREF3_item(settings, 0);
          scope_channel_LOADtoREF4_item(settings, 0);

          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
          //Show the screen math menu
          scope_channel_LOAD_REF1_4_item(settings, 2);
        } 
          
          //---------------------------
      }//end else if
      
      else if(refmenuopen && (xtouch >= settings->menuxpos + CH_REF_MENU_XPOS + 210) && (xtouch <= (settings->menuxpos + CH_REF_MENU_XPOS + CH_REF_MENU_WIDTH))
          &&(ytouch >= CH_REF_MENU_YPOS) && (ytouch <= CH_REF_MENU_YPOS + CH_REF_MENU_HEIGHT))
          //if(mathmenuopen && (xtouch >= settings->menuxpos+380) && (xtouch <= (settings->menuxpos + 500)))// && (!scopesettings.waveviewmode))
        //if(mathmenuopen && (xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + 550))// && (!scopesettings.waveviewmode))
      { 
        //Check on CHx copy to REF1, and in normal view mode
        if((ytouch >= CH_REF_MENU_YPOS + 6) && (ytouch <= CH_REF_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          //memcpy(settings->ref1_tracebuffer, settings->tracebuffer, MAX_SAMPLE_BUFFER_SIZE);
                
          //Display this
          scope_channel_SAVE_REF1_item(settings, 1);
          
          //Save to file ref1
          if(settings->color == CHANNEL1_COLOR) scope_save_REFx_item_file(1); else scope_save_REFx_item_file(5);
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_SAVE_REF1_item(settings, 0);
        }
          //Check on CHx copy to REF2, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 52) && (ytouch <= CH_REF_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          //memcpy(settings->ref2_tracebuffer, settings->tracebuffer, MAX_SAMPLE_BUFFER_SIZE);
                
          //Display this
          scope_channel_SAVE_REF2_item(settings, 1);
          
          //Save to file ref2
          if(settings->color == CHANNEL1_COLOR) scope_save_REFx_item_file(2); else scope_save_REFx_item_file(6);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release(); 
          
          //Display this
          scope_channel_SAVE_REF2_item(settings, 0);
        }
        //Check on CHx copy to REF3, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 98) && (ytouch <= CH_REF_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          //memcpy(settings->ref3_tracebuffer, settings->tracebuffer, MAX_SAMPLE_BUFFER_SIZE);
                
          //Display this
          scope_channel_SAVE_REF3_item(settings, 1);
          
          //Save to file ref3
          if(settings->color == CHANNEL1_COLOR) scope_save_REFx_item_file(3); else scope_save_REFx_item_file(7);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
          
          //Display this
          scope_channel_SAVE_REF3_item(settings, 0);
        }
        //Check on CHx copy to REF4, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 144) && (ytouch <= CH_REF_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //For now just copy the settings to the given struct
          //memcpy(settings->ref4_tracebuffer, settings->tracebuffer, MAX_SAMPLE_BUFFER_SIZE);
                
          //Display this
          scope_channel_SAVE_REF4_item(settings, 1);
          
          //Save to file ref4
          if(settings->color == CHANNEL1_COLOR) scope_save_REFx_item_file(4); else scope_save_REFx_item_file(8);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
          //Display this
          scope_channel_SAVE_REF4_item(settings, 0);
        }
        //Check on OFF button, and in normal view mode
        else if((ytouch >= CH_REF_MENU_YPOS + 190) && (ytouch <= CH_REF_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          if(settings->color == CHANNEL1_COLOR)
          {
            //Load data from file x.ref to buffer refx
            scope_save_REFx_item_file(1);
            scope_save_REFx_item_file(2);
            scope_save_REFx_item_file(3);
            scope_save_REFx_item_file(4);
          }
          else
          {
            //Load data from file x.ref to buffer refx
            scope_save_REFx_item_file(5);
            scope_save_REFx_item_file(6);
            scope_save_REFx_item_file(7);
            scope_save_REFx_item_file(8);
          }
          //settings->ref1 = 1; settings->ref2 = 1; settings->ref3 = 1; settings->ref4 = 1;
                
          //Display this
          scope_channel_SAVE_REF1_4_item(settings, 1);
          


          scope_channel_ref_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();  
          
          //Show the screen math menu
          scope_channel_CLEAR_REF1_4_item(settings, 0);
          scope_channel_LOAD_REF1_4_item(settings, 0);
         
          //scope_channel_COPYtoREF1_item(settings, 0);
          //scope_channel_COPYtoREF2_item(settings, 0);
          //scope_channel_COPYtoREF3_item(settings, 0);
          //scope_channel_COPYtoREF4_item(settings, 0);
          
          //scope_channel_LOADtoREF1_item(settings, 0);
          //scope_channel_LOADtoREF2_item(settings, 0);
          //scope_channel_LOADtoREF3_item(settings, 0);
          //scope_channel_LOADtoREF4_item(settings, 0);
          
          scope_channel_SAVE_REF1_item(settings, 0);
          scope_channel_SAVE_REF2_item(settings, 0);
          scope_channel_SAVE_REF3_item(settings, 0);
          scope_channel_SAVE_REF4_item(settings, 0);
          
          scope_channel_SAVE_REF1_4_item(settings, 2);
          
          
        } 
          
          //---------------------------
      }//end else if SAVE_REFx
      
      //Check on MATH menu
      //else if(mathmenuopen && (xtouch >= settings->menuxpos + CH_MENU_WIDTH) && (xtouch <= settings->menuxpos + CH_MENU_WIDTH + CH_MATH_MENU_WIDTH) && (!scopesettings.waveviewmode))
      else if(mathmenuopen && (xtouch >= settings->menuxpos + CH_MATH_MENU_XPOS) && (xtouch <= (settings->menuxpos + CH_MATH_MENU_XPOS + 70))
          &&(ytouch >= CH_MATH_MENU_YPOS) && (ytouch <= CH_MATH_MENU_YPOS + CH_MATH_MENU_HEIGHT))
          //if(mathmenuopen && (xtouch >= settings->menuxpos+380) && (xtouch <= (settings->menuxpos + 500)))// && (!scopesettings.waveviewmode))
        //if(mathmenuopen && (xtouch >= settings->menuxpos) && (xtouch <= settings->menuxpos + 550))// && (!scopesettings.waveviewmode))
      { 
        //Check on A+B button, and in normal view mode
        if((ytouch >= CH_MATH_MENU_YPOS + 6) && (ytouch <= CH_MATH_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set math mode A + B
          mathmode = 1;
            
          //Show the screen math menu
          scope_channel_math_select(settings);
          /*
          //if (scopesettings.math==1) 
            if (mathmode==1)
          {
            for (uint32 i = 0; i < MAX_SAMPLE_BUFFER_SIZE; i++) 
            {  
                //scopesettings.channel2.tracebuffer[i]=0;
                //scopesettings.channel2.tracebuffer[i] = ((scopesettings.channel2.tracebuffer[i]) + (channel2tracebufferAVG[i]))/2;
                //channel2tracebufferAVG[i]=scopesettings.channel2.tracebuffer[i];
                scopesettings.math_tracebuffer[i] = ((scopesettings.channel1.tracebuffer[i]-128) + (scopesettings.channel2.tracebuffer[i]-128));
            }
            //memcpy(settings->ref_tracebuffer, (scopesettings.channel1.tracebuffer+scopesettings.channel2.tracebuffer), MAX_SAMPLE_BUFFER_SIZE);
          }
          */
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
          //Check on A-B button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 52) && (ytouch <= CH_MATH_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set math mode A - B
          mathmode = 2;

          //Show the screen math menu
          scope_channel_math_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on B-A button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 98) && (ytouch <= CH_MATH_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set math mode A * B
          mathmode = 3;

          //Show the screen math menu
          scope_channel_math_select(settings);
          //scope_channel_open_math_menu(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on A*B button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 144) && (ytouch <= CH_MATH_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set math mode A / B
          mathmode = 4;

          //Show the screen math menu
          scope_channel_math_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on OFF button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 190) && (ytouch <= CH_MATH_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set OFF
          mathmode = 0;

          //Show the screen math menu
          scope_channel_math_select(settings);

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
          
          //---------------------------
      }//end else if //else if(mathmenuopen) close_open_channel_menus(settings, 0); //Close any of the sub menus if open (ref menu or math menu)
      
      else if(mathmenuopen && (xtouch >= settings->menuxpos + CH_MATH_MENU_XPOS + 70) && (xtouch <= (settings->menuxpos + CH_MATH_MENU_XPOS + CH_MATH_MENU_WIDTH))
          &&(ytouch >= CH_MATH_MENU_YPOS) && (ytouch <= CH_MATH_MENU_YPOS + CH_MATH_MENU_HEIGHT))
      { 
        //Check on adding button channal A
        if((ytouch >= CH_MATH_MENU_YPOS + 6) && (ytouch <= CH_MATH_MENU_YPOS + 46))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set Adding channel A
          if (channelA<3)  channelA++; else channelA = 0;
          
          //Show the screen math menu
          scope_channel_math_select(settings);
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        //Check on adding button channal B
        else if((ytouch >= CH_MATH_MENU_YPOS + 52) && (ytouch <= CH_MATH_MENU_YPOS + 92))// && (scopesettings.waveviewmode == 0)) //200 230
        {
          //set Adding channel B
          if (channelB<3)  channelB++; else channelB = 0;
            
          //Show the screen math menu
          scope_channel_math_select(settings);
              
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();   
        }
        /*
        //Check on B-A button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 98) && (ytouch <= CH_MATH_MENU_YPOS + 138))// && (scopesettings.waveviewmode == 0)) //200 230
        {
        //set math mode B-A
        mathmode = 3;
            
        //Show the screen math menu
        scope_channel_math_select(settings);
        //scope_channel_open_math_menu(settings);
              
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();   
        }
        //Check on A*B button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 144) && (ytouch <= CH_MATH_MENU_YPOS + 184))// && (scopesettings.waveviewmode == 0)) //200 230
        {
        //set math mode A*B
        mathmode = 4;
            
        //Show the screen math menu
        scope_channel_math_select(settings);
              
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();   
        }
        //Check on OFF button, and in normal view mode
        else if((ytouch >= CH_MATH_MENU_YPOS + 190) && (ytouch <= CH_MATH_MENU_YPOS + 230))// && (scopesettings.waveviewmode == 0)) //200 230
        {
        //set OFF
        mathmode = 0;
            
        //Show the screen math menu
        scope_channel_math_select(settings);
              
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();   
        }
         */ 
          //---------------------------
      } //end else if //else if(mathmenuopen) close_open_channel_menus(settings, 0); //Close any of the sub menus if open (ref menu or math menu)
              
        //*******************************************************************************************
    
        //*******************************************************************************************

        //Wait until touch is released before checking on a new position
        //tp_i2c_wait_for_touch_release();
        
        //Nothing to do here so wait until touch is released
        //tp_i2c_wait_for_touch_release();
    
      //}
        //}
      //}// tu je have touch
      else
      {
        //Close any of the channel menus if open (close all menu -channel menu, ref menu, math menu)
        close_open_channel_menus(settings, 1);
        
        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        return;
      }//tu ma  byt have touch
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_acquisition_menu_touch(void)
{
  uint32 i,x,y;

  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {   
    //Scan the touch panel for touch
    tp_i2c_read_status();
    
    //Check if there is touch
    if(havetouch)
    {
    /*    
    xtouch_tmp=xtouch-ACQ_MENU_XPOS;
    ytouch_tmp=ytouch-ACQ_MENU_YPOS; 
    scope_test_TP();
    */        
      //Check if touch within the menu field
      if((xtouch >= ACQ_MENU_XPOS) && (xtouch <= ACQ_MENU_XPOS + ACQ_MENU_WIDTH) && (ytouch >= ACQ_MENU_YPOS) && (ytouch <= ACQ_MENU_YPOS + ACQ_MENU_HEIGHT))
      {
        //Check on acquisition frequency being set
        if((ytouch >= ACQ_MENU_YPOS + 32) && (ytouch <= ACQ_MENU_YPOS + 149))
        {
          //Check if in run state. Changing this setting is only allowed when running or single mode trigger
          if((scopesettings.runstate)||(scopesettings.triggermode == 1))
          {//------------------ acquisition_speed_texts start ------------------
            //Check on the different bounding boxes for the separate settings
            for(i=0;i<((sizeof(acquisition_speed_texts) / sizeof(int8 *))-11);i++)//7
            {
              //Calculate the coordinates for this setting
              x = ((i & 3) * 72) + 10 + ACQ_MENU_XPOS;
              y = ((i >> 2) * 23) + 33 + ACQ_MENU_YPOS;

              //Check if touch within this bounding box
              if((xtouch >= x) && (xtouch <= x + 68) && (ytouch >= y) && (ytouch <= y + 20))
              {
                //Set the new speed
                scopesettings.samplerate = i;

                //Set the FPGA sample rate setting without changing the current time per div setting
                //This way the sampling is actually done with the set speed, but the display stays at the set time per div
                fpga_set_sample_rate(i);

                //Display the new setting
                scope_acquisition_speed_select();

                //Update the viable time per div settings
                scope_acquisition_timeperdiv_select();

                //Update the top menu bar display
                scope_acqusition_settings(0);
                
                //Flag conversion in progres
                //scopesettings.display_data_done = 0;
                break;
              }
            }
          }
        }
        //Check on time per div being set
        else if((ytouch >= ACQ_MENU_YPOS + 240) && (ytouch <= ACQ_MENU_YPOS + 380))//190-329
        {
          //Check on the different bounding boxes for the separate settings
          //for(i=0;i<(sizeof(time_div_texts) / sizeof(int8 *));i++)
            for(i=0;i<((sizeof(time_div_texts) / sizeof(int8 *))-11);i++)//9   
          {
            //Calculate the coordinates for this setting
            x = ((i & 3) * 72) + 10 + ACQ_MENU_XPOS;
            y = ((i >> 2) * 23) + 241 + ACQ_MENU_YPOS;  //191

            //Check if touch within this bounding box
            if((xtouch >= x) && (xtouch <= x + 68) && (ytouch >= y) && (ytouch <= y + 20))
            {
              //Set the new time per div
              scopesettings.timeperdiv = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1) - i;

              //Display the new setting
              scope_acquisition_timeperdiv_select();
              scope_acqusition_settings(0);
              break;
            }
          }
        }
        
        //Check on middle button line set
        else if((ytouch >= ACQ_MENU_YPOS + 170) && (ytouch <= ACQ_MENU_YPOS + 210))
        {   
            //Check ACQ mode (short or long) button if touch within this bounding box
            if((xtouch >= ACQ_MENU_XPOS + 20) && (xtouch <= ACQ_MENU_XPOS + 90))
            {
                //Toggle the long_mode state
                //scopesettings.long_mode ^= 1;
                scope_acquisition_ACQ_mode_select();
            }
            
            //Check Average button if touch within this bounding box
            if((xtouch >= ACQ_MENU_XPOS + 120) && (xtouch <= ACQ_MENU_XPOS + 190)&&(!scopesettings.long_mode))
            {        
                //Toggle the average_mode state
                scopesettings.average_mode ^= 1;         
                scope_acquisition_Average_mode_select();
                scope_acqusition_settings(0);
            }    
            
            //Check Long memory button if touch within this bounding box
            if((xtouch >= ACQ_MENU_XPOS + 220) && (xtouch <= ACQ_MENU_XPOS + 290))
            {
                //Toggle the average_mode state
                //scopesettings.long_memory  ^= 1;        
                scopesettings.long_memory  = 0;   //docasne zrusene
                scope_acquisition_Long_memory_select();
                //Update the top menu bar display
                //scope_acqusition_settings(0);
            }
        
        }
      //------------------------------------------------------------------------

        //Wait until touch is released before checking on a new position
        tp_i2c_wait_for_touch_release();
      }
      else
      {
        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        return;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_trigger_menu_touch(void)
{
  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();

    //Check if there is touch
    if(havetouch)
    {
        
  //xtouch_tmp=xtouch-settings->menuxpos;
  //ytouch_tmp=ytouch-CH_MENU_YPOS;   
  //scope_test_TP();
  
  
      //Check if touch within the menu field
      if((xtouch >= TRIGGER_MENU_XPOS) && (xtouch <= TRIGGER_MENU_XPOS+TRIGGER_MENU_WIDTH) && (ytouch >= TRIGGER_MENU_YPOS) && (ytouch <= TRIGGER_MENU_YPOS+TRIGGER_MENU_HEIGHT)) //232
      {
        //Check on trigger mode, and not in waveform view
        //if((ytouch >= TRIGGER_MENU_YPOS+5) && (ytouch <= TRIGGER_MENU_YPOS+29) && (!scopesettings.waveviewmode))
        if((ytouch >= TRIGGER_MENU_YPOS+5) && (ytouch <= TRIGGER_MENU_YPOS+52) && (!scopesettings.waveviewmode))
        {
          //Check on auto
          if((xtouch >= TRIGGER_MENU_XPOS+116) && (xtouch <= TRIGGER_MENU_XPOS+152))//112-148
          { 
            //Set to auto mode
            set_trigger_mode(0);
          }
        //}
        //Check on trigger mode, and not in waveform view
        //if((ytouch >= TRIGGER_MENU_YPOS+28) && (ytouch <= TRIGGER_MENU_YPOS+52) && (!scopesettings.waveviewmode))
        //{
          //Check on single
            else if((xtouch >= TRIGGER_MENU_XPOS+65) && (xtouch <= TRIGGER_MENU_XPOS+112))//67-114
          {
            //Set to single mode
            set_trigger_mode(1);
          }
          //Check on normal
          else if((xtouch >= TRIGGER_MENU_XPOS+156) && (xtouch <= TRIGGER_MENU_XPOS+210))//146-200
          {
            //Set to normal mode
            set_trigger_mode(2);
          }
        }
        //Check on trigger edge, and not in waveform view
        else if((ytouch >= TRIGGER_MENU_YPOS+61) && (ytouch <= TRIGGER_MENU_YPOS+107) && (!scopesettings.waveviewmode)) //120 155 ,125-147
        {
          //Check on rising
          if((xtouch >= TRIGGER_MENU_XPOS+70) && (xtouch <= TRIGGER_MENU_XPOS+125))    //626-666
          {
            //Set the trigger edge to rising
            scopesettings.triggeredge = 0;

            //Update the FPGA
            fpga_set_trigger_edge();

            //Display this
            scope_trigger_edge_select();
            scope_trigger_settings(0);
          }
          //Check on falling
          else if((xtouch >= TRIGGER_MENU_XPOS+150) && (xtouch <= TRIGGER_MENU_XPOS+205))   //671-716
          {
            //Set the trigger edge to falling
            scopesettings.triggeredge = 1;

            //Update the FPGA
            fpga_set_trigger_edge();

            //Display this
            scope_trigger_edge_select();
            scope_trigger_settings(0);
          }
        }
        //Check on trigger channel, and not in waveform view
        else if((ytouch >= TRIGGER_MENU_YPOS+117) && (ytouch <= TRIGGER_MENU_YPOS+163) && (!scopesettings.waveviewmode)) //185 220 ,188-210
        {
          //Check on channel 1
          if((xtouch >= TRIGGER_MENU_XPOS+70) && (xtouch <= TRIGGER_MENU_XPOS+103))
          {
            //Only when the channel is enabled
            if(scopesettings.channel1.enable)
            {
              //Set the channel 1 as trigger source
              scopesettings.triggerchannel = 0;

              //Update the FPGA
              fpga_set_trigger_channel();
              
              //Set the trigger vertical position to match channel 1 trace position
              scope_calculate_trigger_vertical_position();

              //Display this
              scope_trigger_channel_select();
              scope_trigger_settings(0);
            }
          }
          //Check on channel 2
          else if((xtouch >= TRIGGER_MENU_XPOS+121) && (xtouch <= TRIGGER_MENU_XPOS+154))
          {
            //Only when the channel is enabled
            if(scopesettings.channel2.enable)
            {
              //Set channel 2 as trigger source
              scopesettings.triggerchannel = 1;

              //Update the FPGA
              fpga_set_trigger_channel();

              //Set the trigger vertical position to match channel 2 trace position
              scope_calculate_trigger_vertical_position();

              //Display this
              scope_trigger_channel_select();
              scope_trigger_settings(0);
            }
          }
          else if((xtouch >= TRIGGER_MENU_XPOS+172) && (xtouch <= TRIGGER_MENU_XPOS+205))
          {
            //Only when the channel is enabled
            //if(scopesettings.channel2.enable)
            //{
              //Set channel 2 as trigger source
              scopesettings.triggerchannel = 2;

              //Update the FPGA
              //fpga_set_trigger_channel();

              //Set the trigger vertical position to match channel 2 trace position
              //scope_calculate_trigger_vertical_position();

              //Display this
              scope_trigger_channel_select();
              scope_trigger_settings(0);
            //}
          }
        }
        //Check on Always50%, and not in waveform view
        else if((ytouch >= TRIGGER_MENU_YPOS+173) && (ytouch <= TRIGGER_MENU_YPOS+219) && (!scopesettings.waveviewmode)) //125-147
        {
          //Check on always triger 50%
          if((xtouch >= TRIGGER_MENU_XPOS) && (xtouch <= TRIGGER_MENU_XPOS+TRIGGER_MENU_WIDTH))
          { 
          //Toggle the always trigger 50% state
          scopesettings.alwaystrigger50 ^= 1;
          
          //Show the state 
          scope_display_slide_button(TRIGGER_MENU_XPOS + 135, TRIGGER_MENU_YPOS + 186, scopesettings.alwaystrigger50, GREEN_COLOR);//90-186
          }
        }
        //Check on HoldOn, and not in waveform view
        else if((ytouch >= TRIGGER_MENU_YPOS+229) && (ytouch <= TRIGGER_MENU_YPOS+275) && (!scopesettings.waveviewmode)) //125-147
        {
                    //Check on auto
          if((xtouch >= 625) && (xtouch <= 649))//625 649
          { 
            //Set to 100Hz
            //set_trigger_mode(0);
          }
          //Check on single
          else if((xtouch >= 661) && (xtouch <= 681))
          {
            //Set to 1kHz
            //set_trigger_mode(1);
          }
          //Check on normal
          else if((xtouch >= 692) && (xtouch <= 715))
          {
            //Set to 10kHz
            //set_trigger_mode(2);
          }
        }

        //Wait until touch is released before checking on a new position
        tp_i2c_wait_for_touch_release();
      }
      else
      {
        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        return;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_generator_menu_touch(void)
{
    uint32 x,y,z;

  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {   
    //Scan the touch panel for touch
    tp_i2c_read_status();
    
    //Check if there is touch
    if(havetouch)
    {
        
    xtouch_tmp=xtouch-KEY_MENU_XPOS;
    ytouch_tmp=ytouch-KEY_MENU_YPOS; 
    //scope_test_TP();
            
      //Check if touch within the menu field
      if((xtouch >= KEY_MENU_XPOS) && (xtouch <= KEY_MENU_XPOS + KEY_MENU_WIDTH) && (ytouch >= KEY_MENU_YPOS) && (ytouch <= KEY_MENU_YPOS + KEY_MENU_HEIGHT))
      {

        //Check on time per div being set
        if((ytouch >= KEY_MENU_YPOS + 80) && (ytouch <= KEY_MENU_YPOS + 350))//265//190-329
        {
          //Check on the different bounding boxes for the separate settings

          //for(i=0;i<((sizeof(time_div_texts) / sizeof(int8 *))-11);i++)//9 
          //for(i=0;i<((sizeof(keyboard_texts) / sizeof(int8 *)));i++)
          //{
            //Calculate the coordinates for this setting
            //if(((i%5)==0)) x = 5; else x += 58;
            //y=((i/5)*53); 
            x = KEY_MENU_XPOS;
            y = KEY_MENU_YPOS;
            z='0';
            //x = ((i & 3) * 72) + 10 + KEY_MENU_XPOS;
            //y = ((i >> 2) * 23) + 241 + KEY_MENU_YPOS;  //191

            //Check if touch within this bounding box
            if((ytouch >= y + 80) && (ytouch <= y + 110))//riadok 6
            {
                
              if((xtouch >= x + 30) &&  (xtouch <= x + 50))   z='7';    //1 cislo z lava 40
              if((xtouch >= x + 80) &&  (xtouch <= x + 100))  z='8';    //2 cislo z lava 90
              if((xtouch >= x + 140) && (xtouch <= x + 160))  z='9';    //3 cislo z lava 150
              if((xtouch >= x + 200) && (xtouch <= x + 220))  z='m';    //4 cislo z lava 210
              if((xtouch >= x + 250) && (xtouch <= x + 270))  z='k';    //4 cislo z lava 260
                
                
                            
  //Select the font for the texts
  display_set_font(&font_5);//3

    //Main texts in white
  display_set_fg_color(LIGHTGREY_COLOR);
  //Display the texts
  display_fill_rect(KEY_MENU_XPOS + 170, KEY_MENU_YPOS + 34, 120,20);
    //Main texts in white
  display_set_fg_color(WHITE_COLOR);
            //display_text(KEY_MENU_XPOS + 170, KEY_MENU_YPOS + 34, z);
            }        
            
            //Check if touch within this bounding box
            else if((ytouch >= y + 300) && (ytouch <= y + 320))//riadok 2
            {
                              //Select the font for the texts
  display_set_font(&font_5);//3

    //Main texts in white
  display_set_fg_color(DARKGREY_COLOR);
  //Display the texts
  display_fill_rect(KEY_MENU_XPOS + 170, KEY_MENU_YPOS + 34, 120,20);
    //Main texts in white
  display_set_fg_color(WHITE_COLOR);
                
                if((xtouch >= x + 30) && (xtouch <= x + 80))    display_text(KEY_MENU_XPOS + 220, KEY_MENU_YPOS + 34, "100Hz");      //1 cislo z lava 40 min
                if((xtouch >= x + 120) && (xtouch <= x + 180))   display_text(KEY_MENU_XPOS + 230, KEY_MENU_YPOS + 34, "1kHz");   //2 cislo z lava 90 default
                if((xtouch >= x + 220) && (xtouch <= x + 280))  display_text(KEY_MENU_XPOS + 210, KEY_MENU_YPOS + 34, "100kHz");    //3 cislo z lava 150 max

            } 

                        
              //Set the new time per div
              //scopesettings.timeperdiv = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1) - i;

              //Display the new setting
              //scope_acquisition_timeperdiv_select();
              //scope_acqusition_settings(0);
              //break;
            }
          //}
        }
        /*
        //Check on middle button line set
        else if((ytouch >= KEY_MENU_YPOS + 170) && (ytouch <= KEY_MENU_YPOS + 210))
        {   
            //Check ACQ mode (short or long) button if touch within this bounding box
            if((xtouch >= KEY_MENU_XPOS + 20) && (xtouch <= KEY_MENU_XPOS + 90))
            {
                //Toggle the long_mode state
                //scopesettings.long_mode ^= 1;
               // scope_acquisition_ACQ_mode_select();
            }
            
            //Check Average button if touch within this bounding box
            if((xtouch >= KEY_MENU_XPOS + 120) && (xtouch <= KEY_MENU_XPOS + 190)&&(!scopesettings.long_mode))
            {        
                //Toggle the average_mode state
              //  scopesettings.average_mode ^= 1;         
              //  scope_acquisition_Average_mode_select();
              //  scope_acqusition_settings(0);
            }    
            
            //Check Long memory button if touch within this bounding box
            if((xtouch >= KEY_MENU_XPOS + 220) && (xtouch <= KEY_MENU_XPOS + 290))
            {
                //Toggle the average_mode state
                //scopesettings.long_memory  ^= 1;        
               // scopesettings.long_memory  = 0;   //docasne zrusene
               // scope_acquisition_Long_memory_select();
                //Update the top menu bar display
                //scope_acqusition_settings(0);
            }
        
        }
    */
      //------------------------------------------------------------------------

        //Wait until touch is released before checking on a new position
        tp_i2c_wait_for_touch_release();
      }
      else
      {
        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        return;
      }
    }
  //}
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_right_basic_menu_touch(void)
{
  //Check if run/stop or page up button is touched
  if((ytouch >= 63) && (ytouch <= 117))
  {
    //Check on wave view state(Scope mode) for which button is shown
    if(!scopesettings.waveviewmode)
    {
      //Run/stop so highlight that button if touched
      scope_run_stop_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Toggle the run state
      scopesettings.runstate ^= 1;
      
      //Button back to inactive state
      scope_run_stop_button(0);

      //When the scope is switched back to running state, change the sample rate to the current time per div setting
      if(scopesettings.runstate)//ok
      {
        //Long time base - set other values and clear view area
        if(scopesettings.long_mode) scope_preset_values(); 
   
        //Show this on the screen
        scope_acqusition_settings(0);
        
        //Match the volts per division settings for both channels
        //Is needed when vertical zoom has been used
        match_volt_per_div_settings(&scopesettings.channel1);
        match_volt_per_div_settings(&scopesettings.channel2);
      }

      //Display the changed state
      scope_run_stop_text();
    }
    else
    {
      //Previous waveform so highlight that button if touched
      scope_next_wave_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_next_wave_button(0);

      //Check if not on first item
      if(viewcurrentindex > 0)
      {
        //Select the previous one
        viewcurrentindex--;

        //Load the waveform data for the newly selected item
        if(scope_load_trace_data() == VIEW_TRACE_LOAD_ERROR)
        {
          //On error signal need to return to main item view mode
          scopesettings.waveviewmode = 0;
        }
      }
    }
  }
  //Check if auto set or page down button is touched
  else if((ytouch >= 123) && (ytouch <= 177))
  {
    //Check on wave view state (Skope mode) for which button is shown
    if(!scopesettings.waveviewmode)
    {
      //Auto set so highlight that button if touched
      scope_auto_set_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_auto_set_button(0);

      //Short time base run auto setup
      if(!scopesettings.long_mode) scope_do_auto_setup();
    }
    else
    {
      //Next waveform so highlight that button if touched
      scope_previous_wave_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_previous_wave_button(0);

      //Check if not on last item
      //Index starts on zero, available items on 1
      if(viewcurrentindex < (viewavailableitems - 1))
      {
        //Select the next one
        viewcurrentindex++;
        
        //Load the waveform data for the newly selected item
        if(scope_load_trace_data() == VIEW_TRACE_LOAD_ERROR)
        {
          //On error signal need to return to main item view mode
          scopesettings.waveviewmode = 0;
        }
      }
    }
  }
  //Check if time cursor button is touched
  else if((ytouch >= 183) && (ytouch <= 237))
  {
    //Highlight the button if touched
    scope_t_cursor_button(1);

    //Go and setup the t_cursor menu
    //scope_open_t_cursor_menu();
    
    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_t_cursor_button(0);

    //Toggle the enable setting
    scopesettings.timecursorsenable ^= 1;
    
    //Draw the cursor lines
    //scope_update_cursor_possition();
    if(scopesettings.long_mode && (!scopesettings.waveviewmode)) scope_preset_values();
    
  }
  //Check if volt cursor button is touched
  else if((ytouch >= 243) && (ytouch <= 297))
  {
    //Highlight the button if touched
    scope_v_cursor_button(1);
    
    //Go and setup the v_cursor menu
    //scope_open_v_cursor_menu();

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_v_cursor_button(0);

    //Toggle the enable setting
    scopesettings.voltcursorsenable ^= 1;
        
    //Draw the cursor lines
    //scope_update_cursor_possition();
    if(scopesettings.long_mode && (!scopesettings.waveviewmode)) scope_preset_values();
  }
  //Check if measures button is touched
  else if((ytouch >= 303) && (ytouch <= 357))
  {
    //Highlight the button if touched
    scope_measures_button(1);

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_measures_button(0);

    //Save the screen rectangle where the menu will be displayed
    display_set_destination_buffer(displaybuffer2);
    //display_copy_rect_from_screen(231, 263, 501, 215);//499 214
    display_copy_rect_from_screen(232, 232, 499, 246);//499 214

    //Go and setup the measurements menu
    scope_open_measures_menu();

    //Go and handle the menu touch
    handle_measures_menu_touch();

    //Restore the screen when done
    display_set_source_buffer(displaybuffer2);
    //display_copy_rect_to_screen(231, 263, 501, 215);//499 214
    display_copy_rect_to_screen(232, 232, 499, 246);//499 214
  }
  //Check if save picture button is touched
  else if((ytouch >= 363) && (ytouch <= 417))
  {
    //Highlight the button if touched
    scope_save_picture_button(1);
    
    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_save_picture_button(0);
    
    //Save the picture on the SD card
    scope_save_view_item_file(VIEW_TYPE_PICTURE);
  }
  //Check if save or delete wave button is touched
  else if((ytouch >= 423) && (ytouch <= 477))
  {
    //Check on wave view state (Scope mode) for which button is shown
    if(!scopesettings.waveviewmode)
    {
      //Save wave so highlight that button if touched
      scope_save_wave_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_save_wave_button(0);

      //Save the waveform on the SD card
      scope_save_view_item_file(VIEW_TYPE_WAVEFORM);
    }
    else
    {
      //Delete wave so highlight that button if touched
      scope_delete_wave_button(1);

      //Wait until touch is released
      tp_i2c_wait_for_touch_release();

      //Button back to inactive state
      scope_delete_wave_button(0);

      //Ask user for confirmation of the delete
      if(handle_confirm_delete() == VIEW_CONFIRM_DELETE_YES)
      {
        //Need to remove this item from the thumbnails
        scope_remove_item_from_thumbnails(1);

        //save the thumbnail file
        scope_save_thumbnail_file();

        //No more items available then return
        if(viewavailableitems == 0)
        {
          //Signal need to return to main item view mode
          scopesettings.waveviewmode = 0;
        }
        else
        {
          //Otherwise need to see if there is a next image to display or the previous one needs to be used
          if(viewcurrentindex >= viewavailableitems)
          {
            //Use the last available one
            //Index starts on zero, available items on 1
            viewcurrentindex = viewavailableitems - 1;
          }

          //Load the waveform data for the newly selected item
          if(scope_load_trace_data() == VIEW_TRACE_LOAD_ERROR)
          {
            //On error signal need to return to main item view mode
            scopesettings.waveviewmode = 0;
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_right_volts_div_menu_touch(void)
{
  //Check if channel 1 V+ button is touched
  if((ytouch >= 93) && (ytouch <= 155))//78 - 140
  {
    //Highlight the button if touched
    scope_channel_sensitivity_control(&scopesettings.channel1, 0, 1);

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_channel_sensitivity_control(&scopesettings.channel1, 0, 0);

    //Only if channel is enabled
    if(scopesettings.channel1.enable)
    {
    //--------------------------------------------------------------------------    
      if(scopesettings.rightmenustate == 1)
      {
        //Check if setting not already on max
        if(scopesettings.channel1.displayvoltperdiv < 6)
        {
            //Step up to the next setting. (Lowering the setting)
            scopesettings.channel1.displayvoltperdiv++;

            //Show the change on the screen
            scope_channel_settings(&scopesettings.channel1, 0);

            //Only update the FPGA in run mode
            //For waveform view mode the stop state is forced and can't be changed
            if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel1);
        }
      }
    //--------------------------------------------------------------------------
      if(scopesettings.rightmenustate == 2)
      {//change value for - center position on signal
          if (scopesettings.channel1.dc_shift_center < 200) scopesettings.channel1.dc_shift_center++;
          scope_channel_dcshift_value();
          //scope_do_auto_setup();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 3)
      {//change value for - size of signal
          if (scopesettings.channel1.dc_shift_size < 250) scopesettings.channel1.dc_shift_size++;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 4)
      {//change value for - measurement value of signal
          if (scopesettings.channel1.dc_shift_value < 450) scopesettings.channel1.dc_shift_value++;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
    }
  }
  //Check if channel 1 V- button is touched
  else if((ytouch >= 178) && (ytouch <= 238))//163-223
  {
    //Highlight the button if touched
    scope_channel_sensitivity_control(&scopesettings.channel1, 1, 1);

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_channel_sensitivity_control(&scopesettings.channel1, 1, 0);

    //Only if channel is enabled
    if(scopesettings.channel1.enable)
    {
      if(scopesettings.rightmenustate == 1)
      {
        //Check if setting not already on min
        if(scopesettings.channel1.displayvoltperdiv > 0)
        {
            //Original code has some limit on updating when the scope is not running on the time base setting or a previous stored volts per div setting
            //The latter maybe has to do with a stored wave that is being looked at
            //For now just allowing constant change within the max limit

            //Step down to the next setting. (Increasing the setting)
            scopesettings.channel1.displayvoltperdiv--;

            //Show the change on the screen
            scope_channel_settings(&scopesettings.channel1, 0);

            //Only update the FPGA in run mode
            //For waveform view mode the stop state is forced and can't be changed
            if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel1);
        }
      }
    //--------------------------------------------------------------------------
      if(scopesettings.rightmenustate == 2)
      {//change value for - center position on signal
          if (scopesettings.channel1.dc_shift_center > 100) scopesettings.channel1.dc_shift_center--;
          scope_channel_dcshift_value();
          //scope_do_auto_setup();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 3)
      {//change value for - size of signal
          if (scopesettings.channel1.dc_shift_size > 150) scopesettings.channel1.dc_shift_size--;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 4)
      {//change value for - measurement value of signal
          if (scopesettings.channel1.dc_shift_value > 300) scopesettings.channel1.dc_shift_value--;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
    }
  }
 
  //Check if channel 2 V+ button is touched
  else if((ytouch >= 258) && (ytouch <= 320))
  {
    //Highlight the button if touched
    scope_channel_sensitivity_control(&scopesettings.channel2, 0, 1);

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_channel_sensitivity_control(&scopesettings.channel2, 0, 0);

    //Only if channel is enabled
    if(scopesettings.channel2.enable)
    {
    //--------------------------------------------------------------------------    
      if(scopesettings.rightmenustate == 1)
      {
        //Check if setting not already on max
        if(scopesettings.channel2.displayvoltperdiv < 6)
        {
            //Step up to the next setting. (Lowering the setting)
            scopesettings.channel2.displayvoltperdiv++;

            //Show the change on the screen
            scope_channel_settings(&scopesettings.channel2, 0);

            //Only update the FPGA in run mode
            //For waveform view mode the stop state is forced and can't be changed
            if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel2);
        }
      }
    //--------------------------------------------------------------------------
      if(scopesettings.rightmenustate == 2)
      {//change value for - center position on signal
          if (scopesettings.channel2.dc_shift_center < 200) scopesettings.channel2.dc_shift_center++;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 3)
      {//change value for - size of signal
          if (scopesettings.channel2.dc_shift_size < 250) scopesettings.channel2.dc_shift_size++;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 4)
      {//change value for - measurement value of signal
          if (scopesettings.channel2.dc_shift_value < 450) scopesettings.channel2.dc_shift_value++;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
    }
  }
  //Check if channel 2 V- button is touched
  else if((ytouch >= 343) && (ytouch <= 403))
  {
    //Highlight the button if touched
    scope_channel_sensitivity_control(&scopesettings.channel2, 1, 1);

    //Wait until touch is released
    tp_i2c_wait_for_touch_release();

    //Button back to inactive state
    scope_channel_sensitivity_control(&scopesettings.channel2, 1, 0);

    //Only if channel is enabled
    if(scopesettings.channel2.enable)
    {
      if(scopesettings.rightmenustate == 1)
      {
        //Check if setting not already on min
        if(scopesettings.channel2.displayvoltperdiv > 0)
        {
            //Original code has some limit on updating when the scope is not running on the time base setting or a previous stored volts per div setting
            //The latter maybe has to do with a stored wave that is being looked at
            //For now just allowing constant change within the max limit

            //Step down to the next setting. (Increasing the setting)
            scopesettings.channel2.displayvoltperdiv--;

            //Show the change on the screen
            scope_channel_settings(&scopesettings.channel2, 0);

            //Only update the FPGA in run mode
            //For waveform view mode the stop state is forced and can't be changed
            if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel2);
        }
      }
    //--------------------------------------------------------------------------
      if(scopesettings.rightmenustate == 2)
      {//change value for - center position on signal
          if (scopesettings.channel2.dc_shift_center > 100) scopesettings.channel2.dc_shift_center--;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 3)
      {//change value for - size of signal
          if (scopesettings.channel2.dc_shift_size > 150) scopesettings.channel2.dc_shift_size--;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
      if(scopesettings.rightmenustate == 4)
      {//change value for - measurement value of signal
          if (scopesettings.channel2.dc_shift_value > 300) scopesettings.channel2.dc_shift_value--;
          scope_channel_dcshift_value();
      }
    //-------------------------------------------------------------------------- 
    }
  }
  
  //-**********************************************************************************************************
  /*
  //Check if channel 2 V+ button is touched
  else if((ytouch >= 258) && (ytouch <= 320))
  {

    if (scopesettings.channel1.dcoffset<1000) scopesettings.channel1.dcoffset+=5; 
    //Wait until touch is released
    tp_i2c_wait_for_touch_release();
    fpga_set_channel_offset(&scopesettings.channel1);
    
    //scopesettings.channel1.traceposition -= scopesettings.channel1.dcoffset/2;


  }
  //Check if channel 2 V- button is touched
  else if((ytouch >= 343) && (ytouch <= 403))
  {
    if (scopesettings.channel1.dcoffset>-1000) scopesettings.channel1.dcoffset-=5; 
    //Wait until touch is released
    tp_i2c_wait_for_touch_release();
    fpga_set_channel_offset(&scopesettings.channel1);
    
    //scopesettings.channel1.traceposition += scopesettings.channel1.dcoffset/2;

   
  }
   */
  
  //-**********************************************************************************************************
  //Check if 50% trigger or show grid button is touched or calibration value menu
  else if((ytouch >= 423) && (ytouch <= 477))
  {
    //Check on wave view state (Scope mode) for which button is shown
    if(!scopesettings.waveviewmode)
    {   
      if(scopesettings.rightmenustate == 1)
      {
        //50% trigger so highlight that button if touched
        scope_50_percent_trigger_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Button back to inactive state
        scope_50_percent_trigger_button(0);

        //Only when scope is running
        if(scopesettings.runstate)//ok
            {
            //Set the trigger level for the active trigger channel
            scope_do_50_percent_trigger_setup();
            }
      }else
      //-------------------------------------------------------
      if(scopesettings.rightmenustate == 2)
      {
        //Show next so highlight that button if touched
        scope_next_cal_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Button back to inactive state
        scope_next_cal_button(0);
      
        //Switch to next page
        scopesettings.rightmenustate = 3;
        scope_channel_dcshift_value();
      }else    
      //-------------------------------------------------------
      if(scopesettings.rightmenustate == 3)
      {
        //Show next so highlight that button if touched
        scope_next_cal_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Button back to inactive state
        //scope_next_cal_button(0);
        //Button back to inactive state
        scope_end_cal_button(0);
      
        //Switch to next page
        scopesettings.rightmenustate = 4;
        scope_channel_dcshift_value();
      }else 
      //-------------------------------------------------------
      if(scopesettings.rightmenustate == 4)
      {
        //Show end so highlight that button if touched
        scope_end_cal_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Button back to inactive state
        scope_end_cal_button(0);
        
        //save calibration data
        scope_save_input_calibration_data();
        
        //end of dc shift calibration, flag to zero
        //dc_shift_cal = 0;
      
        //Switch to normal right scope menu
        scopesettings.rightmenustate = 0;
        
        //Display the changed state
        scope_setup_right_control_menu();
      }
      //------------------------------------------------------------------
      
      
      
    }
    else
    { 
      if(scopesettings.rightmenustate == 1)
      {
        //Show grid so highlight that button if touched
        scope_show_grid_button(1);

        //Wait until touch is released
        tp_i2c_wait_for_touch_release();

        //Button back to inactive state
        scope_show_grid_button(0);
      
        //Toggle the gird enable status
        scopesettings.gridenable ^= 1;
      }
      
    }   
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

const TOUCHCOORDS measures_item_touch_coords[2][12] =
{
  {
    //Channel 1 coordinates are on the left
    {232, 293, 297, 355}, {295, 355, 297, 355}, {357, 417, 297, 355}, {419, 480, 297, 355},
    {232, 293, 357, 415}, {295, 355, 357, 415}, {357, 417, 357, 415}, {419, 480, 357, 415},
    {232, 293, 417, 476}, {295, 355, 417, 476}, {357, 417, 417, 476}, {419, 480, 417, 476},
  },
  {
    //Channel 2 coordinates are on the right
    {482, 543, 297, 355}, {545, 605, 297, 355}, {607, 667, 297, 355}, {669, 729, 297, 355},
    {482, 543, 357, 415}, {545, 605, 357, 415}, {607, 667, 357, 415}, {669, 729, 357, 415},
    {482, 543, 417, 476}, {545, 605, 417, 476}, {607, 667, 417, 476}, {669, 729, 417, 476},
  }
};

//----------------------------------------------------------------------------------------------------------------------------------

void handle_measures_menu_touch(void)
{
  int channel;
  int item;
  int found;

  //Stay in the menu as long as there is no touch outside the menu
  while(1)
  {
    //Scan the touch panel for touch
    tp_i2c_read_status();

    //Check if there is touch
    if(havetouch)
    {
      //scope_test_TP();
      
      //Check if touch within the menu field
      if((xtouch >= 232) && (xtouch <= 730) && (ytouch >= 232) && (ytouch <= 477))//231-730 264-477
      {
        found = 0;
        channel = 0;

        //Check the touch positions for the separate items until one is found
        while((found == 0) && (channel < 2))
        {
                   //--------------- Only select ch1 or ch2 and all & clear & hide -------------------------------------------------  
         //Check press button select all and clear for ch1 a ch2
         if((ytouch >= 232) && (ytouch <= 264))
         {
            if((xtouch >= 235) && (xtouch <= 285))  //ref1 button
            {scopesettings.source1_measures = 1; scope_source1_measures_menu_item();}
           
            if((xtouch >= 295) && (xtouch <= 355))  //ref2 button
            {scopesettings.source1_measures = 2; scope_source1_measures_menu_item();}

            if((xtouch >= 357) && (xtouch <= 417))  //ref3 button
            {scopesettings.source1_measures = 3; scope_source1_measures_menu_item();}

            if((xtouch >= 419) && (xtouch <= 480))  //ref4 button
            {scopesettings.source1_measures = 4; scope_source1_measures_menu_item();}     
            
            if((xtouch >= 485) && (xtouch <= 535))  //ref5 button
            {scopesettings.source2_measures = 1; scope_source2_measures_menu_item();}

            if((xtouch >= 545) && (xtouch <= 605))  //ref6 button
            {scopesettings.source2_measures = 2; scope_source2_measures_menu_item();} 

            if((xtouch >= 607) && (xtouch <= 667))  //ref7 button  
            {scopesettings.source2_measures = 3; scope_source2_measures_menu_item();}

            if((xtouch >= 669) && (xtouch <= 729))   //ref8 button 
            {scopesettings.source2_measures = 4; scope_source2_measures_menu_item();}  
         }
         
         //--------------- Only select ch1 or ch2 and all & clear & hide -------------------------------------------------  
         //Check press button select all and clear for ch1 a ch2
         if((ytouch >= 264) && (ytouch <= 295))
         {
            if((xtouch >= 235) && (xtouch <= 285))  //ch1 button open menu ch1-ref1-ref2-ref3-ref4
            {scopesettings.source1_measures = 0; scope_source1_measures_menu_item();}
           
            if((xtouch >= 295) && (xtouch <= 355))  //select ALL ch1 button
            {for (int j=0; j<12;j++) {scopesettings.measuresstate[0][j] = 1;scope_measures_menu_item(0,j);}}

            if((xtouch >= 357) && (xtouch <= 417))  //clear ch1 button
            {for (int j=0; j<12;j++) {scopesettings.measuresstate[0][j] = 0;scope_measures_menu_item(0,j);}}

            if((xtouch >= 419) && (xtouch <= 480))  //hide ch1
            { scopesettings.hide_values_CH1 ^= 1; scope_hide_measures_menu_item(); tp_i2c_wait_for_touch_release();}      
            
            if((xtouch >= 485) && (xtouch <= 535))  //ch2 button
            {scopesettings.source2_measures = 0; scope_source2_measures_menu_item();}

            if((xtouch >= 545) && (xtouch <= 605))  //select ALL ch2 button
            {for (int j=0; j<12;j++) {scopesettings.measuresstate[1][j] = 1;scope_measures_menu_item(1,j);}}

            if((xtouch >= 607) && (xtouch <= 667))  //clear ch2 button   
            {for (int j=0; j<12;j++) {scopesettings.measuresstate[1][j] = 0;scope_measures_menu_item(1,j);}}

            if((xtouch >= 669) && (xtouch <= 729))
            { scopesettings.hide_values_CH2 ^= 1; scope_hide_measures_menu_item(); tp_i2c_wait_for_touch_release();}   
         }
         //------------------------------------------------------------------------------------------------
                    
          item = 0;

          //For each channel 12 items
          while((found == 0) && (item<12))
          {
            //Check if touch is on this item
            if((xtouch >= measures_item_touch_coords[channel][item].x1) && (xtouch <= measures_item_touch_coords[channel][item].x2) &&
               (ytouch >= measures_item_touch_coords[channel][item].y1) && (ytouch <= measures_item_touch_coords[channel][item].y2))
            {
              //Toggle the item
              scopesettings.measuresstate[channel][item] ^= 1;

              //There is an array for showing the items on screen. It works on a first come first serve system.
              //It looks for the first free spot and inserts itself there. This gives a messy image of the two channels interleaving with settings
              //On disable the list is shifted to fill the gap
              //For this an items in list counter might be useful

              //A nicer setup is to reserve separate sections per channel on the screen for the enabled items and always show them in a fixed
              //order. Makes reading them easier
              
              //Draw the changed item
              scope_measures_menu_item(channel, item);

              //Signal item found
              found = 1;
            }

            //Next item
            item++;
          }

          //Next channel
          channel++;
        }

        //Wait until touch is released before checking on a new position
        tp_i2c_wait_for_touch_release();
      }
      else
      {
        //Touch outside the menu so wait until touch is released and then quit
        tp_i2c_wait_for_touch_release();

        return;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

const TOUCHCOORDS view_item_touch_coords[16] =
{
  {  4, 178,   4, 116}, {186, 360,   4, 116}, {368, 542,   4, 116}, {550, 726,   4, 116},
  {  4, 178, 124, 236}, {186, 360, 124, 236}, {368, 542, 124, 236}, {550, 726, 124, 236},
  {  4, 178, 244, 356}, {186, 360, 244, 356}, {368, 542, 244, 356}, {550, 726, 244, 356},
  {  4, 178, 364, 476}, {186, 360, 364, 476}, {368, 542, 364, 476}, {550, 726, 364, 476},
};

//----------------------------------------------------------------------------------------------------------------------------------

void handle_view_mode_touch(void)
{
  int32 index;
  int32 found;

  //Stay in this mode as long as the return is not touched
  while(1)
  {
    //Read the touch panel status
    tp_i2c_read_status();

    //Check if the panel is touched
    if(havetouch)
    {
      //Check if touch is in right menu area.
      if((xtouch >= 730))
      {
        //Check if the return button is touched
        if((ytouch >= 4) && (ytouch <= 76))
        {
          //Highlight the button to show it is touched
          scope_return_button(1);
          
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();

          //Back to inactive state
          scope_return_button(0);
          
          //Set other values and clear view area
          scope_preset_values();
          
          //DOne with item view so return
          return;
        }
        //Else check if the select all button is touched
        else if((ytouch >= 84) && (ytouch <= 156))
        {
          //Highlight the button
          scope_select_all_button(1);
          
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
          
          //Deselect the possibly enabled select button
          scope_select_button(0);

          //Depending on the state take action
          if((viewselectmode == VIEW_SELECT_NONE) || (viewselectmode == VIEW_SELECT_INDIVIDUAL))
          {
            //Enable the button
            scope_select_all_button(2);

            //Switch to all items selected mode
            viewselectmode = VIEW_SELECT_ALL;

            //Make sure all selected signs are displayed
            for(index=0;index<viewitemsonpage;index++)
            {
              //Check if the selected sign is not selected
              if(viewitemselected[index] == VIEW_ITEM_NOT_SELECTED)
              {
                //If so make sure it gets displayed
                viewitemselected[index] = VIEW_ITEM_SELECTED_NOT_DISPLAYED;
              }
            }
          }
          else
          {
            //Make sure the button is no longer highlighted
            scope_select_all_button(0);

            //Switch to all items deselected mode
            viewselectmode = VIEW_SELECT_NONE;

            //Not in a selected mode any more so clear the active selected signs
            for(index=0;index<viewitemsonpage;index++)
            {
              //Check if the selected sign is actually displayed
              if(viewitemselected[index] == VIEW_ITEM_SELECTED_DISPLAYED)
              {
                //If so make sure it gets cleared
                viewitemselected[index] = VIEW_ITEM_NOT_SELECTED_DISPLAYED;
              }
            }
          }

          //Update the selected signs
          scope_display_selected_signs();
        }
        //Else check if the select button is touched
        else if((ytouch >= 164) && (ytouch <= 236))
        {
          //Highlight the button to show it is touched
          scope_select_button(1);
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
          
          //Deselect the possibly highlighted select all button
          scope_select_all_button(0);

          //Check if all items are selected or select mode is on
          if(viewselectmode)
          {
            //Make sure the select button is not highlighted
            scope_select_button(0);

            //Switch to all items deselected mode
            viewselectmode = VIEW_SELECT_NONE;

            //Not in a selected mode any more so clear the active selected signs
            for(index=0;index<viewitemsonpage;index++)
            {
              //Check if the selected sign is actually displayed
              if(viewitemselected[index] == VIEW_ITEM_SELECTED_DISPLAYED)
              {
                //If so make sure it gets cleared
                viewitemselected[index] = VIEW_ITEM_NOT_SELECTED_DISPLAYED;
              }
            }
          }
          else
          {
            //Nothing selected so switch to individual select mode
            viewselectmode = VIEW_SELECT_INDIVIDUAL;

            //Highlight the select button
            scope_select_button(2);
          }

          //Update the selected signs
          scope_display_selected_signs();
        }
        //Else check if the delete button is touched
        else if((ytouch >= 244) && (ytouch <= 316))
        {
          //Highlight the button to show it is touched
          scope_delete_button(1);
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
          
          //Back to inactive button
          scope_delete_button(0);
          
          //Need to see if there are items selected for delete
          if(viewselectmode)
          {
            //A select mode is active so check the list to see if there are items selected
            //This is not done in the original code. Activate the single select mode and without selecting an item press delete. The confirm menu is shown
            for(index=0,found=0;index<viewitemsonpage;index++)
            {
              //Check if the current item is selected
              if(viewitemselected[index] == VIEW_ITEM_SELECTED_DISPLAYED)
              {
                //Signal there is at least one item to delete
                found = 1;
                break;
              }
            }

            //Check if there is an item to delete
            if(found)
            {
              //Ask the user if the items should be deleted
              if(handle_confirm_delete() == VIEW_CONFIRM_DELETE_YES)
              {
                //User opted for delete so do this for the selected items
                //Start with the last item on the page to avoid problems with the file number list being modified
                for(index=viewitemsonpage-1;index>=0;index--)
                {
                  //Check if the current item is selected
                  if(viewitemselected[index] == VIEW_ITEM_SELECTED_DISPLAYED)
                  {
                    //Set the current index for this file
                    viewcurrentindex = index + (viewpage * VIEW_ITEMS_PER_PAGE);

                    //Remove the current item from the thumbnails and delete the item from disk
                    scope_remove_item_from_thumbnails(1);
                  }
                }

                //Save the thumbnail file
                scope_save_thumbnail_file();

                //Clear the select flags
                memset(viewitemselected, VIEW_ITEM_NOT_SELECTED, VIEW_ITEMS_PER_PAGE);

                //Clear the select state and the button highlights
                viewselectmode = VIEW_SELECT_NONE;
                scope_select_all_button(0);
                scope_select_button(0);

                //Redisplay the thumbnails
                scope_initialize_and_display_thumbnails();
              }
            }
          }
        }
        //Else check if the page up button is touched
        else if((ytouch >= 324) && (ytouch <= 396))
        {
          //Highlight the button to show it is touched
          scope_page_up_button(1);
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
          
          //Back to inactive button
          scope_page_up_button(0);
          
          //Check if there is a previous page
          if(viewpage > 0)
          {
            //Clear the select mode
            viewselectmode = 0;

            //Deselect the possibly highlighted buttons
            scope_select_all_button(0);
            scope_select_button(0);

            //Clear the item selected flags
            memset(viewitemselected, 0, VIEW_ITEMS_PER_PAGE);

            //Select the previous page
            viewpage--;

            //Display the previous page of thumbnails
            scope_display_thumbnails();
          }
        }
        //Else check if the page down button is touched
        else if((ytouch >= 404) && (ytouch <= 476))
        {
          //Highlight the button to show it is touched
          scope_page_down_button(1);
            
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
          
          //Back to inactive button
          scope_page_down_button(0);
          
          //Check if there is a next page
          if(viewpage < viewpages)
          {
            //Clear the select mode
            viewselectmode = 0;

            //Deselect the possibly highlighted buttons
            scope_select_all_button(0);
            scope_select_button(0);

            //Clear the item selected flags
            memset(viewitemselected, 0, VIEW_ITEMS_PER_PAGE);

            //Select the next page
            viewpage++;

            //Display the next page of thumbnails
            scope_display_thumbnails();
          }
        }
      }
      else
      {
        //Start with no item touched yet and check on first item
        found = 0;
        index = 0;

        //Do until found or last available item checked
        while(index < viewitemsonpage)
        {
          //Check if touch is on this item
          if((xtouch >= view_item_touch_coords[index].x1) && (xtouch <= view_item_touch_coords[index].x2) &&
             (ytouch >= view_item_touch_coords[index].y1) && (ytouch <= view_item_touch_coords[index].y2))
          {
            //Signal item found
            found = 1;

            //Quit the loop when found
            break;
          }

          //Next item
          index++;
        }

        //Need to wait for touch to release before selecting or opening the item
        tp_i2c_wait_for_touch_release();
        
        //Check if touch was on one of the available items
        if(found)
        {
          //Check if in a select mode
          if(viewselectmode)
          {
            //Yes, so toggle the select on this item
            if(viewitemselected[index] == VIEW_ITEM_SELECTED_DISPLAYED)
            {
              //If displayed make sure it gets cleared
              viewitemselected[index] = VIEW_ITEM_NOT_SELECTED_DISPLAYED;
            }
            else
            {
              //If not make sure it gets displayed
              viewitemselected[index] = VIEW_ITEM_SELECTED_NOT_DISPLAYED;
            }

            //Update the selected signs
            //Could improve on this with a single sign update function
            scope_display_selected_signs();
          }
          else
          {
            //Set the current index for this file
            viewcurrentindex = index + (viewpage * VIEW_ITEMS_PER_PAGE);

            //Loaded ok, so take action based on the type of the opened item
            if(viewtype == VIEW_TYPE_PICTURE)
            {
              //Load the bitmap in the screen memory
              if(scope_load_bitmap_data() == VIEW_BITMAP_LOAD_OK)
              {
                //On initialization draw the bottom menu bar with a save of the background
                scope_setup_bottom_file_menu(VIEW_BOTTON_MENU_INIT);

                //Handle the touch
                handle_picture_view_touch();
              }
            }
            else
            {
              //Try to load the trace data for the file indicated by the current index and view type
              if(scope_load_trace_data() == VIEW_TRACE_LOAD_OK)
              {
                //waveform view needs to be handled here as long as the flag is set
                while(scopesettings.waveviewmode)
                {
                  //Use the main touch handler for waveform view
                  touch_handler();

                  //Update the display as needed
                  scope_display_trace_data();

                  //Monitor the battery status
                  battery_check_status();
                }
              }
            }

            //Display the file actions menu on the right side of the screen
            scope_setup_right_file_menu();

            //Display the available thumbnails for the current view type
            scope_initialize_and_display_thumbnails();
          }
        }
      }
    }

    //Need to wait for touch to release before checking again
    tp_i2c_wait_for_touch_release();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void handle_picture_view_touch(void)
{
  //Stay in this mode as long as the return is not touched
  while(1)
  {
    //Read the touch panel status
    tp_i2c_read_status();

    //Check if the panel is touched
    if(havetouch)
    {
      //Check if bottom view menu is shown
      if(viewbottommenustate & VIEW_BOTTON_MENU_SHOW)
      {
        //Check if touch outside of the menu bar
        if(ytouch < 420)
        {
          //Wait until touch is released before taking action
          tp_i2c_wait_for_touch_release();
          
          //Toggle the menu visibility
          viewbottommenustate ^= VIEW_BOTTON_MENU_SHOW;

          //Update accordingly
          scope_setup_bottom_file_menu(viewbottommenustate);
        }
        else
        {
          //Check if in return button region
          if((xtouch > 40) && (xtouch < 160))
          {
            //Highlight the button
            scope_bmp_return_button(1);
            
            //Wait until touch is released before taking action
            tp_i2c_wait_for_touch_release();

            //Back to inactive state
            scope_bmp_return_button(0);
            
            //Clear the menu state. This is needed for the emulator
            viewbottommenustate = VIEW_BOTTON_MENU_HIDE;
            
            //Just return
            return;
          }
          //Check if in delete button region
          else if((xtouch > 240) && (xtouch < 360))
          {
            //Highlight the button
            scope_bmp_delete_button(1);
            
            //Wait until touch is released before taking action
            tp_i2c_wait_for_touch_release();

            //Back to inactive state
            scope_bmp_delete_button(0);
            
            //Ask user for confirmation of the delete
            if(handle_confirm_delete() == VIEW_CONFIRM_DELETE_YES)
            {
              //Need to remove this item from the thumbnails
              scope_remove_item_from_thumbnails(1);

              //save the thumbnails file
              scope_save_thumbnail_file();

              //No more items available then return
              if(viewavailableitems == 0)
              {
                return;
              }

              //Then need to see if there is a next image to display or the previous one needs to be used
              if(viewcurrentindex >= viewavailableitems)
              {
                //Use the last available one
                //Index starts on zero, available items on 1
                viewcurrentindex = viewavailableitems - 1;
              }

              //Display the new picture item
              if(scope_display_picture_item() == VIEW_BITMAP_LOAD_ERROR)
              {
                //On error return to main view
                return;
              }
            }
          }
          //Check if in previous item button region
          else if((xtouch > 440) && (xtouch < 560))
          {
            //Highlight the button
            scope_bmp_previous_button(1);
            
            //Wait until touch is released before taking action
            tp_i2c_wait_for_touch_release();

            //Back to inactive state
            scope_bmp_previous_button(0);
            
            //Check if not on first item
            if(viewcurrentindex > 0)
            {
              //Select the previous one
              viewcurrentindex--;

              //Display the new picture item
              if(scope_display_picture_item() == VIEW_BITMAP_LOAD_ERROR)
              {
                //On error return to main view
                return;
              }
            }
          }
          //Check if in next item button region
          else if((xtouch > 640) && (xtouch < 760))
          {
            //Highlight the button
            scope_bmp_next_button(1);
            
            //Wait until touch is released before taking action
            tp_i2c_wait_for_touch_release();

            //Back to inactive state
            scope_bmp_next_button(0);
            
            //Check if not on last item
            //Index starts on zero, available items on 1
            if(viewcurrentindex < (viewavailableitems - 1))
            {
              //Select the next one
              viewcurrentindex++;

              //Display the new picture item
              if(scope_display_picture_item() == VIEW_BITMAP_LOAD_ERROR)
              {
                //On error return to main view
                return;
              }
            }
          }
        }
      }
      else
      {
        //If not shown, show it again
        scope_setup_bottom_file_menu(VIEW_BOTTON_MENU_SHOW);
      }
    }

    //Need to wait for touch to release before checking again
    tp_i2c_wait_for_touch_release();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void handle_diagnostic_view_touch(void)
{
  //uint8 tmp;                            //for diagnostic menu
  //uint32 *ptr = STARTUP_CONFIG_ADDRESS; //for diagnostic menu
  //boot_menu_start = ptr[0] & 0x07;      //choice 1 for fnirsi firmware, 0 for peco firmware, 2 for FEL mode, 4 view choice menu

  //Stay in this mode as long as the return is not touched
  while(1)
  {
    //Read the touch panel status
    tp_i2c_read_status();

    //Check if the panel is touched
    if(havetouch)
    {
      //scope_test_TP();
      //Check if touch button DEFAULT
      if((xtouch >= 40) && (xtouch <= 90) && (ytouch >= 420) && (ytouch <= 460))//x110-160 370 420 420 460
        {
          //Highlight the button DEFAULT
          scope_restore_button(30, 410, 1);//x50//x100 360 410//550 360  //460 410
          
          //Load a default configuration
          scope_reset_config_data();
          //scope_save_config_data();
        
          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        
          //Go diagnostic info
          scope_open_diagnostic_view();//scope_diagnostic_screen();
        
          //Display the text with font 3 and the red color
          display_set_fg_color(ORANGE_COLOR);
          display_set_font(&font_3);
  
          //display_text(435, 414, "Reset calibration values !!!");
          //display_text(75, 390, "Please run BASE CALIBRATION !!!"); //335 390
          display_text(85, 355, "Settings set do default !!!"); //x90 335 
        }        
      //Check if touch button RESTORE
        else if((xtouch >= 140) && (xtouch <= 190) && (ytouch >= 420) && (ytouch <= 460))//x110-160 370 420 420 460
        {
          //Highlight the button RESTORE
          scope_restore_button(130, 410, 1);//x100 360 410//550 360  //460 410
        
          reload_cal_data = 1;
          //Set stored input calibration values   
          scope_load_input_calibration_data();
          reload_cal_data = 0;
        
          //Cancel to Input calibration
          calibrationfail = 0;

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        
          //Go diagnostic info
          scope_open_diagnostic_view();//scope_diagnostic_screen();
        
          //Display the text with font 3 and the red color
          display_set_fg_color(RED_COLOR);
          display_set_font(&font_3);
  
          //display_text(435, 414, "Reset calibration values !!!");
          display_text(60, 355, "Please run BASE CALIBRATION !!!"); //x75 335 390 
        }
        //Check if touch button RESET
        else if((xtouch >= 240) && (xtouch <= 290) && (ytouch >= 420) && (ytouch <= 460))//x210-260 470 520 420 460
        {
          //Highlight the button RESET
          scope_reset_button(230, 410, 1);//x200 460 410//550 360  //460 410
        
          //Set default channel calibration values   
          //Set default channel Input calibration values
          for(uint8 index=0;index<7;index++)
            {
                //Set default Input calibration values
                scopesettings.channel1.input_calibration[index] = signal_adjusters[index];
                scopesettings.channel2.input_calibration[index] = signal_adjusters[index];
           
                //Set FPGA center level
                scopesettings.channel1.dc_calibration_offset[index] = 860;
                scopesettings.channel2.dc_calibration_offset[index] = 860;
            }
          
          //Set default ADC compensation values
          scopesettings.channel1.adc1compensation = 0;
          scopesettings.channel1.adc2compensation = 0;
          scopesettings.channel2.adc1compensation = 0;
          scopesettings.channel2.adc2compensation = 0; 
        
          //Set default DC shift values  
          scopesettings.channel1.dc_shift_center  = 112;
          scopesettings.channel1.dc_shift_size    = 220;
          scopesettings.channel1.dc_shift_value   = 385;
    
          scopesettings.channel2.dc_shift_center  = 112;
          scopesettings.channel2.dc_shift_size    = 220;
          scopesettings.channel2.dc_shift_value   = 385;
        
          //Ready to Input calibration
          calibrationfail = 1;

          //Wait until touch is released
          tp_i2c_wait_for_touch_release();
        
          //Go diagnostic info
          scope_open_diagnostic_view();//scope_diagnostic_screen();
        }
      /*
        //Check if touch button NEWAUTOSETUP
        //else if((ytouch >= 164) && (ytouch <= 221))
        else if((xtouch >= 328) && (xtouch <= 378) && (ytouch >= 30) && (ytouch <= 60))//588 638 30 60
        {
        //Toggle the new_autosetup
        scopesettings.new_autosetup ^= 1;
          
        //Show the state (autosetup button mode)
        // scope_display_slide_button(588, 40, scopesettings.new_autosetup, GREEN_COLOR);//600 400
          
        //Go diagnostic info
        scope_open_diagnostic_view();//scope_diagnostic_screen();
          
        //Wait until touch is released
        tp_i2c_wait_for_touch_release();
        }
        //Check if touch button DC shift calibration
        //else if((ytouch >= 223) && (ytouch <= 280))
        else if((xtouch >= 328) && (xtouch <= 378) && (ytouch >= 70) && (ytouch <= 100))//588 638 70 100
        {    
        //Toggle the shift_cal
        dc_shift_cal ^= 1;
          
        //Go diagnostic info
        scope_open_diagnostic_view();//scope_diagnostic_screen();
            
        //Wait until touch is released
        tp_i2c_wait_for_touch_release(); 
        }
        //Check if touch button Boot menu
        //else if((ytouch >= 282) && (ytouch <= 339))
        else if((xtouch >= 328) && (xtouch <= 378) && (ytouch >= 110) && (ytouch <= 140))//588 638 110 140
        {
        //Toggle the Boot_menu
        tmp = (boot_menu_start & 0x04)>>2;
        tmp ^= 1;
        boot_menu_start &= 0x03;
        boot_menu_start |= tmp<<2;
            
        //Go diagnostic info
        scope_open_diagnostic_view();//scope_diagnostic_screen();
            
        //Wait until touch is released
        tp_i2c_wait_for_touch_release(); 
        }   
        
        //Check if touch button Default start
        //else if((ytouch >= 341) && (ytouch <= 398))
        else if((xtouch >= 328) && (xtouch <= 378) && (ytouch >= 150) && (ytouch <= 180))
        {    
          
        //Highlight the button Default start
        scope_boot_button(328, 160); //588 160 1
        
        //Toggle the Default start
        tmp = (boot_menu_start & 0x03);
        if (tmp < 2) tmp++; else tmp = 0;  
        boot_menu_start &= 0x04;
        boot_menu_start |= tmp;
               
        //Go diagnostic info
        scope_open_diagnostic_view();//scope_diagnostic_screen();
            
        //Wait until touch is released
        tp_i2c_wait_for_touch_release(); 
        } 
        //Check if touch button lock cursors
        else if((xtouch >= 328) && (xtouch <= 378) && (ytouch >= 190) && (ytouch <= 220))//588 638 70 100
        {    
        //Toggle the lock cursors
        scopesettings.lockcursors ^= 1;
                  
        //Go diagnostic info
        scope_open_diagnostic_view();//scope_diagnostic_screen();
            
        //Wait until touch is released
        tp_i2c_wait_for_touch_release(); 
        }
      */
      //**--------------------------------------------------------
      //Check if touch EXIT button
      else if((xtouch >= 730) && (xtouch <= 780) && (ytouch >= 430) && (ytouch <= 460))   //700 410 //752 440
        {
            //Button for EXIT
            scope_exit_button(710, 410, 1);
            //Touched the text field so wait until touch is released and then quit
            tp_i2c_wait_for_touch_release();

            break;
        }
        
    }

    //Need to wait for touch to release before checking again
    tp_i2c_wait_for_touch_release();
  } //end while
  
  //------------- save settings diagnostic menu ------------------------
  //if(boot_menu_start == 1) boot_menu_start = 0x05;              //fnirsi firmware always with boot menu
  //ptr[0] = boot_menu_start;
  //SAVE the display configuration sector from DRAM to SDcart   //save boot menu and default start
  //sd_card_write(DISPLAY_CONFIG_SECTOR, 1, (uint8 *)0x81BFFC00);
        //-------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 handle_confirm_delete(void)
{
  int32 choice = 0;

  //Save the screen rectangle where the menu will be displayed
  display_copy_rect_from_screen(310, 192, 180, 96);

  //display the confirm delete menu
  //Draw the background in gray
  display_set_fg_color(0x00202020);
  display_fill_rounded_rect(310, 192, 179, 95, 3);//180 96

  //Draw the border in a lighter gray
  display_set_fg_color(LIGHTGREY_COLOR);
  display_draw_rounded_rect(310, 192, 179, 95, 3);//180 96

  //Draw the buttons
  display_fill_rounded_rect(320, 228, 74, 50, 2);
  display_fill_rounded_rect(405, 228, 74, 50, 2);

  //White color for text and use font_3
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);
  display_text(340, 204, "Confirm to delete?");//204
  display_text(348, 245, "NO");//246
  display_text(431, 245, "YES");//246

  //wait for touch
  while(1)
  {
    //Read the touch panel status
    tp_i2c_read_status();

    //Check if the panel is touched
    if(havetouch)
    {
      //Check if touch is on "NO"
      if((xtouch >= 324) && (xtouch <= 390) && (ytouch >= 230) && (ytouch <= 276))
      {
        //Highlight the button
        display_set_fg_color(ITEM_ACTIVE_COLOR);
        display_fill_rounded_rect(320, 228, 74, 50, 2);
        
        //With the text in black
        display_set_fg_color(BLACK_COLOR);
        display_text(348, 246, "NO");
        
        //Set the chosen option
        choice = VIEW_CONFIRM_DELETE_NO;

        //Done so quit the loop
        break;
      }
      //Else check if touch on "YES"
      else if((xtouch >= 409) && (xtouch <= 475) && (ytouch >= 230) && (ytouch <= 276))
      {
        //Highlight the button
        display_set_fg_color(ITEM_ACTIVE_COLOR);
        display_fill_rounded_rect(405, 228, 74, 50, 2);
        
        //With the text in black
        display_set_fg_color(BLACK_COLOR);
        display_text(431, 246, "YES");
        
        //Set the chosen option
        choice = VIEW_CONFIRM_DELETE_YES;

        //Done so quit the loop
        break;
      }

      //Need to wait for touch to release before checking again
      tp_i2c_wait_for_touch_release();
    }
  }

  //Need to wait for touch to release before returning
  tp_i2c_wait_for_touch_release();

  //Restore the original screen
  display_copy_rect_to_screen(310, 192, 180, 96);

  //return the choice
  return(choice);
}

//----------------------------------------------------------------------------------------------------------------------------------

void close_open_menus(uint32 closemain)
{
  //uint8 tmp;                            //for diagnostic menu
  uint32 *ptr = STARTUP_CONFIG_ADDRESS; //for diagnostic menu
  //boot_menu_start = ptr[0] & 0x07;      //choice 1 for fnirsi firmware, 0 for peco firmware, 2 for FEL mode, 4 view choice menu
  
  //Only one of these menu's will be opened at any time

  //Check if the screen brightness slider is open
  if(screenbrightnessopen)
  {
    //Set the button inactive
    scope_system_settings_screen_brightness_item(0);

    //Restore the screen under the screen brightness slider
    display_set_source_buffer(displaybuffer3);
    display_copy_rect_to_screen(S_BRIGHTNESS_MENU_XPOS, S_BRIGHTNESS_MENU_YPOS, S_BRIGHTNESS_MENU_WIDTH, S_BRIGHTNESS_MENU_HEIGHT);

    //Signal it is closed
    screenbrightnessopen = 0;
  }
  //Check if the grid brightness slider is open
  else if(gridbrightnessopen)
  {
    //Set the button inactive
    scope_system_settings_grid_brightness_item(0);

    //Restore the screen under the grid brightness slider
    display_set_source_buffer(displaybuffer3);
    display_copy_rect_to_screen(G_BRIGHTNESS_MENU_XPOS, G_BRIGHTNESS_MENU_YPOS, G_BRIGHTNESS_MENU_WIDTH, G_BRIGHTNESS_MENU_HEIGHT);

    //Signal it is closed
    gridbrightnessopen = 0;
  }
  //Check if the other settings menu is open
  else if(othersettingsopen)
  {
    //Set the button inactive
    scope_system_settings_other_settings_item(0);
    //scope_system_settings_RTC_settings_item(0);

    //Restore the screen under the other_settings_menu
    display_set_source_buffer(displaybuffer3);
    display_copy_rect_to_screen(OTHER_MENU_XPOS, OTHER_MENU_YPOS, OTHER_MENU_WIDTH, OTHER_MENU_HEIGHT);
    
    //----------------------------------- save BOOT settings  ------------------------------------------
    if(boot_menu_start == 1) boot_menu_start = 0x05;                      //fnirsi firmware always with boot menu
    if (dev_mode == 0) {if(boot_menu_start > 0) boot_menu_start |= 0x04;} //fnirsi and FEL firmware always with boot menu for user
    ptr[0] = boot_menu_start;
    //SAVE the display configuration sector from DRAM to SDcart   //save boot menu and default start
    sd_card_write(DISPLAY_CONFIG_SECTOR, 1, (uint8 *)0x81BFFC00);
    //---------------------------------------------------------------------------------------------------
   
    //Signal it is closed
    othersettingsopen = 0;
  }
  //Check if the calibration text is open
  else if(calibrationopen)
  {
    //Set the button inactive
    scope_system_settings_calibration_item(0);

    //Restore the screen under the calibration text
    display_set_source_buffer(displaybuffer3);
    display_copy_rect_to_screen(CALIBRATION_MENU_XPOS, CALIBRATION_MENU_YPOS, CALIBRATION_MENU_WIDTH, CALIBRATION_MENU_HEIGHT);

    //Signal it is closed
    calibrationopen = 0;
  }
  //Check if the RTC settings menu is open
  else if(RTCsettingsopen)
  {
    //Set the button inactive
    scope_system_settings_RTC_settings_item(0);

    //Restore the screen under the RTC_settings_menu
    display_set_source_buffer(displaybuffer3);
    display_copy_rect_to_screen(RTC_MENU_XPOS, RTC_MENU_YPOS, RTC_MENU_WIDTH, RTC_MENU_HEIGHT);    //? display_draw_rect(395, 106, 200, 353)
   
    //Signal it is closed
    RTCsettingsopen = 0;
  }

  //Check if system settings menu has been opened and needs to be closed
  if(closemain && systemsettingsmenuopen)
  {
    //Restore the screen under the system settings menu when done
    display_set_source_buffer(displaybuffer2);
    display_copy_rect_to_screen(SYSTEM_MENU_XPOS, SYSTEM_MENU_YPOS, SYSTEM_MENU_WIDTH, SYSTEM_MENU_HEIGHT);//353

    //Clear the flag so it will be opened next time
    systemsettingsmenuopen = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void close_open_channel_menus(PCHANNELSETTINGS settings, uint32 closemain)
{
  //Only one of these menu's will be opened at any time

  //Check if the ref menu is open
  if(refmenuopen)
  {
    //Set the button inactive
    scope_channel_ref_menu_item(settings, 0);

    //Restore the screen under the ref menu
    display_set_source_buffer(displaybuffer2);
    display_copy_rect_to_screen(settings->menuxpos + CH_REF_MENU_XPOS, CH_REF_MENU_YPOS, CH_REF_MENU_WIDTH, CH_REF_MENU_HEIGHT);

    //Signal it is closed
    refmenuopen = 0;
  }
  //Check if the math menu is open
  else if(mathmenuopen)
  {
    //Set the button inactive
    scope_channel_math_menu_item(settings, 0);

    //Restore the screen under the math menu
    display_set_source_buffer(displaybuffer2);
    display_copy_rect_to_screen(settings->menuxpos + CH_MATH_MENU_XPOS, CH_MATH_MENU_YPOS, CH_MATH_MENU_WIDTH, CH_MATH_MENU_HEIGHT);

    //Signal it is closed
    mathmenuopen = 0;
  }

  //Check if channel settings menu has been opened and needs to be closed
  if(closemain && channelmenuopen)
  {
    //Restore the screen under the system settings menu when done
    display_set_source_buffer(displaybuffer1);
    display_copy_rect_to_screen(settings->menuxpos, CH_MENU_YPOS, CH_MENU_WIDTH, CH_MENU_HEIGHT);;//353

    //Clear the flag so it will be opened next time
    channelmenuopen = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_trigger_point_position(void)
{
  int32 diff;
  int32 position;
  
  int32 positionlimitL;
  int32 positionlimitH;

  //Calculate the distance to move the setting with
  diff = xtouch - previousxtouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_trigger_point_position + diff;
  
  positionlimitH = (((1500        ) * 727.0) / (727.0 / disp_xpos_per_sample))+15;
  positionlimitL = (((1500 - 3000 ) * 727.0) / (727.0 / disp_xpos_per_sample));

  //Limit it on the trace portion of the screen
  
  if(position > positionlimitH)//712
  {
    //And not above 712;
    position = positionlimitH;
  }
        
  if(position < positionlimitL)//2
  {
    //So not below 2
    position = positionlimitL;
  }
  
  //Update the current position
  scopesettings.triggerhorizontalposition = position;
  
  //if(!scopesettings.long_mode || scopesettings.triggermode) scopesettings.triggerhorizontalposition = position;
  
  //Skope mode & long time base & no auto mode - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode && scopesettings.triggermode) scope_preset_values();  
 
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//******************************************************************************
/*
void change_channel_1_HW_offset(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with based on the display mode
  if(scopesettings.xymodedisplay == 0)
  {
    //For normal display use y displacement
    diff = ytouch - previousytouch;
  }
  else
  {
    //For x-y mode display use x displacement
    diff = previousxtouch - xtouch;
  }

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_channel_1_offset - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.channel1.traceposition = position;

  //Write it to the FPGA
  fpga_set_channel_offset(&scopesettings.channel1);
}
*/
//**************************************************************************************
//-----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void change_channel_1_offset(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with based on the display mode
  if(scopesettings.xymodedisplay == 0)
  {
    //For normal display use y displacement
    diff = ytouch - previousytouch;
  }
  else
  {
    //For x-y mode display use x displacement
    diff = previousxtouch - xtouch;
  }

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_channel_1_offset - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }
    //position = 396-position;//add********
  //Update the current position
  scopesettings.channel1.traceposition = position;
  
  //Write it to the FPGA
  fpga_set_channel_offset(&scopesettings.channel1); 
  
  //Skope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values();
}

//----------------------------------------------------------------------------------------------------------------------------------

void change_channel_2_offset(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_channel_2_offset - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.channel2.traceposition = position;

  //Write it to the FPGA
  fpga_set_channel_offset(&scopesettings.channel2); 
  
  //Scope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values(); 
}

//----------------------------------------------------------------------------------------------------------------------------------

void change_trigger_level_offset(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_trigger_level_offset - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.triggerverticalposition = position;
  
  //if(!scopesettings.long_mode || scopesettings.triggermode) scopesettings.triggerverticalposition = position;
  
  //Skope mode & long time base & no auto mode - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode && scopesettings.triggermode) scope_preset_values();
  
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_left_time_cursor_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = xtouch - previousxtouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_left_time_cursor_position + diff;

  //Limit it on the trace portion of the screen and the right time cursor
  if(position < 3)
  {
    //So not below 3
    position = 3;
  }
  else if(position >= scopesettings.timecursor2position)
  {
    //And not right of the right cursor;
    position = scopesettings.timecursor2position - 1;
  }

  //Update the current position
  scopesettings.timecursor1position = position;
  
  //Skope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values(); 
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_right_time_cursor_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = xtouch - previousxtouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_right_time_cursor_position + diff;

  //Limit it on the trace portion of the screen and the left cursor
  if(position <= scopesettings.timecursor1position)
  {
    //So not left of the left cursor
    position = scopesettings.timecursor1position + 1;
  }
  else if(position > 726)//722
  {
    //And not beyond end of the screen;
    position = 726;//722
  }

  //Update the current position
  scopesettings.timecursor2position = position;
  
  //Skope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values();  
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_top_volt_cursor_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_top_volt_cursor_position + diff;//32

  //Limit it on the trace portion of the screen and not below the bottom cursor
  if(position < 49)//47  46
  {
    //So not below 47
    position = 49;//47
  }
  else if(position >= scopesettings.voltcursor2position)
  {
    //And not below the bottom cursor;
    position = scopesettings.voltcursor2position - 1;
  }

  //Update the current position
  scopesettings.voltcursor1position = position;
  
  //Skope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values(); 
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_bottom_volt_cursor_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_bottom_volt_cursor_position + diff;

  //Limit it on the trace portion of the screen and not above the top cursor
  if(position <= scopesettings.voltcursor1position)
  {
    //So not above the top cursor
    position = scopesettings.voltcursor1position + 1;
  }
  else if(position > 449)//447
  {
    //And not above 447;
    position = 449;
  }

  //Update the current position
  scopesettings.voltcursor2position = position;
  
  //Scope mode & long time base - set other values and clear view area
  if((!scopesettings.waveviewmode) && scopesettings.long_mode) scope_preset_values(); 
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref1_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref1_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref1.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref2_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref2_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref2.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref3_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref3_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref3.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref4_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref4_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref4.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref5_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref5_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref5.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref6_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref6_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref6.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref7_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref7_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref7.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void move_ref8_position(void)
{
  int32 diff;
  int32 position;

  //Calculate the distance to move the setting with
  diff = ytouch - previousytouch;

  //Make it based on move speed
  if(scopesettings.movespeed)
  {
    //For slow divide by 5
    diff /= 5;
  }

  //Calculate the new position
  position = (int32)previous_ch_ref8_position - diff;

  //Limit it on the trace portion of the screen
  if(position < 6)
  {
    //So not below 6
    position = 6;
  }
  else if(position > 396)//394
  {
    //And not above 394;
    position = 396;//394
  }

  //Update the current position
  scopesettings.ch_ref8.traceposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------

void match_volt_per_div_settings(PCHANNELSETTINGS settings)
{
  //Check if the sample volts per division differs from the display volts per division setting
  if(settings->samplevoltperdiv != settings->displayvoltperdiv)
  {
    //Match them if so and change them in the FPGA
    settings->samplevoltperdiv = settings->displayvoltperdiv;

    //Set the volts per div for this channel
    fpga_set_channel_voltperdiv(settings);

    //Since the DC offset is influenced set that too
    fpga_set_channel_offset(settings);

    //Wait 50ms to allow the circuit to settle
    timer0_delay(50);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void set_trigger_mode(uint32 mode)
{
  //Set the new mode
  scopesettings.triggermode = mode;

  //Set the new mode in the hardware
  fpga_set_trigger_mode();

  //Make sure the scope is running
  scopesettings.runstate = 1;//ok

  //Show this on the screen
  scope_run_stop_text();
  
  //Check in which state the right menu is in
  //Button back to inactive state
  if(scopesettings.rightmenustate == 0) scope_run_stop_button(0);

  //Match the volts per division settings for both channels
  //Is needed when vertical zoom has been used
  match_volt_per_div_settings(&scopesettings.channel1);
  match_volt_per_div_settings(&scopesettings.channel2);
  
  //Display this
  scope_trigger_mode_select();
  scope_trigger_settings(0);

}

//----------------------------------------------------------------------------------------------------------------------------------
