/* 
 * File:   PC_interface.h
 * Author: Atlan
 *
 * Created on November 2, 2025, 2:52 PM
 */

#ifndef PC_INTERFACE_H
#define PC_INTERFACE_H

//******************************************************************************
//extern sysparam SysParam;
#pragma pack(push,1)
typedef struct {
    uint8 run_state;          // [0]  STOP(0) RUN(1) SINGLE SHOT WAIT (2)
    uint8 reserved1;          // [1]  (unknown)
    uint8 ch2_enabled;             // [2]  CH2 OFF(0) ON(1)

    uint8 reserved2[17];      // [3..19]

    uint8 time_zoom;          // [20] Time base zoom (2=5ns ... 30=10s)

    uint8 reserved3[35];      // [21..55]

    //uint32 horiz_position;    // [56..58] (little endian, use only 3 bytes) 
                                // range 50000šC2350000, center=1200000
    
    uint8 horiz_position[3];  // [56..58] !!! manuálne 3 bajty

    uint8 reserved4[5];       // [59..63]

    uint8 trigger_channel;    // [64] Trigger CH1(0) CH2(1)
    uint8 trigger_mode;       // [65] AUTO(0) NORMAL(1)
    uint8 trigger_edge;       // [66] UP(0) DOWN(1)
    uint8 trigger_source;     // [67] AUTO(0) MANUAL(1)

    uint8 reserved5[4];       // [68..71]

    uint8 trigger_level;      // [72] Trigger level (0-255, center=128)

    uint8 reserved6;          // [73]

    uint8 ch1_volt_zoom;      // [74] CH1 volts zoom (4šC13)

    uint8 reserved7[5];       // [75..79]

    int8 ch1_vert_position[2];  // [80..81]

    uint8 reserved8[34];      // [82..115]

    uint8 ch1_coupling;       // [116] DC(0) AC(1)
    uint8 ch1_probe;          // [117] 1x(0) 10x(1) 100x(2)

    uint8 reserved9[22];      // [118..139]

    uint8 ch2_volt_zoom;      // [140] CH2 volts zoom (4šC13)

    uint8 reserved10[7];      // [141..147]

    int8 ch2_vert_position[2];  // [148..149]

    uint8 reserved11[30];     // [150..179]

    uint8 ch2_coupling;       // [180] DC(0) AC(1)
    uint8 ch2_probe;          // [181] 1x(0) 10x(1) 100x(2)

    uint8 reserved12[67];     // [182..248]

    uint8 xy_mode;            // [249] XY mode OFF(0) ON(1)
    uint8 signal_gen;         // [250] Generator O#pragma pack(push, 1)FF(0) ON(1)

    uint8 reserved13[33];     // [251..283]

    uint8 selected_channel;   // [284] CH1(0) CH2(1)
    uint8 trigger_edit;       // [286] Trigger edit OFF(0) ON(1)
    uint8 menu_flags;         // [287] Menu flags (0=no menu, 1=normal, 4=gen, 8=50%)

    uint8 reserved14[32];     // [288..319] unused padding
} sysparam;
#pragma pack(pop)

//sysparam SysParam;


//------------------------------------------------------------------------------

void ini_SysParam(void);

void usb_CDC_send_data(uint8 *data, uint32 length);
void usb_CDC_process_rx(void);

void send_hex_byte(uint8 b);


const char *my_strstr(const char *haystack, const char *needle);
int my_strcmp(const char *s1, const char *s2);
char *my_strrchr(const char *s, int c);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, int n);
int my_strlen(const char *s);
void my_sprintf_path(char *dst, const char *folder, const char *fname);
int my_strcasecmp(const char *s1, const char *s2);

int my_strncasecmp(const char *s1, const char *s2, int n);


//******************************************************************************

#endif /* PC_INTERFACE_H */
