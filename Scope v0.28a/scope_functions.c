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
#include "ref_and_math.h"
/*
//sinx
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//endsinx
*/
#include <string.h>
#include <math.h>
//----------------------------------------------------------------------------------------------------------------------------------

//void scope_adjust_timebase(void)
void scope_adjust_timebase_and_voltdiv (void)
{
  uint8 min;
  uint8 max;
    
  //Check if touch within the trace display region
  if((previousxtouch > 2) && (previousxtouch < 720) && (previousytouch > 50) && (previousytouch < 470))
  {

  
    if ((scopesettings.long_mode) && (scopesettings.waveviewmode)) {min = 0;   max = 10;}
    if ((!scopesettings.long_mode) &&(scopesettings.waveviewmode)) {min = 11;  max = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1); }
    if(!scopesettings.waveviewmode) {min = 0;  max = ((sizeof(time_div_texts) / sizeof(int8 *)) - 1); }
      
      //Check if touch on the left of the center line
    //if(previousxtouch < 358)
    if(previousxtouch < 358)
    {   
      if((previousytouch > 100)&&(previousytouch < 370))
      {
        //Short time base
        //Check if not already on the highest setting (200mS/div)
        //Long time base
        //Check if not already on the highest setting (50s/div)
        if(scopesettings.timeperdiv > min)
        {
          //Go up in time by taking one of the setting
          scopesettings.timeperdiv--;
        }
      }
      
      if(previousytouch < 100)
      { 
        //Only if channel is enabled
        if(scopesettings.channel1.enable)
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
      }
      
      if(previousytouch > 370)
      {
       //Only if channel is enabled
       if(scopesettings.channel1.enable)
       {
         //Check if setting not already on min
         if(scopesettings.channel1.displayvoltperdiv > 0)
         {
             //Step down to the next setting. (Increasing the setting)
             scopesettings.channel1.displayvoltperdiv--;

             //Show the change on the screen
             scope_channel_settings(&scopesettings.channel1, 0);

             //Only update the FPGA in run mode
             //For waveform view mode the stop state is forced and can't be changed
             if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel1);
         }
       }
      }

    }
    //Check if touch on the right of the center line
    else if(previousxtouch > 362)
    { 
      if((previousytouch > 100)&&(previousytouch < 370))
      { //Long time base
        //Check if not already on the lowest setting (100ms/div)
        //Short time base
        //Check if not already on the lowest setting (5nS/div)
        if(scopesettings.timeperdiv < max)
        {
          //Go down in time by adding one to the setting
          scopesettings.timeperdiv++;
        }
      }
      
      if(previousytouch < 100)
      { 
        //Only if channel is enabled
        if(scopesettings.channel2.enable)
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
      }
      
      if(previousytouch > 370)
      {
       //Only if channel is enabled
       if(scopesettings.channel2.enable)
       {
         //Check if setting not already on min
         if(scopesettings.channel2.displayvoltperdiv > 0)
         {
             //Step down to the next setting. (Increasing the setting)
             scopesettings.channel2.displayvoltperdiv--;

             //Show the change on the screen
             scope_channel_settings(&scopesettings.channel2, 0);

             //Only update the FPGA in run mode
             //For waveform view mode the stop state is forced and can't be changed
             if(scopesettings.runstate) match_volt_per_div_settings(&scopesettings.channel2);
         }
       }
      }
    }
  
    scope_set_timebase();
    
  }
  
}
    
//----------------------------------------------------------------------------------------------------------------------------------
  
  void scope_set_timebase(void)
  {
     //Scope mode, long base skip 20 & 50ms/div
    if(!scopesettings.waveviewmode) 
    {   
        if(scopesettings.timeperdiv == 9)  {scopesettings.timeperdiv = 11;scopesettings.long_mode = 0;}      //9 11
        if(scopesettings.timeperdiv == 10) {scopesettings.timeperdiv = 8;scopesettings.long_mode = 1;}       //10 8
    } 
 
    //Long time base
    if (scopesettings.timeperdiv < 11)  //9
    {   scopesettings.long_mode = 1;
        //Send the time base command for the long time base
        fpga_set_long_timebase(scopesettings.timeperdiv);
    }else 
    {   //Short time base
        scopesettings.long_mode = 0;
        scopesettings.display_data_done = 1;
        //Send the time base command for the sort time base
        fpga_set_time_base(scopesettings.timeperdiv);
    }

    //For time per div set with tapping on the screen the direct relation between the time per div and the sample rate is set
    //but only when the scope is running and set short time base. Otherwise the sample rate of the acquired buffer still is valid.
    //if((scopesettings.runstate))    //ked bezi urob
    //if((scopesettings.runstate)||(scopesettings.triggermode == 1))//ok //ked bezi urob
    //if(scopesettings.runstate)   
    //ak niesme v view mode tak urob
    //if(!scopesettings.waveviewmode) 
    if(((previousytouch < 240)&&(!scopesettings.waveviewmode)&&(scopesettings.triggermode == 1))||(scopesettings.long_mode)||(scopesettings.runstate))  //switch all sample and time per div
    {
        
        //Set the sample rate that belongs to the selected time per div setting
        scopesettings.samplerate = time_per_div_sample_rate[scopesettings.timeperdiv];  
  
        //Set the new setting in the FPGA
        fpga_set_sample_rate(scopesettings.samplerate);
    
        if((scopesettings.triggermode == 1)) //if single mode
        {   //one shot 
            scopesettings.runstate=1; 
            //Display the changed state
            scope_run_stop_text();
        }
      //nebezi nemaz obrazovku
      //if(scopesettings.runstate) //ok
    //if((scopesettings.triggermode != 1)||(scopesettings.long_mode)) scope_preset_values(); 
        scope_preset_values(); 
    }
    

    
    //if((scopesettings.triggermode == 1)) 
        //scope_display_trace_data();
  
    //Show he new setting on the display
    scope_acqusition_settings(0);
}
//----------------------------------------------------------------------------------------------------------------------------------
 
  void scope_preset_values(void)
{
    //Use a separate buffer to clear the screen
    display_set_screen_buffer(displaybuffertmp);
  
    //Clear the trace portion of the screen
    display_set_fg_color(BLACK_COLOR);
    display_fill_rect(0, 48, 730, 432); //display_fill_rect(2, 48, 728, 432);
    
  if(scopesettings.long_mode) //if set long time base, no drawn.
  {
      
    scopesettings.xpos = 4;
    scopesettings.lastx = 3;
    scopesettings.count = 0;
    
    //scopesettings.lastx=726;
    
    memset(channel1tracebuffer, 128, sizeof(channel1tracebuffer));
    memset(channel2tracebuffer, 128, sizeof(channel2tracebuffer));
    
    
    //Auto mode - set flag, signal is triggered=1 
    if(scopesettings.triggermode) triggerlong = 0; else triggerlong = 1;
    
    //y position for the channel 1 trace center pointer.
    scopesettings.channel1.sample1 = 442 - scopesettings.channel1.traceposition;//441
    //y position for the channel 2 trace center pointer.
    scopesettings.channel2.sample1 = 442 - scopesettings.channel2.traceposition;//441  
    
  } 
  else //if((scopesettings.runstate))//1-run
  {
    if((scopesettings.runstate)||(!scopesettings.waveviewmode) )//ok 1-run  1 - wavwe mode opravit?
    { 
        memset(channel1tracebuffer, 128, sizeof(channel1tracebuffer));
        memset(channel2tracebuffer, 128, sizeof(channel2tracebuffer));
    }
    
  scope_display_trace_data();
  }
    
    //Check if not in waveform view mode (Scope mode) with grid disabled
    if((!scopesettings.waveviewmode) || (scopesettings.gridenable))
    {
        //Draw the grid lines and dots based on the grid brightness setting
        scope_draw_grid();
    }
    
    //Draw the cursors with their measurement displays
    scope_draw_time_cursors();
    scope_draw_volt_cursors();
    scope_display_cursor_measurements();
    
    //Draw the signal center, trigger level and trigger position pointers
    scope_draw_pointers();
     
    //Copy it to the actual screen buffer
    display_set_source_buffer(displaybuffertmp);//1
    display_set_screen_buffer((uint16 *)maindisplaybuffer);
    display_copy_rect_to_screen(0, 48, 730, 432);  //44 728 434  
}
  
//----------------------------------------------------------------------------------------------------------------------------------

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
      position = 442 - scopesettings.channel1.traceposition;//441

      //Limit on the top of the displayable region
      if(position < 49)//44
      {
        position = 49;
      }
      //Limit on the bottom of the displayable region
      else if(position > 442)//441
      {
        position = 442;
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
    position = 442 - scopesettings.channel2.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(CHANNEL2_COLOR);
    display_set_bg_color(BLACK_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_pointer(2, position, '2');
  }
  //----------------------------------------------------------------
  
  //Draw ref1 pointer when it is enabled
  if(scopesettings.ref1)
  {
    //y position for the ref 1 trace center pointer
    position = 442 - scopesettings.ch_ref1.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref1.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '1');
  }
  
  //Draw ref2 pointer when it is enabled
  if(scopesettings.ref2)
  {
    //y position for the ref 2 trace center pointer
    position = 442 - scopesettings.ch_ref2.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref2.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '2');
  }
  
  //Draw ref3 pointer when it is enabled
  if(scopesettings.ref3)
  {
    //y position for the ref 3 trace center pointer
    position = 442 - scopesettings.ch_ref3.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref3.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '3');
  }
  
  //Draw ref4 pointer when it is enabled
  if(scopesettings.ref4)
  {
    //y position for the ref 4 trace center pointer
    position = 442 - scopesettings.ch_ref4.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref4.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '4');
  }
  
  //Draw ref5 pointer when it is enabled
  if(scopesettings.ref5)
  {
    //y position for the ref 1 trace center pointer
    position = 442 - scopesettings.ch_ref5.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref5.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '5');
  }
  
  //Draw ref6 pointer when it is enabled
  if(scopesettings.ref6)
  {
    //y position for the ref 2 trace center pointer
    position = 442 - scopesettings.ch_ref6.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref6.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '6');
  }
  
  //Draw ref7 pointer when it is enabled
  if(scopesettings.ref7)
  {
    //y position for the ref 3 trace center pointer
    position = 442 - scopesettings.ch_ref7.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref7.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '7');
  }
  
  //Draw ref8 pointer when it is enabled
  if(scopesettings.ref8)
  {
    //y position for the ref 4 trace center pointer
    position = 442 - scopesettings.ch_ref8.traceposition;//441

    //Limit on the top of the displayable region
    if(position < 49)//44
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Set the colors for drawing
    display_set_fg_color(scopesettings.ch_ref8.color);
    display_set_bg_color(WHITE_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_0);

    //Draw the pointer
    display_left_REF_pointer(2, position, '8');
  }
  
  //----------------------------------------------------------------

  //Need to think about trigger position in 200mS - 20mS/div settings. Not sure if they work or need to be done in software
  //The original scope does not show them for 50mS and 20mS/div

  //Draw trigger position and level pointer when in normal display mode & sortmode or (long mode with single & normal trigger)
  if((scopesettings.xymodedisplay == 0)&&((!scopesettings.long_mode)||(scopesettings.triggermode)))
  {
    //Set the colors for drawing
    if(disp_have_trigger == 1) display_set_fg_color(RED_COLOR); 
    else display_set_fg_color(TRIGGER_COLOR);
    display_set_bg_color(BLACK_COLOR);

    //Select the font for this pointer id
    display_set_font(&font_3); 
    
    //x position for the trigger position pointer
    position = scopesettings.triggerhorizontalposition;

    //Limit on the left of the displayable region
    if(position < 3)//3
    { //Out of limit on the left of the displayable region
      display_right_pointer(3, 48, 'H');//45
    }
    //Limit on the right of the displayable region
    else if(position > 713)
    {//Out of limit on the right of the displayable region
      display_left_pointer(706, 48, 'H');//45
    }
    else 
    //Draw the pointer
    display_top_pointer(position, 48, 'H');//47 45
    
    //-----------------------------------------------------
    //y position for the trigger level pointer
    position = 442 - scopesettings.triggerverticalposition;//441

    //Limit on the top of the displayable region
    if(position < 49)
    {
      position = 49;
    }
    //Limit on the bottom of the displayable region
    else if(position > 442)//441
    {
      position = 442;
    }

    //Need to reset the fore ground color
    //Select the channel based on the current trigger color
    if(scopesettings.triggerchannel == 0) display_set_fg_color(CHANNEL1_COLOR);
            else display_set_fg_color(CHANNEL2_COLOR);   
    

    //Draw the pointer
    display_right_pointer(707, position, 'T');
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_calculate_trigger_vertical_position()
{
  //PCHANNELSETTINGS settings;
  int32 position;
    //uint8  displayvoltperdiv;

  //Select the channel based on the current trigger channel
  if(scopesettings.triggerchannel == 0)
  {
    //settings = &scopesettings.channel1;
     position = scopesettings.channel1.triggerverticalposition;  
     //displayvoltperdiv = scopesettings.channel1.displayvoltperdiv;
  }
  else
  {
    //settings = &scopesettings.channel2;
     position = scopesettings.channel2.triggerverticalposition;
     //displayvoltperdiv = scopesettings.channel2.displayvoltperdiv;
  }

//position = position*displayvoltperdiv;
  //Center the trigger level around 0 point
 // position = scopesettings.triggerlevel - 128;

  //Adjust it for the current volt per div setting
//  position = (position * settings->input_calibration[settings->displayvoltperdiv]) >> VOLTAGE_SHIFTER;

  //Add the trace center to it
  //position = settings->traceposition + position;

  //Limit to extremes
  if(position < 0)
  {
    position = 0;
  }
  else if(position > 401)
  {
    position = 401;
  }

  //Set as new position
  scopesettings.triggerverticalposition = position;
}

//----------------------------------------------------------------------------------------------------------------------------------
    
void scope_acquire_trace_data(void)
{
  uint32 data;
  
  if((scopesettings.runstate)&&(scopesettings.display_data_done)&&(touchstate == 0))//runstate ok
  //if((scopesettings.runstate) && (scopesettings.display_data_done))//runstate ok
  {
    //Set the trigger level
    fpga_set_trigger_level();

    //Write the time base setting to the FPGA
    fpga_set_time_base(scopesettings.timeperdiv);

    //Sampling with trigger circuit enabled (standard memory mode)
    //if(scopesettings.long_memory) scopesettings.samplemode = 0; else scopesettings.samplemode = 1;

    //Start the conversion
    fpga_do_conversion();
    
    //Flag conversion in progres
    scopesettings.display_data_done = 0;
    
        //while(fpga_done_conversion()==0);
        //timerHOLD=timerH;
        //while(timerHOLD);
        //fpga_do_conversion();
  }
  
  //Check until conversion done or touch panel active
  if((fpga_done_conversion())&&(scopesettings.runstate)&&(touchstate == 0)) 
  {
    //Check if in single mode
    if(scopesettings.triggermode == 1)
    {
      //Switch to stopped
      scopesettings.runstate = 0;//ok

      //Show this on the screen
      scope_run_stop_text();
      
      //Check in which state the right menu is in
      //Button back to inactive state
      if(scopesettings.rightmenustate == 0) scope_run_stop_button(0);
    }

    //Get trigger point information
    //Later on used to send to the FPGA with command 0x1F
    data = fpga_prepare_for_transfer();

    //Just using the same calculation for every setting solves the frequency calculation error
    //The signal representation still is correct and the trigger point seems to be more valid also
    //The original uses time base dependent processing here, but this seems to do the trick on all ranges
    //The software needs to verify the trigger to make it more stable
   
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 40);  //x , y , sirka, vyska
      //display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      display_decimal(550, 10, data);
    
    /*
    if(data < 750)
    {
      //Less then 750 make it bigger
      //data = data + 3345;
      data = data + 7441;
    }
    else
    {
      //More then 750 make it smaller
      data = data - 750; 
    }
    */
     // scopesettings.settriggerpoint=4096;
    
    if(data < (scopesettings.settriggerpoint/2))
    {
      //Less then 750 make it bigger
      //data = data + 3345;
      data = data + 7441;
    }
    else
    {
      //More then 750 make it smaller
      data = data - (scopesettings.settriggerpoint/2); 
    }       
            
    
    //display_decimal(550, 20, data);

/*
      display_text(550, 10, "data"); 
      display_decimal(600, 10, data);
      display_text(550, 20, "timerH"); 
      display_decimal(600, 20, timerH);
      
    */
        
    //if(scopesettings.long_memory) data=10;
   
    //Only need a single count variable for both channels, since they run on the same sample rate
    //This can be changed to a global define
    scopesettings.nofsamples  = SAMPLES_PER_ADC;//5000
    scopesettings.samplecount = SAMPLE_COUNT;//10000

    //Check if channel 1 is enabled
    if(scopesettings.channel1.enable)
    {
      //Get the samples for channel 1
      fpga_read_sample_data(&scopesettings.channel1, data);

      //Check if always 50% trigger is enabled and the trigger is on this channel
      if(scopesettings.alwaystrigger50 && (scopesettings.triggerchannel == 0))
      {
        //Use the channel 1 center value as trigger level  
        scope_do_50_percent_trigger_setup();
      }
    }

    //Check if channel 2 is enabled
    if(scopesettings.channel2.enable)
    {
      //Get the samples for channel 2
      fpga_read_sample_data(&scopesettings.channel2, data);

      //Check if always 50% trigger is enabled and the trigger is on this channel
      if(scopesettings.alwaystrigger50 && scopesettings.triggerchannel)
      {
        //Use the channel 2 center value as trigger level
        scope_do_50_percent_trigger_setup();
      }
    }

    //Need to improve on this for a more stable displaying. On the low sample rate settings it seems to flip between two positions.
    //Determine the trigger position based on the selected trigger channel
    //scope_process_trigger(scopesettings.nofsamples); 
    scope_process_trigger(scopesettings.settriggerpoint); //1500
    //disp_have_trigger = 0;
    
    //flag data displayed, 1-for next conversion ready
    scopesettings.display_data_done = 1; 
    //flag next conversion for long memory mode
   
    
    //Display the trace data
  //if((scopesettings.samplemode == 0)||(!scopesettings.long_memory)) 
   // scope_display_trace_data(); //ak nebude dobre, dat do trace data podmienku pre vymazanie adoverload az ked data done =1
  }
  //Display the trace data
  //if((scopesettings.samplemode == 0)||(!scopesettings.long_memory)) 
      scope_display_trace_data();
}

//----------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------

void scope_process_trigger(uint32 count)
{
  uint8  *buffer;
  uint32  index;
  uint32  level = scopesettings.triggerlevel;
  uint32  sample1;
  uint32  sample2;

  //Select the trace buffer to process based on the trigger channel
  if(scopesettings.triggerchannel == 0)
  {
    //Channel 1 buffer
    buffer = (uint8 *)channel1tracebuffer;
  }
  else
  {
    //Channel 2 buffer
    buffer = (uint8 *)channel2tracebuffer;
  }

   disp_have_trigger = 0;
  
  //Set a starting point for checking on trigger
  //Count is half a sample buffer!!
  index = count - 20; // 6  //10 bolo
  count = 40;   //5 20
  
  //Need a better check here, maybe over a wider range of samples
  while(count--)
  {
    sample1 = buffer[index];
    sample2 = buffer[index + 1];

    if(((scopesettings.triggeredge == 0) && (sample1 < level) && (sample2 >= level)) ||
       ((scopesettings.triggeredge == 1) && (sample1 >= level) && (sample2 < level)))

    {
      //Set the current index as trigger point
      disp_trigger_index = index;

      //Signal trigger has been found
      disp_have_trigger = 1;

      //Done with checking
      break;
    }

    //Select next sample to check
    index--;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

uint32 scope_do_baseline_calibration(void)
{
  uint32 flag = 1;
  uint32 voltperdiv;
  uint8 backup_invert_ch1;
  uint8 backup_invert_ch2;
  //uint8 backup_samplerate;
  //uint8 backup_timeperdiv; 

  //Disable the trigger circuit
  scopesettings.samplemode = 0;

  //Switch back to AUTO trigger mode to avoid lockup in NORMAL or SINGLE mode
  scopesettings.triggermode = 0;
  
  //Show it on the display
  scope_trigger_settings(0);
  
  //Also set the FPGA to the AUTO trigger mode
  fpga_set_trigger_mode();
  
  //Set number of samples
  //This can be changed to a global define
  scopesettings.samplecount = SAMPLE_COUNT;
  scopesettings.nofsamples  = SAMPLES_PER_ADC;

  //Send the command for setting the trigger level to the FPGA
  fpga_write_cmd(0x17);
  fpga_write_byte(0);
  
  //Sample rate ant time per div settings backup
  //backup_samplerate = scopesettings.samplerate;
  //backup_timeperdiv = scopesettings.timeperdiv;
  
  //Invert channel settings backup and OFF
  backup_invert_ch1 = scopesettings.channel1.invert;
  backup_invert_ch2 = scopesettings.channel2.invert;
  scopesettings.channel1.invert = 0;
  scopesettings.channel2.invert = 0;
  
  //Set offset to 0
  scopesettings.channel1.dcoffset = 0;
  scopesettings.channel2.dcoffset = 0;

  //Clear the compensation values before doing the calibration
  calibrationsettings.adc1compensation = 0;
  calibrationsettings.adc2compensation = 0;

  //Setup for channel 1 calibration
  calibrationsettings.voltperdivcommand = 0x33;
  calibrationsettings.offsetcommand     = 0x32;
  calibrationsettings.adc1command       = 0x20;
  calibrationsettings.adc2command       = 0x21;

  //Use the trace buffer of this channel
  calibrationsettings.tracebuffer = (uint8 *)channel1tracebuffer;
  
  display_set_font(&font_3);
  //Display the text in channel color
  display_set_fg_color(CHANNEL1_COLOR);
  display_text(409, 255, "Ch1....");
  
  //Calibrate this channel
  flag &= scope_do_channel_calibration(); 

  //Check if calibration was successful
  if(flag)
  { 
    //Display the text in channel color
    display_set_fg_color(GREEN_COLOR); 
    display_text(470, 255, "OK");
    timer0_delay(300);
    
    //Copy the results if so
    for(voltperdiv=0;voltperdiv<6;voltperdiv++)
    {
      //For each volt per division setting there is a DC calibration offset
      scopesettings.channel1.dc_calibration_offset[voltperdiv] = calibrationsettings.dc_calibration_offset[voltperdiv];
    }

    //The last one is also for the highest sensitivity setting
    scopesettings.channel1.dc_calibration_offset[6] = scopesettings.channel1.dc_calibration_offset[5];

    //Copy the ADC compensation values
    scopesettings.channel1.adc1compensation = calibrationsettings.adc1compensation;
    scopesettings.channel1.adc2compensation = calibrationsettings.adc2compensation;
  }

  //Setup for channel 2 calibration
  calibrationsettings.voltperdivcommand = 0x36;
  calibrationsettings.offsetcommand     = 0x35;
  calibrationsettings.adc1command       = 0x22;
  calibrationsettings.adc2command       = 0x23;

  //Use the trace buffer of this channel
  calibrationsettings.tracebuffer = (uint8 *)channel2tracebuffer;
  
  //Draw the background in dark grey
  display_set_fg_color(DARKGREY_COLOR);
  display_fill_rect(400, 250, 100, 28);
//  display_fill_rounded_rect(395, 222, 109, 58, 3);
  //Display the text in channel color
  display_set_fg_color(CHANNEL2_COLOR);
  display_text(409, 255, "Ch2....");
  
  //Calibrate this channel
  flag &= scope_do_channel_calibration();

  //Check if calibration was successful
  if(flag)
  { 
    //Display the text in channel color
    display_set_fg_color(GREEN_COLOR); 
    display_text(470, 255, "OK");
    timer0_delay(300);
    
    //Copy the results if so
    for(voltperdiv=0;voltperdiv<6;voltperdiv++)
    {
      //For each volt per division setting there is a DC calibration offset
      scopesettings.channel2.dc_calibration_offset[voltperdiv] = calibrationsettings.dc_calibration_offset[voltperdiv];
    }

    //The last one is also for the highest sensitivity setting
    scopesettings.channel2.dc_calibration_offset[6] = scopesettings.channel2.dc_calibration_offset[5];

    //Copy the ADC compensation values
    scopesettings.channel2.adc1compensation = calibrationsettings.adc1compensation;
    scopesettings.channel2.adc2compensation = calibrationsettings.adc2compensation;
  } 
  
  //Set a default sample rate and time per division setting
  //scopesettings.timeperdiv = 23;//12
  //scopesettings.samplerate = time_per_div_sample_rate[23];//12
  
  //Restore backup time per div and sample rate
  //scopesettings.timeperdiv = backup_timeperdiv;
  //scopesettings.samplerate = backup_samplerate;

  //Load the normal operation settings back into the FPGA
  fpga_set_channel_voltperdiv(&scopesettings.channel1);
  fpga_set_channel_offset(&scopesettings.channel1);
  fpga_set_channel_voltperdiv(&scopesettings.channel2);
  fpga_set_channel_offset(&scopesettings.channel2);
  fpga_set_sample_rate(scopesettings.samplerate);
  
  //Show the new settings
  scope_acqusition_settings(0);
  
  //Restore invert channel settings
  scopesettings.channel1.invert = backup_invert_ch1;
  scopesettings.channel2.invert = backup_invert_ch2;
  
  //Display the channel menu select buttons and their settings
  scope_channel_settings(&scopesettings.channel1, 0);
  scope_channel_settings(&scopesettings.channel2, 0);
 
  return(flag);
}

//----------------------------------------------------------------------------------------------------------------------------------

#define HIGH_DC_OFFSET   500
#define LOW_DC_OFFSET   1200

uint32 scope_do_channel_calibration(void)
{
  uint32 flag = 1;
  int32  samplerate;
  int32  voltperdiv;
  int32  highaverage;
  int32  lowaverage;
  int32  dcoffsetstep;
  int32  compensationsum = 0;

  //Calibrate for the hardware sensitivity settings
  for(voltperdiv=0;voltperdiv<6;voltperdiv++)
  {
    //Set the to do sensitivity setting in the FPGA
    calibrationsettings.samplevoltperdiv = voltperdiv;
    fpga_set_channel_voltperdiv(&calibrationsettings);

    //Wait 60ms to allow the relays to settle
    timer0_delay(60);//50

    //Start with the first set of averages
    samplerateindex = 0;

    //Do the measurements on two sample rates. 200MSa/s and 50KSa/s
    for(samplerate=0;samplerate<18;samplerate+=11)//ok      //0 a 11  4 a 7   
    {
      //Set the selected sample rate
      fpga_set_sample_rate(samplerate);

      //Set the matching time base
      fpga_set_time_base(sample_rate_time_per_div[samplerate]);

      //Set the high DC offset in the FPGA (Lower value returns higher ADC reading)
      calibrationsettings.dc_calibration_offset[voltperdiv] = HIGH_DC_OFFSET;
      fpga_set_channel_offset(&calibrationsettings);

      //Wait 30ms before sampling
      timer0_delay(30);//25

      //Start the conversion
      fpga_do_conversion();
      //Wait until conversion done or touch panel active
      while (fpga_done_conversion() == 0);

      //Get the data from a sample run
      fpga_read_sample_data(&calibrationsettings, 100);

      //Need the average as one reading here. Only use ADC1 data
      highaverage = calibrationsettings.adc1rawaverage;

      //Set the low DC offset in the FPGA (Higher value returns lower ADC reading)
      calibrationsettings.dc_calibration_offset[voltperdiv] = LOW_DC_OFFSET;
      fpga_set_channel_offset(&calibrationsettings);

      //Wait 30ms before sampling
      timer0_delay(30);

      //Start the conversion
      fpga_do_conversion();
      //Wait until conversion done or touch panel active
      while (fpga_done_conversion() == 0);

      //Get the data from a sample run
      fpga_read_sample_data(&calibrationsettings, 100);

      //Need the average as another reading here. Only use ADC1 data
      lowaverage = calibrationsettings.adc1rawaverage;

      //Calculate the DC offset step for a single ADC bit change for this volt/div setting
      //Low DC offset minus high DC offset (1200 - 500) = 700. Scaled up for fixed point calculation ==> 700 << 20 = 734003200
      dcoffsetstep = 734003200 / (highaverage - lowaverage);

      //Calculate the average DC offset settings for both the low to center as the high to center readings
      highaverage = HIGH_DC_OFFSET + (((highaverage - 128) * dcoffsetstep) >> 20);
      lowaverage  = LOW_DC_OFFSET  - (((128 - lowaverage) * dcoffsetstep) >> 20);

      //Set the result for this sample rate and volt per division setting
      samplerateaverage[samplerateindex][voltperdiv] = (highaverage + lowaverage) / 2;

      //Save the dc offset step for final calibration after compensation data has been determined
      sampleratedcoffsetstep[samplerateindex][voltperdiv] = dcoffsetstep;

      //Select the next set of sample indexes
      samplerateindex++;
    }
  }

  //Set the sample rate to 2MSa/s
  fpga_set_sample_rate(6);

  //Set the matching time base
  fpga_set_time_base(sample_rate_time_per_div[6]);

  //Average and test the results on a third sample rate
  for(voltperdiv=0;voltperdiv<6;voltperdiv++)
  {
    //Set the average of the two measurements as the DC offset to work with
    calibrationsettings.dc_calibration_offset[voltperdiv] = (samplerateaverage[0][voltperdiv] + samplerateaverage[1][voltperdiv]) / 2;

    //Set the to do sensitivity setting in the FPGA
    calibrationsettings.samplevoltperdiv = voltperdiv;
    fpga_set_channel_voltperdiv(&calibrationsettings);

    //Set the new DC channel offset in the FPGA
    fpga_set_channel_offset(&calibrationsettings);

    //Wait 60ms before sampling
    timer0_delay(60);

    //Start the conversion
    fpga_do_conversion();
    //Wait until conversion done or touch panel active
    while (fpga_done_conversion() == 0);

    //Get the data from a sample run
    fpga_read_sample_data(&calibrationsettings, 100);

    //Check if the average reading is outside allowed range
    if((calibrationsettings.adc1rawaverage < 125) || (calibrationsettings.adc1rawaverage > 131))
    //if((calibrationsettings.adc1rawaverage < 95) || (calibrationsettings.adc1rawaverage > 160))
    {
      //When deviation is more then 3, signal it as a failure
      flag = 0;
      
      //Display the text in red color
      display_set_fg_color(RED_COLOR);
      display_text(455, 255, "FAIL 1");
      timer0_delay(1500);
    }

    //Sum the ADC differences
    compensationsum += (calibrationsettings.adc2rawaverage - calibrationsettings.adc1rawaverage);
  }

  //Calculate the average of the ADC difference
  compensationsum /= 6;

  //Split the difference on the two ADC's
  calibrationsettings.adc1compensation = compensationsum / 2;
  calibrationsettings.adc2compensation = -1 * (compensationsum - calibrationsettings.adc1compensation);

  //Check if the found result is within limits
  if((compensationsum < -20) || (compensationsum > 20))
  {
    //Not so clear the flag
    flag = 0;
    
    //Draw the background in dark grey
    display_set_fg_color(DARKGREY_COLOR);
    display_fill_rect(450, 250, 50, 28);
    
    //Display the text in red color
    display_set_fg_color(RED_COLOR);
    display_text(455, 255, "FAIL 2");
    timer0_delay(1500);
  }

  //Adjust the center point DC offsets with the found compensation values
  for(voltperdiv=0;voltperdiv<6;voltperdiv++)
  {
    //The DC offset is based on the pre compensated ADC1 reading, so need to adjust with the average DC offset step times the ADC1 compensation value
    calibrationsettings.dc_calibration_offset[voltperdiv] = (int16)calibrationsettings.dc_calibration_offset[voltperdiv] + (((int32)calibrationsettings.adc1compensation * (((int32)sampleratedcoffsetstep[0][voltperdiv] + (int32)sampleratedcoffsetstep[1][voltperdiv]) / 2)) >> 20);
  }

  //Return the result of the tests. ZERO (True) if all tests passed
  return(flag);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_input_calibration(void)
{
    int32   voltsCH1 = 1;
    int32   voltsCH2 = 1;
    int32   valueCH1;
    int32   valueCH2;
    
    int32   calibVolt = 0;
    uint8   vic = 0;        //vic - Value Input Calibration
            
    PVOLTCALCDATA vcdCH1;
    PVOLTCALCDATA vcdCH2;
    
    //Clear flag error input calibration
    calibrationfail = 0;
    
    switch (calibrationopen)
      {
      case 3:  //Signal calibration 300mV
            
      //Enable channel 1 & channel 2
      scopesettings.channel1.enable = 1;
      scopesettings.channel2.enable = 1;
      
      //Invert channel mode OFF
      scopesettings.channel1.invert = 0;
      scopesettings.channel2.invert = 0;
    
      //Set the channel to DC coupling ( settings->coupling = 0; )
      scopesettings.channel1.coupling = 0;  //0-DC
      scopesettings.channel2.coupling = 0;  //0-DC
      
      fpga_set_channel_coupling(&scopesettings.channel1);
      fpga_set_channel_coupling(&scopesettings.channel2);
    
      //Set the channel magnification 1x
      scopesettings.channel1.magnification = 1;   
      scopesettings.channel2.magnification = 1;
     
      //Signal calibration 300mV
      calibVolt = 30000;
    
      //choice value input_calibration[X] 5 for 100mV/div
      vic = 5; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    
     
      //Set offset to 0
      scopesettings.channel1.dcoffset = 0;
      scopesettings.channel2.dcoffset = 0;
      
      //Set fpga samplevoltperdiv = displayvoltperdiv; Set fpga dcoffset
      match_volt_per_div_settings(&scopesettings.channel1);
      match_volt_per_div_settings(&scopesettings.channel2);
      
      //Disable the trigger circuit
      scopesettings.samplemode = 0;

      //Switch back to AUTO trigger mode to avoid lockup in NORMAL or SINGLE mode
      scopesettings.triggermode = 0;
  
      //Show it on the display
      scope_trigger_settings(0);
  
      //Also set the FPGA to the AUTO trigger mode
      fpga_set_trigger_mode();
    
      //Set a default sample rate and time per division setting
      scopesettings.timeperdiv = 20;//9+11 //9 is 500KSa 200us/div 12 is 5MSa/s 20us/div
      scopesettings.samplerate = time_per_div_sample_rate[20];//9+11 200us/div
    
      //Set the new sample rate in the FPGA
      fpga_set_sample_rate(scopesettings.samplerate);
      //Set the new timeperdiv rate in the FPGA
      fpga_set_time_base(scopesettings.timeperdiv);

      //Show the new settings
      scope_acqusition_settings(0);
      
      //To make sure the scope will be running after auto set, set the run mode to running
      //scopesettings.runstate = 1;//ok
  
      //Show this on the screen
      //scope_run_stop_text();
      //scope_run_stop_button(0);
    
      break;
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 4:    
      //Signal calibration 600mV
      calibVolt = 60000;
      //choice value input_calibration[X]
      vic = 4; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    
    
      break;
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 5:  
      //Signal calibration 1.5V
      calibVolt = 150000;
    
      //choice value input_calibration[X]
      vic = 3; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    
    
      break;    
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 6:  
      //Signal calibration 3V
      calibVolt = 300000;
    
      //choice value input_calibration[X]
      vic = 2; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    

      break;    
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 7:  
      //Signal calibration 7.5V
      calibVolt = 750000;
    
      //choice value input_calibration[X]
      vic = 1; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    

      break;    
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 8:  
      //Signal calibration 15V
      calibVolt = 1500000;
      //calibVolt = 1280000;
    
      //choice value input_calibration[X] 5 for 100mV/div etc.
      vic = 0; 
    
      //Set sample 100mV/div ( 5->100mV, 4->200mV, 3->500mV, 2->1V, 1->2.5V, 0->5V )
      scopesettings.channel1.displayvoltperdiv = vic;    
      scopesettings.channel2.displayvoltperdiv = vic;    
     
      break;   
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
      default: break; 
    }
    //----------------------------------- FOR ALLL -------------------------------------------  
  
    //Set fpga samplevoltperdiv=displayvoltperdiv; Set fpga dcoffset
    match_volt_per_div_settings(&scopesettings.channel1);
    match_volt_per_div_settings(&scopesettings.channel2);
    
    //Adjust the trigger level to 50% setting
    //scope_do_50_percent_trigger_setup();  //netreba lebo triger off
    
    //Display the channel menu select buttons and their settings
    scope_channel_settings(&scopesettings.channel1, 0);
    scope_channel_settings(&scopesettings.channel2, 0);
    
    
    //Calculate the voltage based on the channel settings
    vcdCH1 = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel1.magnification][scopesettings.channel1.displayvoltperdiv];
    vcdCH2 = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel2.magnification][scopesettings.channel2.displayvoltperdiv];
    
    //Start the conversion
    fpga_do_conversion();
    //Wait until conversion done or touch panel active
    while (fpga_done_conversion() == 0);
  
    //Get the data from a sample run
    fpga_read_sample_data(&scopesettings.channel1, 100);
    fpga_read_sample_data(&scopesettings.channel2, 100);
    
    //calculate Vmax
    //valueCH1 = scopesettings.channel1.max-128;
    //valueCH2 = scopesettings.channel2.max-128;
    valueCH1 = scopesettings.channel1.average-128;
    valueCH2 = scopesettings.channel2.average-128;
    //valueCH1 = scopesettings.channel1.center-128;
    //valueCH2 = scopesettings.channel2.center-128;

    //Start default input_calibration value
    scopesettings.channel1.input_calibration[vic] = 3000000;
            
    //modify input_calibration for specify value for CH1
    while (voltsCH1 != calibVolt)   
      {
      scopesettings.channel1.input_calibration[vic]++;
      if (scopesettings.channel1.input_calibration[vic] > 4000000) {calibrationfail = 1;break;}
      voltsCH1 = (valueCH1 * scopesettings.channel1.input_calibration[vic]) >> VOLTAGE_SHIFTER;
      voltsCH1 *= vcdCH1->mul_factor; 
      }
    
    //Start default input_calibration value
    scopesettings.channel2.input_calibration[vic] = 3000000;
    
    //modify input_calibration for specify value for CH2
    while (voltsCH2 != calibVolt) 
      {
      scopesettings.channel2.input_calibration[vic]++;
      if (scopesettings.channel2.input_calibration[vic] > 4000000) {calibrationfail = 1;break;}
      voltsCH2 = (valueCH2 * scopesettings.channel2.input_calibration[vic]) >> VOLTAGE_SHIFTER;
      voltsCH2 *= vcdCH2->mul_factor; 
      }
    
    //For 50mV  is calibration 100mV*2 ( only software calculation )  
    if ( calibrationopen == 3 )
        {
        scopesettings.channel1.input_calibration[6] = scopesettings.channel1.input_calibration[5]*2;  
        scopesettings.channel2.input_calibration[6] = scopesettings.channel2.input_calibration[5]*2; 
        }

    //FAILED - show the calibration FAILED text
    if (calibrationfail) {scope_show_Input_calibration_fail(); calibrationopen = 9;}
        else calibrationopen++;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_do_50_percent_trigger_setup(void)
{
  uint32 center50 = 0;
    
  //Check which channel is the active trigger channel
  if(scopesettings.triggerchannel == 0)
  {
    //Use the channel 1 center value   
    //Get the sample and adjust the data for the correct voltage per div setting
    center50 = ((scopesettings.channel1.center-128) * scopesettings.channel1.input_calibration[scopesettings.channel1.samplevoltperdiv]) >> VOLTAGE_SHIFTER;
    //scopesettings.channel1.triggerverticalposition = center50 + scopesettings.channel1.traceposition;
    
    //scopesettings.channel1.triggerverticalposition = center50 + scopesettings.channel1.traceposition - scopesettings.channel1.dcoffset/2;
    
    scopesettings.channel1.triggerverticalposition = center50 + scopesettings.channel1.traceposition;// + scopesettings.channel1.dcoffset;  
  }
  else if(scopesettings.triggerchannel == 1)
  {
    //Use the channel 2 center value
    //Get the sample and adjust the data for the correct voltage per div setting
    center50 = ((scopesettings.channel2.center-128) * scopesettings.channel2.input_calibration[scopesettings.channel2.samplevoltperdiv]) >> VOLTAGE_SHIFTER;
    scopesettings.channel2.triggerverticalposition = center50 + scopesettings.channel2.traceposition;
  }

  //Set the trigger vertical position position to match the new trigger level
  scope_calculate_trigger_vertical_position();
}

//----------------------------------------------------------------------------------------------------------------------------------
void scope_do_auto_setup(void)
{ 
    if (!scopesettings.new_autosetup) scope_do_auto_setup_old();
    else scope_do_auto_setup_new();
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_do_auto_setup_old(void)
{   
  PCHANNELSETTINGS settings;
  
  register uint32 multiply;

  int32  screentime;
  int32  timeperdiv;
  int32  samplerate = 0;
    
  uint32 flags = 0;
  uint32 voltperdiv;
  uint32 overload = 0;

  uint32 minvpplevel;
  uint32 center1;
  uint32 center2;
  uint32 offset;
  uint32 max;
  uint32 min;    
  uint32 p2pCH1 = 0; 
  uint32 p2pCH2 = 0;
  uint32 center;  
  
  //No need to do auto setup if no channel is enabled
  if((scopesettings.channel1.enable == 0) && (scopesettings.channel2.enable == 0))
  {
    return;
  }
  
  //Disable the trigger circuit
  scopesettings.samplemode = 0;
  
  //Modification on 09-03-2022. Begin
  //Switch back to AUTO trigger mode to avoid lockup in NORMAL or SINGLE mode
  scopesettings.triggermode = 0;
  
  //Show it on the display
  scope_trigger_settings(0);
  
  //Also set the FPGA to the AUTO trigger mode
  fpga_set_trigger_mode();
  
  //To make sure the scope will be running after auto set, set the run mode to running
  scopesettings.runstate = 1;//ok
  
  //Show this on the screen
  scope_run_stop_text();
  scope_run_stop_button(0);
  
  //Set number of samples
  //This can be changed to a global define
  scopesettings.samplecount = SAMPLE_COUNT;
  scopesettings.nofsamples  = SAMPLES_PER_ADC;

  //Send the command for setting the trigger level to the FPGA
  //fpga_write_cmd(0x17);
  //fpga_write_byte(0);
  
  //-*************************************************************************************
  
  //Check on which bottom check level needs to be used
  if((scopesettings.channel1.enable) && (scopesettings.channel2.enable))
  {
    //Both channels enabled use lower value??
    minvpplevel = 70; //70
  }
  else
  {
    //Only one channel enabled use this value??
    minvpplevel = 110;//110 130 140 150
  }
  
  //Setup for channel 1 if it is enabled
  if(scopesettings.channel1.enable)
  {
    //Trace offset on low end
    center1 = 200;//200
    scopesettings.channel1.traceposition = 200;
    scopesettings.channel1.dcoffset = 0;
    fpga_set_channel_offset(&scopesettings.channel1);
    
    //Signal channel is enabled and needs to be done
    flags |= 0x01;
  }

  //Setup for channel 2 if it is enabled
  if(scopesettings.channel2.enable)
  {
    //Trace offset on low end
    center2 = 200;//200
    scopesettings.channel2.traceposition = 200;
    scopesettings.channel2.dcoffset = 0;
    fpga_set_channel_offset(&scopesettings.channel2);
    
    //Signal channel is enabled and needs to be done
    flags |= 0x02;
  }

  //-**************************************************************************
  //Select the channel to work with
  if(scopesettings.triggerchannel == 0)
  {
    settings = &scopesettings.channel1;
  }
  else
  {
    settings = &scopesettings.channel2;
  }

  settings->samplevoltperdiv = 0;//0
  //Set the volts per div for this channel
  fpga_set_channel_voltperdiv(settings);
  //Wait 50ms to allow the circuit to settle
  timer0_delay(50);
 
  settings->frequencyvalid = 0;
  //settings->ADoverload = 0;
  overload = 0;
  
  //----------------------------------------------------------------------------
     
  while((!settings->frequencyvalid)&&(!overload))
  {
    for(samplerate=0;samplerate<4;samplerate++)// 6ide do 5ky    //4 do 3
    {
        //Set the selected sample rate
        fpga_set_sample_rate(samplerate_for_autosetup[samplerate]);

        //Set the matching time base
        fpga_set_time_base(sample_rate_time_per_div[samplerate_for_autosetup[samplerate]]);

        settings->ADoverload = 0;
        
        //Start the conversion
        fpga_do_conversion();
        //Wait until conversion done or touch panel active
        while (fpga_done_conversion() == 0);
    
        //Get the data from a sample run
        fpga_read_sample_data(settings, 5);//100
        
        //Check if there is a frequency reading and break the loop if so
        if(settings->frequencyvalid)
        {
        break;
        }
   
        if((settings->samplevoltperdiv < 6)&&(settings->ADoverload == 0)) 
        {
            settings->samplevoltperdiv++;
            //Set the volts per div for this channel
            fpga_set_channel_voltperdiv(settings);
            //Wait 50ms to allow the circuit to settle
            timer0_delay(50);
            //match_volt_per_div_settings(settings);
            //samplerate=0;
        } 
        else 
        {   overload = 1;
            if(settings->ADoverload) {if(settings->samplevoltperdiv > 0)settings->samplevoltperdiv--;fpga_set_channel_voltperdiv(settings);timer0_delay(50);}
            settings->frequencyvalid = 0;
            break;
        }
    }
  }//end while
  
  //Start the conversion
  fpga_do_conversion();
  //Wait until conversion done or touch panel active
  while (fpga_done_conversion() == 0);
  
  //Get the data from a sample run
  fpga_read_sample_data(settings, 5);//100
  
  //When there is a frequency reading determine the needed time per division setting
  if(settings->frequencyvalid)
  {
    //Can't use the frequency here since it is based on the scopesettings.samplerate variable, which is not used here
    //Calculate the time in nanoseconds for getting three periods on the screen
    screentime = (float)settings->periodtime * sample_time_converters[samplerate_for_autosetup[samplerate]];

    //Match the found time to the nearest time per division setting
    for(timeperdiv=0;timeperdiv<24;timeperdiv++)
    {
      //When the found time is higher than the selected time per division quit the search
      if(screentime > time_per_div_matching[timeperdiv])
      {
        break;
      }
    }

    //Check if not on the first setting
    if(timeperdiv)
    {
      //If so take one of to get to the right one to use. Also ensures not selecting a non existing setting if none found
      timeperdiv--;
    }

    //Select the found time per division
    scopesettings.timeperdiv = timeperdiv+11;
    scopesettings.samplerate = time_per_div_sample_rate[timeperdiv+11];
  }
  else
  {
    //Set a default sample rate and time per division setting
    scopesettings.timeperdiv = 23;//12
    scopesettings.samplerate = time_per_div_sample_rate[23];//12
  }
  
  //Set the new sample rate in the FPGA
  fpga_set_sample_rate(scopesettings.samplerate);

  //Show the new settings
  scope_acqusition_settings(0);

//-------------------------------------------------------------------------------------------------------------------------  
     
   //Walk through the sensitivity settings until enabled channels are done
  for(voltperdiv=0;voltperdiv<6 && flags!=0;voltperdiv++)
  {     
    //Check if channel still needs to be done
    if(flags & 0x01)
    {
      //Set the current sensitivity
      scopesettings.channel1.displayvoltperdiv = voltperdiv;
      match_volt_per_div_settings(&scopesettings.channel1);
    }
    
    //Check if channel still needs to be done
    if(flags & 0x02)
    {
      //Set the current sensitivity
      scopesettings.channel2.displayvoltperdiv = voltperdiv;
      match_volt_per_div_settings(&scopesettings.channel2);
    }
    
    
  //-********************************************************************************************
    //Reset ADoverload flag(flags);
    scopesettings.channel1.ADoverload = 0;
    scopesettings.channel2.ADoverload = 0;
      
    //Start the conversion
    fpga_do_conversion();
    //Wait until conversion done or touch panel active
    while (fpga_done_conversion() == 0);
    //---------------- CH1 ---------------------------------------------------
    //Get the data from a sample run
    fpga_read_sample_data(&scopesettings.channel1, 5);//100
    fpga_read_sample_data(&scopesettings.channel2, 5);//100
    
    //Check the channel data if not yet done
    if(flags & 0x01)
    {
    settings = &scopesettings.channel1;
    
    multiply=(settings->input_calibration[settings->samplevoltperdiv]);//*(settings->dc_shift_center)/100; ;
    
    //Get the sample and adjust the data  
    max = ((settings->maxDC * multiply ) >> VOLTAGE_SHIFTER);//3289650
    //center = ((settings->center * multiply ) >> VOLTAGE_SHIFTER);//3289650
    min = ((settings->minDC * multiply ) >> VOLTAGE_SHIFTER);//3289650
    p2pCH1 = ((settings->peakpeakDC * multiply ) >> VOLTAGE_SHIFTER);//3289650
    /*
        //Get the sample and adjust the data  
    max = settings->maxAS;//3289650
    //center = ((settings->center * multiply ) >> VOLTAGE_SHIFTER);//3289650
    min = settings->minAS;//3289650
    p2pCH1 = settings->peakpeakAS;//3289650
    */
    
    
      //Check if signal in good range 
      if((max >= 391) || (min < 60) || (p2pCH1 > minvpplevel))//301 100 391-60
      { //When signal in range stop scanning the channel
        if (settings->ADoverload) 
            {
            if(settings->samplevoltperdiv > 0)settings->samplevoltperdiv--;
            match_volt_per_div_settings(settings);
            }
        
        flags &= 0x2;
      }
    }    

    //Check the channel data if not yet done
    if(flags & 0x02)
    {   
    settings = &scopesettings.channel2;
    
    multiply=(settings->input_calibration[settings->samplevoltperdiv]);
    //multiply = 3289650;
    //Get the sample and adjust the data
    max = (settings->maxDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    //center = ((settings->center) * multiply ) >> VOLTAGE_SHIFTER;//3289650
    min = (settings->minDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    p2pCH2 = (settings->peakpeakDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    
      //Check if signal in good range 
      if((max >= 391) || (min < 60) || (p2pCH2 > minvpplevel)) 
      {
        //When signal in range stop scanning the channel
         if (settings->ADoverload) 
            {
            if(settings->samplevoltperdiv > 0)settings->samplevoltperdiv--;
            match_volt_per_div_settings(settings);
            }
         flags &= 0x1;
      }
    }    
  }
  
  //Start the conversion
  fpga_do_conversion();
  //Wait until conversion done or touch panel active
  while (fpga_done_conversion() == 0);
  
  //--------------------------- CH1 --------------------------------------------
  
  //Get the data from a sample run
  fpga_read_sample_data(&scopesettings.channel1, 5);//100
  fpga_read_sample_data(&scopesettings.channel2, 5);//100

   
  //Check if channel enabled
  if(scopesettings.channel1.enable)
  {
    settings = &scopesettings.channel1;
      
    //Check if on fore last sensitivity still not done
    if(flags & 0x01)
    {
      //Switch to last sensitivity if so
      settings->displayvoltperdiv = 6;
      match_volt_per_div_settings(&scopesettings.channel1);
    }
    
   //If DC mode calculate to offset
   if(!settings->coupling)
   {
    
    //multiply=(settings->input_calibration[settings->samplevoltperdiv]);
    //center = (((settings->centerAS) * multiply ) >> VOLTAGE_SHIFTER);//3289650
    center = settings->centerDC+73;
    //center= (center*settings->dc_shift_center)/100; 
    //center= (center+73); 
     

    //Adjust the center line based on targeted center
    if(center < center1)
    {
      //Calculate a new trace offset
      //settings->traceposition += ((center1 - center)*112)/100;  //120 mensie cislo posuva hore priebeh
      settings->traceposition += ((center1 - center)*(settings->dc_shift_center))/100;  //120 mensie cislo posuva hore priebeh
      //settings->traceposition += ((center1 - center)-(settings->dc_shift_center));  //120 mensie cislo posuva hore priebeh
      //settings->traceposition += (center1 - center);  //120 mensie cislo posuva hore priebeh
      
      //Make sure it is not out of range
      if(settings->traceposition > 399)//385
      {
          settings->traceposition = 399;
      }
    }
    else
    {//kladny offset
      //Calculate the needed offset
      //offset = ((center - center1)*112)/100;    //110
      offset = ((center - center1)*settings->dc_shift_center)/100;    //110
      //offset = ((center - center1)-settings->dc_shift_center);    //110
      //offset = (center - center1);    //110
       
      //Check if current setting large enough to subtract the offset
      if(settings->traceposition > offset)
      {
          //Take of the found offset
          settings->traceposition -= offset;
      }
      else
      {
          //Limit on zero
          settings->traceposition = 0;
      }
    }
    
    //Write the new offset to the FPGA
    settings->dcoffset = (center1 - (settings->traceposition))*2;
   }//end dc copuling 
  }//end channel1 enable
  
  //--------------------------------- CH2 --------------------------------------
  //Check if channel enabled
  if(scopesettings.channel2.enable)
  {
    settings = &scopesettings.channel2;
    
    //Check if on fore last sensitivity still not done
    if(flags & 0x02)
    {
      //Switch to last sensitivity if so
      settings->displayvoltperdiv = 6;
      match_volt_per_div_settings(&scopesettings.channel2);
    }
    
    //multiply=(settings->input_calibration[settings->samplevoltperdiv]);
    //center = ((settings->center) * multiply ) >> VOLTAGE_SHIFTER;//3289650
    center = settings->centerDC+73;

   //If DC mode calculate to offset
   if(!settings->coupling)
   {
    //Adjust the center line based on targeted center
    if(center < center2)
    {
        //Calculate a new trace offset
        //settings->traceposition += ((center2 - center)*112)/100;
        settings->traceposition += ((center2 - center)*settings->dc_shift_center)/100;
      
        //Make sure it is not out of range
        if(settings->traceposition > 399)//385
        {
            settings->traceposition = 399;
        }
    }
    else
    {
      //Calculate the needed offset
      //offset = ((center - center2)*112)/100;
      offset = ((center - center2)*settings->dc_shift_center)/100;
       
      //Check if current setting large enough to subtract the offset
      if(settings->traceposition > offset)
      {
          //Take of the found offset
          settings->traceposition -= offset;
      }
      else
      {
          //Limit on zero
          settings->traceposition = 0;
      }
    }
    
    //Write the new offset to the FPGA
    settings->dcoffset = (center2 - (settings->traceposition))*2;
   }//end dc copuling 
  }//end channel2 enable
  
//--------------------------- End CH1 & CH2 ------------------------------------
  
  //Check on which bottom check level needs to be used
  if((scopesettings.channel1.enable) && (scopesettings.channel2.enable))
  {
      if ((scopesettings.channel1.traceposition + 100 ) < 399) scopesettings.channel1.traceposition += 100;
      if ((scopesettings.channel2.traceposition - 100 ) > 0)   scopesettings.channel2.traceposition -= 100;
      
      //scopesettings.channel1.traceposition = 399-copesettings.channel1.traceposition ;
      //scopesettings.channel1.traceposition = 399-copesettings.channel1.traceposition ;
      
      //while ((scopesettings.channel1.traceposition+p2pCH1)<299) scopesettings.channel1.traceposition++;
      //while ((scopesettings.channel2.traceposition-p2pCH2)>0)   scopesettings.channel2.traceposition--;
      
      
      //if ((scopesettings.channel1.traceposition + p2pCH1 ) < 399) scopesettings.channel1.traceposition += p2pCH1;
      //if ((scopesettings.channel2.traceposition - p2pCH2 ) > 0)   scopesettings.channel2.traceposition -= p2pCH2;
      
      
      //if ((scopesettings.channel1.traceposition + minvpplevel ) < 399) scopesettings.channel1.traceposition += minvpplevel;
      //if ((scopesettings.channel2.traceposition - minvpplevel ) > 0)   scopesettings.channel2.traceposition -= minvpplevel;
  }
  
  //If AC mode set to offset to zero
  //if(scopesettings.channel1.coupling) scopesettings.channel1.dcoffset = 0;  //1-AC
  //if(scopesettings.channel2.coupling) scopesettings.channel2.dcoffset = 0;  //1-AC
  
  //Write the new offset to the FPGA
  fpga_set_channel_offset(&scopesettings.channel1);
  fpga_set_channel_offset(&scopesettings.channel2);
  
  //Display the new channel setting
  scope_channel_settings(&scopesettings.channel1, 0);
  scope_channel_settings(&scopesettings.channel2, 0);
  
  //Show it on the display
  scope_trigger_settings(0);
  
  //Also set the FPGA to the AUTO trigger mode
  fpga_set_trigger_mode();
  
  //Adjust the trigger level to 50% setting
  scope_do_50_percent_trigger_setup();
  
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void scope_do_auto_setup_new(void)
{   
  PCHANNELSETTINGS settings;
  
  register uint32 multiply;

  int32  screentime;
  int32  timeperdiv;
  int32  samplerate = 0;
    
  uint32 flags = 0;
  uint32 voltperdiv;
  //uint32 overload = 0;

  uint32 minvpplevel;
  uint32 center1;
  uint32 center2;
  uint32 offset;
  uint32 max;
  uint32 min;    
  uint32 p2pCH1 = 0; 
  uint32 p2pCH2 = 0;
  uint32 center;  
  
  //No need to do auto setup if no channel is enabled
  if((scopesettings.channel1.enable == 0) && (scopesettings.channel2.enable == 0))
  {
    return;
  }
  
  //Disable the trigger circuit
  scopesettings.samplemode = 0;
  
  //Modification on 09-03-2022. Begin
  //Switch back to AUTO trigger mode to avoid lockup in NORMAL or SINGLE mode
  scopesettings.triggermode = 0;
  
  //Show it on the display
  scope_trigger_settings(0);
  
  //Also set the FPGA to the AUTO trigger mode
  fpga_set_trigger_mode();
  
  //To make sure the scope will be running after auto set, set the run mode to running
  scopesettings.runstate = 1;//ok
  
  //Show this on the screen
  scope_run_stop_text();
  scope_run_stop_button(0);
  
  //Set number of samples
  //This can be changed to a global define
  scopesettings.samplecount = SAMPLE_COUNT;
  scopesettings.nofsamples  = SAMPLES_PER_ADC;

  //Send the command for setting the trigger level to the FPGA
  //fpga_write_cmd(0x17);
  //fpga_write_byte(0);
  if (scopesettings.timeperdiv < 14)
  {
  scopesettings.timeperdiv = 23;//12
  scopesettings.samplerate = time_per_div_sample_rate[23];//12
  }
  
  //Set the new sample rate in the FPGA
  fpga_set_sample_rate(scopesettings.samplerate);
  
  //-*************************************************************************************
  
  //Check on which bottom check level needs to be used
  if((scopesettings.channel1.enable) && (scopesettings.channel2.enable))
  {
    //Both channels enabled use lower value??
    minvpplevel = 70; 
  }
  else
  {
    //Only one channel enabled use this value??
    minvpplevel = 110;//110 130
  }
  
  //Setup for channel 1 if it is enabled
  if(scopesettings.channel1.enable)
  {
    //Trace offset on low end
    center1 = 200;//200;
    scopesettings.channel1.traceposition = 200;
    scopesettings.channel1.dcoffset = 0;
    fpga_set_channel_offset(&scopesettings.channel1);
    
    //Signal channel is enabled and needs to be done
    flags |= 0x01;
  }

  //Setup for channel 2 if it is enabled
  if(scopesettings.channel2.enable)
  {
    //Trace offset on low end
    center2 = 200;//200;
    scopesettings.channel2.traceposition = 200;
    scopesettings.channel2.dcoffset = 0;
    fpga_set_channel_offset(&scopesettings.channel2);
    
    //Signal channel is enabled and needs to be done
    flags |= 0x02;
  }

  //-********************************************************* 
   //Walk through the sensitivity settings until enabled channels are done
  for(voltperdiv=0;voltperdiv<6 && flags!=0;voltperdiv++)
  {     
    //Check if channel still needs to be done
    if(flags & 0x01)
    {
      //Set the current sensitivity
      scopesettings.channel1.displayvoltperdiv = voltperdiv;
      match_volt_per_div_settings(&scopesettings.channel1);
    }
    
    //Check if channel still needs to be done
    if(flags & 0x02)
    {
      //Set the current sensitivity
      scopesettings.channel2.displayvoltperdiv = voltperdiv;
      match_volt_per_div_settings(&scopesettings.channel2);
    }
    
    
  //-********************************************************************************************
    //Reset ADoverload flag(flags);
    scopesettings.channel1.ADoverload = 0;
    scopesettings.channel2.ADoverload = 0;
      
    //Start the conversion
    fpga_do_conversion();
    //Wait until conversion done or touch panel active
    while (fpga_done_conversion() == 0);
    //---------------- CH1 ---------------------------------------------------
    //Get the data from a sample run
    fpga_read_sample_data(&scopesettings.channel1, 5);//100
    fpga_read_sample_data(&scopesettings.channel2, 5);//100
    
    //Check the channel data if not yet done
    if(flags & 0x01)
    {
    settings = &scopesettings.channel1;
    
    multiply=(settings->input_calibration[settings->samplevoltperdiv]);
    
    //Get the sample and adjust the data  
    max = (settings->maxDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    //center = (settings->center * multiply ) >> VOLTAGE_SHIFTER;//3289650
    min = (settings->minDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    p2pCH1 = (settings->peakpeakDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    
      //Check if signal in good range 
      if((max >= 391) || (min < 60) || (p2pCH1 > minvpplevel))//301 100
      { //When signal in range stop scanning the channel
        if (settings->ADoverload) 
            {
            if(settings->samplevoltperdiv > 0)settings->samplevoltperdiv--;
            match_volt_per_div_settings(settings);
            }
        
        flags &= 0x2;
      }
    }    

    //Check the channel data if not yet done
    if(flags & 0x02)
    {   
    settings = &scopesettings.channel2;
    
    multiply=(settings->input_calibration[settings->samplevoltperdiv]);
    
    //Get the sample and adjust the data
    max = (settings->maxDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    //center = ((settings->center) * multiply ) >> VOLTAGE_SHIFTER;//3289650
    min = (settings->minDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    p2pCH2 = (settings->peakpeakDC * multiply ) >> VOLTAGE_SHIFTER;//3289650
    
      //Check if signal in good range 
      if((max >= 391) || (min < 60) || (p2pCH2 > minvpplevel)) 
      {
        //When signal in range stop scanning the channel
         if (settings->ADoverload) 
            {
            if(settings->samplevoltperdiv > 0)settings->samplevoltperdiv--;
            match_volt_per_div_settings(settings);
            }
         flags &= 0x1;
      }
    }    
  }
  
  //Start the conversion
  fpga_do_conversion();
  //Wait until conversion done or touch panel active
  while (fpga_done_conversion() == 0);
  
  //--------------------------- CH1 --------------------------------------------
  
  //Get the data from a sample run
  fpga_read_sample_data(&scopesettings.channel1, 5);//100
  fpga_read_sample_data(&scopesettings.channel2, 5);//100

   
  //Check if channel enabled
  if(scopesettings.channel1.enable)
  {
    settings = &scopesettings.channel1;
      
    //Check if on fore last sensitivity still not done
    if(flags & 0x01)
    {
      //Switch to last sensitivity if so
      settings->displayvoltperdiv = 6;
      match_volt_per_div_settings(&scopesettings.channel1);
    }
    
   //If DC mode calculate to offset
   if(!settings->coupling)
   {
    
    //center = ((settings->center) * multiply ) >> VOLTAGE_SHIFTER;//3289650
    //center = settings->center;
    center = settings->centerDC+73;

    //Adjust the center line based on targeted center
    if(center < center1)
    {
      //Calculate a new trace offset
      //settings->traceposition += ((center1 - center)*112)/100;  //120 mensie cislo posuva hore priebeh
      if (settings->invert) settings->traceposition -= ((center1 - center)*settings->dc_shift_center)/100; 
        else settings->traceposition += ((center1 - center)*settings->dc_shift_center)/100;  //120 mensie cislo posuva hore priebeh
      
      //Make sure it is not out of range
      if(settings->traceposition > 399)//385
      {
          settings->traceposition = 399;
      }
    }
    else
    {
      //Calculate the needed offset
      //offset = ((center - center1)*112)/100;    //110
        offset = ((center - center1)*settings->dc_shift_center)/100;    //110
       
      //Check if current setting large enough to subtract the offset
      if(settings->traceposition > offset)
      {
          //Take of the found offset (of inverting ON or OFF)
          if (settings->invert) settings->traceposition += offset; else settings->traceposition -= offset;
      }
      else
      {
          //Limit on zero
          settings->traceposition = 0;
      }
    }
    
    //Write the new offset to the FPGA
    settings->dcoffset = (center1 - (settings->traceposition))*2;
   }//end dc copuling 
  }//end channel1 enable
  
  //--------------------------------- CH2 --------------------------------------
  //Check if channel enabled
  if(scopesettings.channel2.enable)
  {
    settings = &scopesettings.channel2;
    
    //Check if on fore last sensitivity still not done
    if(flags & 0x02)
    {
      //Switch to last sensitivity if so
      settings->displayvoltperdiv = 6;
      match_volt_per_div_settings(&scopesettings.channel2);
    }
    
    //center = ((settings->center) * multiply ) >> VOLTAGE_SHIFTER;//3289650
    //center = settings->center;
    center = settings->centerDC+73;

   //If DC mode calculate to offset
   if(!settings->coupling)
   {
    //Adjust the center line based on targeted center
    if(center < center2)
    {
        //Calculate a new trace offset
        //settings->traceposition += ((center2 - center)*112)/100;
        if (settings->invert) settings->traceposition -= ((center2 - center)*settings->dc_shift_center)/100;
            else settings->traceposition += ((center2 - center)*settings->dc_shift_center)/100;
      
        //Make sure it is not out of range
        if(settings->traceposition > 399)//385
        {
            settings->traceposition = 399;
        }
    }
    else
    {
      //Calculate the needed offset
      //offset = ((center - center2)*112)/100;
      offset = ((center - center2)*settings->dc_shift_center)/100;
       
      //Check if current setting large enough to subtract the offset
      if(settings->traceposition > offset)
      {
          //Take of the found offset (of inverting ON or OFF)
          if (settings->invert) settings->traceposition += offset; else settings->traceposition -= offset;
      }
      else
      {
          //Limit on zero
          settings->traceposition = 0;
      }
    }
    
    //Write the new offset to the FPGA
    settings->dcoffset = (center2 - (settings->traceposition))*2;
   }//end dc copuling 
  }//end channel2 enable
  
//--------------------------- End CH1 & CH2 ------------------------------------
  
  //Check on which bottom check level needs to be used
  if((scopesettings.channel1.enable) && (scopesettings.channel2.enable))
  {
      if ((scopesettings.channel1.traceposition + 100 ) < 399) scopesettings.channel1.traceposition += 100;
      if ((scopesettings.channel2.traceposition - 100 ) > 0)   scopesettings.channel2.traceposition -= 100;
      
      //scopesettings.channel1.traceposition = 399-copesettings.channel1.traceposition ;
      //scopesettings.channel1.traceposition = 399-copesettings.channel1.traceposition ;
      
      //while ((scopesettings.channel1.traceposition+p2pCH1)<299) scopesettings.channel1.traceposition++;
      //while ((scopesettings.channel2.traceposition-p2pCH2)>0)   scopesettings.channel2.traceposition--;
      
      
      //if ((scopesettings.channel1.traceposition + p2pCH1 ) < 399) scopesettings.channel1.traceposition += p2pCH1;
      //if ((scopesettings.channel2.traceposition - p2pCH2 ) > 0)   scopesettings.channel2.traceposition -= p2pCH2;
      
      
      //if ((scopesettings.channel1.traceposition + minvpplevel ) < 399) scopesettings.channel1.traceposition += minvpplevel;
      //if ((scopesettings.channel2.traceposition - minvpplevel ) > 0)   scopesettings.channel2.traceposition -= minvpplevel;
  }
  
  //If AC mode set to offset to zero
  //if(scopesettings.channel1.coupling) scopesettings.channel1.dcoffset = 0;  //1-AC
  //if(scopesettings.channel2.coupling) scopesettings.channel2.dcoffset = 0;  //1-AC
  
  //Write the new offset to the FPGA
  fpga_set_channel_offset(&scopesettings.channel1);
  fpga_set_channel_offset(&scopesettings.channel2);
  
  //Display the new channel setting
  scope_channel_settings(&scopesettings.channel1, 0);
  scope_channel_settings(&scopesettings.channel2, 0);
  
  //******************************************************************************************************************
  //------------------------------------------------------------------------------------------------------------------
    //Select the channel to work with
  if(scopesettings.triggerchannel == 0)
  {
    settings = &scopesettings.channel1;
  }
  else
  {
    settings = &scopesettings.channel2;
  }
  
    settings->frequencyvalid = 0;
 
  //----------------------------------------------------------------------------
     
    for(samplerate=0;samplerate<4;samplerate++)// 6ide do 5ky    //4 do 3
    {
        /*
        //Show the new settings
        //scope_acqusition_settings(0);
          display_set_fg_color(BLACK_COLOR);
  //Fill the settings background
  display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
  //Display the text with the green color
  //display_set_fg_color(GREEN_COLOR);
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_2);
  display_text(550, 0, "Xtouch");
  display_decimal(550, 10, samplerate);
  
  scopesettings.samplerate = samplerate_for_autosetup[samplerate];
  scopesettings.timeperdiv = sample_rate_time_per_div[samplerate_for_autosetup[samplerate]];
  
            //Show the new settings
  scope_acqusition_settings(0);
        timer0_delay(1000);
        */
  
        //Set the selected sample rate
        fpga_set_sample_rate(samplerate_for_autosetup[samplerate]);

        //Set the matching time base
        fpga_set_time_base(sample_rate_time_per_div[samplerate_for_autosetup[samplerate]]);

        //Start the conversion
        fpga_do_conversion();
        //Wait until conversion done or touch panel active
        while (fpga_done_conversion() == 0);
    
        //Get the data from a sample run
        fpga_read_sample_data(settings, 100);//100
        
        //Check if there is a frequency reading and break the loop if so
        if(settings->frequencyvalid)
        {
        break;
        }
   
    }
  
  //When there is a frequency reading determine the needed time per division setting
  if(settings->frequencyvalid)
  {
    //Can't use the frequency here since it is based on the scopesettings.samplerate variable, which is not used here
    //Calculate the time in nanoseconds for getting three periods on the screen
    screentime = (float)settings->periodtime * sample_time_converters[samplerate_for_autosetup[samplerate]];

    //Match the found time to the nearest time per division setting
    for(timeperdiv=0;timeperdiv<24;timeperdiv++)
    {
      //When the found time is higher than the selected time per division quit the search
      if(screentime > time_per_div_matching[timeperdiv])
      {
        break;
      }
    }

    //Check if not on the first setting
    if(timeperdiv)
    {
      //If so take one of to get to the right one to use. Also ensures not selecting a non existing setting if none found
      timeperdiv--;
    }

    //Select the found time per division
    scopesettings.timeperdiv = timeperdiv+11;
    scopesettings.samplerate = time_per_div_sample_rate[timeperdiv+11];
  }
  else
  {
    //Set a default sample rate and time per division setting
    scopesettings.timeperdiv = 23;//12
    scopesettings.samplerate = time_per_div_sample_rate[23];//12
  }
  
  //Set the new sample rate in the FPGA
  fpga_set_sample_rate(scopesettings.samplerate);

  //Show the new settings
  scope_acqusition_settings(0);
  
  //Start the conversion
  fpga_do_conversion();
  //Wait until conversion done or touch panel active
  while (fpga_done_conversion() == 0);
    
  //Get the data from a sample run
  fpga_read_sample_data(settings, 100);//100
   
  //------------------------------------------------------------------------------------------------------------------
  //******************************************************************************************************************
  
  //Adjust the trigger level to 50% setting
  scope_do_50_percent_trigger_setup();
  
  //Show it on the display
  scope_trigger_settings(0);
  
  //Also set the FPGA to the AUTO trigger mode
  fpga_set_trigger_mode();
 
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_trace_data(void)
{
  PTHUMBNAILDATA thumbnaildata; 
  //thumbnaildata = &viewthumbnaildata[0]; 
  thumbnaildata = &viewthumbnaildata[viewcurrentindex];
  
  //See if it is possible to rework this to fixed point. A 32 bit mantissa is not accurate enough though

  //On the scope the last pixel interleaving is not working properly. Don't know why.

  //Check out the sin x/x interpolation downloaded from keysight (Sine wave reproduction using Sinc interpolation - v.1.0.py)

  //This needs more code to skip when stopped and no changes are made. It seems to be ok without this, but what is not needed is not needed
  //In the display trace routine the display only needs to be redrawn when certain conditions apply
  //  The user changes the sample rate or the time per div setting. Might need to block the setting of the sample rate when stopped because that makes no change to the current sample buffer.
  //  The user moves pointers around. Trace up and down should work, and trigger left and right. Changing the trigger level should not do anything, or even be disabled

  //Need to compensate for the position being on the left side of the pointer
  //uint32 triggerposition = scopesettings.triggerhorizontalposition+4; //+7
  int32 triggerposition = scopesettings.triggerhorizontalposition; //+7
  //int32 triggerposition = scopesettings.triggerhorizontalposition;//*disp_xpos_per_sample; //+7

  //Check if a trigger position has been found
  if(disp_have_trigger == 0)
  {
    //When not use the center of the sample buffer
    
    //disp_trigger_index = (scopesettings.samplecount / 2); 
    disp_trigger_index = (1500); // 
  } 

  //Make sure the two settings are in range of the tables!!!!
  //SKipp displaying if not????

  //The amount of x positions needed per sample is based on the number of pixels per division, the set time per division and the sample rate.
  if(scopesettings.long_mode) disp_xpos_per_sample = (5.0*frequency_per_div[scopesettings.timeperdiv]) / sample_rate[scopesettings.samplerate];
    else disp_xpos_per_sample = (50.0 * frequency_per_div[scopesettings.timeperdiv]) / sample_rate[scopesettings.samplerate];
  //disp_xpos_per_sample = (5.0 * frequency_per_div[scopesettings.timeperdiv]) / sample_rate[scopesettings.samplerate];
  
    //scope_test_longbase_value();

  //This gives the step size for going through the samples. Is also the linear interleaving step for the y direction
  disp_sample_step = 1.0 / disp_xpos_per_sample;

  //The displayable x range is based on the number of samples and the number of x positions needed per sample
  //Halved to allow trigger position to be in the center
  double xrange = (scopesettings.samplecount * disp_xpos_per_sample) / 2.0;

  //x range needs to be at least 1 pixel
  if(xrange < 1.0)
  {
    xrange = 1.0;
  }
  else if(xrange > 725.0)
  {
    //Limit on max screen pixels to avoid disp_xend becoming 0x80000000 due to overflow
    //xrange = 725.0;//comment
  }

  //Calculate the start and end x coordinates
  disp_xstart = triggerposition - xrange;
  disp_xend = triggerposition + xrange;
  
  //Limit on actual start of trace display
  if(disp_xstart < 3)
  {
    disp_xstart = 3;
  }

  //And limit on the actual end of the trace display
  if(disp_xend > 727)//725
  {
    disp_xend = 727;
  }
  
  //And limit for long view the trace display
  if(disp_xend < 1)
  {
    disp_xend = 1;
  }
  
  //Determine first sample to use based on a full screen worth of samples and the trigger position in relation to the number of pixels on the screen
  //disp_first_sample = disp_trigger_index - (((725.0 / disp_xpos_per_sample) * triggerposition) / 725.0) - 1;
  disp_first_sample = disp_trigger_index - (((727.0 / disp_xpos_per_sample) * triggerposition) / 727.0) - 1;

  //If below the first sample limit it on the first sample
  if(disp_first_sample < 0)
  {
    disp_first_sample = 0;
  }

  //This makes sure no reading outside the buffer can occur
  if(disp_sample_step > ((scopesettings.samplecount) / 2))
  {
    disp_sample_step = (scopesettings.samplecount) / 2;
  }

  //If samplestep > 1 might be an idea to draw the in between samples on the same x position to avoid aliasing
  //If sample step < 1 then skip drawing on x positions. The draw line function does the linear interpolation

  //Use a separate buffer to clear the screen
  display_set_screen_buffer(displaybuffertmp);//1

  //Clear the trace portion of the screen
  display_set_fg_color(BLACK_COLOR);
  display_fill_rect(2, 48, 728, 432);
  //display_fill_rect(2, 44, 728, 434);       //opravit rozmery

  //Check if not in waveform view mode (Scope mode) with grid disabled
  if((!scopesettings.waveviewmode) || (scopesettings.gridenable))
  {
    //Draw the grid lines and dots based on the grid brightness setting
    scope_draw_grid();
  }

  //Check if scope is in normal display mode
  if(scopesettings.xymodedisplay == 0)
  {
    //The calculations done above need to go here??

    //Check if channel1 is enabled
    if(scopesettings.channel1.enable)
    {          
        //Average mode active only auto and normal mode, no waveview mode
        if((scopesettings.average_mode)&&(!scopesettings.waveviewmode)&&((scopesettings.triggermode==0)||(scopesettings.triggermode==2)))
        {  
            for (size_t i = 0; i < MAX_SAMPLE_BUFFER_SIZE; i++) 
            {  
                //scopesettings.channel1.tracebuffer[i]=0;
                scopesettings.channel1.tracebuffer[i] = ((scopesettings.channel1.tracebuffer[i]) + (channel1tracebufferAVG[i]))/2;
                channel1tracebufferAVG[i]=scopesettings.channel1.tracebuffer[i];
            }         
        }
      //Set the trace color for the current channel if overload change to red
      if (scopesettings.channel1.ADoverload)display_set_fg_color(RED_COLOR); 
        else display_set_fg_color(CHANNEL1_COLOR);
        
      scope_display_channel_linear_trace(&scopesettings.channel1);       
      //scope_display_channel_sinx_trace(&scopesettings.channel1);
    
      //clear flag overload ADC channel 1      
      if(scopesettings.display_data_done==0) scopesettings.channel1.ADoverload = 0;
        
    }

    //Check if channel2 is enabled
    if(scopesettings.channel2.enable)
    {  
        //Average mode active only auto and normal mode, no waveview mode
        if((scopesettings.average_mode)&&(!scopesettings.waveviewmode)&&((scopesettings.triggermode==0)||(scopesettings.triggermode==2)))
        {
            for (size_t i = 0; i < MAX_SAMPLE_BUFFER_SIZE; i++) 
            {  
                //scopesettings.channel2.tracebuffer[i]=0;
              
                scopesettings.channel2.tracebuffer[i] = ((scopesettings.channel2.tracebuffer[i]) + (channel2tracebufferAVG[i]))/2;
                channel2tracebufferAVG[i]=scopesettings.channel2.tracebuffer[i];
                
                //scopesettings.channel2.tracebuffer[i] = scopesettings.channel1.tracebuffer[i];
            }
        }    
        //This can be reduced in parameters by using the channel structure as input and add the color as item in the structure 
        //Go and do the actual trace drawing  
          //Set the trace color for the current channel if overload change to red
        if (scopesettings.channel2.ADoverload)display_set_fg_color(RED_COLOR); 
            else display_set_fg_color(CHANNEL2_COLOR);
        
        scope_display_channel_linear_trace(&scopesettings.channel2);
    
        //clear flag overload ADC channel 2        
        if(scopesettings.display_data_done==0) scopesettings.channel2.ADoverload = 0;
        
    }
      //uint32 backup_settings = 0;
  
  //backup_settings = settings;
    //Check and displaying of REFx.
    display_REFx_data();
    
    //Check and displaying of MATHx.
    display_MATHx_data();
    
    //settings = backup_settings;
    //--------------------------------------------------------------------------

    //scope_test_trigerposition_value();
    
    
    //--------------------------------------------------------------------------
    //Displaying of FFT needs to be added here.

  }
  else
  {
    //Scope set to x y display mode
    //Set x-y mode display trace color
    display_set_fg_color(XYMODE_COLOR);

    uint32 index = disp_trigger_index - 315;
    uint32 last = index + 730;

    //Need two samples per channel
    uint32 x1,x2;
    uint32 y1,y2;

    //Get the samples for the first point
    x1 = scope_get_x_sample(&scopesettings.channel1, index);
    y1 = scope_get_y_sample(&scopesettings.channel2, index);
    
    //Handle all the needed samples
    for(;index<last;index++)
    {
      //Get the samples for the next point
      x2 = scope_get_x_sample(&scopesettings.channel1, index);
      y2 = scope_get_y_sample(&scopesettings.channel2, index);

      //Draw the line between these two points
      display_draw_line(x1, y1, x2, y2);

      //Swap the points for drawing the next part
      x1 = x2;
      y1 = y2;
    }
  }

  //Draw the cursors with their measurement displays
  scope_draw_time_cursors();
  scope_draw_volt_cursors();
  scope_display_cursor_measurements();

  //Draw the signal center, trigger level and trigger position pointers
  scope_draw_pointers();

  //Show the enabled measurements on the screen
  scope_display_measurements();  

  //Check if in waveform view
  if(scopesettings.waveviewmode)
  {
    //Display the file name
    //Use white text and font_0
    display_set_fg_color(WHITE_COLOR);
    display_set_font(&font_0);
 
          
    if(onoffRTC) //if RTCon view rtc info
      {
      decodethumbnailfilename(thumbnaildata->filename);
      display_text(488, 70, filenameRTC);   //500 48 , 490 58
      } else display_text(555, 70, thumbnaildata->filename); //550 48 //display_text(555, 70, viewfilename); //550 48
  }
  
  //Copy it to the actual screen buffer
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  display_copy_rect_to_screen(2, 48, 728, 432);
  //display_copy_rect_to_screen(2, 44, 728, 434);       //opravit rozmery
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_get_x_sample(PCHANNELSETTINGS settings, int32 index)
{
  register int32 sample;

  //Center adjust the sample
  sample = (int32)settings->tracebuffer[index] - 128;

  //Get the sample and adjust the data for the correct voltage per div setting
  sample = (sample * settings->input_calibration[settings->samplevoltperdiv]) >> VOLTAGE_SHIFTER;
  
  //increase sample and adjust the data for the correct value DCshift on DC mode
  //sample += ((settings->dcoffset*100)/settings->dc_shift_size);

  //Scale the sample based on the two volt per div settings when they differ
  if(settings->displayvoltperdiv != settings->samplevoltperdiv)
  {
    //Scaling factor is based on the two volts per division settings
    sample = (sample * vertical_scaling_factors[settings->displayvoltperdiv][settings->samplevoltperdiv]) / 10000;
  }
  
  //Offset the sample on the screen
  //****************************************************************************
  if(settings->displayvoltperdiv == 6) sample = settings->traceposition + sample + (((settings->dcoffset*100)/settings->dc_shift_size)*2);
    else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
  //sample = settings->traceposition + sample + (settings->dcoffset*100)/220;
  // ak je softverove nasobenie 50mV 1x, 100mV 1x, dc posun nasobit *2 
  //if(settings->displayvoltperdiv == 6) sample = settings->traceposition + sample + (((settings->dcoffset*100)/settings->dc_shift_size)*2);
    //else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
  //if(settings->displayvoltperdiv == 6) sample = (settings->traceposition) + sample*4; else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
  //if(settings->displayvoltperdiv == 6) sample = ((settings->traceposition)) + sample;
  //else sample = settings->traceposition + sample;
  //sample = 202+ sample;
  //****************************************************************************

  //Limit sample on min displayable
  if(sample < 0)
  {
    sample = 0;
  }

  //Limit the sample on max displayable
  if(sample > 399)//401 403
  {
    sample = 399;//401 403
  }

  //The x center position has an extra offset compared to the y trace position
  return(sample + 163);//165
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_get_y_sample(PCHANNELSETTINGS settings, int32 index)
{
  register int32 sample;
  
  if (math_sample) sample = (int32)scopesettings.math_tracebuffer[index] - 128;
  //Center adjust the sample
  else sample = (int32)settings->tracebuffer[index] - 128;


    //Get the sample and adjust the data for the correct voltage per div setting
    sample = (sample * settings->input_calibration[settings->samplevoltperdiv]) >> VOLTAGE_SHIFTER;

    //increase sample and adjust the data for the correct value DCshift on DC mode
    //sample += ((settings->dcoffset*100)/settings->dc_shift_size);

    //Scale the sample based on the two volt per div settings when they differ
    if(settings->displayvoltperdiv != settings->samplevoltperdiv)
    {
      //Scaling factor is based on the two volts per division settings
      sample = (sample * vertical_scaling_factors[settings->displayvoltperdiv][settings->samplevoltperdiv]) / 10000;
    }

    //Offset the sample on the screen
    //****************************************************
    //220 - nastavi poziciu signalu na 0 pri aplikacii offsetu, mensia hodnota posuva signal vyssie
    //sample = settings->traceposition + sample + (settings->dcoffset*100)/220;//220 OK,  204
    //sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
    //if(settings->displayvoltperdiv == 6) sample = settings->traceposition + sample + (((settings->dcoffset*100)/settings->dc_shift_size)*2);
      //else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
    if(settings->displayvoltperdiv == 6) sample = settings->traceposition + sample + (((settings->dcoffset*100)/settings->dc_shift_size)*2);
    //if(settings->displayvoltperdiv == 6) sample = ((settings->traceposition)) + sample + ((settings->dcoffset*100)/settings->dc_shift_size); //else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
    else sample = settings->traceposition + sample + ((settings->dcoffset*100)/settings->dc_shift_size);
    //else sample = settings->traceposition + sample;
    //****************************************************************************

    //Limit sample on min displayable
    if(sample < 0)
    {
      sample = 0;
    }

    //Limit the sample on max displayable
    if(sample > 399)//401  403 ok
    {
      sample = 399;//401
    }

  //Display y coordinates are inverted to signal orientation
  return(448 - sample);//448
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_channel_sinx_trace(PCHANNELSETTINGS settings)
{
    /*
  double inputindex;

  register int32 previousindex;
  register int32 currentindex;
  register int32 sample1 = 0;
  register int32 sample2 = 0;
  register uint32 lastx = disp_xstart;
  register uint32 xpos = disp_xstart + 1;
*/
  //register PDISPLAYPOINTS tracepoints = settings->tracepoints;

  //Set the trace color for the current channel if overload change to red
  if (settings->ADoverload)display_set_fg_color(RED_COLOR); 
        else display_set_fg_color(settings->color);

  //Get the processed sample
 // sample1 = scope_get_y_sample(settings, disp_first_sample);
  /*
  
    int pole[N];
   int sincpole[N];  // Array for interpolated signal (same size as input)

    for (int i = 0; i < N; i++) {
        //pole[i] = scope_get_y_sample(settings, disp_first_sample+i); // Predpokladáme signál 50 Hz
        pole[i] = scope_get_y_sample(settings, i); 
        //pole[i] = 200-(scopesettings.channel1.tracebuffer[i] - 128);
    }

    // Time points for the interpolated signal (700 samples)
    double t_interp[N];
    for (int i = 0; i < N; i++) {
        t_interp[i] = (double)i / (N - 1) * 400;  // Scale to [0, 400]
    }

    // Time points for the sampled signal (scaled to [0, 400])
    double t_sampled[N];
    for (int i = 0; i < N; i++) {
        t_sampled[i] = (double)i / (N - 1) * 400;  // Scale to [0, 400]
    }

    // Perform sinc interpolation
    sinc_interpolation(t_interp, t_sampled, pole, sincpole, N);

    //display_draw_horz_line(uint32 ypos, uint32 xstart, uint32 xend);
    
    for (int i = 20; i < N; i++) {
        //printf("t_interp[%d] = %f, sincpole[%d] = %f\n", i, t_interp[i], i, sincpole[i]);
        
        display_draw_horz_line(sincpole[i], i, i);
        //display_draw_horz_line(pole[i], i, i);
        //display_draw_horz_line(i+50, i, i);
        //display_draw_horz_line(i+100, i, i+1);
    }
   */ 
    /*
    // Výpis pôvodného signálu
    printf("Original Signal (700 samples):\n");
    for (int i = 0; i < N; i++) {
        printf("t_sampled[%d] = %f, pole[%d] = %f\n", i, t_sampled[i], i, pole[i]);
    }

    // Výpis interpolovaného signálu
    printf("\nInterpolated Signal (Sinc Interpolation):\n");
    for (int i = 0; i < INTERPOLATED_SIZE; i++) {
        printf("t_interp[%d] = %f, sincpole[%d] = %f\n", i, t_interp[i], i, sincpole[i]);
    }
*/
    


  
  
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_channel_linear_trace(PCHANNELSETTINGS settings)
{
  double inputindex;

  register int32 previousindex;
  register int32 currentindex;
  register int32 sample1 = 0;
  register int32 sample2 = 0;
  register uint32 lastx = disp_xstart;
  register uint32 xpos = disp_xstart + 1;

  register PDISPLAYPOINTS tracepoints = settings->tracepoints;

  //Set the trace color for the current channel
  //display_set_fg_color(settings->color);
  
  //Set the trace color for the current channel if overload change to red
  //if (settings->ADoverload)display_set_fg_color(RED_COLOR); 
  //      else display_set_fg_color(settings->color);

  //Get the processed sample
  sample1 = scope_get_y_sample(settings, disp_first_sample);

  //Store the first sample in the trace points buffer
  tracepoints->x = lastx;
  tracepoints->y = sample1;
  tracepoints++;

  //Start with one tracepoint
  settings->noftracepoints = 1;

  //Step to the next input index
  inputindex = disp_first_sample + disp_sample_step;

  //The previous index is the index of the first sample
  previousindex = disp_first_sample;

  //Process the sample data to screen data
  for(; xpos < disp_xend; inputindex += disp_sample_step, xpos++)
  {
    //Get the current integer index into the sample buffer
    currentindex = inputindex;

    //Check if linear approximation needs to be done. (Only when step < 1) pixels are skipped if so.
    if(currentindex != previousindex)
    {
      //Set new previous index
      previousindex = currentindex;

      //Get the processed sample
      sample2 = scope_get_y_sample(settings, currentindex);

      //Store the second sample in the screen buffer
      tracepoints->x = xpos;
      tracepoints->y = sample2;
      tracepoints++;

      //One more tracepoint
      settings->noftracepoints++;
      
      //Need to draw a line here
      display_draw_line(lastx, sample1, xpos, sample2);

      sample1 = sample2;

      lastx = xpos;
    }
  }

  //When step less then 1 the last pixel needs to be interpolated between current sample and next sample.
  if(disp_sample_step < 1.0)
  {
    //Calculate the scaler for the last y value based on the x distance from the last drawn position to the end of the screen
    //divided by the x distance it takes to where the next position should be drawn (Number of x steps per sample)
    double scaler =  (727.0 - lastx) / disp_xpos_per_sample;    // (1 / samplestep);    //725

    //Get the processed sample
    sample2 = scope_get_y_sample(settings, inputindex);

    sample2 = sample1 + ((double)((double)sample2 - (double)sample1) / scaler);

    //Store the last sample in the screen buffer
    tracepoints->x = xpos;
    tracepoints->y = sample2;
    tracepoints++;

    //One more tracepoint
    settings->noftracepoints++;

    //Draw the last line
    display_draw_line(lastx, sample1, xpos, sample2);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_cursor_measurements(void)
{
  uint32 height = 5;//5
  uint32 ch1ypos = 54;//52
  uint32 ch2ypos = 54;//52
  uint32 delta;
  char   displaytext[10];

  //Check if need to do anything here
  if(scopesettings.timecursorsenable || (scopesettings.voltcursorsenable && (scopesettings.channel1.enable || scopesettings.channel2.enable)))
  {
    //Check if time cursor is enabled
    if(scopesettings.timecursorsenable)
    {
      //Add height for two text lines
      height += 28;//32  30

      //Shift the voltage text positions down
      ch1ypos += 30;
      ch2ypos += 30;//32
    }

    //Check if volt cursor is enabled
    if(scopesettings.voltcursorsenable)
    {
      //Check if channel 1 is enabled
      if(scopesettings.channel1.enable)
      {
        //Add height for one text line
        height += 50;//16  48

        //Shift the channel 2 voltage text down
        ch2ypos += 50;//16  48 
      }

      //Check if channel 2 is enabled
      if(scopesettings.channel2.enable)
      {
        //Add height for one text line
        height += 50;//16  48 52
      }
    }

    //Set gray background for the cursor measurements
    display_set_fg_color(GREY_COLOR);

    //Draw rounded rectangle depending on what is enabled.
    display_fill_rounded_rect(25, 49, 88, height, 2);//5   47

    //Use white text and font_0
    display_set_fg_color(WHITE_COLOR);
    display_set_font(&font_0);

    //Check if time cursor is enabled
    if(scopesettings.timecursorsenable)
    {
      //Time texts are always on the top two lines
      //Get the time delta based on the cursor positions
      delta = scopesettings.timecursor2position - scopesettings.timecursor1position;

      //Get the time calculation data for this time base setting.
      PSCREENTIMECALCDATA tcd = (PSCREENTIMECALCDATA)&screen_time_calc_data[scopesettings.timeperdiv];

      //For the time multiply with the scaling factor and display based on the time scale
      delta *= tcd->mul_factor;

      //Format the time for displaying
      scope_print_value(displaytext, delta, tcd->time_scale, "T ", "s");
      display_text(30, 50, displaytext);//10 52

      //Calculate the frequency for this time. Need to adjust for it to stay within 32 bits
      delta /= 10;
      delta = 1000000000 / delta;

      //Format the frequency for displaying
      scope_print_value(displaytext, delta, tcd->freq_scale, "F ", "Hz");
      display_text(30, 66, displaytext);//10 68
    }

    //Check if volt cursor is enabled
    if(scopesettings.voltcursorsenable)
    {
      PVOLTCALCDATA vcd;
      int16        Y_1;
      int16        Y_2;
      int32        Y1;
      int32        Y2;
      uint32       dY;

      //Get the volts delta based on the cursor positions
      delta = scopesettings.voltcursor2position - scopesettings.voltcursor1position;

      //Check if channel 1 is enabled
      if(scopesettings.channel1.enable)
      {
        //Use yellow text for font_0
        display_set_fg_color(CHANNEL1_COLOR);
        //display_set_font(&font_0);
        
        //Get the volts Y_1 based on the cursor position
        Y_1 = 449 - scopesettings.voltcursor1position - scopesettings.channel1.traceposition;//447
        //Get the volts Y_2 based on the cursor position
        Y_2 = 449 - scopesettings.voltcursor2position - scopesettings.channel1.traceposition;//447
        
        //Calculate the voltage based on the channel 1 settings
        vcd = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel1.magnification][scopesettings.channel1.displayvoltperdiv];

        //Multiply with the scaling factor for the channel 1 settings
        Y1 = Y_1 * vcd->mul_factor;
        Y2 = Y_2 * vcd->mul_factor;
        dY = delta * vcd->mul_factor;

        
        //Check if measures is V or A
        if(scopesettings.channel1.V_A == 0)
        {   //Channel 1 text has a variable position
            //Format the voltage for displaying
            scope_print_value(displaytext, Y1, vcd->volt_scale, "Y1 ", "V");
            display_text(30, ch1ypos, displaytext);
            scope_print_value(displaytext, Y2, vcd->volt_scale, "Y2 ", "V");
            display_text(30, ch1ypos+32, displaytext);
        
            //Use orange text for font_0
            display_set_fg_color(ORANGE_COLOR);
            scope_print_value(displaytext, dY, vcd->volt_scale, "dY ", "V");
            display_text(30, ch1ypos+16, displaytext);
        }
        else
        {
            scope_print_value(displaytext, Y1, vcd->volt_scale, "Y1 ", "A");
            display_text(30, ch1ypos, displaytext);
            scope_print_value(displaytext, Y2, vcd->volt_scale, "Y2 ", "A");
            display_text(30, ch1ypos+32, displaytext);
        
            //Use orange text for font_0
            display_set_fg_color(ORANGE_COLOR);
            scope_print_value(displaytext, dY, vcd->volt_scale, "dY ", "A");
            display_text(30, ch1ypos+16, displaytext);
        }
      }
    //************************************************************************************************
      //Check if channel 2 is enabled
      if(scopesettings.channel2.enable)
      {
        //Use blue text for font_0
        display_set_fg_color(CHANNEL2_COLOR);
        //display_set_font(&font_0);
        
        //Get the volts Y_1 based on the cursor position
        Y_1 = 449 - scopesettings.voltcursor1position - scopesettings.channel2.traceposition;//447
        //Get the volts Y_2 based on the cursor position
        Y_2 = 449 - scopesettings.voltcursor2position - scopesettings.channel2.traceposition;
        
        //Calculate the voltage based on the channel 2 settings
        vcd = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel2.magnification][scopesettings.channel2.displayvoltperdiv];

        //Multiply with the scaling factor for the channel 2 settings
        Y1 = Y_1 * vcd->mul_factor;
        Y2 = Y_2 * vcd->mul_factor;
        dY = delta * vcd->mul_factor;

        
        //Check if measures is V or A
        if(scopesettings.channel2.V_A == 0)    
        {   //Channel 2 text has a variable position
            //Format the voltage for displaying
            scope_print_value(displaytext, Y1, vcd->volt_scale, "Y1 ", "V");
            display_text(30, ch2ypos, displaytext);
            scope_print_value(displaytext, Y2, vcd->volt_scale, "Y2 ", "V");
            display_text(30, ch2ypos+32, displaytext);
            
            //Use magenta blue text for font_0
            display_set_fg_color(0x9999FF);
            scope_print_value(displaytext, dY, vcd->volt_scale, "dY ", "V");
            display_text(30, ch2ypos+16, displaytext);
        }
        else 
        {   scope_print_value(displaytext, Y1, vcd->volt_scale, "Y1 ", "A");
            display_text(30, ch2ypos, displaytext);
            scope_print_value(displaytext, Y2, vcd->volt_scale, "Y2 ", "A");
            display_text(30, ch2ypos+32, displaytext);
            
            //Use magenta blue text for font_0
            display_set_fg_color(0x9999FF);
            scope_print_value(displaytext, dY, vcd->volt_scale, "dY ", "A");
            display_text(30, ch2ypos+16, displaytext);
        }
        
        
        
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_measurements(void)
{
  //Check if channel1 is active and hide measurements is 0-off and set short time base 
  if((scopesettings.channel1.enable||scopesettings.ref1||scopesettings.ref2||scopesettings.ref3||scopesettings.ref4)
    &&(!scopesettings.hide_values_CH1)&&(!scopesettings.long_mode))
  {
    //Display the enabled measurements if it is active
    switch (scopesettings.source1_measures)
    {
      case 0: scope_display_channel_measurements(&scopesettings.channel1, scopesettings.measuresstate[0], 5, CHANNEL1_COLOR); break;
      case 1: scope_display_channel_measurements(&scopesettings.ch_ref1,  scopesettings.measuresstate[0], 5, scopesettings.ch_ref1.color); break;
      case 2: scope_display_channel_measurements(&scopesettings.ch_ref2,  scopesettings.measuresstate[0], 5, scopesettings.ch_ref2.color); break;
      case 3: scope_display_channel_measurements(&scopesettings.ch_ref3,  scopesettings.measuresstate[0], 5, scopesettings.ch_ref3.color); break;
      case 4: scope_display_channel_measurements(&scopesettings.ch_ref4,  scopesettings.measuresstate[0], 5, scopesettings.ch_ref4.color); break;
      default: scope_display_channel_measurements(&scopesettings.channel1, scopesettings.measuresstate[0], 5, CHANNEL1_COLOR); break;
    }
  }

  //Check if channel2 is active and hide measurements is 0-off and set short time base 
  if((scopesettings.channel2.enable||scopesettings.ref5||scopesettings.ref6||scopesettings.ref7||scopesettings.ref8)
    &&(!scopesettings.hide_values_CH2)&&(!scopesettings.long_mode))
  {
    //Display the enabled measurements if it is active
    switch (scopesettings.source2_measures)
    {
      case 0: scope_display_channel_measurements(&scopesettings.channel2, scopesettings.measuresstate[1], 366, CHANNEL2_COLOR); break;
      case 1: scope_display_channel_measurements(&scopesettings.ch_ref5,  scopesettings.measuresstate[1], 366, scopesettings.ch_ref5.color); break;
      case 2: scope_display_channel_measurements(&scopesettings.ch_ref6,  scopesettings.measuresstate[1], 366, scopesettings.ch_ref6.color); break;
      case 3: scope_display_channel_measurements(&scopesettings.ch_ref7,  scopesettings.measuresstate[1], 366, scopesettings.ch_ref7.color); break;
      case 4: scope_display_channel_measurements(&scopesettings.ch_ref8,  scopesettings.measuresstate[1], 366, scopesettings.ch_ref8.color); break;
      default: scope_display_channel_measurements(&scopesettings.channel2, scopesettings.measuresstate[1], 366, CHANNEL2_COLOR); break;
    }
  }
  
}

//----------------------------------------------------------------------------------------------------------------------------------

const MEASUREMENTFUNCTION measurementfunctions[12] =
{
  scope_display_vmax,
  scope_display_vmin,
  scope_display_vavg,
  scope_display_vrms,
  scope_display_vpp,
  scope_display_vp,
  scope_display_freq,
  scope_display_cycle,
  scope_display_time_plus,
  scope_display_time_min,
  scope_display_duty_plus,
  scope_display_duty_min
};

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_channel_measurements(PCHANNELSETTINGS settings, uint8 *measuresstate, uint32 xstart, uint32 color)
{
  int i;
  int x,xe,y;

  x  = xstart;
  xe = xstart + 359;
  y  = 426; //456 move up - long base
  
  //Process the twelve possible measurements for this channel
  for(i=0;i<12;i++)
  {
    //Check if the current one is enabled
    if(measuresstate[i])
    {
      //Set gray background for the measurements tile
      display_set_fg_color(GREY_COLOR);

      //Fill rounded rectangle for the tile
      display_fill_rounded_rect(x, y, 118, 22, 2);

      //Set the channel color for the name part
      display_set_fg_color(color);
      
      //Fill rounded rectangle for the name part
      display_fill_rounded_rect(x + 3, y + 3, 45, 16, 2);
      
      //Use black text and font_0 for the name
      display_set_fg_color(BLACK_COLOR);
      display_set_font(&font_0);
      display_text(x + 6, y + 3, (char *)measurement_names[i]);

      //Measurement in white text
      display_set_fg_color(WHITE_COLOR);
      
      //Erase any previous text
      measurementtext[0] = 0;
      
      //Display the data for this measurement
      measurementfunctions[i](settings);
      
      //Display the formated data
      display_text(x + 55, y + 3, measurementtext);

      //Select next column
      x += 120;
      
      //Check if past the last for this row
      if(x > xe)
      {
        //Back to beginning of the rows
        x = xstart;
        
        //Up to the row above
        y -= 24;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vmax(PCHANNELSETTINGS settings)
{
  //For the maximum take of the center ADC value
  scope_display_voltage(settings, settings->max - 128);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vmin(PCHANNELSETTINGS settings)
{
  //For the minimum take of the center ADC value
  scope_display_voltage(settings, settings->min - 128);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vavg(PCHANNELSETTINGS settings)
{
  //For the average take of the center ADC value
  scope_display_voltage(settings, settings->average - 128);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vrms(PCHANNELSETTINGS settings)
{
  //The rms has already been centered during the summation so use it as is
  scope_display_voltage(settings, settings->rms);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vpp(PCHANNELSETTINGS settings)
{
  //For the peak peak just use the value as is
  scope_display_voltage(settings, settings->peakpeak);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_vp(PCHANNELSETTINGS settings)
{
  //Determine the two absolute extremes
  int32 min = 128 - settings->min;
  int32 max = settings->max - 128;
  
  //Display the biggest of the two
  if(min > max)
  {
    //Use the below the center value when it is the biggest
    scope_display_voltage(settings, min);
  }
  else
  {
    //Use the above the center value when it is the biggest
    scope_display_voltage(settings, max);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_freq(PCHANNELSETTINGS settings)
{
  if(settings->frequencyvalid)
  {
    //Format the frequency for displaying
    scope_print_value(measurementtext, settings->frequency, freq_calc_data[scopesettings.samplerate].freq_scale, "", "Hz");
  }
  else
  {
    strcpy(measurementtext, "xxxHz");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_cycle(PCHANNELSETTINGS settings)
{
  //Only when the frequency is valid calculate the time
  if(settings->frequencyvalid)
   // if((settings->frequencyvalid)||(settings->enable))
  {
    //Format the time for displaying
    scope_print_value(measurementtext, (((uint64)settings->periodtime * time_calc_data[scopesettings.samplerate].mul_factor) >> 20), time_calc_data[scopesettings.samplerate].time_scale, "", "s");
  }
  else
  {
    strcpy(measurementtext, "xxxs");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_time_plus(PCHANNELSETTINGS settings)
{
  //Only when the frequency is valid calculate the time
  if(settings->frequencyvalid)
  {
    //Format the time for displaying
    scope_print_value(measurementtext, (((uint64)settings->hightime * time_calc_data[scopesettings.samplerate].mul_factor) >> 20), time_calc_data[scopesettings.samplerate].time_scale, "", "s");
  }
  else
  {
    strcpy(measurementtext, "xxxs");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_time_min(PCHANNELSETTINGS settings)
{
  //Only when the frequency is valid calculate the time
  if(settings->frequencyvalid)
  {
    //Format the time for displaying
    scope_print_value(measurementtext, (((uint64)settings->lowtime * time_calc_data[scopesettings.samplerate].mul_factor) >> 20), time_calc_data[scopesettings.samplerate].time_scale, "", "s");
  }
  else
  {
    strcpy(measurementtext, "xxxs");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_duty_plus(PCHANNELSETTINGS settings)
{
  char *buffer;
  
  //Only when the frequency is valid calculate the time
  if(settings->frequencyvalid)
  {
    //Format the time for displaying
    buffer = scope_print_decimal(measurementtext, (((uint64)settings->hightime * 1000) / settings->periodtime), 1, 0);
    
    //Add the duty cycle sign
    strcpy(buffer, "%"); 
  }
  else
  {
    strcpy(measurementtext, "xx%");
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_duty_min(PCHANNELSETTINGS settings)
{
  char *buffer;

  //Only when the frequency is valid calculate the time
  if(settings->frequencyvalid)
  {
    //Format the time for displaying
    buffer = scope_print_decimal(measurementtext, (((uint64)settings->lowtime * 1000) / settings->periodtime), 1, 0);

    //Add the duty cycle sign
    strcpy(buffer, "%");  
  }
  else
  {
    strcpy(measurementtext, "xx%");
  }
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

void scope_display_voltage(PCHANNELSETTINGS settings, int32 value)
{
  PVOLTCALCDATA vcd;
  int32         volts;
  
  //Calculate the voltage based on the channel settings
  vcd = (PVOLTCALCDATA)&volt_calc_data[settings->magnification][settings->displayvoltperdiv];

  //Adjust the data for the correct voltage per div setting
  volts = (value * settings->input_calibration[settings->samplevoltperdiv]) >> VOLTAGE_SHIFTER;

  //Scale the data based on the two volt per div settings when they differ
  if(settings->displayvoltperdiv != settings->samplevoltperdiv)
  {
    //Scaling factor is based on the two volts per division settings
    volts = (volts * vertical_scaling_factors[settings->displayvoltperdiv][settings->samplevoltperdiv]) / 10000;
  }
  
  //Multiply with the scaling factor for the channel settings
  volts *= vcd->mul_factor;

  //Format the voltage for displaying
  if(settings->V_A == 0) scope_print_value(measurementtext, volts, vcd->volt_scale, "", "V");
    else scope_print_value(measurementtext, volts, vcd->volt_scale, "", "A");
  
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_print_value(char *buffer, int32 value, uint32 scale, char *header, char *sign)
{
  uint32 negative = 0;
  
  //Copy the header into the string buffer
  buffer = strcpy(buffer, header);

  //Check if negative value
  if(value < 0)
  {
    //Negate if so and signal negative sign needed
    value = -value;
    negative = 1;
  }
  
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
    buffer = scope_print_decimal(buffer, value, 2, negative);
  }
  else if(value < 10000)
  {
    //More then 1000 but less then 10000 means xx.y
    value /= 10;
    buffer = scope_print_decimal(buffer, value, 1, negative);
  }
  else
  {
    //More then 10000 and less then 100000 means xxx
    value /= 100;
    buffer = scope_print_decimal(buffer, value, 0, negative);
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

char *scope_print_decimal(char *buffer, int32 value, uint32 decimals, uint32 negative)
{
  char   b[12];
  uint32 i = 12;   //Start beyond the array since the index is pre decremented
  uint32 s;

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
        if ((value/10) == 0) b[--i] = '0';  //if 0.xxV then 0 before .
      }

      //Take of the current digit
      value /= 10; 
    }
  }

  //Check if negative number and if so put a minus in front of it
  if(negative)
    b[--i] = '-';
  
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
  uint32 *ptr = viewfilesetupdata;
  uint32 index = 0;
  uint32 channel;
  uint32 measurement;
  uint32 checksum = 0;

  //Best to clear the buffer first since not all bytes are used
  memset((uint8 *)viewfilesetupdata, 0, sizeof(viewfilesetupdata));

  //Put in a version number for the waveform view file
  ptr[1] = WAVEFORM_FILE_VERSION;
  //memcpy(&ptr[1], WAVEFORM_FILE_VERSION, 8);
  
  //Put in a firmware version for the waveform view file
  memcpy(&ptr[2], VERSION_STRING, 7);
    
  //Leave space for file version and checksum data  21
  index = CHANNEL1_SETTING_OFFSET;
  
  //Copy the needed channel 1 settings and measurements 
  ptr[index++] = scopesettings.channel1.enable;
  ptr[index++] = scopesettings.channel1.displayvoltperdiv;
  ptr[index++] = scopesettings.channel1.samplevoltperdiv;
  ptr[index++] = scopesettings.channel1.fftenable;
  ptr[index++] = scopesettings.channel1.coupling;
  ptr[index++] = scopesettings.channel1.magnification;
  ptr[index++] = scopesettings.channel1.V_A;
  ptr[index++] = scopesettings.channel1.traceposition;
  ptr[index++] = scopesettings.channel1.min;
  ptr[index++] = scopesettings.channel1.max;
  ptr[index++] = scopesettings.channel1.average;
  ptr[index++] = scopesettings.channel1.rms;
  ptr[index++] = scopesettings.channel1.center;
  ptr[index++] = scopesettings.channel1.peakpeak;
  ptr[index++] = scopesettings.channel1.frequencyvalid;
  ptr[index++] = scopesettings.channel1.frequency;
  ptr[index++] = scopesettings.channel1.lowtime;
  ptr[index++] = scopesettings.channel1.hightime;
  ptr[index++] = scopesettings.channel1.periodtime;
  ptr[index++] = scopesettings.channel1.dcoffset;
  ptr[index++] = scopesettings.channel1.invert;

  //Leave some space for channel 2 settings changes 21
  index = CHANNEL2_SETTING_OFFSET;
  
  //Copy the needed channel 2 settings and measurements 
  ptr[index++] = scopesettings.channel2.enable;
  ptr[index++] = scopesettings.channel2.displayvoltperdiv;
  ptr[index++] = scopesettings.channel2.samplevoltperdiv;
  ptr[index++] = scopesettings.channel2.fftenable;
  ptr[index++] = scopesettings.channel2.coupling;
  ptr[index++] = scopesettings.channel2.magnification;
  ptr[index++] = scopesettings.channel2.V_A;
  ptr[index++] = scopesettings.channel2.traceposition;
  ptr[index++] = scopesettings.channel2.min;
  ptr[index++] = scopesettings.channel2.max;
  ptr[index++] = scopesettings.channel2.average;
  ptr[index++] = scopesettings.channel2.rms;
  ptr[index++] = scopesettings.channel2.center;
  ptr[index++] = scopesettings.channel2.peakpeak;
  ptr[index++] = scopesettings.channel2.frequencyvalid;
  ptr[index++] = scopesettings.channel2.frequency;
  ptr[index++] = scopesettings.channel2.lowtime;
  ptr[index++] = scopesettings.channel2.hightime;
  ptr[index++] = scopesettings.channel2.periodtime;
  ptr[index++] = scopesettings.channel2.dcoffset;
  ptr[index++] = scopesettings.channel2.invert;

  //Leave some space for trigger information changes    11
  index = TRIGGER_SETTING_OFFSET;
  
  //Copy the needed scope trigger settings      
  ptr[index++] = scopesettings.timeperdiv;
  ptr[index++] = scopesettings.samplerate;
  ptr[index++] = scopesettings.triggermode;
  ptr[index++] = scopesettings.triggeredge;
  ptr[index++] = scopesettings.triggerchannel;
  ptr[index++] = scopesettings.triggerlevel;
  //ptr[index++] = scopesettings.triggerhorizontalposition;
  
  ptr[index++] = scopesettings.triggerhorizontalposition>>16;         //int32 !!!
  ptr[index++] = scopesettings.triggerhorizontalposition & 0xFFFF;    //int32 !!!
  
  ptr[index++] = scopesettings.triggerverticalposition;
  ptr[index++] = disp_have_trigger;
  ptr[index++] = disp_trigger_index;

  //Leave some space for other scope settings changes   10
  index = OTHER_SETTING_OFFSET;
  
  //Copy the needed other scope settings        
  ptr[index++] = scopesettings.movespeed;
  ptr[index++] = scopesettings.rightmenustate;
  ptr[index++] = scopesettings.screenbrightness;
  ptr[index++] = scopesettings.gridbrightness;
  ptr[index++] = scopesettings.alwaystrigger50;
  ptr[index++] = scopesettings.xymodedisplay;
  ptr[index++] = scopesettings.confirmationmode;
  ptr[index++] = scopesettings.long_mode;
  ptr[index++] = scopesettings.average_mode; 
  ptr[index++] = scopesettings.long_memory;

  //Leave some space for other cursor settings changes  6
  index = CURSOR_SETTING_OFFSET;
  
  //Copy the cursor settings                    
  ptr[index++] = scopesettings.timecursorsenable;
  ptr[index++] = scopesettings.voltcursorsenable;
  ptr[index++] = scopesettings.timecursor1position;
  ptr[index++] = scopesettings.timecursor2position;
  ptr[index++] = scopesettings.voltcursor1position;
  ptr[index++] = scopesettings.voltcursor2position;
  ptr[index++] = scopesettings.lockcursors; //lock move cursors
  
  index = MEASUREMENT_SETTING_OFFSET;
  
  //Copy measurements HIDE states
  ptr[index++] = scopesettings.hide_values_CH1;
  ptr[index++] = scopesettings.hide_values_CH2;
  
  //Copy the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(measurement=0;measurement<12;measurement++)
    {
      //Copy the current measurement state and point to the next one
       ptr[index++] = scopesettings.measuresstate[channel][measurement];
    }
  }
  
  //Calculate a checksum over the settings data
  for(index=1;index<VIEW_NUMBER_OF_SETTINGS;index++)
  {
    checksum += ptr[index];
  }
  
  //Add the sample data too
  for(index=0;index<750;index++)
  {
    //Add both the channels
    checksum += channel1tracebuffer[index];
    checksum += channel2tracebuffer[index];
  }

  //Store the checksum at the beginning of the file
  ptr[0] = checksum;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_setup_from_file(void)
{
  uint32 *ptr = viewfilesetupdata;
  uint32 index = 0;
  uint32  tmp = 0;
  uint32 channel;
  uint32 measurement;

  //Leave space for file version and checksum data  21
  index = CHANNEL1_SETTING_OFFSET;
  
  //Copy the needed channel 1 settings and measurements
  scopesettings.channel1.enable            = ptr[index++];
  scopesettings.channel1.displayvoltperdiv = ptr[index++];
  scopesettings.channel1.samplevoltperdiv  = ptr[index++];
  scopesettings.channel1.fftenable         = ptr[index++];
  scopesettings.channel1.coupling          = ptr[index++];
  scopesettings.channel1.magnification     = ptr[index++];
  scopesettings.channel1.V_A               = ptr[index++];
  scopesettings.channel1.traceposition     = ptr[index++];
  scopesettings.channel1.min               = ptr[index++];
  scopesettings.channel1.max               = ptr[index++];
  scopesettings.channel1.average           = ptr[index++];
  scopesettings.channel1.rms               = ptr[index++];
  scopesettings.channel1.center            = ptr[index++];
  scopesettings.channel1.peakpeak          = ptr[index++];
  scopesettings.channel1.frequencyvalid    = ptr[index++];
  scopesettings.channel1.frequency         = ptr[index++];
  scopesettings.channel1.lowtime           = ptr[index++];
  scopesettings.channel1.hightime          = ptr[index++];
  scopesettings.channel1.periodtime        = ptr[index++];
  scopesettings.channel1.dcoffset          = ptr[index++];
  scopesettings.channel1.invert            = ptr[index++];

  //Leave some space for channel 2 settings changes 21
  index = CHANNEL2_SETTING_OFFSET;
  
  //Copy the needed channel 2 settings and measurements
  scopesettings.channel2.enable            = ptr[index++];
  scopesettings.channel2.displayvoltperdiv = ptr[index++];
  scopesettings.channel2.samplevoltperdiv  = ptr[index++];
  scopesettings.channel2.fftenable         = ptr[index++];
  scopesettings.channel2.coupling          = ptr[index++];
  scopesettings.channel2.magnification     = ptr[index++];
  scopesettings.channel2.V_A               = ptr[index++];
  scopesettings.channel2.traceposition     = ptr[index++];
  scopesettings.channel2.min               = ptr[index++];
  scopesettings.channel2.max               = ptr[index++];
  scopesettings.channel2.average           = ptr[index++];
  scopesettings.channel2.rms               = ptr[index++];
  scopesettings.channel2.center            = ptr[index++];
  scopesettings.channel2.peakpeak          = ptr[index++];
  scopesettings.channel2.frequencyvalid    = ptr[index++];
  scopesettings.channel2.frequency         = ptr[index++];
  scopesettings.channel2.lowtime           = ptr[index++];
  scopesettings.channel2.hightime          = ptr[index++];
  scopesettings.channel2.periodtime        = ptr[index++];
  scopesettings.channel2.dcoffset          = ptr[index++];
  scopesettings.channel2.invert            = ptr[index++];
  
  //Leave some space for trigger information changes    11
  index = TRIGGER_SETTING_OFFSET;
  
  //Copy the needed scope trigger settings
  scopesettings.timeperdiv                = ptr[index++];
  scopesettings.samplerate                = ptr[index++];
  scopesettings.triggermode               = ptr[index++];
  scopesettings.triggeredge               = ptr[index++];
  scopesettings.triggerchannel            = ptr[index++];
  scopesettings.triggerlevel              = ptr[index++];
  //scopesettings.triggerhorizontalposition = ptr[index++];
  
  tmp                                     = ptr[index++];
  scopesettings.triggerhorizontalposition = tmp<<16;
  scopesettings.triggerhorizontalposition+= ptr[index++]; //int32 !!!
  
  scopesettings.triggerverticalposition   = ptr[index++];
  disp_have_trigger                       = ptr[index++];
  disp_trigger_index                      = ptr[index++];

  //Leave some space for other scope settings changes   10
  index = OTHER_SETTING_OFFSET;
  
  //Copy the needed other scope settings
  scopesettings.movespeed        = ptr[index++];
  scopesettings.rightmenustate   = ptr[index++];
  scopesettings.screenbrightness = ptr[index++];
  scopesettings.gridbrightness   = ptr[index++];
  scopesettings.alwaystrigger50  = ptr[index++];
  scopesettings.xymodedisplay    = ptr[index++];
  scopesettings.confirmationmode = ptr[index++];
  scopesettings.long_mode        = ptr[index++];
  scopesettings.average_mode     = ptr[index++];
  scopesettings.long_memory      = ptr[index++];
  scopesettings.long_memory      = 0;           //docasne zrusene

  index = CURSOR_SETTING_OFFSET;
  
  //Copy the cursor settings
  scopesettings.timecursorsenable   = ptr[index++];
  scopesettings.voltcursorsenable   = ptr[index++];
  scopesettings.timecursor1position = ptr[index++];
  scopesettings.timecursor2position = ptr[index++];
  scopesettings.voltcursor1position = ptr[index++];
  scopesettings.voltcursor2position = ptr[index++];
  scopesettings.lockcursors         = ptr[index++]; //lock move cursors
  
  //Leave some space for other cursor settings changes  26
  index = MEASUREMENT_SETTING_OFFSET;
  
  //Copy measurements HIDE states
  scopesettings.hide_values_CH1 = ptr[index++];
  scopesettings.hide_values_CH2 = ptr[index++];
  
  //Copy the measurements enable states
  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(measurement=0;measurement<12;measurement++)
    {
      //Copy the current measurement state and point to the next one
      scopesettings.measuresstate[channel][measurement] = ptr[index++];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_check_waveform_file(void)
{
  uint32 index;
  uint32 checksum = 0;
  
  //Calculate a checksum over the settings data
  for(index=1;index<VIEW_NUMBER_OF_SETTINGS;index++)
  {
    checksum += viewfilesetupdata[index];
  }
  
  //Add the sample data too
  for(index=0;index<750;index++)
  {
    //Add both the channels
    checksum += channel1tracebuffer[index];
    checksum += channel2tracebuffer[index];
  }

  //Check if it matches the checksum in the file
  if(viewfilesetupdata[0] == checksum)
  {
    return(0);
  }
  
  //Something is wrong so signal it
  return(-1);
}

//----------------------------------------------------------------------------------------------------------------------------------

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

  //Copy the path name first
  memcpy(viewfilename, view_file_path[viewtype & VIEW_TYPE_MASK].name, view_file_path[viewtype & VIEW_TYPE_MASK].length);
  
  //Copy in the decimal file number
  memcpy(&viewfilename[view_file_path[viewtype & VIEW_TYPE_MASK].length], &b[i], s);

  //Add the extension
  memcpy(&viewfilename[view_file_path[viewtype & VIEW_TYPE_MASK].length + s], view_file_extension[viewtype & VIEW_TYPE_MASK], 5);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_load_thumbnail_file(void)
{
  int32  result;
  uint32 size;

  //Set the name in the global buffer for message display
  strcpy(viewfilename, view_file_path[viewtype & VIEW_TYPE_MASK].name);

  //Check the status of the directory for this view type
  result = f_stat(viewfilename, 0);

  //See if there is an error
  if(result != FR_OK)
  {
    //If so check if the directory does not exist
    if(result == FR_NO_FILE)
    {
      //Create the directory
      result = f_mkdir(viewfilename);

      if(result != FR_OK)
      {
        //Show a message stating creating the directory failed
        scope_display_file_status_message(MESSAGE_DIRECTORY_CREATE_FAILED, 0);

        //No sense to continue, so return with an error
        return(-1);
      }

      //Set the name in the global buffer for message display
      strcpy(viewfilename, thumbnail_file_names[viewtype & VIEW_TYPE_MASK]);

      //With the directory created it is also needed to create the thumbnail file
      result = f_open(&viewfp, viewfilename, FA_CREATE_ALWAYS | FA_WRITE);

      if(result != FR_OK)
      {
        //Show a message stating creating the file failed
        scope_display_file_status_message(MESSAGE_FILE_CREATE_FAILED, 0);

        //No sense to continue, so return with an error
        return(-1);
      }

      //Reset the number of available items
      viewavailableitems = 0;

      //Write the no thumbnails yet data
      result = f_write(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

      //Close the file
      f_close(&viewfp);

      if(result != FR_OK)
      {
        //Show a message stating writing the file failed
        scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

        //No sense to continue, so return with an error
        return(-1);
      }

      //No items to be loaded so done
      return(0);
    }
    else
    {
      //Show a message stating the file system failed
      scope_display_file_status_message(MESSAGE_FILE_SYSTEM_FAILED, 1);

      //No sense to continue, so return with an error
      return(-1);
    }
  }

  //Clear the file number list to avoid errors when swapping between the two types
  memset(viewfilenumberdata, 0, sizeof(viewfilenumberdata));

  //Set the name in the global buffer for message display
  strcpy(viewfilename, thumbnail_file_names[viewtype & VIEW_TYPE_MASK]);

  //Try to open the thumbnail file for this view type
  result = f_open(&viewfp, viewfilename, FA_READ);

  //Check the result
  if(result == FR_OK)
  {
    //Opened ok, so read the number of items
    result = f_read(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

    if(result != FR_OK)
    {
      //Show a message stating reading the file failed
      scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);

      //Close the file
      f_close(&viewfp);

      //No sense to continue, so return with an error
      return(-1);
    }

    //Based on the number of available items load the rest of the data
    if(viewavailableitems)
    {
      //Calculate the number of bytes to read for the file number list
      size = viewavailableitems * sizeof(uint16);

      //Check if there is an error
      if(size > VIEW_FILE_NUMBER_DATA_SIZE)
      {
        //Show a message stating that the thumbnail file is corrupt
        scope_display_file_status_message(MESSAGE_THUMBNAIL_FILE_CORRUPT, 0);

        //Reset the number of available items
        viewavailableitems = 0;

        //Write the no thumbnails yet data
        result = f_write(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

        //Close the file
        f_close(&viewfp);

        if(result != FR_OK)
        {
          //Show a message stating writing the file failed
          scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

          //No sense to continue, so return with an error
          return(-1);
        }

        //No items to be loaded any more so done
        return(0);
      }

      //Read the file number data
      result = f_read(&viewfp, viewfilenumberdata, size, 0);

      if(result != FR_OK)
      {
        //Show a message stating reading the file failed
        scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);

        //Close the file
        f_close(&viewfp);

        //No sense to continue, so return with an error
        return(-1);
      }

      //Calculate the number of bytes to read for the thumbnail data
      size = viewavailableitems * sizeof(THUMBNAILDATA);

      //Check if there is an error
      if(size > VIEW_THUMBNAIL_DATA_SIZE)
      {
        //Show a message stating that the thumbnail file is corrupt
        scope_display_file_status_message(MESSAGE_THUMBNAIL_FILE_CORRUPT, 0);

        //Reset the number of available items
        viewavailableitems = 0;

        //Write the no thumbnails yet data
        result = f_write(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

        //Close the file
        f_close(&viewfp);

        if(result != FR_OK)
        {
          //Show a message stating writing the file failed
          scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

          //No sense to continue, so return with an error
          return(-1);
        }

        //No items to be loaded any more so done
        return(0);
      }

      //Read the thumbnail data
      result = f_read(&viewfp, viewthumbnaildata, size, 0);

      if(result != FR_OK)
      {
        //Show a message stating reading the file failed
        scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);

        //Close the file
        f_close(&viewfp);

        //No sense to continue, so return with an error
        return(-1);
      }
    }

    //Close the file
    f_close(&viewfp);
  }
  //Failure then check if file does not exist
  else if(result == FR_NO_FILE)
  {
    //Need the file so create it
    result = f_open(&viewfp, viewfilename, FA_CREATE_ALWAYS | FA_WRITE);

    //Check if file is created ok
    if(result == FR_OK)
    {
      //Reset the number of available items
      viewavailableitems = 0;

      //Write the no thumbnails yet data
      result = f_write(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

      //Close the file
      f_close(&viewfp);

      if(result != FR_OK)
      {
        //Show a message stating writing the file failed
        scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

        //No sense to continue, so return with an error
        return(-1);
      }
    }
    else
    {
      //Show a message stating creating the file failed
      scope_display_file_status_message(MESSAGE_FILE_CREATE_FAILED, 0);

      //No sense to continue, so return with an error
      return(-1);
    }
  }

  //Signal all went well
  return(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_save_thumbnail_file(void)
{
  int32  result;
  uint32 size;

  //Set the name in the global buffer for message display
  strcpy(viewfilename, thumbnail_file_names[viewtype & VIEW_TYPE_MASK]);

  //Try to open the thumbnail file for this view type
  result = f_open(&viewfp, viewfilename, FA_CREATE_ALWAYS | FA_WRITE);

  //Only if the file is opened write to it
  if(result == FR_OK)
  {
    //Write the number of available items to the file
    result = f_write(&viewfp, &viewavailableitems, sizeof(viewavailableitems), 0);

    if(result != FR_OK)
    {
      //Show a message stating writing the file failed
      scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

      //Close the file
      f_close(&viewfp);

      //No sense to continue, so return with an error
      return(-1);
    }

    //Based on the number of available items write the rest of the data
    if(viewavailableitems)
    {
      //Calculate the number of bytes to write for the file number list
      size = viewavailableitems * sizeof(uint16);

      //Write the file number list to the file
      result = f_write(&viewfp, viewfilenumberdata, size, 0);

      if(result != FR_OK)
      {
        //Show a message stating writing the file failed
        scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

        //Close the file
        f_close(&viewfp);

        //No sense to continue, so return with an error
        return(-1);
      }

      //Calculate the number of bytes to write for the thumbnail data
      size = viewavailableitems * sizeof(THUMBNAILDATA);

      //Write the thumbnail data to the file
      result = f_write(&viewfp, viewthumbnaildata, size, 0);

      if(result != FR_OK)
      {
        //Show a message stating writing the file failed
        scope_display_file_status_message(MESSAGE_FILE_WRITE_FAILED, 0);

        //Close the file
        f_close(&viewfp);

        //No sense to continue, so return with an error
        return(-1);
      }
    }

    //Close the file
    f_close(&viewfp);
  }
  else
  {
    //Show a message stating the file system failed
    scope_display_file_status_message(MESSAGE_FILE_SYSTEM_FAILED, 1);

    //No sense to continue, so return with an error
    return(-1);
  }

  //Signal no problem occurred
  return(0);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_view_item_file(int32 type)
{
  uint32  newnumber;
  uint32  result;
  uint16 *fnptr;
  uint16 *eptr;

  //Save the current view type to be able to determine if the thumbnail file need to be reloaded
  uint32 currentviewtype = viewtype;

  //Switch to the given type
  viewtype = type;

  //Load the thumbnail file for this type. Needed for finding the file name and to add the thumbnail
  if(scope_load_thumbnail_file() != 0)
  {
    //Loading the thumbnail file failed so no sense in going on
    return;
  }

  //Check if there is still room for a new item
  if(viewavailableitems >= VIEW_MAX_ITEMS)
  {
    //Show the user there is no more room for a new item
    scope_display_file_status_message(MESSAGE_THUMBNAIL_FILE_FULL, 1);

    //No sense to continue
    return;
  }

  //Set the end pointer
  eptr = &viewfilenumberdata[viewavailableitems];

  //Find the first free file number
  //Most likely a more efficient solution for this problem exists, but this beats the original code where they try if a file number is free on the SD card with f_open
  for(newnumber=1;newnumber<VIEW_MAX_ITEMS;newnumber++)
  {
    //Start at the beginning of the list
    fnptr = viewfilenumberdata;

    //Go through the list to see if the current number is in the list
    while(fnptr < eptr)
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

  //Bump all the entries in the list up
  memmove(&viewfilenumberdata[1], &viewfilenumberdata[0], viewavailableitems * sizeof(uint16));

  //Fill in the new number
  viewfilenumberdata[0] = newnumber;

  //Bump the thumbnails up to make room for the new one
  memmove(&viewthumbnaildata[1], &viewthumbnaildata[0], viewavailableitems * sizeof(THUMBNAILDATA));

  //Setup the filename for in the thumbnail
  scope_print_file_name(newnumber);
  
  //Create the thumbnail
  scope_create_thumbnail(&viewthumbnaildata[0]);

  //One more item in the list
  viewavailableitems++;
  
  //save the amended thumbnail file
  scope_save_thumbnail_file();

  //Copy the filename from the thumbnail filename, since the global one got written over in the saving of the thumbnail
  //Might need a re write of the message setup
  strcpy(viewfilename, viewthumbnaildata[0].filename); 
  
  //Open the new file. On failure signal this and quit
  result = f_open(&viewfp, viewfilename, FA_CREATE_ALWAYS | FA_WRITE);

  //Check if file created without problems
  if(result == FR_OK)
  {
    //For pictures the bitmap header and the screen data needs to be written
    if(type == VIEW_TYPE_PICTURE)
    {     
      if(tag_in_BMP)
      {
        //Set black color for background
        display_set_fg_color(BLACK_COLOR);

        //Display the file name
        PTHUMBNAILDATA thumbnaildata;
        thumbnaildata = &viewthumbnaildata[0];//0 viewcurrentindex

        if(onoffRTC) //if RTCon view rtc info
          {
            decodethumbnailfilename(thumbnaildata->filename);
            //Clear the text area
            display_fill_rect(488, 70, 210, 15);//555 140
            //Display the file name
            //Use white text and font_0
            display_set_fg_color(WHITE_COLOR);
            display_set_font(&font_0);
            display_text(488, 70, filenameRTC);
          }
          else 
          {
            //Clear the text area
            display_fill_rect(555, 70, 140, 15);//555 140
            //Display the file name
            //Use white text and font_0
            display_set_fg_color(WHITE_COLOR);
            display_set_font(&font_0);
            display_text(555, 70, thumbnaildata->filename);
          }
      }
      
      //Write the bitmap header
      result = f_write(&viewfp, bmpheader, sizeof(bmpheader), 0);

      //Check if still ok to proceed
      if(result == FR_OK)
      {
        //Write the pixel data
        result = f_write(&viewfp, (uint8 *)maindisplaybuffer, PICTURE_DATA_SIZE, 0);
      }
    }
    //For pictures the bitmap header and the screen data needs to be written
    else if(type == VIEW_TYPE_WAVEFORM)
    {
      //For the waveform the setup and the waveform data needs to be written
      //Save the settings for the trace portion of the data and write them to the file
      scope_prepare_setup_for_file();

      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)channel1tracebuffer, SAMPLE_COUNT, 0)) == FR_OK)//3000
        {
          //Save the channel 2 raw sample data
          result = f_write(&viewfp, (uint8 *)channel2tracebuffer, SAMPLE_COUNT, 0);//3000
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
    scope_display_file_status_message(MESSAGE_FILE_CREATE_FAILED, 0);
  }

  //When a picture is saved while viewing a waveform, reload the waveform lists
  if((type == VIEW_TYPE_PICTURE) && (currentviewtype == VIEW_TYPE_WAVEFORM) && (scopesettings.waveviewmode))
  {
    //Restore the previous view type
    viewtype = currentviewtype;

    //Load the thumbnail file
    scope_load_thumbnail_file();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_remove_item_from_thumbnails(uint32 delete)
{
  //Set the index to the next item
  uint32 nextindex = viewcurrentindex + 1;

  //Calculate the number of items to move
  uint32 count = (viewavailableitems - nextindex);

  //Only delete the file when requested
  if(delete)
  {
    //Set the name in the global buffer for message display
    strcpy(viewfilename, viewthumbnaildata[viewcurrentindex].filename);

    //Delete the file from the SD card
    if(f_unlink(viewfilename) != FR_OK)
    {
      //Signal unable to create the file
      scope_display_file_status_message(MESSAGE_FILE_DELETE_FAILED, 0);
    }
  }

  //Bump all the entries in the file number list down
  memmove(&viewfilenumberdata[viewcurrentindex], &viewfilenumberdata[nextindex], count * sizeof(uint16));

  //Bump the thumbnails down to erase the removed one
  memmove(&viewthumbnaildata[viewcurrentindex], &viewthumbnaildata[nextindex], count * sizeof(THUMBNAILDATA));

  //One less item available
  viewavailableitems--;

  //Clear the freed up slot
  viewfilenumberdata[viewavailableitems] = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_load_trace_data(void)
{
  //For info of waveform mishmash error  
  //int8 str[19];  
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
    //Checks on correct number of bytes read might be needed
    //Load the setup data to the file setup data buffer
    if((result = f_read(&viewfp, (uint8 *)viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
    {
      //Copy the loaded data to the settings
      scope_restore_setup_from_file();

      //Check if the version of the file is wrong
      if(viewfilesetupdata[1] != WAVEFORM_FILE_VERSION)
      {
        //No need to load the rest of the data
        //result = WAVEFORM_FILE_ERROR;

        //Show the user the file is not correct
        scope_display_file_status_message(MESSAGE_WAV_VERSION_MISMATCH, 0);
        
        //Show the additional info of error
        scope_display_file_status_message(MESSAGE_ERROR_VERSION_MISMATCH, 1);     
      }
      else
      {
        //Load the channel 1 sample data      
        if((result = f_read(&viewfp, (uint8 *)channel1tracebuffer, SAMPLE_COUNT, 0)) == FR_OK)//3000
        {
          //Load the channel 2 sample data
          if((result = f_read(&viewfp, (uint8 *)channel2tracebuffer, SAMPLE_COUNT, 0)) == FR_OK)//3000
          {
            //Do a check on file validity
            if((result = scope_check_waveform_file()) == 0)
            {
              //Switch to stopped and waveform viewing mode
              scopesettings.runstate = 0;
              scopesettings.waveviewmode = 1;
              //waveviewmode - trigger position always been found
              //disp_have_trigger = 1;
                      
              //Show the normal scope screen
              scope_setup_main_screen();

              //display the trace data
              scope_display_trace_data();
            }
            else
            {
              //Checksum error so signal that to the user
              result = WAVEFORM_FILE_ERROR;

              //Show the user the file is not correct
              scope_display_file_status_message(MESSAGE_WAV_CHECKSUM_ERROR, 0);
            }
          }
        }
      }
    }

    //Done with the file so close it
    f_close(&viewfp);

    //Check if one of the reads failed
    if((result != FR_OK) && (result != WAVEFORM_FILE_ERROR))
    {
      //Signal unable to write to the file
      scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);
    }
  }
  else
  {
    //Signal unable to open the file
    scope_display_file_status_message(MESSAGE_FILE_OPEN_FAILED, 0);
  }

  //Check if all went well
  if(result == FR_OK)
  {
    //Tell it to the caller
    return(VIEW_TRACE_LOAD_OK);
  }

  //Remove the current item from the thumbnails and delete the item from disk
  scope_remove_item_from_thumbnails(1);

  //Save the thumbnail file
  scope_save_thumbnail_file();

  return(VIEW_TRACE_LOAD_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_load_bitmap_data(void)
{
  uint32 result;

  //Set the name in the global buffer for message display
  strcpy(viewfilename, viewthumbnaildata[viewcurrentindex].filename);

  //Try to open the file for reading
  result = f_open(&viewfp, viewfilename, FA_READ);

  //Check if file opened ok
  if(result == FR_OK)
  {
    //Read the bitmap header to verify if the bitmap can be displayed
    result = f_read(&viewfp, viewbitmapheader, PICTURE_HEADER_SIZE, 0);

    //Check if still ok to proceed
    if(result == FR_OK)
    {
      //Check if the header matches what it should be
      if(memcmp(viewbitmapheader, bmpheader, PICTURE_HEADER_SIZE) == 0)
      {
        //Load the bitmap data directly onto the screen
        result = f_read(&viewfp, (uint8 *)maindisplaybuffer, PICTURE_DATA_SIZE, 0);
      }
      else
      {
        //Signal a header mismatch detected
        result = PICTURE_HEADER_MISMATCH;

        //Show the user the file is not correct
        scope_display_file_status_message(MESSAGE_BMP_HEADER_MISMATCH, 0);
      }
    }

    //Done with the file so close it
    f_close(&viewfp);

    //Check if one of the reads failed
    if((result != FR_OK) && (result != PICTURE_HEADER_MISMATCH))
    {
      //Signal unable to read from the file
      scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);
    }
  }
  else
  {
    //Signal unable to open the file
    scope_display_file_status_message(MESSAGE_FILE_OPEN_FAILED, 0);
  }

  //Check if all went well
  if(result == FR_OK)
  {
    //Tell it to the caller
    return(VIEW_BITMAP_LOAD_OK);
  }

  //Remove the current item from the thumbnails and delete the item from disk
  scope_remove_item_from_thumbnails(1);

  //Save the thumbnail file
  scope_save_thumbnail_file();

  return(VIEW_BITMAP_LOAD_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 load_picture(const char* filename)
{
  uint32 result;
  //uint32 x = 300;
  //display_set_font(&font_2);

  //Set the name in the global buffer for message display
  //strcpy(viewfilename, viewthumbnaildata[viewcurrentindex].filename);

  //Try to open the file for reading
  result = f_open(&viewfp, filename, FA_READ);

  //Check if file opened ok
  if(result == FR_OK)
  {
    //Read the bitmap header to verify if the bitmap can be displayed
    result = f_read(&viewfp, viewbitmapheader, PICTURE_HEADER_SIZE, 0);

    //Check if still ok to proceed
    if(result == FR_OK)
    {
      //Check if the header matches what it should be
      if(memcmp(viewbitmapheader, bmpheader, PICTURE_HEADER_SIZE) == 0)
      {
        //Load the bitmap data directly onto the screen
        result = f_read(&viewfp, (uint8 *)maindisplaybuffer, PICTURE_DATA_SIZE, 0);
      }
      else
      {
        //Signal a header mismatch detected
        result = PICTURE_HEADER_MISMATCH;

        //Show the user the file is not correct
        scope_display_file_status_message(MESSAGE_BMP_HEADER_MISMATCH, 0);
        //display_set_fg_color(RED_COLOR);
        //display_text(300, 200, "MESSAGE_BMP_HEADER_MISMATCH");
        //timer0_delay(3000);
      }
    }

    //Done with the file so close it
    f_close(&viewfp);

    //Check if one of the reads failed
    if((result != FR_OK) && (result != PICTURE_HEADER_MISMATCH))
    {
      //Signal unable to read from the file
      scope_display_file_status_message(MESSAGE_FILE_READ_FAILED, 0);
      //display_set_fg_color(RED_COLOR);
      //display_text(300, 215, "MESSAGE_FILE_READ_FAILED");
      //timer0_delay(3000);
    }
  }
  else
  {
    //Signal unable to open the file
    scope_display_file_status_message(MESSAGE_FILE_OPEN_FAILED, 0);
    //display_set_fg_color(RED_COLOR);
    //display_text(300, 230, "MESSAGE_FILE_OPEN_FAILED");
    //display_set_fg_color(YELLOW_COLOR);
    //display_text(250, 250, "Please copy the update.bmp file to the SD card.");
    //timer0_delay(5000);
  }

  //Check if all went well
  if(result == FR_OK)
  {
    //Tell it to the caller
    return(VIEW_BITMAP_LOAD_OK);
  }

  return(VIEW_BITMAP_LOAD_ERROR);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_sync_thumbnail_files(void)
{
  uint32 save = 0;

  //Handle the two types of list files
  for(viewtype=0;viewtype<VIEW_MAX_TYPES;viewtype++)
  {
    //Load the thumbnail file for this type
    if(scope_load_thumbnail_file() != 0)
    {
      //Loading the thumbnail file failed so no sense in going on for this type
      continue;
    }

    //Go through the items in the thumbnail file and check if the needed files still exist on the SD card
    for(viewcurrentindex=0;viewcurrentindex<viewavailableitems;)
    {
      //Set the name in the global buffer for message display
      strcpy(viewfilename, viewthumbnaildata[viewcurrentindex].filename);

      //Try to open the file. On failure remove it from the lists
      if(f_open(&viewfp, viewfilename, FA_READ) == FR_NO_FILE)
      {
        //Remove the current item from the thumbnails without delete, since it is already removed from the SD card
        scope_remove_item_from_thumbnails(0);

        //Signal saving of the thumbnail file is needed
        save = 1;
      }
      else
      {
        //File exists so close it
        f_close(&viewfp);
        
        //Point to the next item. Only needed if item still exists, because it is removed from the list otherwise
        viewcurrentindex++;
      }
    }

    //Check if there was a change
    if(save)
    {
      //Save the thumbnail file if so
      scope_save_thumbnail_file();
    }
  }
  //viewcurrentindex = 0; //0-points to the last in the row
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_initialize_and_display_thumbnails(void)
{
  //No pages yet
  viewpages = 0;

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
  PTHUMBNAILDATA thumbnaildata;

  //Determine the first index based on the current page
  uint32 index = viewpage * VIEW_ITEMS_PER_PAGE;

  //Start with first item for drawing
  uint32 xpos = VIEW_ITEM_XSTART;
  uint32 ypos = VIEW_ITEM_YSTART;

  uint32 button, x, y;

  //Set black color for background
  display_set_fg_color(BLACK_COLOR);

  //Clear the thumbnail display area
  display_fill_rect(0, 0, 730, 480); //733

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

    //Draw the available items on the screen
    while(index < lastindex)
    {
      //Create a nicer thumbnail by drawing the menu buttons and some extra lines
      y = ypos + 1;

      //Main menu button blue
      display_set_fg_color(0x00000078);
      display_fill_rect(xpos, y, 20, 8);

      //Speed text
      display_fill_rect(xpos + 112, y, 10, 8);

      //Channel 1 button
      display_set_fg_color(CHANNEL1_COLOR);
      display_fill_rect(xpos + 38, y, 8, 8);

      //Channel 2 button
      display_set_fg_color(CHANNEL2_COLOR);
      display_fill_rect(xpos + 65, y, 8, 8);

      //Trigger menu button
      display_fill_rect(xpos + 132, y, 8, 8);

      //Battery
      display_fill_rect(xpos + 160, y, 8, 4);

      //Light grey for the buttons
      display_set_fg_color(0x00303030);

      x = xpos + 173;
      y = ypos + 3;

      //Draw the right buttons
      for(button=0;button<8;button++)
      {
        display_fill_rect(x, y, 8, 10);

        y += 15;
      }

      //Set grey color for trace border
      display_set_fg_color(0x00909090);
      display_draw_rect(xpos + 2, ypos + 11, VIEW_ITEM_WIDTH - 13, VIEW_ITEM_HEIGHT - 25);

      //Draw a grid
      display_set_fg_color(0x00606060);

      //Draw the center lines
      display_draw_horz_line(ypos + 60, xpos + 3, xpos + 169);
      display_draw_vert_line(xpos + 86, ypos + 12, ypos + VIEW_ITEM_HEIGHT - 16);

      //Point to the current thumbnail
      thumbnaildata = &viewthumbnaildata[index];

      //Display the thumbnail
      
      //Acquisition button, changing the color of the button depending on the time base mode (long- short)
      if(thumbnaildata->long_mode)display_set_fg_color(ORANGE_COLOR); else display_set_fg_color(GREEN_COLOR);
      display_fill_rect(xpos + 92, y-123, 8, 8);  //y-123 because of (Draw the right buttons) is y=8*15+3
      
      //Need to make a distinction between normal display and xy display mode
      if(thumbnaildata->xydisplaymode == 0)
      {
        //Normal mode
        //To avoid errors make sure the positions are in range
        //Data is read back from file so could be modified
        if(thumbnaildata->disp_xstart < 3)
        {
          thumbnaildata->disp_xstart = 3;
        }

        if(thumbnaildata->disp_xend > 169)
        {
          thumbnaildata->disp_xend = 169;
        }

        //Set the x start position based on the given start x.
        uint32 xs = xpos + thumbnaildata->disp_xstart;
        uint32 xe = xpos + thumbnaildata->disp_xend;

        //Offset the trace data to below the signal area border
        y = ypos + 12;

        //Check if channel 1 is enabled
        if(thumbnaildata->channel1enable)
        {
          scope_display_thumbnail_data(xs, xe, y, CHANNEL1_COLOR, thumbnaildata->channel1data);
        }

        //Check if channel 2 is enabled
        if(thumbnaildata->channel2enable)
        {
          scope_display_thumbnail_data(xs, xe, y, CHANNEL2_COLOR, thumbnaildata->channel2data);
        }
      }
      else
      {
        //xy display mode so set the trace color for it
        display_set_fg_color(XYMODE_COLOR);

        //Point to the data of the two channels
        uint8 *channel1data = thumbnaildata->channel1data;
        uint8 *channel2data = thumbnaildata->channel2data;

        //Start with first sample
        uint32 sample = 0;

        //Center the xy display
        uint32 y = ypos + 12;

        //Keep the samples in registers
        register uint32 x1, x2, y1, y2;

        //Load the first samples
        x1 = *channel1data + xpos;
        y1 = *channel2data + y;

        //Point to the next samples
        channel1data++;
        channel2data++;

        //Draw the trace
        while(sample < 172)
        {
          //Get second samples
          x2 = *channel1data + xpos;
          y2 = *channel2data + y;

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

      //Set white color for item border
      display_set_fg_color(0x00808000);

      //Draw the border
      display_draw_rect(xpos, ypos, VIEW_ITEM_WIDTH, VIEW_ITEM_HEIGHT);

      //Need to make a distinction between normal display and xy display mode for displaying the pointers
      if(thumbnaildata->xydisplaymode == 0)
      {
        //Channel pointers position bases
        x = xpos + 3;
        y = ypos + 12;

        //Check if channel 1 is enabled
        if(thumbnaildata->channel1enable)
        {
          //Limit the position to the extremes
          if(thumbnaildata->channel1traceposition > 92)
          {
            thumbnaildata->channel1traceposition = 92;
          }

          //If so draw its pointer
          scope_thumbnail_draw_pointer(x, y + thumbnaildata->channel1traceposition, THUMBNAIL_POINTER_RIGHT, CHANNEL1_COLOR);
        }

        //Check if channel 2 is enabled
        if(thumbnaildata->channel2enable)
        {
          //Limit the position to the extremes
          if(thumbnaildata->channel2traceposition > 92)
          {
            thumbnaildata->channel2traceposition = 92;
          }

          //If so draw its pointer
          scope_thumbnail_draw_pointer(x, y + thumbnaildata->channel2traceposition, THUMBNAIL_POINTER_RIGHT, CHANNEL2_COLOR);
        }

        //Trigger level position base
        x = xpos + 170;

        //Limit the position to the extremes
        if(thumbnaildata->triggerverticalposition > 92)
        {
          thumbnaildata->triggerverticalposition = 92;
        }
    
        //Draw the trigger level pointer
        //Check if trigger channel 1 or channel 2 is use
        if(thumbnaildata->triggerchannel==0)
          {scope_thumbnail_draw_pointer(x, y + thumbnaildata->triggerverticalposition, THUMBNAIL_POINTER_LEFT, CHANNEL1_COLOR);}
            else {scope_thumbnail_draw_pointer(x, y + thumbnaildata->triggerverticalposition, THUMBNAIL_POINTER_LEFT, CHANNEL2_COLOR);}

        //Limit the position to the extremes
        if(thumbnaildata->triggerhorizontalposition < 3)
        {
          thumbnaildata->triggerhorizontalposition = 3;
        }
        else if(thumbnaildata->triggerhorizontalposition > 165)
        {
          thumbnaildata->triggerhorizontalposition = 165;
        }

        //Draw the trigger position pointer
        scope_thumbnail_draw_pointer(xpos + thumbnaildata->triggerhorizontalposition, y, THUMBNAIL_POINTER_DOWN, TRIGGER_COLOR);
      }
      else
      {
//Draw the pointers here
      }

      //Display the file name in the bottom left corner
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      
      if(onoffRTC) 
            {
            decodethumbnailfilename(thumbnaildata->filename);
            display_text(xpos + 7, ypos + 105, filenameRTC);
            }
            else display_text(xpos + 7, ypos + 105, thumbnaildata->filename);

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
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_thumbnail_data(uint32 xstart, uint32 xend, uint32 ypos, uint32 color, uint8 *buffer)
{
  register uint32 x;
  register uint32 sample1, sample2;

  //Set the trace color
  display_set_fg_color(color);

  //Get the first sample
  sample1 = *buffer++;

  //Position it within the thumbnail on screen
  sample1 += ypos;

  //Do while the samples last
  for(x=xstart;x<xend;x++)
  {
    //Get the second sample
    sample2 = *buffer++;

    //Position it within the thumbnail on screen
    sample2 += ypos;

    //Draw the line for these samples
    display_draw_line(x, sample1, x + 1, sample2);

    //Swap the samples
    sample1 = sample2;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_create_thumbnail(PTHUMBNAILDATA thumbnaildata)
{
  uint16 position;

  //Set the thumbnails filename
  strcpy(thumbnaildata->filename, viewfilename);

  //if RTCon add timestamp
  if(onoffRTC) 
    {
    readnameRTC();
    modnametoRTC(thumbnaildata->filename, filenameRTC);
    } else modnameClearRTC(thumbnaildata->filename);

  //Calculate and limit pointer position for channel 1
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

  //Set the parameters for channel 1
  thumbnaildata->channel1enable        = scopesettings.channel1.enable;
  thumbnaildata->channel1traceposition = (uint8)(((position - 44) * 10000) / 42210);

  //Calculate and limit pointer position for channel 2
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

  //Set the parameters for channel 2
  thumbnaildata->channel2enable      = scopesettings.channel2.enable;
  thumbnaildata->channel2traceposition = (uint8)(((position - 44) * 10000) / 42210);

  //Calculate and limit pointer position for trigger level
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

  //Set trigger information vertical
  thumbnaildata->triggerverticalposition   = (uint8)(((position - 44) * 10000) / 42210);
  //Set trigger channel information (for color in thumbnail)
  thumbnaildata->triggerchannel = scopesettings.triggerchannel;
  
   //Set trigger information horizontal
  thumbnaildata->triggerhorizontalposition = (scopesettings.triggerhorizontalposition * 10000) / 42899;

  //Set the xy display mode
  thumbnaildata->xydisplaymode = scopesettings.xymodedisplay;
  
  //Set the long time base display mode
  thumbnaildata->long_mode = scopesettings.long_mode;

  //Set the display start and end x positions. Conversion to thumbnail x coordinates is dividing by 4,2899
  thumbnaildata->disp_xstart = (disp_xstart * 10000) / 42899;
  thumbnaildata->disp_xend   = (disp_xend * 10000) / 42899;

  //Check which display mode is active
  if(scopesettings.xymodedisplay == 0)
  {
    //Normal mode so check on channel 1 being enabled
    if(scopesettings.channel1.enable)
    {
      //Process the trace points
      scope_thumbnail_set_trace_data(&scopesettings.channel1, thumbnaildata->channel1data);
    }

    //Check on channel 2 being enabled
    if(scopesettings.channel2.enable)
    {
      //Process the trace points
      scope_thumbnail_set_trace_data(&scopesettings.channel2, thumbnaildata->channel2data);
    }
  }
  else
  { //X-Y MODE
    //Use less samples to not overwrite the second buffer
    uint32 index = disp_trigger_index - 317;
    uint32 last = index + 728;

    uint8 *buffer1 = thumbnaildata->channel1data;   
    uint8 *buffer2 = thumbnaildata->channel2data;
    
    //Copy and scale every 4th sample for this channel
    for(;index<last;index+=4)
    {
      //Adjust the samples to fit the thumbnail screen. Channel 1 is x, channel 2 is y
      *buffer1++ = (scope_get_x_sample(&scopesettings.channel1, index) * 10000) / 42210;
      *buffer2++ = ((scope_get_y_sample(&scopesettings.channel2, index) - 47) * 10000) / 42210;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_thumbnail_set_trace_data(PCHANNELSETTINGS settings, uint8 *buffer)
{
  int32  index;
  uint32 pattern;

  //Point to the first and second trace point for easy access
  PDISPLAYPOINTS ptr1 = &settings->tracepoints[0];
  PDISPLAYPOINTS ptr2 = &settings->tracepoints[1];

  //Process the points
  for(index=1;index<settings->noftracepoints;index++)
  {
    //FIll in the blanks between the given points
    scope_thumbnail_calculate_trace_data(ptr1->x, ptr1->y, ptr2->x, ptr2->y);

    //Select the next points
    ptr1++;
    ptr2++;
  }

  //Down sample the points in to the given buffer
  //This yields a max of 182 points, which is more then is displayed on the thumbnail screen
  for(index=disp_xstart,pattern=0;index<=disp_xend;index+=4,pattern++)
  {
    //Adjust the y point to fit the thumbnail screen. First trace y position on screen is 47. The available height on the thumbnail is 95 pixels so divide by 4,2210
    *buffer++ = (uint8)(((thumbnailtracedata[index] - 47) * 10000) / 42210);
     
    //Skip one more sample every third loop
    if(pattern == 2)
    {
      pattern = -1;
      index++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_thumbnail_calculate_trace_data(int32 xstart, int32 ystart, int32 xend, int32 yend)
{
  register int32  x, dx;
  register int32  yacc;
  register int32  ystep;

  //Calculate delta x.
  dx = xend - xstart;

  //Calculate the y segment length
  ystep = ((yend - ystart) << 16) / dx;

  //Initialize the y accumulator for broken pixel accounting
  yacc = ystart << 16;

  //Set the start and end points
  thumbnailtracedata[xstart] = ystart;
  thumbnailtracedata[xend]   = yend;

  //Check if there are points in between
  if(dx > 2)
  {
    //Handle the in between x positions
    for(x=xstart+1;x<xend;x++)
    {
      //Calculate the y point of this segment
      yacc += ystep;

      //Set it in the buffer
      thumbnailtracedata[x] = yacc >> 16;
    }
  } else thumbnailtracedata[xstart+1] = yacc >> 16;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_thumbnail_draw_pointer(uint32 xpos, uint32 ypos, uint32 direction, uint32 color)
{
  uint32 x1, y1, x2, y2, w, h;

  //Set the pointer color
  display_set_fg_color(color);

  //Setup the coordinates based on the direction
  switch(direction)
  {
    //Pointing to the right
    default:
    case THUMBNAIL_POINTER_RIGHT:
      x1 = xpos;
      y1 = ypos;
      x2 = x1 + 4;
      y2 = y1 + 1;
      w = 4;
      h = 3;
      break;

    //Pointing to the left
    case THUMBNAIL_POINTER_LEFT:
      x1 = xpos - 4;
      y1 = ypos;
      x2 = x1 - 1;
      y2 = y1 + 1;
      w = 4;
      h = 3;
      break;

    //Pointing down
    case THUMBNAIL_POINTER_DOWN:
      x1 = xpos;
      y1 = ypos;
      x2 = x1 + 1;
      y2 = y1 + 4;
      w = 3;
      h = 4;
      break;
  }

  //Draw the body
  display_fill_rect(x1, y1, w, h);

  //Draw the point
  display_fill_rect(x2, y2, 1, 1);
}

//----------------------------------------------------------------------------------------------------------------------------------

int32 scope_display_picture_item(void)
{
  //Display the new item
  if(scope_load_bitmap_data() == VIEW_BITMAP_LOAD_ERROR)
  {
    //Return on an error
    return(VIEW_BITMAP_LOAD_ERROR);
  }

  //And draw the bottom menu bar with a save of the background
  scope_setup_bottom_file_menu(VIEW_BOTTON_MENU_INIT);

  return(VIEW_BITMAP_LOAD_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_display_selected_signs(void)
{
  uint32 index = 0;
  uint32 xpos = VIEW_ITEM_SELECTED_XSTART;
  uint32 ypos = VIEW_ITEM_SELECTED_YSTART;

  //Set the colors for displaying the selected sign. White sign on blue background
  display_set_fg_color(WHITE_COLOR);
  display_set_bg_color(BLUE_COLOR);

  //Can't have more selects than items on the page
  while(index < viewitemsonpage)
  {
    //Handle the current item based on its state
    switch(viewitemselected[index])
    {
      case VIEW_ITEM_SELECTED_NOT_DISPLAYED:
        if (viewavailableitems)   //Are there items to choose from?
            {
            //Make a copy of the screen under the selected sign location
            display_copy_rect_from_screen(xpos, ypos, 30, 30);

            //Display the selected sign
            display_copy_icon_use_colors(select_sign_icon, xpos, ypos, 30, 30);

            //Switch to displayed state
            viewitemselected[index] = VIEW_ITEM_SELECTED_DISPLAYED;
            }
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
//void scope_display_file_status_message(int32 msgid, int32 index, int32 alwayswait)
{
  uint32 checkconfirmation = scopesettings.confirmationmode;
  char version_str[9];  // 8 hex char + '\0'
  
  //Set the thumbnails filename
  PTHUMBNAILDATA thumbnaildata;
  thumbnaildata = &viewthumbnaildata[viewcurrentindex];//0

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
  display_set_destination_buffer(displaybuffertmp);//2
  display_copy_rect_from_screen(260, 210, 280, 60);

  //Draw the background in grey
  display_set_fg_color(0x00202020);
  display_fill_rounded_rect(260, 210, 279, 59, 3);//280 60

  //Draw the border in a lighter grey
  display_set_fg_color(0x00303030);
  display_draw_rounded_rect(260, 210, 279, 59, 3);//280 60

  //White color for text and use font_3
  display_set_fg_color(WHITE_COLOR);
  display_set_font(&font_3);

  switch(msgid)
  {
    case MESSAGE_SAVE_SUCCESSFUL:
      thumbnaildata = &viewthumbnaildata[0];
      display_text(270, 220, "File saved successfully");

      //Don't wait for confirmation in case of success, unless requested
      checkconfirmation = alwayswait;
      break;

    case MESSAGE_FILE_CREATE_FAILED:
      thumbnaildata = &viewthumbnaildata[0];
      display_text(270, 220, "Failed to create file");
      break;

    case MESSAGE_FILE_OPEN_FAILED:
      display_text(270, 220, "Failed to open file");
      break;

    case MESSAGE_FILE_WRITE_FAILED:
      thumbnaildata = &viewthumbnaildata[0];
      display_text(270, 220, "Failed to write to file");
      break;

    case MESSAGE_FILE_READ_FAILED:
      display_text(270, 220, "Failed to read from file");
      break;

    case MESSAGE_FILE_SEEK_FAILED:
      display_text(270, 220, "Failed to seek in file");
      break;

    case MESSAGE_FILE_DELETE_FAILED:
      display_text(270, 220, "Failed to delete file");
      break;

    case MESSAGE_DIRECTORY_CREATE_FAILED:
      display_text(270, 220, "Failed to create directory");
      break;

    case MESSAGE_FILE_SYSTEM_FAILED:
      display_text(270, 220, "File system failure");
      break;

    case MESSAGE_THUMBNAIL_FILE_CORRUPT:
      display_text(270, 220, "Corrupt thumbnail file");
      break;

    case MESSAGE_THUMBNAIL_FILE_FULL:
      display_text(270, 220, "Thumbnail file is full");
      break;

    case MESSAGE_BMP_HEADER_MISMATCH:
      display_text(270, 220, "Bitmap header mismatch");
      break;
      
    case MESSAGE_WAV_VERSION_MISMATCH:
      display_text(270, 220, "Waveform file version mismatch");
      break;
      
    case MESSAGE_ERROR_VERSION_MISMATCH:
      //Show additional info of error
      display_text(290, 213, "Actual FW: ");  //270
      display_text(370, 213, VERSION_STRING);        //Show actual FW version information 
      display_text(430, 213, "Version: ");
      format_version_full(WAVEFORM_FILE_VERSION, version_str); 
      display_text(490, 213, version_str);          //Show actual file version information 
              
      display_text(270, 231, "Wave file FW: ");
      extract_version_string(viewfilesetupdata[2], viewfilesetupdata[3], version_str); 
      display_text(370, 231, version_str);          //Show wave file - FW version information 
      display_text(430, 231, "Version: ");
      format_version_full(viewfilesetupdata[1], version_str);      
      display_text(490, 231, version_str);          //Show wave file - file version information 
      break;
      
    case MESSAGE_WAV_CHECKSUM_ERROR:
      display_text(270, 220, "Waveform file checksum error");
      break;
      
    case MESSAGE_REF_BUFF_EMPTY_ERROR:
      display_text(270, 220, "REF buffer empty !!!");
      //timer0_delay(2000); // Wait 3 sec
      break;
  }

  //Display the file name in question
  if(onoffRTC) //if RTCon view rtc info
    {
      decodethumbnailfilename(thumbnaildata->filename);
      display_text(270, 248, filenameRTC);//270 245
    }
    else display_text(270, 248, thumbnaildata->filename);

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
    timer0_delay(700);//500
  }

  //Restore the original screen
  display_set_source_buffer(displaybuffertmp);//2
  display_copy_rect_to_screen(260, 210, 280, 60);

  //Need to restore the screen buffer pointer
  display_restore_screen_buffer();
}

//----------------------------------------------------------------------------------------------------------------------------------
// Configuration data functions
//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_configuration_data(void)
{
#if 0
  //Get the settings data form the flash memory
  //sys_spi_flash_read(0x001FD000, (uint8 *)settingsworkbuffer, sizeof(settingsworkbuffer));
#else
  //Load the settings data from its sector on the SD card
  if(sd_card_read(SETTINGS_SECTOR, 1, (uint8 *)settingsworkbuffer) != SD_OK)
  {
    settingsworkbuffer[2] = 0;
    settingsworkbuffer[3] = 0;
  }
#endif

  //Restore the settings from the loaded data
  scope_restore_config_data();

  //Set the FPGA commands for channel 1
  scopesettings.channel1.enablecommand     = 0x02;
  scopesettings.channel1.couplingcommand   = 0x34;
  scopesettings.channel1.voltperdivcommand = 0x33;
  scopesettings.channel1.offsetcommand     = 0x32;
  scopesettings.channel1.adc1command       = 0x20;
  scopesettings.channel1.adc2command       = 0x21;
  scopesettings.channel1.averagecommand    = 0x24;
  

  //Set the menu and button data for channel 1
  scopesettings.channel1.color        = CHANNEL1_COLOR;
  scopesettings.channel1.buttonxpos   = CH1_BUTTON_XPOS;
  scopesettings.channel1.menuxpos     = CH1_MENU_XPOS;
  scopesettings.channel1.voltdivypos  = CH1_VOLT_DIV_MENU_YPOS;
  scopesettings.channel1.touchedcolor = CH1_TOUCHED_COLOR;
  scopesettings.channel1.buttontext   = "CH1";

  //Set the trace and display buffer pointers for channel 1
  scopesettings.channel1.tracebuffer      = (uint8 *)channel1tracebuffer;
  scopesettings.channel1.tracepoints      = channel1pointsbuffer;

  //Set the FPGA commands for channel 2
  scopesettings.channel2.enablecommand     = 0x03;
  scopesettings.channel2.couplingcommand   = 0x37;
  scopesettings.channel2.voltperdivcommand = 0x36;
  scopesettings.channel2.offsetcommand     = 0x35;
  scopesettings.channel2.adc1command       = 0x22;
  scopesettings.channel2.adc2command       = 0x23;
  scopesettings.channel2.averagecommand    = 0x26;

  //Set the menu and button data for channel 2
  scopesettings.channel2.color        = CHANNEL2_COLOR;
  scopesettings.channel2.buttonxpos   = CH2_BUTTON_XPOS;
  scopesettings.channel2.menuxpos     = CH2_MENU_XPOS;
  scopesettings.channel2.voltdivypos  = CH2_VOLT_DIV_MENU_YPOS;
  scopesettings.channel2.touchedcolor = CH2_TOUCHED_COLOR;
  scopesettings.channel2.buttontext   = "CH2";

  //Set the trace and display buffer pointers for channel 2
  scopesettings.channel2.tracebuffer      = (uint8 *)channel2tracebuffer;
  scopesettings.channel2.tracepoints      = channel2pointsbuffer; 
  
  //Set the trace and display buffer pointers for REF1-4
  scopesettings.ch_ref1.tracebuffer = (uint8 *)ref1_tracebuffer;
  scopesettings.ch_ref2.tracebuffer = (uint8 *)ref2_tracebuffer;
  scopesettings.ch_ref3.tracebuffer = (uint8 *)ref3_tracebuffer;
  scopesettings.ch_ref4.tracebuffer = (uint8 *)ref4_tracebuffer;
  scopesettings.ch_ref5.tracebuffer = (uint8 *)ref5_tracebuffer;
  scopesettings.ch_ref6.tracebuffer = (uint8 *)ref6_tracebuffer;
  scopesettings.ch_ref7.tracebuffer = (uint8 *)ref7_tracebuffer;
  scopesettings.ch_ref8.tracebuffer = (uint8 *)ref8_tracebuffer;
  
  scopesettings.ch_ref1.tracepoints      = ref1pointsbuffer;
  scopesettings.ch_ref2.tracepoints      = ref2pointsbuffer;
  scopesettings.ch_ref3.tracepoints      = ref3pointsbuffer;
  scopesettings.ch_ref4.tracepoints      = ref4pointsbuffer;
  scopesettings.ch_ref5.tracepoints      = ref5pointsbuffer;
  scopesettings.ch_ref6.tracepoints      = ref6pointsbuffer;
  scopesettings.ch_ref7.tracepoints      = ref7pointsbuffer;
  scopesettings.ch_ref8.tracepoints      = ref8pointsbuffer;
  
  //Set the trace buffer pointers for math functions
  scopesettings.math_tracebuffer = (uint8 *)math_channel_tracebuffer;
          
  //Switch to RUN
  scopesettings.runstate = 1;
  //Ready for start new sample
  scopesettings.display_data_done = 1;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_configuration_data(void)
{
  //Save the settings for writing to the flash
  scope_save_config_data();

#if 0
  //Write it to the flash
  //sys_spi_flash_write(0x001FD000, (uint8 *)settingsworkbuffer, sizeof(settingsworkbuffer));
#else
  //Write the data to its sector on the SD card
  sd_card_write(SETTINGS_SECTOR, 1, (uint8 *)settingsworkbuffer);
#endif
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_input_calibration_data(void)
{
    uint16 buffer[256];
    uint32 index;
    uint32  checksum = 0;
    uint16 *ptr;
    uint32 *ptr32;
    
    //Load the settings data from its sector on the SD card
    if(sd_card_read(INPUT_CALIBRATION_SECTOR, 1, (uint8 *)buffer) != SD_OK)
    {
    //Clear buffer
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0xFFFF;                 //load default data
    buffer[1] = 0;
    }
    
    //Calculate a checksum over the loaded data
    for(index=8;index<60;index++) 
        {
        checksum += buffer[index];
        timer0_delay(1);    //So that FOR is not canceled during C translation
        }
  
    //Check if the checksum is a match as well as the version number
    if(((buffer[0] == (checksum >> 16))&&(buffer[1] == (checksum & 0xFFFF))&&(buffer[0]<0xFFFF)&&(buffer[0]>0))||(reload_cal_data == 1))
    {       
        //Point to the calibration settings
        ptr = &settingsworkbuffer[CALIBRATION_SETTING_OFFSET];

        //Restore the working set values from the saved values
        for(index=0;index<6;index++,ptr++)
        {
            //Copy the data for both channels
            scopesettings.channel1.dc_calibration_offset[index] = ptr[0];
            scopesettings.channel2.dc_calibration_offset[index] = ptr[6];
        }

        //The last entry is a copy of the fore last value
        scopesettings.channel1.dc_calibration_offset[6] = scopesettings.channel1.dc_calibration_offset[5];
        scopesettings.channel2.dc_calibration_offset[6] = scopesettings.channel2.dc_calibration_offset[5];
    
        //Point to the calibration settings
        ptr = &settingsworkbuffer[CALIBRATION_SETTING_OFFSET_C];

        //Restore the ADC compensation values
        scopesettings.channel1.adc1compensation = *ptr++;
        scopesettings.channel1.adc2compensation = *ptr++;
        scopesettings.channel2.adc1compensation = *ptr++;
        scopesettings.channel2.adc2compensation = *ptr++; 
         
        //Point to the input calibration settings
        ptr32 = (uint32*)&buffer[INPUT_CALIBRATION_SETTING_OFFSET];
    
        //Restore the Input calibration values from the saved values
        for(index=0;index<7;index++,ptr32++)
        {
            //Copy the data for both channels
            scopesettings.channel1.input_calibration[index] = ptr32[0];
            scopesettings.channel2.input_calibration[index] = ptr32[8];
        }
        
    //Point to the DC shift calibration settings        12
    ptr32 = (uint32*)&buffer[DC_SHIFT_SETTING_OFFSET];//40
    
    scopesettings.channel1.dc_shift_center = ptr32[0];
    scopesettings.channel1.dc_shift_size   = ptr32[1];
    scopesettings.channel1.dc_shift_value  = ptr32[2];
    
    scopesettings.channel2.dc_shift_center = ptr32[4];
    scopesettings.channel2.dc_shift_size   = ptr32[5];
    scopesettings.channel2.dc_shift_value  = ptr32[6];
    
    }
    else
    {   
        //Set default channel calibration values
        for(index=0;index<7;index++)
            {
            //Set FPGA center level
            scopesettings.channel1.dc_calibration_offset[index] = 860;//750
            scopesettings.channel2.dc_calibration_offset[index] = 860;//750
            }
  
        //Set default ADC compensation values
        scopesettings.channel1.adc1compensation = 0;
        scopesettings.channel1.adc2compensation = 0;
        scopesettings.channel2.adc1compensation = 0;
        scopesettings.channel2.adc2compensation = 0; 
        
        //Set default channel Input calibration values
        for(index=0;index<7;index++)
            {
            //Set default Input calibration values
            scopesettings.channel1.input_calibration[index] = signal_adjusters[index];
            scopesettings.channel2.input_calibration[index] = signal_adjusters[index];
            }
        
        scopesettings.channel1.dc_shift_center = 174;
        scopesettings.channel1.dc_shift_size   = 220;
        scopesettings.channel1.dc_shift_value  = 385;
    
        scopesettings.channel2.dc_shift_center = 174;
        scopesettings.channel2.dc_shift_size   = 220;
        scopesettings.channel2.dc_shift_value  = 385;
    
        calibrationfail = 1;    //Ready to input calibration
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_input_calibration_data(void)
{
    uint16 buffer[256];
    uint32 index;
    uint32  checksum = 0;
    uint32 *ptr32;
    
    /*
        scopesettings.channel1.dc_shift_center  = 112;
        scopesettings.channel1.dc_shift_size  = 220;
        scopesettings.channel1.dc_shift_value = 385;
    
        scopesettings.channel2.dc_shift_center  = 112;
        scopesettings.channel2.dc_shift_size  = 220;
        scopesettings.channel2.dc_shift_value = 385;
     */
     
    
    //Clear buffer
    memset(buffer, 0x00, sizeof(buffer));
    
    //Point to the Input calibration settings                   28
    ptr32 = (uint32*)&buffer[INPUT_CALIBRATION_SETTING_OFFSET];//8
  
    for(index=0;index<7;index++,ptr32++)
        {
        //Copy the data for both channels
        ptr32[0] = scopesettings.channel1.input_calibration[index];
        ptr32[8] = scopesettings.channel2.input_calibration[index];
        }
    
    //Point to the DC shift calibration settings        12
    ptr32 = (uint32*)&buffer[DC_SHIFT_SETTING_OFFSET];//40
    
    ptr32[0] = scopesettings.channel1.dc_shift_center;
    ptr32[1] = scopesettings.channel1.dc_shift_size;
    ptr32[2] = scopesettings.channel1.dc_shift_value;
    
    ptr32[4] = scopesettings.channel2.dc_shift_center;
    ptr32[5] = scopesettings.channel2.dc_shift_size;
    ptr32[6] = scopesettings.channel2.dc_shift_value;

     //Calculate a checksum over the settings data
    for(index=8;index<60;index++)   //40
    {
        checksum += buffer[index];
        timer0_delay(1);    //So that FOR is not canceled during C translation
    }
      
    //Save the checksum
    buffer[0] = checksum >> 16;
    buffer[1] = checksum;

    //Write the data to its sector on the SD card
    sd_card_write(INPUT_CALIBRATION_SECTOR, 1, (uint8 *)buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_reset_config_data(void)
{
  //uint32 index;
  uint32 *ptr = STARTUP_CONFIG_ADDRESS;     //for save and load boot menu settings
  
  //Load a default configuration in case of settings in flash being corrupted
   
  //Enable channel 1, ac coupling, 1x magnification, 50V/div, fft disabled and trace line in top part of the screen
  scopesettings.channel1.enable            = 1;
  scopesettings.channel1.coupling          = 1; //1-AC
  scopesettings.channel1.magnification     = 1; //1-1x
  scopesettings.channel1.V_A               = 0; //0-Volt
  scopesettings.channel1.displayvoltperdiv = 0;
  scopesettings.channel1.samplevoltperdiv  = 0;
  scopesettings.channel1.fftenable         = 0;
  scopesettings.channel1.traceposition     = 300;
  scopesettings.channel1.triggerverticalposition= 300;
  scopesettings.channel1.dcoffset = 0;
  scopesettings.channel1.invert = 0;            //1-invert channel

  //Enable channel 2, ac coupling, 1x magnification, 50V/div, fft disabled and trace line in bottom part of the screen
  scopesettings.channel2.enable            = 1;
  scopesettings.channel2.coupling          = 1; //1-AC
  scopesettings.channel2.magnification     = 1; //1-1x
  scopesettings.channel2.V_A               = 0; //0-Volt
  scopesettings.channel2.displayvoltperdiv = 0;
  scopesettings.channel2.samplevoltperdiv  = 0;
  scopesettings.channel2.fftenable         = 0;
  scopesettings.channel2.traceposition     = 100;
  scopesettings.channel2.triggerverticalposition= 100;
  scopesettings.channel2.dcoffset = 0;
  scopesettings.channel2.invert = 0;            //1-invert channel

  //Set trigger mode to auto, trigger edge to rising, trigger channel to channel 1, trigger position and trigger screen offset to center of the screen
  scopesettings.triggermode     = 0;
  scopesettings.triggeredge     = 0;
  scopesettings.triggerchannel  = 0;
  scopesettings.triggerhorizontalposition = 357;    //int32
  scopesettings.triggerverticalposition   = 300;

  //Set move speed to fast
  scopesettings.movespeed = 0;

  //Set time base to 200uS/div
  scopesettings.timeperdiv = 20;//9+11 12+11

  //Set the related acquisition speed which is 500KSa/s
  scopesettings.samplerate = 8;//9 16 5+11

  //Select CH1 is source1 & CH2 is source2
  scopesettings.source1_measures = 0;
  scopesettings.source2_measures = 0;
    
  //Enable some default measurements
  scopesettings.hide_values_CH1 = 0;
  scopesettings.hide_values_CH2 = 0;

  //Turn time cursor off and set some default positions
  scopesettings.timecursorsenable   = 0;
  scopesettings.timecursor1position = 183;
  scopesettings.timecursor2position = 547;

  //Turn volt cursor of and set some default positions
  scopesettings.voltcursorsenable   = 0;
  scopesettings.voltcursor1position = 167;
  scopesettings.voltcursor2position = 328;
  //Turn together move cursors
  scopesettings.lockcursors         = 1;   //Lock move cursors

  //Set right menu to normal state
  scopesettings.rightmenustate = 0;

  //Set screen brightness to 20, grid brightness to 25, grid enable, always 50% trigger off, x-y display mode off and confirmation mode disabled
  scopesettings.screenbrightness = 20;
  scopesettings.gridbrightness   = 25;
  scopesettings.gridenable       = 1;  
  scopesettings.alwaystrigger50  = 0;
  scopesettings.xymodedisplay    = 0;
  scopesettings.confirmationmode = 0;
  scopesettings.long_mode        = 0;   //Set the sort time base display mode
  scopesettings.average_mode     = 0;   //No average displayed data
  scopesettings.long_memory      = 0;   //No long memory mode
  scopesettings.new_autosetup    = 1;   //New auto setup
  scopesettings.maxlight         = 0;   //Max light disable
  scopesettings.ref1             = 0;   //ref1 waveform disable
  scopesettings.ref2             = 0;   //ref2 waveform disable
  scopesettings.ref3             = 0;   //ref3 waveform disable
  scopesettings.ref4             = 0;   //ref4 waveform disable
  scopesettings.ref5             = 0;   //ref5 waveform disable
  scopesettings.ref6             = 0;   //ref6 waveform disable
  scopesettings.ref7             = 0;   //ref7 waveform disable
  scopesettings.ref8             = 0;   //ref8 waveform disable
  scopesettings.ref_on_startup   = 0;   //DISABLE reference waveforms on start
  
  //triggerlong = 0;
  //trigger50 = 0;      //Clear flag for channel settings 50% button
  onoffRTC    = 0;      //0-RTC Off
  USB_CH340   = 0;      //0-USB like MAss storage
  dev_mode    = 0;      //1-dev mode active
  tag_in_BMP  = 1;      //1-add a tag to BMP (RTC stamp or file name)
  
  channelA = 0;       //ch1 to channel A math function
  channelB = 1;       //ch2 to channel B math function
  
  
  
  //----------------------------------------------------------------------------
  ptr[0] = 4;  //PECO + menu //value for default start firmware (0-pepco,1-fnirsi, 2-FEL, <3 skip menu)
  //SAVE the display configuration sector from DRAM to SDcart   //save boot menu and default start
  sd_card_write(DISPLAY_CONFIG_SECTOR, 1, (uint8 *)0x81BFFC00);
  
  //----------------------------------------------------------------------------
  //Load input calibration value 
  scope_load_input_calibration_data();
  //----------------------------------------------------------------------------  
  restore = 1;      //Flag - view restore text in display
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_config_data(void)
{
  uint32  channel;
  uint32  index;
  uint32  checksum = 0;
  uint16 *ptr;

  //Set a version number for checking if the settings match the current firmware
  settingsworkbuffer[2] = SETTING_SECTOR_VERSION_HIGH;
  settingsworkbuffer[3] = SETTING_SECTOR_VERSION_LOW;
   
  //Point to the channel 1 settings                 11
  ptr = &settingsworkbuffer[CHANNEL1_SETTING_OFFSET];
  
  //Save the channel 1 settings
  *ptr++ = scopesettings.channel1.enable;
  *ptr++ = scopesettings.channel1.coupling;
  *ptr++ = scopesettings.channel1.magnification;
  *ptr++ = scopesettings.channel1.V_A;
  *ptr++ = scopesettings.channel1.displayvoltperdiv;
  *ptr++ = scopesettings.channel1.samplevoltperdiv;
  *ptr++ = scopesettings.channel1.fftenable;
  *ptr++ = scopesettings.channel1.traceposition;
  *ptr++ = scopesettings.channel1.triggerverticalposition;
  *ptr++ = scopesettings.channel1.dcoffset;
  *ptr++ = scopesettings.channel1.invert;          

  //Point to the channel 2 settings                 11
  ptr = &settingsworkbuffer[CHANNEL2_SETTING_OFFSET];
  
  //Save the channel 2 settings
  *ptr++ = scopesettings.channel2.enable;
  *ptr++ = scopesettings.channel2.coupling;
  *ptr++ = scopesettings.channel2.magnification;
  *ptr++ = scopesettings.channel2.V_A;
  *ptr++ = scopesettings.channel2.displayvoltperdiv;
  *ptr++ = scopesettings.channel2.samplevoltperdiv;
  *ptr++ = scopesettings.channel2.fftenable;
  *ptr++ = scopesettings.channel2.traceposition;
  *ptr++ = scopesettings.channel2.triggerverticalposition;
  *ptr++ = scopesettings.channel2.dcoffset;
  *ptr++ = scopesettings.channel2.invert;            

  //Point to the trigger settings                   9
  ptr = &settingsworkbuffer[TRIGGER_SETTING_OFFSET];
  
  //Save trigger settings
  *ptr++ = scopesettings.timeperdiv;
  *ptr++ = scopesettings.samplerate;
  *ptr++ = scopesettings.triggermode;
  *ptr++ = scopesettings.triggeredge;
  *ptr++ = scopesettings.triggerchannel;
  *ptr++ = scopesettings.triggerlevel;
  *ptr++ = scopesettings.triggerhorizontalposition>>16;         //int32 !!!
  *ptr++ = scopesettings.triggerhorizontalposition & 0xFFFF;    //int32 !!!
  *ptr++ = scopesettings.triggerverticalposition;
  
  //Point to the other settings                     28
  ptr = &settingsworkbuffer[OTHER_SETTING_OFFSET];
  
  //Save the other settings
  *ptr++ = scopesettings.movespeed;
  *ptr++ = scopesettings.rightmenustate;
  *ptr++ = scopesettings.confirmationmode;
  *ptr++ = scopesettings.screenbrightness;
  *ptr++ = scopesettings.gridbrightness;
  *ptr++ = scopesettings.alwaystrigger50;
  *ptr++ = scopesettings.xymodedisplay;
  *ptr++ = scopesettings.long_mode;
  *ptr++ = scopesettings.average_mode;
  *ptr++ = scopesettings.long_memory;
  *ptr++ = scopesettings.new_autosetup;
  *ptr++ = scopesettings.maxlight;
  *ptr++ = scopesettings.ref1;     
  *ptr++ = scopesettings.ref2;    
  *ptr++ = scopesettings.ref3;       
  *ptr++ = scopesettings.ref4;     
  *ptr++ = scopesettings.ref5;         
  *ptr++ = scopesettings.ref6;          
  *ptr++ = scopesettings.ref7;         
  *ptr++ = scopesettings.ref8;          
  *ptr++ = scopesettings.ref_on_startup;
  
  *ptr++ = onoffRTC;
  *ptr++ = USB_CH340;
  *ptr++ = dev_mode;
  *ptr++ = tag_in_BMP;
    
  *ptr++ = mathmode;
  //refmode
  
  *ptr++ = channelA;
  *ptr++ = channelB;
  
  //Point to the cursor settings                    7
  ptr = &settingsworkbuffer[CURSOR_SETTING_OFFSET];
  
  //Save the time cursor settings
  *ptr++ = scopesettings.timecursorsenable;
  *ptr++ = scopesettings.timecursor1position;
  *ptr++ = scopesettings.timecursor2position;

  //Save the volt cursor settings
  *ptr++ = scopesettings.voltcursorsenable;
  *ptr++ = scopesettings.voltcursor1position;
  *ptr++ = scopesettings.voltcursor2position;
  //Save cursor settings
  *ptr++ = scopesettings.lockcursors;   //lock move cursors

  //Point to the first measurement enable setting   28
  ptr = &settingsworkbuffer[MEASUREMENT_SETTING_OFFSET];
  
  //Save the measurements source states
  *ptr++ = scopesettings.source1_measures;
  *ptr++ = scopesettings.source2_measures;
  
  //Save the measurements enable states
  *ptr++ = scopesettings.hide_values_CH1;
  *ptr++ = scopesettings.hide_values_CH2;

  for(channel=0;channel<2;channel++)
  {
    //12 measurements per channel
    for(index=0;index<12;index++)
    {
      //Copy the current measurement state and point to the next one
      *ptr++ = scopesettings.measuresstate[channel][index];
    }
  }

  //Point to the calibration settings               12
  ptr = &settingsworkbuffer[CALIBRATION_SETTING_OFFSET];

  //Copy the working set values to the saved values
  for(index=0;index<6;index++,ptr++)
    {
    //Copy the data for both channels
    ptr[0] = scopesettings.channel1.dc_calibration_offset[index];
    ptr[6] = scopesettings.channel2.dc_calibration_offset[index];
    }

  //Point to the calibration settings               4
  ptr = &settingsworkbuffer[CALIBRATION_SETTING_OFFSET_C];
  
  //Save the ADC compensation values
  *ptr++ = scopesettings.channel1.adc1compensation;
  *ptr++ = scopesettings.channel1.adc2compensation;
  *ptr++ = scopesettings.channel2.adc1compensation;
  *ptr++ = scopesettings.channel2.adc2compensation;
  
//-----------------------------------------------------------------
  
  //Calculate a checksum over the settings data
  for(index=2;index<256;index++)//256
  {
    checksum += settingsworkbuffer[index];
  }
  
  //Save the checksum
  settingsworkbuffer[0] = checksum >> 16;
  settingsworkbuffer[1] = checksum;
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_config_data(void)
{
  uint32  channel;
  uint32  index;
  uint32  checksum = 0;
  uint32  tmp = 0;
  uint16 *ptr;

  //Calculate a checksum over the loaded data
  for(index=2;index<256;index++)
  {
    checksum += settingsworkbuffer[index];
  }
  
  //Check if the checksum is a match as well as the version number
  if((settingsworkbuffer[0] == (checksum >> 16)) && (settingsworkbuffer[1] == (checksum & 0xFFFF)) && (settingsworkbuffer[2] == SETTING_SECTOR_VERSION_HIGH) && (settingsworkbuffer[3] == SETTING_SECTOR_VERSION_LOW))
  {
    //Point to the channel 1 settings
    ptr = &settingsworkbuffer[CHANNEL1_SETTING_OFFSET];

    //Restore the channel 1 settings
    scopesettings.channel1.enable            = *ptr++;
    scopesettings.channel1.coupling          = *ptr++;
    scopesettings.channel1.magnification     = *ptr++;
    scopesettings.channel1.V_A               = *ptr++;
    scopesettings.channel1.displayvoltperdiv = *ptr++;
    scopesettings.channel1.samplevoltperdiv  = *ptr++;
    scopesettings.channel1.fftenable         = *ptr++;
    scopesettings.channel1.traceposition     = *ptr++;
    scopesettings.channel1.triggerverticalposition= *ptr++;
    scopesettings.channel1.dcoffset          = *ptr++;
    scopesettings.channel1.invert            = *ptr++;

    //Point to the channel 2 settings
    ptr = &settingsworkbuffer[CHANNEL2_SETTING_OFFSET];

    //Restore the channel 2 settings
    scopesettings.channel2.enable            = *ptr++;
    scopesettings.channel2.coupling          = *ptr++;
    scopesettings.channel2.magnification     = *ptr++;
    scopesettings.channel2.V_A               = *ptr++;
    scopesettings.channel2.displayvoltperdiv = *ptr++;
    scopesettings.channel2.samplevoltperdiv  = *ptr++;
    scopesettings.channel2.fftenable         = *ptr++;
    scopesettings.channel2.traceposition     = *ptr++;
    scopesettings.channel2.triggerverticalposition= *ptr++; 
    scopesettings.channel2.dcoffset          = *ptr++;
    scopesettings.channel2.invert            = *ptr++;

    //Point to the trigger settings
    ptr = &settingsworkbuffer[TRIGGER_SETTING_OFFSET];

    //Restore trigger settings
    scopesettings.timeperdiv                 = *ptr++;
    scopesettings.samplerate                 = *ptr++;
    scopesettings.triggermode                = *ptr++;
    scopesettings.triggeredge                = *ptr++;
    scopesettings.triggerchannel             = *ptr++;
    scopesettings.triggerlevel               = *ptr++;
    tmp                                      = *ptr++; 
    scopesettings.triggerhorizontalposition  =tmp<<16;
    scopesettings.triggerhorizontalposition += *ptr++; //int32 !!!
    //scopesettings.triggerhorizontalposition = 357;
    scopesettings.triggerverticalposition    = *ptr++;
    
    //Point to the other settings
    ptr = &settingsworkbuffer[OTHER_SETTING_OFFSET];

    //Restore the other settings
    scopesettings.movespeed        = *ptr++;
    scopesettings.rightmenustate   = *ptr++;
    scopesettings.confirmationmode = *ptr++;
    scopesettings.screenbrightness = *ptr++;
    scopesettings.gridbrightness   = *ptr++;
    scopesettings.gridenable       = 1; 
    scopesettings.alwaystrigger50  = *ptr++;
    scopesettings.xymodedisplay    = *ptr++;
    scopesettings.long_mode        = *ptr++;
    scopesettings.average_mode     = *ptr++; 
    scopesettings.long_memory      = *ptr++;
    scopesettings.long_memory      = 0;           //docasne zrusene, aj v file setup zrusene
    scopesettings.new_autosetup    = *ptr++;
    scopesettings.maxlight         = *ptr++;
    scopesettings.ref1             = *ptr++;     
    scopesettings.ref2             = *ptr++;     
    scopesettings.ref3             = *ptr++;     
    scopesettings.ref4             = *ptr++;     
    scopesettings.ref5             = *ptr++;         
    scopesettings.ref6             = *ptr++;          
    scopesettings.ref7             = *ptr++;         
    scopesettings.ref8             = *ptr++;
    scopesettings.ref_on_startup   = *ptr++;
    
    onoffRTC    = *ptr++;
    USB_CH340   = *ptr++;   //0-USB like MAss storage, 1-serial port
    //USB_CH340   = 0;        //0-USB like MAss storage, 1-serial port
    dev_mode    = *ptr++; 
    tag_in_BMP  = *ptr++; 
    
    mathmode = *ptr++; 
    channelA = *ptr++; 
    channelB = *ptr++; //19

    //scopesettings.channel1.invert = 0; //signal inverted
    
    //Point to the cursor settings
    ptr = &settingsworkbuffer[CURSOR_SETTING_OFFSET];

    //Restore the time cursor settings
    scopesettings.timecursorsenable   = *ptr++;
    scopesettings.timecursor1position = *ptr++;
    scopesettings.timecursor2position = *ptr++;

    //Restore the volt cursor settings
    scopesettings.voltcursorsenable   = *ptr++;
    scopesettings.voltcursor1position = *ptr++;
    scopesettings.voltcursor2position = *ptr++;
    //Restore cursor settings
    scopesettings.lockcursors         = *ptr++;

    //Point to the first measurement enable setting
    ptr = &settingsworkbuffer[MEASUREMENT_SETTING_OFFSET];

    //Restore the measurements source states
    scopesettings.source1_measures = *ptr++;
    scopesettings.source2_measures = *ptr++;
  
    //Restore the measurements enable states
    scopesettings.hide_values_CH1 = *ptr++;
    scopesettings.hide_values_CH2 = *ptr++;
    
    for(channel=0;channel<2;channel++)
    {
      //12 measurements per channel
      for(index=0;index<12;index++)
      {
        //Copy the current measurement state and point to the next one
        scopesettings.measuresstate[channel][index] = *ptr++;
      }
    }
    //Load reference waveforms
    scope_load_ALLREF_file();
    //--------------------------------------------------------------------------
    //Load input calibration value 
    scope_load_input_calibration_data();
    //-------------------------------------------------------------------------- 
  }
  else
  {
    //Load a default set on failure
    scope_reset_config_data();
    
    //Save it to the SD card
    scope_save_configuration_data();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

#ifndef USE_TP_CONFIG
#ifdef SAVE_TP_CONFIG
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
#endif

//----------------------------------------------------------------------------------------------------------------------------------
