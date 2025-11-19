//----------------------------------------------------------------------------------------------------------------------------------

#ifndef VARIABLES_H
#define VARIABLES_H

//----------------------------------------------------------------------------------------------------------------------------------

#include <string.h>

#include "types.h"
#include "font_structs.h"
#include "fnirsi_1013d_scope.h"
#include "ff.h"

//----------------------------------------------------------------------------------------------------------------------------------
//Version info
//----------------------------------------------------------------------------------------------------------------------------------

#define VERSION_STRING             "v0.28a" // sample to 8000, data triger to 8192 mod kurzor_position

#define VERSION_STRING_XPOS             698//690//681     //690 
#define VERSION_STRING_YPOS              24

//----------------------------------------------------------------------------------------------------------------------------------
//Defines
//----------------------------------------------------------------------------------------------------------------------------------

#define VIEW_NOT_ACTIVE                   0
#define VIEW_ACTIVE                       1

#define VIEW_ITEM_XSTART                  2
#define VIEW_ITEM_YSTART                  0
#define VIEW_ITEM_WIDTH                 182
#define VIEW_ITEM_HEIGHT                120

#define VIEW_ITEM_TRACE_POINTS          182//180

#define VIEW_ITEM_XNEXT                 182
#define VIEW_ITEM_YNEXT                 120

#define VIEW_ITEM_XLAST                 720

#define VIEW_THUMBNAIL_DATA_SIZE     400000
#define VIEW_FILE_NUMBER_DATA_SIZE     2000

#define VIEW_MAX_ITEMS                 1000

#define VIEW_ITEMS_PER_PAGE              16

#define VIEW_TYPE_MASK                    3//1

#define VIEW_TYPE_PICTURE                 0
#define VIEW_TYPE_WAVEFORM                1
#define VIEW_TYPE_REF                     2

#define VIEW_MAX_TYPES                    3//2

#define VIEW_ITEM_SELECTED_XSTART        77
#define VIEW_ITEM_SELECTED_YSTART        45

//States for displaying the selected signs
#define VIEW_ITEM_NOT_SELECTED            0
#define VIEW_ITEM_SELECTED_NOT_DISPLAYED  1
#define VIEW_ITEM_SELECTED_DISPLAYED      2
#define VIEW_ITEM_NOT_SELECTED_DISPLAYED  3

//States for select mode
#define VIEW_SELECT_NONE                  0
#define VIEW_SELECT_ALL                   1
#define VIEW_SELECT_INDIVIDUAL            2

//States for bottom file menu displaying
#define VIEW_BOTTON_MENU_INIT             3     //This is a combination of init and show, so show needs to be 1
#define VIEW_BOTTON_MENU_SHOW             1     //Needs to be 1 / 0 for the show and hide since an exor is used to toggle the modes
#define VIEW_BOTTON_MENU_HIDE             0
#define VIEW_BOTTOM_MENU_ACTIVE           8     //A flag needed for the simulator to signal picture viewing is active, so it can give touch on quit

#define VIEW_TRACE_LOAD_OK                0
#define VIEW_TRACE_LOAD_ERROR             1

#define VIEW_BITMAP_LOAD_OK               0
#define VIEW_BITMAP_LOAD_ERROR            1

#define VIEW_CONFIRM_DELETE_NO            1
#define VIEW_CONFIRM_DELETE_YES           2

#define VIEW_NUMBER_OF_SETTINGS         200

#define INPUT_CALIBRATION_SECTOR        708    //Location of the input calibration data on the SD card
#define INPUT_CALIBRATION_SETTING_OFFSET  8    //256 all, used 14*32bit=28; (8-40byte)  //0 and 1 checksum
#define DC_SHIFT_SETTING_OFFSET          40    //256 all, used 6*32bit=12;  (40-52byte) //8-52byte calculate checksum

#define SETTINGS_SECTOR                 709    //old 700 Location of the settings on the SD card for now
#define DISPLAY_CONFIG_SECTOR           710
#define STARTUP_CONFIG_ADDRESS          (uint32*)0x81BFFC1F //value for default start firmware (0-pepco,1-fnirsi, 2-FEL, <3 skip menu)
                                               //configuration_data         file_setup 
#define CHANNEL1_SETTING_OFFSET          8     //24 all, used 11*16bit=11   used 21*16bit
#define CHANNEL2_SETTING_OFFSET          32    //24 all, used 11*16bit      used 21*16bit 
#define TRIGGER_SETTING_OFFSET           56    //24 all, used  9*16bit      used 11*16bit
#define OTHER_SETTING_OFFSET             80    //40 all, used 28*16bit      used 10*16bit 
#define CURSOR_SETTING_OFFSET            120   //16 all, used  7*16bit      used  7*16bit
#define MEASUREMENT_SETTING_OFFSET       136   //32 all, used 28*16bit      used 26*16bit
#define CALIBRATION_SETTING_OFFSET       168   //16 all, used 12*16bit   
#define CALIBRATION_SETTING_OFFSET_C     184   // 8 all, used  4*16bit    
                                       //192 next  max is VIEW_NUMBER_OF_SETTINGS
                                                
#define SETTING_SECTOR_VERSION_HIGH  0x0100    // 0x0100    //load default settings
#define SETTING_SECTOR_VERSION_LOW   0x0014    //>0x0002    //nova verzia ma ma 2 na 709 sector

#define WAVEFORM_FILE_VERSION    0x01000002    //Version 1.0.0.2

#define WAVEFORM_FILE_ERROR             200

#define THUMBNAIL_POINTER_RIGHT           0
#define THUMBNAIL_POINTER_LEFT            1
#define THUMBNAIL_POINTER_DOWN            2

#define PICTURE_HEADER_SIZE               70
#define PICTURE_DATA_SIZE                 (800 * 480 * 2)                              //trace data
#define PICTURE_FILE_SIZE                 (PICTURE_HEADER_SIZE + PICTURE_DATA_SIZE)    //Bitmap header + pixel data

#define PICTURE_HEADER_MISMATCH           100

#define MESSAGE_SAVE_SUCCESSFUL           0
#define MESSAGE_FILE_CREATE_FAILED        1
#define MESSAGE_FILE_OPEN_FAILED          2
#define MESSAGE_FILE_WRITE_FAILED         3
#define MESSAGE_FILE_READ_FAILED          4
#define MESSAGE_FILE_SEEK_FAILED          5
#define MESSAGE_FILE_DELETE_FAILED        6

#define MESSAGE_DIRECTORY_CREATE_FAILED   7

#define MESSAGE_FILE_SYSTEM_FAILED        8

#define MESSAGE_THUMBNAIL_FILE_CORRUPT    9
#define MESSAGE_THUMBNAIL_FILE_FULL      10

#define MESSAGE_BMP_HEADER_MISMATCH      11

#define MESSAGE_WAV_VERSION_MISMATCH     12
#define MESSAGE_ERROR_VERSION_MISMATCH   13

#define MESSAGE_WAV_CHECKSUM_ERROR       14//13

#define MESSAGE_REF_BUFF_EMPTY_ERROR     15//14

//----------------------------------------------------------------------------------------------------------------------------------
//Number of bits used for fixed point calculations on the voltages

#define VOLTAGE_SHIFTER                21

//----------------------------------------------------------------------------------------------------------------------------------
//Menu positions and dimensions
//----------------------------------------------------------------------------------------------------------------------------------
//Keyboard button and menu

#define KEY_BUTTON_XPOS                     400//150

#define KEY_BUTTON_YPOS                       5
#define KEY_BUTTON_WIDTH                     30
#define KEY_BUTTON_HEIGHT                    35

#define KEY_BUTTON_BG_WIDTH                  40 //103
#define KEY_BUTTON_BG_HEIGHT                 35

#define KEY_MENU_XPOS          KEY_BUTTON_XPOS

#define KEY_MENU_YPOS                        50//
#define KEY_MENU_WIDTH                      297// sirka
#define KEY_MENU_HEIGHT                     394//

#define KEY_TOUCHED_COLOR            0x000000FF

//----------------------------------------------------------------------------------------------------------------------------------
//MENU button 

#define MENU_BUTTON_XPOS        0
#define MENU_BUTTON_YPOS        3
#define MENU_BUTTON_WIDTH      70
#define MENU_BUTTON_HEIGHT     38

//----------------------------------------------------------------------------------------------------------------------------------
//Main menu (2, 46, 147, 292)

#define MAIN_MENU_XPOS            2
#define MAIN_MENU_YPOS           46
#define MAIN_MENU_WIDTH         147
#define MAIN_MENU_HEIGHT        292

//Menu system settings
#define SYSTEM_MENU_XPOS                     150
#define SYSTEM_MENU_YPOS          MAIN_MENU_YPOS//46
#define SYSTEM_MENU_WIDTH                    244
#define SYSTEM_MENU_HEIGHT                   413

//Screen brightness settings
#define S_BRIGHTNESS_MENU_XPOS               395// nema byt 396 tj 2pixely medzera ?
#define S_BRIGHTNESS_MENU_YPOS    MAIN_MENU_YPOS//46
#define S_BRIGHTNESS_MENU_WIDTH              331
#define S_BRIGHTNESS_MENU_HEIGHT              58

//Grid brightness settings
#define G_BRIGHTNESS_MENU_XPOS               395
#define G_BRIGHTNESS_MENU_YPOS               104//46
#define G_BRIGHTNESS_MENU_WIDTH              331
#define G_BRIGHTNESS_MENU_HEIGHT              58

//Other settings    59step
#define OTHER_MENU_XPOS                      395
#define OTHER_MENU_YPOS                      46//163//46
#define OTHER_MENU_WIDTH                     210
#define OTHER_MENU_HEIGHT                    413//296

//Calibration menu
#define CALIBRATION_MENU_XPOS                395
#define CALIBRATION_MENU_YPOS                222//46
#define CALIBRATION_MENU_WIDTH               199
#define CALIBRATION_MENU_HEIGHT               59

//RTC settings menu
#define RTC_MENU_XPOS               395
#define RTC_MENU_YPOS               106//46
#define RTC_MENU_WIDTH              200
#define RTC_MENU_HEIGHT             353
//----------------------------------------------------------------------------------------------------------------------------------
//Max light button

#define MAXLIGHT_BUTTON_XPOS                75
#define MAXLIGHT_BUTTON_YPOS                 3
#define MAXLIGHT_BUTTON_WIDTH               43//24//sirka
#define MAXLIGHT_BUTTON_HEIGHT              38

//----------------------------------------------------------------------------------------------------------------------------------
//Time text 12:00:00 (647, 5, 50, 13)

#define TIME_TEXT_XPOS                     123
#define TIME_TEXT_YPOS                       3
#define TIME_TEXT_WIDTH                     48
#define TIME_TEXT_HEIGHT                    13

//----------------------------------------------------------------------------------------------------------------------------------
//Run and stop text

#define RUN_STOP_TEXT_XPOS                 127
#define RUN_STOP_TEXT_YPOS                  22
#define RUN_STOP_TEXT_WIDTH                 38
#define RUN_STOP_TEXT_HEIGHT                18

//----------------------------------------------------------------------------------------------------------------------------------
//Channel button and menu

#define CH1_BUTTON_XPOS                    174
#define CH2_BUTTON_XPOS                    281

#define CH_BUTTON_YPOS                       5
#define CH_BUTTON_WIDTH                     30
#define CH_BUTTON_HEIGHT                    35

#define CH_BUTTON_BG_WIDTH                 103
#define CH_BUTTON_BG_HEIGHT                 35

#define CH1_MENU_XPOS      3//CH1_BUTTON_XPOS-157
#define CH2_MENU_XPOS      3//CH2_BUTTON_XPOS-157

#define CH_MENU_YPOS                        46
#define CH_MENU_WIDTH                      418//416//334+82
#define CH_MENU_HEIGHT                     338

#define CH1_TOUCHED_COLOR            0x000000FF
#define CH2_TOUCHED_COLOR            0x00FF0000

//----------------------------------------------------------------------------------------------------------------------------------
//Channel REF menu

#define CH_REF_MENU_XPOS        CH_MENU_WIDTH + 2

#define CH_REF_MENU_YPOS         46
#define CH_REF_MENU_WIDTH       277//268//140//100//
#define CH_REF_MENU_HEIGHT      236//338

//----------------------------------------------------------------------------------------------------------------------------------
//Channel MATH menu

#define CH_MATH_MENU_XPOS        CH_MENU_WIDTH + 2

#define CH_MATH_MENU_YPOS        92//46
#define CH_MATH_MENU_WIDTH      171//172//140//72//100//
#define CH_MATH_MENU_HEIGHT     236//338

//----------------------------------------------------------------------------------------------------------------------------------
//channel voltage per division buttons

#define CH1_VOLT_DIV_MENU_YPOS              99
#define CH2_VOLT_DIV_MENU_YPOS             264

//----------------------------------------------------------------------------------------------------------------------------------
// Acquisition button and menu

#define ACQ_BUTTON_XPOS                    388
#define ACQ_BUTTON_YPOS                      5
#define ACQ_BUTTON_WIDTH                    30
#define ACQ_BUTTON_HEIGHT                   35

#define ACQ_BUTTON_BG_WIDTH                103
#define ACQ_BUTTON_BG_HEIGHT                35

#define ACQ_MENU_XPOS          ACQ_BUTTON_XPOS
#define ACQ_MENU_YPOS                       46
#define ACQ_MENU_WIDTH                     304
#define ACQ_MENU_HEIGHT                    386

//----------------------------------------------------------------------------------------------------------------------------------
//Move fast button

#define MOVE_FAST_BUTTON_XPOS       496
#define MOVE_FAST_BUTTON_YPOS         5
#define MOVE_FAST_BUTTON_WIDTH       44
#define MOVE_FAST_BUTTON_HEIGHT      35

//next button 545

//----------------------------------------------------------------------------------------------------------------------------------
// Trigger button and menu

#define TRIGGER_BUTTON_XPOS                    546//560
#define TRIGGER_BUTTON_YPOS                      5
#define TRIGGER_BUTTON_WIDTH                    31
#define TRIGGER_BUTTON_HEIGHT                   35

#define TRIGGER_BUTTON_BG_WIDTH                 80
#define TRIGGER_BUTTON_BG_HEIGHT                35

#define TRIGGER_MENU_XPOS      TRIGGER_BUTTON_XPOS-38//28
#define TRIGGER_MENU_YPOS                       46
#define TRIGGER_MENU_WIDTH                     220//210//166
#define TRIGGER_MENU_HEIGHT                    280//280//246//336

//----------------------------------------------------------------------------------------------------------------------------------
// Signal generator button and menu

#define GEN_BUTTON_XPOS                    630//560
#define GEN_BUTTON_YPOS                      5
#define GEN_BUTTON_WIDTH                    28
#define GEN_BUTTON_HEIGHT                   35

#define GEN_BUTTON_BG_WIDTH                 62//80
#define GEN_BUTTON_BG_HEIGHT                35

#define GEN_MENU_XPOS       GEN_BUTTON_XPOS-100//28
#define GEN_MENU_YPOS                       46
#define GEN_MENU_WIDTH                     220//210//166
#define GEN_MENU_HEIGHT                    280//280//246//336

//----------------------------------------------------------------------------------------------------------------------------------
// Battery button and menu

#define BATTERY_BUTTON_XPOS                    546//560
#define BATTERY_BUTTON_YPOS                      5
#define BATTERY_BUTTON_WIDTH                    31
#define BATTERY_BUTTON_HEIGHT                   35

#define BATTERY_BUTTON_BG_WIDTH                 80
#define BATTERY_BUTTON_BG_HEIGHT                35

#define BATTERY_MENU_XPOS      BATTERY_BUTTON_XPOS-38//28
#define BATTERY_MENU_YPOS                       46
#define BATTERY_MENU_WIDTH                     220//210//166
#define BATTERY_MENU_HEIGHT                    280//280//246//336

//----------------------------------------------------------------------------------------------------------------------------------
//Working window 2  (confirm progress etc....) 

#define WIN2_XPOS        300//280
#define WIN2_YPOS        177//185
#define WIN2_WIDTH       200//268//140//100//
#define WIN2_HEIGHT      70//135//125//110//338

//----------------------------------------------------------------------------------------------------------------------------------
// Sampling system

#define MAX_SAMPLE_BUFFER_SIZE            16382//3000//4095
#define UINT32_SAMPLE_BUFFER_SIZE         (MAX_SAMPLE_BUFFER_SIZE / 4)

#define SAMPLE_COUNT                      MAX_SAMPLE_BUFFER_SIZE //samplecount  10000
#define SAMPLES_PER_ADC                   (SAMPLE_COUNT / 2)        //nofsamples 5000

//----------------------------------------------------------------------------------------------------------------------------------
//Typedefs
//----------------------------------------------------------------------------------------------------------------------------------

typedef struct tagTouchCoords           TOUCHCOORDS,          *PTOUCHCOORDS;

typedef struct tagDisplayPoints         DISPLAYPOINTS,        *PDISPLAYPOINTS;

typedef struct tagChannelSettings       CHANNELSETTINGS,      *PCHANNELSETTINGS;
typedef struct tagScopeSettings         SCOPESETTINGS,        *PSCOPESETTINGS;

typedef struct tagThumbnailData         THUMBNAILDATA,        *PTHUMBNAILDATA;

typedef struct tagPathInfo              PATHINFO,             *PPATHINFO;

typedef struct tagScreenTimeCalcData    SCREENTIMECALCDATA,   *PSCREENTIMECALCDATA;
typedef struct tagVoltCalcData          VOLTCALCDATA,         *PVOLTCALCDATA;
typedef struct tagFreqCalcData          FREQCALCDATA,         *PFREQCALCDATA;
typedef struct tagTimeCalcData          TIMECALCDATA,         *PTIMECALCDATA;

//----------------------------------------------------------------------------------------------------------------------------------

typedef void (*MEASUREMENTFUNCTION)(PCHANNELSETTINGS settings);

//----------------------------------------------------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------------------------------------------------

struct tagTouchCoords
{
  uint16 x1;
  uint16 x2;
  uint16 y1;
  uint16 y2;
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagDisplayPoints
{
  uint16 x;
  uint16 y;  
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagChannelSettings
{
  //Settings
  uint8  enable;
  uint8  coupling;
  uint8  magnification;
  uint8  V_A;
  uint8  displayvoltperdiv;
  uint8  samplevoltperdiv;
  uint8  fftenable;
  uint8  invert;
  
  //Trace on screen position
  uint16 traceposition;
  uint16 triggerverticalposition;  //Screen position of the trigger level indicator

  //New setting for controlling the ground level of the ADC differential input
  //uint16 dcoffset;
  uint8  ADoverload;
  int16  dcoffset;
  
  //Inter ADC difference compensation
  int16  compensation;
  int16  adc1compensation;
  int16  adc2compensation;
  
  //DC offset calibration for center level of the ADC's
  uint16 dc_calibration_offset[7];
  
  //Input divider calibration values
  int32 input_calibration[7];
  
  //DC shift calibration value
  uint32 dc_shift_center;    //zero-center position on signal
  int32  dc_shift_size;      //size displayed signal
  int32  dc_shift_value;     //adjust measurement values
  
  //Measurements
  int32  minDC;
  int32  maxDC;
  int32  centerDC;
  int32  peakpeakDC;
  
  int32  min;
  int32  max;
  int32  average;
  int32  center;
  int32  peakpeak;
  uint32 rms;
  uint32 frequencyvalid;
  uint32 frequency;
  uint32 lowtime;
  uint32 hightime;
  uint32 periodtime;
  
  //Frequency determination work variables
  uint32 highlevel;
  uint32 lowlevel;
  uint32 state;
  uint32 zerocrossings;
  uint32 lowsamplecount;
  uint32 lowdivider;
  uint32 highsamplecount;
  uint32 highdivider;
  uint32 previousindex;
  
  //Auto ranging space
  uint32 maxscreenspace;
  
  //Calibration measurement data
  uint32 rawaverage;
  uint32 adc1rawaverage;
  uint32 adc2rawaverage;
  
  //Sample data
  uint8 *tracebuffer;
  uint8 *buffer;
  uint8 *tmp_tracebuffer;
  
  //Screen data
  PDISPLAYPOINTS tracepoints;
  uint32         noftracepoints;
  
  //Sample gathering options
  uint8 checkfirstadc;
  uint8 enabletrigger;
  
  //FPGA commands
  uint8 enablecommand;            //Needs to be set to 0x02 for channel 1 and 0x03 for channel 2
  uint8 couplingcommand;          //Needs to be set to 0x34 for channel 1 and 0x37 for channel 2
  uint8 voltperdivcommand;        //Needs to be set to 0x33 for channel 1 and 0x36 for channel 2
  uint8 offsetcommand;            //Needs to be set to 0x32 for channel 1 and 0x35 for channel 2
  uint8 adc1command;              //Needs to be set to 0x20 for channel 1 and 0x22 for channel 2
  uint8 adc2command;              //Needs to be set to 0x21 for channel 1 and 0x23 for channel 2
  uint8 averagecommand;           //Needs to be set to 0x24 for channel 1 and 0x26 for channel 2
  
  //Channel color
  uint32 color;
  
  //Channel button and menu
  uint32 buttonxpos;
  uint32 menuxpos;
  uint32 voltdivypos;
  uint32 touchedcolor;
  
  //long time base
  uint32    sample1;
  uint32    sample2;
    
  int8 *buttontext;
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagScopeSettings
{
  CHANNELSETTINGS channel1;
  CHANNELSETTINGS channel2;
  
  CHANNELSETTINGS ch_ref1;
  CHANNELSETTINGS ch_ref2;
  CHANNELSETTINGS ch_ref3;
  CHANNELSETTINGS ch_ref4;
  
  CHANNELSETTINGS ch_ref5;
  CHANNELSETTINGS ch_ref6;
  CHANNELSETTINGS ch_ref7;
  CHANNELSETTINGS ch_ref8;
  
  uint8  ref1;
  uint8  ref2;
  uint8  ref3;
  uint8  ref4;
  uint8  ref5;
  uint8  ref6;
  uint8  ref7;
  uint8  ref8;
 
  
  //Sample data
  uint8 *math_tracebuffer;  //for mathematic function 

  uint16 samplecount;       //Number of samples in trace buffer
  uint16 nofsamples;        //Number of samples to read from the FPGA
  
  uint8 samplerate;
  uint8 timeperdiv;         //for time base
  uint8 triggermode;        //0-auto mode, 1- single mode, 2-normal mode
  uint8 triggeredge;
  uint8 triggerchannel;
  
  int32  triggerhorizontalposition;     //uint16   //Position on screen of the trigger point in the signal displaying
  uint16 triggerverticalposition;       //Screen position of the trigger level indicator
  uint16 triggerlevel;                  //Actual trigger level set to the FPGA
  uint16 settriggerpoint;               //Set start triger point in FPGA
    uint16 totalsamples;               //Set start triger point in FPGA
  
  uint8 samplemode;                     //New for mode select in the fpga_do_conversion function
   
  uint32    count;          //long time base, buffer count
  uint32    xpos;           //long time base
  uint32    lastx;          //long time base
  
  uint8 movespeed;
  uint8 genmode;
  
  uint8 math;
    
  uint8 rightmenustate;
  uint8 waveviewmode;
  
  uint8 batterychargelevel;
  uint8 batterycharging;
  uint8 runstate;
  uint8 conversion_done;     //flag for conversion, 0-start, 1-conversion done
  uint8 display_data_done;   //flag data displayed, 1-for next conversion ready
  
  uint8 screenbrightness;
  uint8 gridbrightness;
  uint8 gridenable;
  uint8 alwaystrigger50;
  uint8 xymodedisplay;
  uint8 confirmationmode;
  uint8 long_mode;            //flag long_time base 1-active, 0-sort_time base
  uint8 average_mode;         //flag for average display data
  uint8 long_memory;          //flag for long memory. 1-long memory & long trigger
  
  uint8 new_autosetup;        //flag switch between old and 1-new autosetup
  uint8 maxlight;             //flag switch between user setting backlight or 1-max backlight
  uint8 ref_on_startup;       //flag on reference waveforms on startup, 1 active
  
  uint8 lockcursors;          //flag for lock move cursors
  uint8 timecursorsenable;
  uint8 voltcursorsenable;
  
  uint16 timecursor1position;
  uint16 timecursor2position;
  
  uint16 voltcursor1position;
  uint16 voltcursor2position;
  
  uint8 hide_values_CH1;       //Hidden display of selected measured values on the display
  uint8 hide_values_CH2;       //Hidden display of selected measured values on the display
  
  uint32 measures_color1;
  uint32 measures_color2;
  uint8 source1_measures;
  uint8 source2_measures;
  
  uint8 measuresstate[10][12];  //2-12, new ref1-8
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagThumbnailData
{
  int8  filename[33]; 
  uint8 channel1enable;
  uint8 channel2enable;
  uint8 channel1traceposition;
  uint8 channel2traceposition;
  uint8 triggerverticalposition;
  uint8 triggerchannel;  
  uint8 triggerhorizontalposition;
  uint8 xydisplaymode;
  uint8 long_mode;
  uint8 disp_xstart;
  uint8 disp_xend;
  uint8 channel1data[VIEW_ITEM_TRACE_POINTS];
  uint8 channel2data[VIEW_ITEM_TRACE_POINTS];
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagPathInfo
{
  char   *name;
  uint32  length;
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagScreenTimeCalcData
{
  uint32 mul_factor;
  uint8  time_scale;
  uint8  freq_scale;
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagVoltCalcData
{
  uint32 mul_factor;
  uint8  volt_scale;
};

//----------------------------------------------------------------------------------------------------------------------------------

struct tagFreqCalcData
{
  uint32 sample_rate;
  uint8  freq_scale;
};
        
//----------------------------------------------------------------------------------------------------------------------------------

struct tagTimeCalcData
{
  uint32 mul_factor;
  uint8  time_scale;
};
        
//----------------------------------------------------------------------------------------------------------------------------------
//Linker variables
//----------------------------------------------------------------------------------------------------------------------------------

extern uint8 BSS_START;
extern uint8 BSS_END;

//----------------------------------------------------------------------------------------------------------------------------------
//Timer data
//----------------------------------------------------------------------------------------------------------------------------------

extern volatile uint32 timer0ticks;
extern volatile uint16   timerRTC;
//extern volatile uint32   timerHOLD;
//extern volatile uint32   timerH;

//----------------------------------------------------------------------------------------------------------------------------------
//CH340 serial data
//----------------------------------------------------------------------------------------------------------------------------------
extern volatile uint16  curchar;

//----------------------------------------------------------------------------------------------------------------------------------
//Touch data
//----------------------------------------------------------------------------------------------------------------------------------

#ifndef USE_TP_CONFIG
extern uint8 tp_config_data[186];

extern uint32 xscaler;
extern uint32 yscaler;
#endif

extern uint8  havetouch;
extern uint16 xtouch;
extern uint16 ytouch;

extern uint16 xtouch_tmp;       //len pre vyvojove ucely
extern uint16 ytouch_tmp;       //len pre vyvojove ucely

extern uint8 xswap;
extern uint8 yswap;

extern uint8 config_valid;

extern uint8 boot_menu_start;

//----------------------------------------------------------------------------------------------------------------------------------
//RTC DS3231 data
//----------------------------------------------------------------------------------------------------------------------------------

extern uint8 hour;
extern uint8 minute;
extern uint8 sec;
extern uint8 day;
extern uint8 month;
extern uint8 year;

extern char     buffertime[9];
extern char     filenameRTC[32];

extern uint8    onoffRTC;           //1-RTC on, time stamp for file and thumbnail
extern uint8    tag_in_BMP;         //1-time stamp or name file in picture BMP
/*
extern  int32    offset;
extern  int32    max1;
extern  int32    min1;    
extern  int32    p2p1; 
extern  int32    center1x;
*/
//----------------------------------------------------------------------------------------------------------------------------------
//State machine data
//----------------------------------------------------------------------------------------------------------------------------------

extern uint8 mounted_to_PC; //flag DSO (MSC) is connected and mounted to PC

extern uint16 previousxtouch;
extern uint16 previousytouch;

extern uint16 xtouchdisplacement;
extern uint16 ytouchdisplacement;

extern uint16 maxdisplacement;

extern uint8 touchstate;

extern uint32 previoustimerticks;

extern uint8 systemsettingsmenuopen;
extern uint8 screenbrightnessopen;
extern uint8 gridbrightnessopen;
extern uint8 othersettingsopen;
extern uint8 calibrationopen;
extern uint8 RTCsettingsopen;

extern uint8 diagnosticmenuopen;

extern uint8 channelmenuopen;
extern uint8 refmenuopen;
extern uint8 mathmenuopen;
extern uint8 refmode;
extern uint8 mathmode;
extern uint8 channelA;
extern uint8 channelB;

extern uint8 calibrationfail;
extern uint8 triggerlong;
extern uint8 trigger50;
extern uint8 restore;


extern uint8 dc_shift_cal;
extern uint8 reload_cal_data;

extern uint8 USB_CH340; 
extern uint8 dev_mode;

/*
 extern uint8 ref1_sample;
extern uint8 ref2_sample;
extern uint8 ref3_sample;
extern uint8 ref4_sample;
 */
extern uint8 math_sample;

//extern uint8 ref_ch2_sample;

//extern uint8 tmp_ACQ_mode; 
//extern uint8 long_timebase;

//----------------------------------------------------------------------------------------------------------------------------------
//Scope data
//----------------------------------------------------------------------------------------------------------------------------------

extern FATFS fs;

extern SCOPESETTINGS scopesettings;

extern CHANNELSETTINGS calibrationsettings;

extern SCOPESETTINGS savedscopesettings1;
extern SCOPESETTINGS savedscopesettings2;

extern uint32 channel1tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern uint32 channel1tracebufferAVG[MAX_SAMPLE_BUFFER_SIZE];

//extern uint32 channel1_ref1_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel1_ref2_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel1_ref3_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel1_ref4_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

//extern uint32 channel1_tmp_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern DISPLAYPOINTS channel1pointsbuffer[730];
//-------------------------------------------------------------
extern uint32 channel2tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern uint32 channel2tracebufferAVG[MAX_SAMPLE_BUFFER_SIZE];

//extern uint32 channel2_ref1_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel2_ref2_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel2_ref3_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
//extern uint32 channel2_ref4_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

//extern uint32 channel2_tmp_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern DISPLAYPOINTS channel2pointsbuffer[730];
//-------------------------------------------------------------

extern uint32 ref1_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref2_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref3_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref4_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern uint32 ref5_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref6_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref7_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];
extern uint32 ref8_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern DISPLAYPOINTS ref1pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref2pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref3pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref4pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display

extern DISPLAYPOINTS ref5pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref6pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref7pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display
extern DISPLAYPOINTS ref8pointsbuffer[730];      //Buffer to store the x,y positions of the trace on the display

extern uint32 math_channel_tracebuffer[UINT32_SAMPLE_BUFFER_SIZE];

extern uint16 thumbnailtracedata[730]; 

extern uint16 settingsworkbuffer[256];

//New variables for trace displaying
extern double disp_xpos_per_sample;
extern double disp_sample_step;

extern int32 disp_first_sample;

extern uint32 AVG_trigger;
extern uint32 AVG_trigger1;
extern uint32 disp_have_trigger;
extern uint32 disp_trigger_index;

extern int32 disp_xstart;
extern int32 disp_xend;

//----------------------------------------------------------------------------------------------------------------------------------
//Distances of touch point to traces and cursors
//----------------------------------------------------------------------------------------------------------------------------------

extern uint16 distance_channel_1;
extern uint16 distance_channel_2;

extern uint16 distance_trigger_level;

extern uint16 distance_time_cursor_left;
extern uint16 distance_time_cursor_right;

extern uint16 distance_volt_cursor_top;
extern uint16 distance_volt_cursor_bottom;

extern uint16 distance_ch_ref_1;
extern uint16 distance_ch_ref_2;
extern uint16 distance_ch_ref_3;
extern uint16 distance_ch_ref_4;
extern uint16 distance_ch_ref_5;
extern uint16 distance_ch_ref_6;
extern uint16 distance_ch_ref_7;
extern uint16 distance_ch_ref_8;

//----------------------------------------------------------------------------------------------------------------------------------
//Previous trace and cursor settings
//----------------------------------------------------------------------------------------------------------------------------------

extern uint16 previous_channel_1_offset;
extern uint16 previous_channel_2_offset;

extern uint16 previous_trigger_level_offset;

extern int32 previous_trigger_point_position;//uint16

extern uint16 previous_left_time_cursor_position;
extern uint16 previous_right_time_cursor_position;

extern uint16 previous_top_volt_cursor_position;
extern uint16 previous_bottom_volt_cursor_position;

extern uint16 previous_ch_ref1_position;
extern uint16 previous_ch_ref2_position;
extern uint16 previous_ch_ref3_position;
extern uint16 previous_ch_ref4_position;
extern uint16 previous_ch_ref5_position;
extern uint16 previous_ch_ref6_position;
extern uint16 previous_ch_ref7_position;
extern uint16 previous_ch_ref8_position;

//----------------------------------------------------------------------------------------------------------------------------------
//Calibration data
//----------------------------------------------------------------------------------------------------------------------------------

extern uint32 samplerateindex;

//Average data for calibration calculations
extern uint32 samplerateaverage[2][6];

//Single ADC bit dc offset step per input sensitivity setting
extern uint32 sampleratedcoffsetstep[2][6];

//----------------------------------------------------------------------------------------------------------------------------------
//Predefined data
//----------------------------------------------------------------------------------------------------------------------------------

extern const int8 *volt_div_texts[7][7];//3-7
extern const int16 volt_div_texts_x_offsets[7][7];//21

//extern const int8 *volt_div_texts[7][12];//3-7
//extern const int16 volt_div_texts_x_offsets[7][12];//21

extern const int8 *ampere_div_texts[7][7];
//extern const int8 ampere_div_texts_x_offsets[49];

//extern const uint32 timebase_settings[24];
extern const uint32 timebase_settings[35];//35 33

extern const uint32 sample_rate_settings[29];//18

extern const float sample_time_converters[18];

//extern const uint32 time_per_div_matching[33];
extern const uint32 time_per_div_matching[24];

extern const uint32 samplerate_for_autosetup[4];//4

extern const SCREENTIMECALCDATA screen_time_calc_data[35];

//extern const SCREENTIMECALCDATA screen_time_calc_data[24];

extern const VOLTCALCDATA volt_calc_data[7][7];//3-7

extern const int32 signal_adjusters[7];

extern const int32 multiply; 
//extern int32 multiply; 

extern const FREQCALCDATA freq_calc_data[29];//18

extern const TIMECALCDATA time_calc_data[29];//18

extern const char *magnitude_scaler[8];

extern const int32 vertical_scaling_factors[7][7];

extern const PATHINFO view_file_path[3];
extern const char     view_file_extension[3][5];
extern const char    *thumbnail_file_names[3];

extern const uint8 bmpheader[PICTURE_HEADER_SIZE];

extern const uint32 frequency_per_div[35];//33
//extern const uint32 frequency_per_div[24];

extern const uint32 sample_rate[29];//18 25 27

//extern const uint8 time_per_div_sample_rate[24];
extern const uint8 time_per_div_sample_rate[35];//33
extern const uint8 sample_rate_time_per_div[18];//18
//extern const uint8 viable_time_per_div[18][24];
extern const uint8 viable_time_per_div[29][35];//18 a 33

//extern const int8 *L_time_div_texts[9];

//extern const int8 *time_div_texts[24];
//extern const int8 time_div_text_x_offsets[24];

extern const int8 *time_div_texts[35];//33
extern const int8 time_div_text_x_offsets[35];//33

extern const int8 *acquisition_speed_texts[29];//18
extern const int8 acquisition_speed_text_x_offsets[29];//18

extern const int8 *keyboard_texts[20];//18
extern const int8 keyboard_texts_x_offsets[20];//18

extern const char *measurement_names[12];

//----------------------------------------------------------------------------------------------------------------------------------
//For touch filtering on slider movement
//----------------------------------------------------------------------------------------------------------------------------------

extern uint16 prevxtouch;

//----------------------------------------------------------------------------------------------------------------------------------
//Data for picture and waveform view mode
//----------------------------------------------------------------------------------------------------------------------------------

extern FIL     viewfp;
extern DIR     viewdir;
extern FILINFO viewfileinfo;

extern char viewfilename[33];//32 v0.26r4 change

extern uint8 viewactive;

extern uint8 viewtype;

extern uint8 viewselectmode;
extern uint8 viewpage;
extern uint8 viewpages;
extern uint8 viewitemsonpage;

extern uint8 viewbottommenustate;

extern uint16 viewcurrentindex;

extern uint16 viewavailableitems;

extern uint8 viewitemselected[VIEW_ITEMS_PER_PAGE];

extern THUMBNAILDATA viewthumbnaildata[VIEW_MAX_ITEMS];

extern uint16 viewfilenumberdata[VIEW_MAX_ITEMS];

extern uint8 viewbitmapheader[PICTURE_HEADER_SIZE];

extern uint32 viewfilesetupdata[VIEW_NUMBER_OF_SETTINGS];

//----------------------------------------------------------------------------------------------------------------------------------
//Display data
//----------------------------------------------------------------------------------------------------------------------------------

//This first buffer is defined as 32 bits to be able to write it to file
extern uint32 maindisplaybuffer[SCREEN_SIZE / 2];

extern uint16 displaybuffer1[SCREEN_SIZE];
extern uint16 displaybuffer2[SCREEN_SIZE];
extern uint16 displaybuffer3[SCREEN_SIZE];
extern uint16 displaybuffertmp[SCREEN_SIZE];

extern uint16 gradientbuffer[SCREEN_HEIGHT];

extern char measurementtext[20];

//----------------------------------------------------------------------------------------------------------------------------------
//Fonts
//----------------------------------------------------------------------------------------------------------------------------------

extern FONTDATA font_0;
extern FONTDATA font_2;
extern FONTDATA font_3;
extern FONTDATA font_4;
extern FONTDATA font_5;

//----------------------------------------------------------------------------------------------------------------------------------
//Icons
//----------------------------------------------------------------------------------------------------------------------------------

extern const uint8 system_settings_icon[];
extern const uint8 Other_settings_icon[];
extern const uint8 Right_drop_menu_icon[];
//extern const uint8 Right_drop_menu_icon1[];
extern const uint8 Generator_menu_icon[];
//extern const uint8 Generator_menu_icon1[];
extern const uint8 picture_view_icon[];
extern const uint8 waveform_view_icon[];
extern const uint8 diagnostic_view_icon[];
extern const uint8 usb_icon[];
extern const uint8 screen_brightness_icon[];
extern const uint8 light_icon[];
extern const uint8 grid_brightness_icon[];
extern const uint8 trigger_50_percent_icon[];
extern const uint8 baseline_calibration_icon[];
extern const uint8 x_y_mode_display_icon[];
extern const uint8 confirmation_icon[];
extern const uint8 RTC_icon[];
extern const uint8 Left_icon[];
extern const uint8 Right_icon[];
extern const uint8 return_arrow_icon[];
extern const uint8 left_REF_pointer_icon[];
extern const uint8 left_pointer_icon[];
extern const uint8 right_pointer_icon[];
extern const uint8 top_pointer_icon[];
extern const uint8 select_sign_icon[];
extern const uint8 waste_bin_icon[];
extern const uint8 previous_picture_icon[];
extern const uint8 next_picture_icon[];

//----------------------------------------------------------------------------------------------------------------------------------

extern const uint8 letter_c_icon[];
extern const uint8 letter_e_icon[];
extern const uint8 letter_o_icon[];
extern const uint8 letter_p_icon[];
extern const uint8 letter_s_icon[];

//----------------------------------------------------------------------------------------------------------------------------------

#endif /* VARIABLES_H */

