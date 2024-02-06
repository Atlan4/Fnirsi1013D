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
//----------------------------------------------------------------------------------------------------------------------------------
 
  void scope_preset_values(void)
{
    //Use a separate buffer to clear the screen
    display_set_screen_buffer(displaybuffertmp);
  
    //Clear the trace portion of the screen
    display_set_fg_color(BLACK_COLOR);
    display_fill_rect(2, 48, 728, 431);
    
  if(scopesettings.long_mode) //if set long time base, no drawn.
  {
    scopesettings.xpos = 4;
    scopesettings.lastx = 3;
    scopesettings.count = 0;
    
    
    //Auto mode - set flag, signal is triggered=1 
    if(scopesettings.triggermode) triggerlong = 0; else triggerlong = 1;
    scopesettings.triggerhorizontalposition = 360;
  
    
    //y position for the channel 1 trace center pointer.
    scopesettings.channel1.sample1 = 442 - scopesettings.channel1.traceposition;//441
    //y position for the channel 2 trace center pointer.
    scopesettings.channel2.sample1 = 442 - scopesettings.channel2.traceposition;//441  
    
    //fpga_set_long_timebase(scopesettings.timeperdiv);
    
  } 
  else 
  {
      //Clear the sample memory
      memset(channel1tracebuffer, 128, sizeof(channel1tracebuffer));
      memset(channel2tracebuffer, 128, sizeof(channel2tracebuffer));
      scope_display_trace_data();
  }
    
    //Check if not in waveform view mode with grid disabled
    if((scopesettings.waveviewmode == 0) || scopesettings.gridenable == 0)
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
  display_copy_rect_to_screen(2, 48, 728, 431);  //44 728 434  


}
  
//----------------------------------------------------------------------------------------------------------------------------------
/*
void scope_preset_long_mode(void)
{

}
*/
//----------------------------------------------------------------------------------------------------------------------------------

void scope_get_long_timebase_data(void)
{
  //Default timeout for 50S/div
  uint32    timeout = 2000;
  uint32    curticks;

  
      if((!scopesettings.runstate)&&(!triggerlong)&&(scopesettings.triggermode))
        {scope_check_long_trigger();}
  
  //Check if run or stop mode - 0-Run mode
  if((scopesettings.runstate == 0)&&(triggerlong))   
  {
      
  //Get the delay setting based on the time base
    switch(scopesettings.timeperdiv)
    {
    //50S/div
    case 0:
      timeout = 1000;
      break;
    //20S/div
    case 1:
      timeout = 400;
      break;

    //10S/div
    case 2:
      timeout = 200;
      break;

    //5S/div
    case 3:
      timeout = 100;
      break;

    //2S/div
    case 4:
      timeout = 40;
      break;

    //1S/div
    case 5:
      timeout = 20;
      break;

    //500mS/div
    case 6:
      timeout = 10;
      break;

    //200mS/div
    case 7:
      timeout = 4;
      break;

    //100mS/div
    case 8:
      timeout = 2;
      break;
      
    default:
        break;      
  }
  
  //Make the timeout timer tick related by adding it to the previous capture
  //timeout += scopesettings.previoustimerticks;
  timeout += previoustimerticks;
  
  //For smaller timeouts (500mS/div, 200mS/div and 100mS/div) stay in the wait loop even if there is touch
  //while((scopesettings.timeperdiv > 5) || (havetouch == 0))
  while((havetouch == 0))
  {
    //Get the current ticks
    curticks = timer0_get_ticks();
  
    //Check if there is a timeout
    //While technically prone to error on timer ticks overflow the scope has to be running for >49 days before it occurs
    if(curticks >= timeout)
    {
      //Save the current ticks for next timeout and bail out the loop
      //scopesettings.previoustimerticks = curticks;
      previoustimerticks = curticks;
      goto skip_delay;
    }
    
    //Scan the touch panel to see if there is user input
    tp_i2c_read_status();
  }
    
  //Wait an extra 40 milliseconds when there was touch
  timer0_delay(30);
  
skip_delay:

      //flag signal is triggered=1 for auto mode
    if ( scopesettings.count >2998) //2998
    {  if(scopesettings.triggermode == 1) 
        {//stop osciloskop
        scopesettings.runstate = 1;
          //Show this on the screen
        scope_run_stop_text();
  
        //Check in which state the right menu is in
        //Button back to inactive state
        if(scopesettings.rightmenustate == 0) scope_run_stop_button(0);
        
        triggerlong = 0;
    
        }
       if(scopesettings.triggermode == 2) scope_preset_values();
    }
  if ( scopesettings.count > 2999)   //2999 
  {scopesettings.count = 0;      //reset buffer pointer 
  //scopesettings.triggerhorizontalposition = 10; 
  //    scopesettings.xpos = 4;
    //scopesettings.lastx = 3;
    
    //Draw the grid lines and dots based on the grid brightness setting
        scope_draw_grid();
    
    
    //Draw the signal center, trigger level and trigger position pointers
    scope_draw_pointers();
   //disp_first_sample = 0;
  
  }

  //Check channel 1 is enabled
  if(scopesettings.channel1.enable) 
    //Read, accumulate and average 10 bytes of channel 1 trace data
    scopesettings.channel1.tracebuffer[scopesettings.count] = fpga_average_trace_data(0x24);

  //Check channel 2 is enabled
  if(scopesettings.channel2.enable)   
    //Read, accumulate and average 10 bytes of channel 2 trace data
    scopesettings.channel2.tracebuffer[scopesettings.count] = fpga_average_trace_data(0x26);


  if ( scopesettings.xpos >726) //725
  {
      
    //Use a separate buffer to clear the screen
    display_set_screen_buffer(displaybuffertmp);//1

    //Clear the trace portion of the screen
    display_set_fg_color(BLACK_COLOR);
    display_fill_rect(2, 48, 728, 431); //403
    
    //Calculate the start and end x coordinates
    //if (scopesettings.count >363 )disp_xstart = (scopesettings.count - 350); else {disp_xstart = 0; disp_xend = 726;}
    //if (scopesettings.count < 2648 )disp_xend = (scopesettings.count + 350); else {disp_xstart = 2300;disp_xend = 3000;}
    
    //disp_xend = scopesettings.count + xrange;
    
    //scopesettings.triggerhorizontalposition -= 725; 

    //Check if not in waveform view mode with grid disabled
    if((scopesettings.waveviewmode == 0) || scopesettings.gridenable == 0)
    {
        //Draw the grid lines and dots based on the grid brightness setting
        scope_draw_grid();
    }
    
    //Draw the signal center, trigger level and trigger position pointers
    scope_draw_pointers();
    
      //Draw the cursors with their measurement displays
    scope_draw_time_cursors();
    scope_draw_volt_cursors();
    scope_display_cursor_measurements();
    
    scopesettings.xpos = 4;
    scopesettings.lastx = 3;
    
    
            //Copy it to the actual screen buffer
  display_set_source_buffer(displaybuffertmp);//1
  display_set_screen_buffer((uint16 *)maindisplaybuffer);
  display_copy_rect_to_screen(2, 48, 728, 431); //403
  //display_copy_rect_to_screen(2, 44, 728, 434);       //opravit rozmery
  }
  
      
    //Check channel 1 is enabled
  if(scopesettings.channel1.enable) 
  {     
      //Get the sample CH1
        scopesettings.channel1.sample2 = scope_get_y_sample(&scopesettings.channel1,  scopesettings.count); 

        //Set CH1 display trace color 
        display_set_fg_color(CHANNEL1_COLOR);
        //Draw the line 
        display_draw_line(scopesettings.lastx, scopesettings.channel1.sample1, scopesettings.xpos, scopesettings.channel1.sample2);
        scopesettings.channel1.sample1 =  scopesettings.channel1.sample2;
  }
        
  //Check channel 2 is enabled
  if(scopesettings.channel2.enable) 
  {
        //Get the sample CH2
        scopesettings.channel2.sample2 = scope_get_y_sample(&scopesettings.channel2,  scopesettings.count);
        
            //Set CH2 display trace color 
        display_set_fg_color(CHANNEL2_COLOR);
        //Draw the last line
        display_draw_line(scopesettings.lastx, scopesettings.channel2.sample1, scopesettings.xpos, scopesettings.channel2.sample2);
        scopesettings.channel2.sample1 =  scopesettings.channel2.sample2;
  }
    
  scopesettings.lastx =  scopesettings.xpos;  
  scopesettings.xpos++;
  scopesettings.count++;
  disp_first_sample = scopesettings.count;
  
  } //end runstate
}

//----------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------

void scope_check_long_trigger(void)
{
    uint32 sample1AVG = 0;
    uint32 sample2AVG = 0;
    uint32 sample1 = 0;
    uint32 sample2 = 0;
    uint32 level = 0;
    
    level = scopesettings.triggerverticalposition;
    
    //triggerlong = 1; 
    //scope_run_stop_text();
    
 
      //Check trigger channel 1 or channel 2
    if((scopesettings.triggerchannel == 0))
    {
    //Read, accumulate and average 10 bytes of channel 1 trace data multiply
    sample1AVG = fpga_average_trace_data(0x24);
    timer0_delay(5);
    sample2AVG = fpga_average_trace_data(0x24);
    //scopesettings.channel1.sample1 = sampleAVG;
    //Get the sample and adjust the data for the correct voltage per div setting
    //sample = ((sampleAVG) * scopesettings.channel1.input_calibration[scopesettings.channel1.samplevoltperdiv]) >> VOLTAGE_SHIFTER;
    sample1 = ((sample1AVG) * multiply) >> VOLTAGE_SHIFTER;
    sample2 = ((sample2AVG) * multiply) >> VOLTAGE_SHIFTER;
    scopesettings.channel1.sample1 = sample2;
    
    }else
    {  
    //Read, accumulate and average 10 bytes of channel 2 trace data
    sample1AVG = fpga_average_trace_data(0x26);
    timer0_delay(5);
    sample2AVG = fpga_average_trace_data(0x26);
    //scopesettings.channel2.sample1 = sampleAVG;
    //Get the sample and adjust the data for the correct voltage per div setting
    //sample = ((sampleAVG) * scopesettings.channel2.input_calibration[scopesettings.channel2.samplevoltperdiv]) >> VOLTAGE_SHIFTER;
    sample1 = ((sample1AVG) * multiply) >> VOLTAGE_SHIFTER;
    sample2 = ((sample2AVG) * multiply) >> VOLTAGE_SHIFTER;
    
    scopesettings.channel2.sample1 = sample2;
    
    }
   
    //if((scopesettings.triggerverticalposition == sample)||(scopesettings.triggerverticalposition == sample+1)) 
    //{triggerlong = 1; scope_run_stop_text();}//scopesettings.triggerhorizontalposition = 3000; }
    
    
    
         if(((scopesettings.triggeredge == 0) && (sample1 < level) && (sample2 >= level)) ||
       ((scopesettings.triggeredge == 1) && (sample1 >= level) && (sample2 < level)))
         {triggerlong = 1; scope_run_stop_text();}
     
     
    /*
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      //display_text(650, 10, buffertime);
 
      display_decimal(600, 10, sampleAVG);
      display_decimal(600, 20, sample);
      display_decimal(600, 30, scopesettings.triggerverticalposition);
*/
    
    /*
         //Set the trigger level
    fpga_set_trigger_level();
    
    fpga_set_trigger_mode();
    
    fpga_set_trigger_edge();
    
        //Enable disableeee trigger system???
    fpga_write_cmd(0x0F);
    fpga_write_byte(0x00);
    
      //Set the FPGA for short time base mode
  //fpga_write_cmd(0x28);
  //fpga_write_byte(0x00);
    
      //Reset the sample system
  fpga_write_cmd(0x01);
  fpga_write_byte(0x01);
  
  //Send check on ready command
  fpga_write_cmd(0x05);
  
  //Wait for the flag to become 1
  while((fpga_read_byte() & 1) == 0);
  
  //Test again to make sure it was no glitch?????
  while((fpga_read_byte() & 1) == 0);
  
  //Done with reset
  fpga_write_cmd(0x01);
  fpga_write_byte(0x00);
  
  //Send check on triggered or buffer full command to the FPGA
  fpga_write_cmd(0x0A);
  
      //Wait for the FPGA to signal triggered or touch panel is touched
    while(((fpga_read_byte() & 1) == 0) && (havetouch == 0))
    {
      //Scan the touch panel
      tp_i2c_read_status();
    }
    
    //Disable trigger system???
    fpga_write_cmd(0x0F);
    fpga_write_byte(0x01);
  

    
    triggerlong = 1; 
    scope_run_stop_text();
    
     */
    
}

//----------------------------------------------------------------------------------------------------------------------------------
/*
void scope_check_long_trigger(void)
{
  
}
*/

//----------------------------------------------------------------------------------------------------------------------------------
//**********************************************************************************************************************************
//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_ADoffset_value(void)
{
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      //display_text(650, 10, buffertime);
      //uint32 center = (scopesettings.channel1.center * signal_adjusters[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;   
      
       
 // uint32 offset;
  uint32    max1;
  uint32    min1;    
  uint32    p2p1; 
  uint32    center1x; 
  uint32    dcoffset;
  uint32    dcoffsetm;
      
    max1 = (scopesettings.channel1.max * scopesettings.channel1.input_calibration[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;
    center1x = (scopesettings.channel1.center * scopesettings.channel1.input_calibration[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;
    min1 = (scopesettings.channel1.min * scopesettings.channel1.input_calibration[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;
    p2p1 = (scopesettings.channel1.peakpeak * scopesettings.channel1.input_calibration[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;
    dcoffset = (scopesettings.channel1.dcoffset * scopesettings.channel1.input_calibration[scopesettings.channel1.displayvoltperdiv]) >> VOLTAGE_SHIFTER;
    dcoffsetm = (scopesettings.channel1.dcoffset * multiply ) >> VOLTAGE_SHIFTER;//3289650
    
    //scopesettings.channel1.dcoffset
      
      display_decimal(550, 0, max1 );
      display_decimal(550, 10, center1x);
      display_decimal(550, 20, min1);
      display_decimal(550, 30, p2p1);
      display_decimal(550, 40, dcoffset);
      display_decimal(550, 50, dcoffsetm);
      
      display_decimal(600, 0, scopesettings.channel1.max );
      display_decimal(600, 10, scopesettings.channel1.center );
      display_decimal(600, 20, scopesettings.channel1.min );
      display_decimal(600, 30, scopesettings.channel1.peakpeak );
      display_decimal(600, 40, scopesettings.channel1.dcoffset );
      
      //display_decimal(630, 20, scopesettings.voltcursor1position );
      //display_decimal(630, 30, scopesettings.voltcursor2position );
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_sunlight(void)
{
  //Draw the background
  display_fill_rect(159, 116, 226, 36);
/*
  if(mode == 0)
  {
    //Inactive so white foreground and grey background
    display_set_fg_color(WHITE_COLOR);
    display_set_bg_color(DARKGREY_COLOR);
  }
  else
  {
      */
    //Active so black foreground and yellow background
    display_set_fg_color(BLACK_COLOR);
    display_set_bg_color(YELLOW_COLOR);
 // }
 

  //Display the icon with the set colors
  display_copy_icon_use_colors(light_icon, 350, 10, 24, 24);    //light-sun icon
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_longbase_value(void)
{
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      //display_text(650, 10, buffertime);

      
      display_decimal(600, 10, scopesettings.timeperdiv);
      display_decimal(600, 20, scopesettings.samplerate);
      
}
//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_trigerposition_value(void)
{
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 220);  //x , y , sirka, vyska
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      //display_text(650, 10, buffertime);

      //display_decimal(600, 0, triggerposition);
      display_decimal(600, 10, disp_xstart);
      display_decimal(600, 20, disp_xend);
      display_decimal(600, 30, disp_first_sample);
      
      
      //display_decimal(600, 50,  ((disp_trigger_index - disp_first_sample + 1) * 727.0) / (disp_trigger_index - disp_first_sample >= 0 ? (727.0 / disp_xpos_per_sample) : 1));
      //display_decimal(600, 60,  int32 ((disp_trigger_index - disp_first_sample + 1) * 727.0) / (727.0 / disp_xpos_per_sample));
      //display_decimal(600, 50,  (((1500          ) * 727.0) / (727.0 / disp_xpos_per_sample)+15));
      //display_decimal(600, 60,  (((1500 - 3000 ) * 727.0) / (727.0 / disp_xpos_per_sample)));
      
     // (disp_trigger_index - disp_first_sample + 1) / (727.0 / disp_xpos_per_sample)
      
      //display_decimal(600, 70, disp_trigger_index);
      //display_decimal(600, 80, disp_xpos_per_sample);  
      
      display_decimal(600, 210, scopesettings.timeperdiv);
      display_decimal(600, 220, scopesettings.samplerate);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_calibration_value(void)
{
      display_set_fg_color(BLACK_COLOR);
      //Fill the settings background
      display_fill_rect(550, 0, 100, 250);  //x , y , sirka, vyska 220
      display_set_fg_color(WHITE_COLOR);
      display_set_font(&font_2);
      //display_text(650, 10, buffertime);
  
      display_decimal(600, 40, scopesettings.channel1.dc_calibration_offset[0]);
      display_decimal(600, 50, scopesettings.channel1.dc_calibration_offset[1]);
      display_decimal(600, 60, scopesettings.channel1.dc_calibration_offset[2]);
      display_decimal(600, 70, scopesettings.channel1.dc_calibration_offset[3]);
      display_decimal(600, 80, scopesettings.channel1.dc_calibration_offset[4]);
      display_decimal(600, 90, scopesettings.channel1.dc_calibration_offset[5]);
      display_decimal(600, 100, scopesettings.channel1.dc_calibration_offset[6]);
      
      display_decimal(600, 120, scopesettings.channel2.dc_calibration_offset[0]);
      display_decimal(600, 130, scopesettings.channel2.dc_calibration_offset[1]);
      display_decimal(600, 140, scopesettings.channel2.dc_calibration_offset[2]);
      display_decimal(600, 150, scopesettings.channel2.dc_calibration_offset[3]);
      display_decimal(600, 160, scopesettings.channel2.dc_calibration_offset[4]);
      display_decimal(600, 170, scopesettings.channel2.dc_calibration_offset[5]);
      display_decimal(600, 180, scopesettings.channel2.dc_calibration_offset[6]);
      
      display_decimal(600, 210, scopesettings.channel1.adc1compensation);
      display_decimal(600, 220, scopesettings.channel1.adc2compensation);
      display_decimal(600, 230, scopesettings.channel2.adc1compensation);
      display_decimal(600, 240, scopesettings.channel2.adc2compensation);
      
      display_decimal(650, 40, scopesettings.channel1.input_calibration[0]);
      display_decimal(650, 50, scopesettings.channel1.input_calibration[1]);
      display_decimal(650, 60, scopesettings.channel1.input_calibration[2]);
      display_decimal(650, 70, scopesettings.channel1.input_calibration[3]);
      display_decimal(650, 80, scopesettings.channel1.input_calibration[4]);
      display_decimal(650, 90, scopesettings.channel1.input_calibration[5]);
      display_decimal(650, 100, scopesettings.channel1.input_calibration[6]);
      
      display_decimal(650, 120, scopesettings.channel2.input_calibration[0]);
      display_decimal(650, 130, scopesettings.channel2.input_calibration[1]);
      display_decimal(650, 140, scopesettings.channel2.input_calibration[2]);
      display_decimal(650, 150, scopesettings.channel2.input_calibration[3]);
      display_decimal(650, 160, scopesettings.channel2.input_calibration[4]);
      display_decimal(650, 170, scopesettings.channel2.input_calibration[5]);
      display_decimal(650, 180, scopesettings.channel2.input_calibration[6]);
      
    int32   volts;
    int32   value;
    PVOLTCALCDATA vcd;
    
    //Calculate the voltage based on the channel settings
    vcd = (PVOLTCALCDATA)&volt_calc_data[scopesettings.channel2.magnification][scopesettings.channel2.displayvoltperdiv];
    
    //value=scopesettings.channel1.peakpeak;
    value=scopesettings.channel2.max-128;
    
    volts = (value * scopesettings.channel2.input_calibration[5]) >> VOLTAGE_SHIFTER;
    
    volts *= vcd->mul_factor;
      
    display_decimal(650, 190, value);
    display_decimal(650, 200, volts);
    
    display_decimal(600, 190, scopesettings.channel2.displayvoltperdiv);
    display_decimal(600, 200, scopesettings.channel2.magnification);
    
}
//----------------------------------------------------------------------------------------------------------------------------------