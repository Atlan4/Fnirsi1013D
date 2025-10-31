//----------------------------------------------------------------------------------------------------------------------------------

#ifndef FNIRSI_1013D_SCOPE_H
#define FNIRSI_1013D_SCOPE_H

//----------------------------------------------------------------------------------------------------------------------------------

#include "types.h"

//----------------------------------------------------------------------------------------------------------------------------------

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   480

#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

//----------------------------------------------------------------------------------------------------------------------------------

#define CHANNEL1_COLOR         0x00FFFF00   //YELLOW_COLOR
#define REF1_COLOR           0x00FFA500   //ORANGE_COLOR
#define REF2_COLOR           0x00E9967A  //_COLOR
#define REF3_COLOR           0x00FF6347   //_COLOR
#define REF4_COLOR           0x00FF4500  //_COLOR
#define CHANNEL2_COLOR         0x0000FFFF   //BLUE_COLOR
#define REF5_COLOR           0x0087CEFA   //BLUE_COLOR
#define REF6_COLOR           0x001E90FF   //_COLOR
#define REF7_COLOR           0x000000FF   //_COLOR
#define REF8_COLOR           0x00191970   //_COLOR
#define TRIGGER_COLOR          0x0000FF00   //GREEN_COLOR
#define MATH_COLOR             0x00FF00FF 


#define CHANNEL1_TRIG_COLOR    0x00CCCC00   //  
#define CHANNEL2_TRIG_COLOR    0x0000CCCC   //
#define EXTERN_TRIG_COLOR      0x00FF8000

#define XYMODE_COLOR           0x00FF00FF   //MAGENTA_COLOR

#define CURSORS_COLOR          0x0000AA11   //

#define ITEM_ACTIVE_COLOR      0x00EF9311   //

#define BLACK_COLOR            0x00000000   //*
#define DARKGREY_COLOR         0x00181818   //*
#define LIGHTGREY_COLOR        0x00333333   //* lebo dark?
#define MIDLEGREY_COLOR        0x00383838   //*
#define GREY_COLOR             0x00444444   //* 
#define LIGHTGREY1_COLOR       0x00606060   //* 
#define LIGHTGREY2_COLOR       0x00BBBBBB   //* 
#define WHITE_COLOR            0x00FFFFFF   //*
#define LIGHTRED_COLOR         0x00FF4444   //*
#define RED_COLOR              0x00FF0000   //*
#define YELLOW_COLOR           0x00FFFF00   //*
#define PALEYELLOW_COLOR       0x00FFFF80   //*
#define DARKGREEN_COLOR        0x0000BB00   //*
#define GREEN_COLOR            0x0000FF00   //*
//#define LIGHTBLUE_COLOR        0x006666FF   //*
#define BLUE_COLOR             0x0000FFFF   //*
#define LDARKBLUE_COLOR        0x006666FF   //*
#define DARKBLUE_COLOR         0x00000078   //* 55
#define ORANGE_COLOR           0x00FF8000   //*
#define MAGENTA_COLOR          0x00FF00FF   //*

      //Light gray for the buttons
      //display_set_fg_color(0x00303030);

//----------------------------------------------------------------------------------------------------------------------------------

#define TOUCH_STATE_INACTIVE                 0x00
#define TOUCH_STATE_HAVE_DISPLACEMENT        0x01
#define TOUCH_STATE_X_Y_MODE                 0x02
#define TOUCH_STATE_MOVE_CHANNEL_1           0x03
#define TOUCH_STATE_MOVE_CHANNEL_2           0x04
#define TOUCH_STATE_MOVE_TRIGGER_LEVEL       0x05
#define TOUCH_STATE_MOVE_TIME_CURSOR_LEFT    0x06
#define TOUCH_STATE_MOVE_TIME_CURSOR_RIGHT   0x07
#define TOUCH_STATE_MOVE_VOLT_CURSOR_TOP     0x08
#define TOUCH_STATE_MOVE_VOLT_CURSOR_BOTTOM  0x09
#define TOUCH_STATE_MOVE_REF1                0x0A
#define TOUCH_STATE_MOVE_REF2                0x0B
#define TOUCH_STATE_MOVE_REF3                0x0C
#define TOUCH_STATE_MOVE_REF4                0x0D
#define TOUCH_STATE_MOVE_REF5                0x0E
#define TOUCH_STATE_MOVE_REF6                0x0F
#define TOUCH_STATE_MOVE_REF7                0x10
#define TOUCH_STATE_MOVE_REF8                0x11

#define TOUCH_STATE_MASK                     0x1F//0x0F

#define TOUCH_STATE_MOVE_TRIGGER_POINT       0x80

//----------------------------------------------------------------------------------------------------------------------------------

#endif /* FNIRSI_1013D_SCOPE_H */

