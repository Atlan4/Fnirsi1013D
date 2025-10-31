/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.c to edit this template
 */

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
#include "sd_card_interface.h"  //only diagnostic menu?
#include "display_lib.h"
#include "ff.h"
#include "DS3231.h"

#include "usb_interface.h"
#include "variables.h"

#include "sin_cos_math.h"
#include  "ref_and_math.h"

#include <string.h>



//----------------------------------------------------------------------------------------------------------------------------------
void display_REFx_data(void)  //display 
{

  
    //Check if REF1 CH1 is enabled
    if(scopesettings.ref1)     
    {  
      //Set color trace
      display_set_fg_color(scopesettings.ch_ref1.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref1);  
    }
    
    //Check if REF2 CH1 is enabled
    if(scopesettings.ref2)
    {          
      display_set_fg_color(scopesettings.ch_ref2.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref2);  
    }
    
    //Check if REF3 CH1 is enabled
    if(scopesettings.ref3)
    {          
      display_set_fg_color(scopesettings.ch_ref3.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref3);  
    }
        
    //Check if REF4 CH1 is enabled
    if(scopesettings.ref4)
    {          
      display_set_fg_color(scopesettings.ch_ref4.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref4);  
    }
    
    //Check if REF5 CH2 is enabled
    if(scopesettings.ref5)     
    {  
      //Set color trace
      display_set_fg_color(scopesettings.ch_ref5.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref5);  
    }
    
    //Check if REF6 CH2 is enabled
    if(scopesettings.ref6)
    {          
      display_set_fg_color(scopesettings.ch_ref6.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref6);  
    }
    
    //Check if REF7 CH2 is enabled
    if(scopesettings.ref7)
    {          
      display_set_fg_color(scopesettings.ch_ref7.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref7);  
    }
        
    //Check if REF8 CH2 is enabled
    if(scopesettings.ref8)
    {          
      display_set_fg_color(scopesettings.ch_ref8.color);
      scope_display_channel_linear_trace(&scopesettings.ch_ref8);  
    }

}


void copy_ch_to_ref(CHANNELSETTINGS *dest, const CHANNELSETTINGS *src, uint32 *tracebuffer) {
    int backup = dest->color;  // Záloha pôvodnej farby z cieľovej štruktúry

    memcpy(dest, src, sizeof(CHANNELSETTINGS));  // Skopíruj celé nastavenia zo zdroja
    dest->tracebuffer = (uint8 *)tracebuffer;  // Priraď nový tracebuffer
    memcpy(dest->tracebuffer, src->tracebuffer, MAX_SAMPLE_BUFFER_SIZE);  // Skopíruj dáta do tracebufferu
    dest->color = backup;  // Obnov farbu
}

//----------------------------------------------------------------------------------------------------------------------------------    

void display_MATHx_data(void)  //display 
{
  //Check if mathmode is enabled
  if(mathmode)
  {        
    display_set_fg_color(MATH_COLOR);
    
    math_sample=1;
    compute_all_math_ops(channelA, channelB);
        
    scope_display_channel_linear_trace(&scopesettings.channel1);  //vyuziva parametre ch na zobrazenie signalu math
    math_sample=0;
  }
     
}
          /*
            for (uint32 i = 0; i < MAX_SAMPLE_BUFFER_SIZE; i++) 
            {  
                //scopesettings.channel2.tracebuffer[i]=0;
                //scopesettings.channel2.tracebuffer[i] = ((scopesettings.channel2.tracebuffer[i]) + (channel2tracebufferAVG[i]))/2;
                //channel2tracebufferAVG[i]=scopesettings.channel2.tracebuffer[i];
              if((channalA==1)&&(channalB==1){ scopesettings.math_tracebuffer[i] = ((scopesettings.channel1.tracebuffer[i]) + (scopesettings.channel2.tracebuffer[i]-128)); }
                
            }
            //memcpy(settings->ref_tracebuffer, (scopesettings.channel1.tracebuffer+scopesettings.channel2.tracebuffer), MAX_SAMPLE_BUFFER_SIZE);
           */

//----------------------------------------------------------------------------------------------------------------------------------   

void compute_all_math_ops(int channelA, int channelB) 
{
    int8 result = 0;
    uint8* A = get_channel_data(channelA);
    uint8* B = get_channel_data(channelB);

    if (!A || !B) return;  // Ošetrenie neplatného vstupu

    for (int i = 0; i < MAX_SAMPLE_BUFFER_SIZE; i++) 
    {
        int8 a = A[i];
        int8 b = B[i]-128;
        
        if (mathmode==1) result = a + b;
        if (mathmode==2) result = a - b;
        if (mathmode==3) result = (a * b);// /100
        if (mathmode==4) result = (b != 0) ? a / b : 0;
        
        scopesettings.math_tracebuffer[i] = result;
    }
}

uint8* get_channel_data(int channel) 
{
    switch (channel) {
        case 0: return (uint8 *)channel1tracebuffer;
        case 1: return (uint8 *)channel2tracebuffer;
        case 2: return (uint8 *)ref1_tracebuffer;
        case 3: return (uint8 *)ref2_tracebuffer;
        default: return NULL;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
void scope_load_ALLREF_file(void)
{
  //Load ref waveforms on startup
  if( scopesettings.ref_on_startup )
  {
    //Load data from file x.ref to buffer refx (1-4)
    scope_load_REFx_item_file(1);
    scope_load_REFx_item_file(2);
    scope_load_REFx_item_file(3);
    scope_load_REFx_item_file(4);

    //Load data from file x.ref to buffer refx (5-8)
    scope_load_REFx_item_file(5);
    scope_load_REFx_item_file(6);
    scope_load_REFx_item_file(7);
    scope_load_REFx_item_file(8);
  }
  
  //Disable view ref waveforms
  //if( scopesettings.ref_on_startup ) { scopesettings.ch_ref1.enable = 1; scopesettings.ch_ref2.enable = 1; scopesettings.ch_ref3.enable = 1; scopesettings.ch_ref4.enable = 1;
  //                                     scopesettings.ch_ref5.enable = 1; scopesettings.ch_ref6.enable = 1; scopesettings.ch_ref7.enable = 1; scopesettings.ch_ref8.enable = 1; }
  //else 
  // if( !scopesettings.ref_on_startup )    { scopesettings.ch_ref1.enable = 0; scopesettings.ch_ref2.enable = 0; scopesettings.ch_ref3.enable = 0; scopesettings.ch_ref4.enable = 0;
  //       scopesettings.ch_ref5.enable = 0; scopesettings.ch_ref6.enable = 0; scopesettings.ch_ref7.enable = 0; scopesettings.ch_ref8.enable = 0; }
}
//----------------------------------------------------------------------------------------------------------------------------------

void scope_load_REFx_item_file(uint8 number)
{
  //Point to the file numbers
  //uint16 *fnptr = (uint16 *)viewfilenumberdata;

  uint32 result;

  //Switch to the given type
  viewtype = VIEW_TYPE_REF;

  scope_print_file_name(number);
  
  //Copy the filename to thumbnail filename
  //Might need a re write of the message setup
  strcpy( viewthumbnaildata[0].filename, viewfilename); 

  //Try to open the file for reading
  result = f_open(&viewfp, viewfilename, FA_READ);

  //Check if file opened ok
  if(result == FR_OK)
  {
    //Checks on correct number of bytes read might be needed
    //Load the setup data to the file setup data buffer
    if((result = f_read(&viewfp, (uint8 *)viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
    //if((result = f_read(&viewfp, &scopesettings.ch_ref1, sizeof(CHANNELSETTINGS), 0)) == FR_OK)  
      
    {
      //Copy the loaded data to the settings
      scope_restore_setup_from_REFx_file(number);

      //Check if the version of the file is wrong
      if(viewfilesetupdata[1] != WAVEFORM_FILE_VERSION)
      {
        //No need to load the rest of the data
        result = WAVEFORM_FILE_ERROR;

        //Show the user the file is not correct
        scope_display_file_status_message(MESSAGE_WAV_VERSION_MISMATCH, 0);
      }
      else
      {
      //------------------------------------------------------------------------ 
        if(number == 1)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref1_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref1_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }
        else if(number == 2)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref2_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref2_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }        
        else if(number == 3)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref3_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref3_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }        
        else if(number == 4)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref4_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }
        if(number == 5)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref5_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref1_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }
        else if(number == 6)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref6_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref2_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }        
        else if(number == 7)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref7_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref3_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }        
        else if(number == 8)
        {
          //Load the channel 1 sample data      
          if((result = f_read(&viewfp, (uint8 *)ref8_tracebuffer, 3000, 0)) == FR_OK)
          {
            //Load the channel 2 sample data
            //if((result = f_read(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0)) == FR_OK);   
          } 
        }         
      //------------------------------------------------------------------------      
      }
    }
    
    /*
     
                 //Do a check on file validity
            if(!(result = scope_check_waveform_file())) { } else
            {
              //Checksum error so signal that to the user
              result = WAVEFORM_FILE_ERROR;

              //Show the user the file is not correct
              scope_display_file_status_message(MESSAGE_WAV_CHECKSUM_ERROR, 0);
            }
     */

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
/*
  //Check if all went well
  if(result == FR_OK)
  {
    //Tell it to the caller
    return(VIEW_TRACE_LOAD_OK);
  }
*/
  //return(VIEW_TRACE_LOAD_ERROR);
  //return(VIEW_TRACE_LOAD_OK);
}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_save_REFx_item_file(uint8 number)
{
  //uint32  newnumber;
  uint32  result;
  //uint16 *fnptr;
  //uint16 *eptr;

  //Switch to the given type
  viewtype = VIEW_TYPE_REF;

  //Load the thumbnail file for this type. Needed for finding the file name and to add the thumbnail
  if(scope_load_thumbnail_file() != 0)
  {
    //Loading the thumbnail file failed so no sense in going on
    return;
  }


  //Setup the filename for in the thumbnail
  scope_print_file_name(number);

  
  //Check if ref buffer is no empty
  if(1)
  {
    //Show the user there is no more room for a new item
   // scope_display_file_status_message(MESSAGE_REF_BUFF_EMPTY_ERROR, 0);
    
    //No sense to continue
   // return;
  }

  //Copy the filename to thumbnail filename
  //Might need a re write of the message setup
  strcpy( viewthumbnaildata[0].filename, viewfilename); 
  
  //Open the new file. On failure signal this and quit
  result = f_open(&viewfp, viewfilename, FA_CREATE_ALWAYS | FA_WRITE);
  
  //Check if file created without problems
  if(result == FR_OK)
  {
    //For the waveform the setup and the waveform data needs to be written
    //Save the settings for the trace portion of the data and write them to the file
    scope_prepare_setup_for_REFx_file(number);
    
    if(number == 1)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
        //if((result = f_write(&viewfp, &scopesettings.ch_ref1, sizeof(CHANNELSETTINGS), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref1_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref1_tracebuffer, 3000, 0); 
        }
      }
    }
    
    else if(number == 2)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref2_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref2_tracebuffer, 3000, 0);
        }
      }
    }
    
    else if(number == 3)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref3_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref3_tracebuffer, 3000, 0);
        }
      }
    }
    
    else if(number == 4)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref4_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0);
        }
      }
    }
    else if(number == 5)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref5_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0);
        }
      }
    }
    else if(number == 6)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref6_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0);
        }
      }
    }
    else if(number == 7)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref7_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0);
        }
      }
    }
    else if(number == 8)
    { 
      //Write the setup data to the file
      if((result = f_write(&viewfp, viewfilesetupdata, sizeof(viewfilesetupdata), 0)) == FR_OK)
      {
        //Write the trace data to the file
        //Save the channel 1 raw sample data
        if((result = f_write(&viewfp, (uint8 *)ref8_tracebuffer, 3000, 0)) == FR_OK)
        {
          //Save the channel 2 raw sample data
          //result = f_write(&viewfp, (uint8 *)channel2_ref4_tracebuffer, 3000, 0);
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

}

//----------------------------------------------------------------------------------------------------------------------------------

void scope_restore_setup_from_REFx_file(uint8 number)
{
  PCHANNELSETTINGS settings;
  
  //uint32 *ptr = viewfilesetupdata;
  uint32 index = 0;
    
  
  settings = &scopesettings.channel1;
  
  switch (number)
  {
    case 1: settings = &scopesettings.ch_ref1; break;
    case 2: settings = &scopesettings.ch_ref2; break;
    case 3: settings = &scopesettings.ch_ref3; break;
    case 4: settings = &scopesettings.ch_ref4; break;
    case 5: settings = &scopesettings.ch_ref5; break;
    case 6: settings = &scopesettings.ch_ref6; break;
    case 7: settings = &scopesettings.ch_ref7; break;
    case 8: settings = &scopesettings.ch_ref8; break;
    default: break;
  }
  
  //Leave space for file version and checksum data  21
  index = CHANNEL1_SETTING_OFFSET;
  
 memcpy(settings, (uint8 *)&viewfilesetupdata[index], sizeof(CHANNELSETTINGS));

  
  /*
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
  */
  
}

//----------------------------------------------------------------------------------------------------------------------------------

//These functions are for handling the settings to and from file
void scope_prepare_setup_for_REFx_file(uint8 number)
{
  PCHANNELSETTINGS settings;

  uint32 *ptr = viewfilesetupdata;
  uint32 index = 0;
  uint32 checksum = 0;
  
  settings = &scopesettings.channel1;
  
  switch (number)
  {
    case 1: settings = &scopesettings.ch_ref1; break;
    case 2: settings = &scopesettings.ch_ref2; break;
    case 3: settings = &scopesettings.ch_ref3; break;
    case 4: settings = &scopesettings.ch_ref4; break;
    case 5: settings = &scopesettings.ch_ref5; break;
    case 6: settings = &scopesettings.ch_ref6; break;
    case 7: settings = &scopesettings.ch_ref7; break;
    case 8: settings = &scopesettings.ch_ref8; break;
    default: break;
  }

  //Best to clear the buffer first since not all bytes are used for settings
  memset((uint8 *)viewfilesetupdata, 0, sizeof(viewfilesetupdata));

  //Put in a version number for the waveform view file
  ptr[1] = WAVEFORM_FILE_VERSION;
  
  //Put in a firmware version for the waveform view file
  memcpy(&ptr[2], VERSION_STRING, 7);
  
  //Leave space for file version and checksum data  21
  index = CHANNEL1_SETTING_OFFSET;
  
  //memcpy(&viewfilesetupdata+index, &scopesettings.ch_ref3, sizeof(CHANNELSETTINGS));
  //memcpy((uint8 *)&ptr[index], &scopesettings.ch_ref1, sizeof(CHANNELSETTINGS));
  memcpy((uint8 *)&ptr[index], settings, sizeof(CHANNELSETTINGS));

  
  /*
  //Copy the needed channel 1 settings and measurements 
  ptr[index++] = settings->enable;
  ptr[index++] = settings->displayvoltperdiv;
  ptr[index++] = settings->samplevoltperdiv;
  ptr[index++] = settings->fftenable;
  ptr[index++] = settings->coupling;
  ptr[index++] = settings->magnification;
  ptr[index++] = settings->V_A;
  ptr[index++] = settings->traceposition;
  ptr[index++] = settings->min;
  ptr[index++] = settings->max;
  ptr[index++] = settings->average;
  ptr[index++] = settings->rms;
  ptr[index++] = settings->center;
  ptr[index++] = settings->peakpeak;
  ptr[index++] = settings->frequencyvalid;
  ptr[index++] = settings->frequency;
  ptr[index++] = settings->lowtime;
  ptr[index++] = settings->hightime;
  ptr[index++] = settings->periodtime;
  ptr[index++] = settings->dcoffset;
  ptr[index++] = settings->invert;
  */
  /*
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
  */
  /*
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
  */

  
  //Calculate a checksum over the settings data
  for(index=1;index<VIEW_NUMBER_OF_SETTINGS;index++)
  //  for(index=1;index<24;index++)
  {
    checksum += ptr[index];
  }
  
  //Add the sample data too
  for(index=0;index<750;index++)
  {
    if(number == 1)
    { 
      //Add both the channels
      checksum += ref1_tracebuffer[index];
      //checksum += channel2_ref1_tracebuffer[index];
    }
    else if(number == 2)
    { 
      //Add both the channels
      checksum += ref2_tracebuffer[index];
      //checksum += channel2_ref2_tracebuffer[index];
    }
    else if(number == 3)
    { 
      //Add both the channels
      checksum += ref3_tracebuffer[index];
      //checksum += channel2_ref3_tracebuffer[index];
    }
    else if(number == 4)
    { 
      //Add both the channels
      checksum += ref4_tracebuffer[index];
      //checksum += channel2_ref4_tracebuffer[index];
    }
    else if(number == 5)
    { 
      //Add both the channels
      checksum += ref5_tracebuffer[index];
      //checksum += channel2_ref4_tracebuffer[index];
    }
    else if(number == 6)
    { 
      //Add both the channels
      checksum += ref6_tracebuffer[index];
      //checksum += channel2_ref4_tracebuffer[index];
    }
    else if(number == 7)
    { 
      //Add both the channels
      checksum += ref7_tracebuffer[index];
      //checksum += channel2_ref4_tracebuffer[index];
    }
    else if(number == 8)
    { 
      //Add both the channels
      checksum += ref8_tracebuffer[index];
      //checksum += channel2_ref4_tracebuffer[index];
    }
  }

  //Store the checksum at the beginning of the file
  ptr[0] = checksum;
  
  //Restore settings pointer
  //settings = backup;

}

//----------------------------------------------------------------------------------------------------------------------------------
