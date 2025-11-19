/* 
 * File:   PC_interface.c
 * Author: Atlan
 * 
 * Created on November 2, 2025, 2:52 PM
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
#include "sd_card_interface.h"
#include "display_lib.h"
#include "ff.h"
#include "DS3231.h"

#include "usb_interface.h"
#include "cdc_class.h"
#include "variables.h"

#include "PC_interface.h"

//#include "sin_cos_math.h"

//sinx
//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>

//endsinx

//#include <stdint.h>
//#include <stddef.h>

//#include <string.h>

/* Buffers and indices */
extern uint8 usb_rx[1024];
extern volatile uint32 usb_rx_in_idx;
extern volatile uint32 usb_rx_out_idx;

extern uint8 usb_tx[32768];
extern volatile uint32 usb_tx_in_idx;
extern volatile uint32 usb_tx_out_idx;

//------------------------------------------------------------------------------
sysparam SysParam;

//******************************************************************************
void ini_SysParam(void)
{
    uint8 i = 0;

    // Vynuluj celú štruktúru pre istotu
    memset(&SysParam, 0, sizeof(SysParam));

    // --- Základné systémové stavy ---
    SysParam.run_state       = 1;     // RUN (0=STOP,1=RUN,2=SINGLE/WAIT)
    SysParam.reserved1       = 0;
    SysParam.ch2_enabled     = 1;     // CH2 zapnutý (0=OFF,1=ON)

    for (i = 0; i < 17; i++) SysParam.reserved2[i] = 0;

    SysParam.time_zoom       = 16;    // Time base zoom (2=5ns ... 30=10s)

    for (i = 0; i < 35; i++) SysParam.reserved3[i] = 0;

    // --- Horizontálna pozícia (3 bajty little-endian, stred = 0x00124F80 = 1200000) ---
    SysParam.horiz_position[0] = 0x80;
    SysParam.horiz_position[1] = 0x4F;
    SysParam.horiz_position[2] = 0x12;

    for (i = 0; i < 5; i++) SysParam.reserved4[i] = 0;

    // --- Trigger ---
    SysParam.trigger_channel = 0;     // CH1 trigger
    SysParam.trigger_mode    = 0;     // AUTO
    SysParam.trigger_edge    = 0;     // UP
    SysParam.trigger_source  = 0;     // AUTO
    //for (i = 0; i < 4; i++) SysParam.reserved5[i] = 0;

    SysParam.trigger_level   = 128;   // Stredná úroveň
    SysParam.reserved6       = 0;

    // --- CH1 nastavenia ---
    SysParam.ch1_volt_zoom   = 10;    // 10 = cca 0.5V/div (4–13)
    //for (i = 0; i < 5; i++) SysParam.reserved7[i] = 0;

    SysParam.ch1_vert_position[0] = 0;
    SysParam.ch1_vert_position[1] = 0;

    //for (i = 0; i < 34; i++) SysParam.reserved8[i] = 0;

    SysParam.ch1_coupling    = 0;     // DC
    SysParam.ch1_probe       = 0;     // 1x

   // for (i = 0; i < 22; i++) SysParam.reserved9[i] = 0;

    // --- CH2 nastavenia ---
    SysParam.ch2_volt_zoom   = 10;    // 10 = rovnaký zoom ako CH1
   // for (i = 0; i < 7; i++) SysParam.reserved10[i] = 0;

    //SysParam.ch2_vert_position = 0;   // stred obrazovky
    SysParam.ch1_vert_position[0] = 0;
    SysParam.ch1_vert_position[1] = 0;
    
    //for (i = 0; i < 30; i++) SysParam.reserved11[i] = 0;

    SysParam.ch2_coupling    = 0;     // DC
    SysParam.ch2_probe       = 0;     // 1x

    //for (i = 0; i < 67; i++) SysParam.reserved12[i] = 0;

    // --- Doplnkové režimy ---
    SysParam.xy_mode         = 0;     // XY mode OFF
    SysParam.signal_gen      = 0;     // Signal generator OFF

    //for (i = 0; i < 33; i++) SysParam.reserved13[i] = 0;

    // --- Menu / výber kanála ---
    SysParam.selected_channel = 0;    // CH1
    SysParam.trigger_edit     = 0;    // OFF
    SysParam.menu_flags       = 1;    // normal

    //for (i = 0; i < 32; i++) SysParam.reserved14[i] = 0;
}
/*

void ini_SysParam(void) 
{
  uint8 i = 0;
  
  //﻿Inicializácia SysParam so všetkými reserved na 0 a predvolenými hodnotami
  SysParam.run_state = 1;         //STOP(0), RUN(1), SINGLE SHOT WAIT(2)      [uint8]
  SysParam.reserved1         = 0;        // reserved byte                             [uint8]
  SysParam.ch2_enabled            = 0;        // CH2 OFF(0), ON(1)                         [uint8]

 for( i=0; i<17; i++) SysParam.reserved2[i] = 0;// [uint8]

  SysParam.time_zoom         = 16;       // Time base zoom (2=5ns ... 30=10s)        [uint8]

  for( i=0; i<35; i++) SysParam.reserved3[i] = 0;   // [uint8]

  //SysParam.horiz_position    = 1200000;  // Horizontal position, center               [uint32_t]
  SysParam.horiz_position[0]    = 0x80;
  SysParam.horiz_position[1]    = 0x4F;
  SysParam.horiz_position[2]    = 0x12;

  for( i=0; i<5; i++) SysParam.reserved4[i] = 0;    // [uint8]

  SysParam.trigger_channel   = 0;        // Trigger CH1(0), CH2(1)                     [uint8]
  SysParam.trigger_mode      = 0;        // AUTO(0), NORMAL(1)                         [uint8]
  SysParam.trigger_edge      = 0;        // UP(0), DOWN(1)                             [uint8]
  SysParam.trigger_source    = 0;        // AUTO(0), MANUAL(1)                         [uint8]

  for( i=0; i<4; i++) SysParam.reserved5[i] = 0;    // [uint8]

  SysParam.trigger_level     = 128;      // Trigger level (0–255, center=128)         [uint8]
  SysParam.reserved6         = 0;        //[uint8]
  SysParam.ch1_volt_zoom     = 10;        // CH1 volts zoom (4–13)                      [uint8]

  for( i=0; i<5; i++) SysParam.reserved7[i] = 0;    // [uint8]

  SysParam.ch1_vert_position[0] = 0;        // CH1 vertical position                       [int16_t]
  SysParam.ch1_vert_position[1] = 0;

  for( i=0; i<34; i++) SysParam.reserved8[i] = 0;   //[uint8]

  SysParam.ch1_coupling      = 0;        //CH1 DC(0), AC(1)                           [uint8]
  SysParam.ch1_probe         = 0;        // CH1 probe 1x(0), 10x(1), 100x(2)         [uint8]

  for( i=0; i<22; i++) SysParam.reserved9[i] = 0;   //[uint8]

  for( i=0; i<33; i++) SysParam.reserved13[i] = 0;  // [uint8]

  SysParam.selected_channel  = 0;        // Selected channel CH1(0), CH2(1)           [uint8]
  SysParam.trigger_edit      = 0;        // Trigger edit OFF(0), ON(1)                 [uint8]
  SysParam.menu_flags        = 1;        // Menu flags: 0=no menu, 1=normal, 4=gen, 8=50% [uint8]

  for( i=0; i<32; i++) SysParam.reserved14[i] = 0;  // [uint8]
}
*/
//******************************************************************************



//extern sysparam SysParam;
/*
void usb_CDC_send_data(uint8 *data, uint32 length)
{
    if (!usb_connect || data == NULL || length == 0)
        return;

    uint32 i = 0;

    while (i < length)
    {
        // vypočítaj počet voľných bajtov v TX buffri
        uint32 free_space = circular_buffer_free(usb_tx_in_idx, usb_tx_out_idx, sizeof(usb_tx));

        // ak nie je miesto, spusti odosielanie a čakaj
        if (free_space == 0)
        {
            usb_CDC_in_ep_callback();  // odošli čo sa dá
            continue;                  // počkaj, kým sa buffer uvoľní
        }

        // koľko bajtov môžeme teraz zapísať
        uint32 to_copy = length - i;
        if (to_copy > free_space)
            to_copy = free_space;

        // zapíš dáta do TX kruhového bufferu
        for (uint32 n = 0; n < to_copy; n++)
        {
            usb_tx[usb_tx_in_idx++] = data[i++];
            usb_tx_in_idx %= sizeof(usb_tx);
        }

        // spusti prenos
        usb_CDC_in_ep_callback();
    }
*/
    // po

    /*
     // Pomocná funkcia na odoslanie dát cez TX buffer
void usb_CDC_send_data(uint8 *data, uint32 length)
{
    for(uint32 i = 0; i < length; i++)
    {
        uint32 next = (usb_tx_in_idx + 1) % sizeof(usb_tx);
        if(next == usb_tx_out_idx)
            break; // buffer full
        usb_tx[usb_tx_in_idx] = data[i];
        usb_tx_in_idx = next;
    }
}
*/

// Spracovanie prijatých dát z USB (vola sa napr. v hlavnej slučke)
void usb_CDC_process_rx(void)
{
    static char cmd_buf[16];
    static uint8 idx = 0;

    while(usb_rx_out_idx != usb_rx_in_idx)
    {
        char c = usb_rx[usb_rx_out_idx++];
        usb_rx_out_idx %= sizeof(usb_rx);

        if(c == '\n' || c == '\r')
        {
            cmd_buf[idx] = 0;

            if(strcmp(cmd_buf, "#ECHO") == 0)
            {
               // usb_ch340_send_data((uint8*)&SysParam, sizeof(SysParam));
            }

            idx = 0; // reset command buffer
        }
        else if(idx < sizeof(cmd_buf) - 1)
        {
            cmd_buf[idx++] = c;
        }
    }
}

/*
 if(strcmp(cmd_buf, "#ECHO") == 0)
{
    char msg[64];
    sprintf(msg, "Run state: %d, CH2: %d, TimeZoom: %d\r\n",
            SysParam.run_state, SysParam.ch2_on, SysParam.time_zoom);
    usb_ch340_send_data((uint8*)msg, strlen(msg));
}
 */

/*
 
 void gini_Sys1Param(void) 
{
    display_set_fg_color(BLACK_COLOR);
    display_fill_rect(488, 420, 250, 50); // Vyčistí priestor na zobrazenie textu

    // Nastavenie farby textu a fontu
    display_set_fg_color(WHITE_COLOR);
    display_set_font(&font_0);

    // Zobrazenie textu
    display_text(488, 440, message);
    display_decimal(600, 440, value);
    display_text(495, 455, viewfilename);

    timer0_delay(3000); // Počká 3 sekundy
}
 */
//*********************************************
// prevod bajtu na dva hex znaky
/*
 void send_hex_byte(uint8 b)
{
    char out[2];
    const char hex[] = "0123456789ABCDEF";
    out[0] = hex[(b >> 4) & 0x0F];
    out[1] = hex[b & 0x0F];
    usb_ch340_send_data((uint8*)out, 2);
}
 */

void send_hex_byte(uint8 byte)
{
    char hex[3];
    const char hexchars[] = "0123456789ABCDEF";
    hex[0] = hexchars[(byte >> 4) & 0x0F];
    hex[1] = hexchars[byte & 0x0F];
    hex[2] = 0;
    usb_CDC_send_text(hex);
}


//******************************************************************************

//Simple strstr replacement: find substring within string
const char *my_strstr(const char *haystack, const char *needle)
{
  if (!*needle) return haystack; // Empty substring always matches

  const char *p1 = haystack;
  while (*p1)
  {
    const char *p1_begin = p1;
    const char *p2 = needle;

    // Compare characters
    while (*p1 && *p2 && (*p1 == *p2))
    {
      p1++;
      p2++;
    }

    if (!*p2) return p1_begin; // Found match

    p1 = p1_begin + 1; // Move to next position
  }

  return 0; // Not found
}

//-----------------------------------------------------------------

int my_strcmp(const char *s1, const char *s2)
{
  while (*s1 && (*s1 == *s2)) 
  {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *my_strrchr(const char *s, int c)
{
    const char *last = 0;
    do {
        if (*s == (char)c)
            last = s;
    } while (*s++);
    return (char*)last;
}

char *my_strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *my_strncpy(char *dest, const char *src, int n)
{
    int i;
    for (i = 0; i < n && src[i]; i++) dest[i] = src[i];
    for (; i < n; i++) dest[i] = '\0';
    return dest;
}

int my_strlen(const char *s)
{
    int len = 0;
    while (*s++) len++;
    return len;
}

// Simple sprintf for path joining (no formatting)
void my_sprintf_path(char *dst, const char *folder, const char *fname)
{
    while (*folder) *dst++ = *folder++;
    *dst++ = '/';
    while (*fname) *dst++ = *fname++;
    *dst = 0;
}
/*
// Case-insensitive compare
int my_strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        if (c1 != c2) return (unsigned char)c1 - (unsigned char)c2;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}
*/
// jednoduchá funkcia na porovnanie reťazcov bez ohľadu na veľkosť písmen
int my_strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        char c1 = (*s1 >= 'a' && *s1 <= 'z') ? *s1 - 32 : *s1;
        char c2 = (*s2 >= 'a' && *s2 <= 'z') ? *s2 - 32 : *s2;
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int my_strncasecmp(const char *s1, const char *s2, int n)
{
    for (int i = 0; i < n; i++) {
        char c1 = s1[i];
        char c2 = s2[i];
        if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
        if (c1 != c2 || c1 == '\0' || c2 == '\0')
            return (unsigned char)c1 - (unsigned char)c2;
    }
    return 0;
}


//******************************************************************************