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
#define CHANNEL2_COLOR         0x0000FFFF   //BLUE_COLOR
#define TRIGGER_COLOR          0x0000FF00   //GREEN_COLOR

#define CHANNEL1_TRIG_COLOR    0x00CCCC00   //  
#define CHANNEL2_TRIG_COLOR    0x0000CCCC   //

#define XYMODE_COLOR           0x00FF00FF   //MAGENTA_COLOR

#define CURSORS_COLOR          0x0000AA11   //

#define ITEM_ACTIVE_COLOR      0x00EF9311   //

#define BLACK_COLOR            0x00000000   //*
#define DARKGREY_COLOR         0x00181818   //*
#define LIGHTGREY_COLOR        0x00333333   //* lebo dark?
//#define LIGHTGREY1_COLOR       0x00444444   //* od drak ?
#define GREY_COLOR             0x00444444   //zisiti kod farby
#define WHITE_COLOR            0x00FFFFFF   //*
#define RED_COLOR              0x00FF0000   //*
#define YELLOW_COLOR           0x00FFFF00   //*
#define GREEN_COLOR            0x0000FF00   //*
#define BLUE_COLOR             0x0000FFFF   //*
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

#define TOUCH_STATE_MASK                     0x0F

#define TOUCH_STATE_MOVE_TRIGGER_POINT       0x80

//----------------------------------------------------------------------------------------------------------------------------------

#endif /* FNIRSI_1013D_SCOPE_H */

