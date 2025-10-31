/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   ref_and_math.h
 * Author: dk
 *
 * Created on April 29, 2025, 10:52 AM
 */


void display_REFx_data(void);
void copy_ch_to_ref(CHANNELSETTINGS *dest, const CHANNELSETTINGS *src, uint32 *tracebuffer);

void display_MATHx_data(void);

uint8* get_channel_data(int channel);
void compute_all_math_ops(int channelA, int channelB);

//for save and load file
void scope_load_ALLREF_file(void);
void scope_load_REFx_item_file(uint8 number);
void scope_save_REFx_item_file(uint8 number);
void scope_restore_setup_from_REFx_file(uint8 number);
void scope_prepare_setup_for_REFx_file(uint8 number);


