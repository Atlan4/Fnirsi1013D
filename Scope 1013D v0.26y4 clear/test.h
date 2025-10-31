//----------------------------------------------------------------------------------------------------------------------------------

#include "fnirsi_1013d_scope.h"
#include "variables.h"

// Definovanie konštánt
#define PI 3.14159265358979323846
#define N 100  // Počet vzoriek v pôvodnom poli
//#define INTERPOLATED_SIZE 720  // Počet vzoriek po interpolácii

// FIR koeficienty pre dolnopriepustný filter (toto sú hypotetické hodnoty)

#define FIR_ORDER (sizeof(fir_coeffs) / sizeof(fir_coeffs[0]))


//----------------------------------------------------------------------------------------------------------------------------------

FRESULT convert_waveforms_to_csv_excel(void);

void display_text_message(char* message, uint8 value);



double sinc(double x);
//void sinc_interpolation(double* t_interp, double* t_sampled, int* sampled_signal, int* sincpole, int interp_size, int sampled_size);
void sinc_interpolation(double* t_interp, double* t_sampled, int* sampled_signal, int* sincpole, int size);

void moving_average(uint8* input, size_t size, size_t window_size);

void exponential_moving_average(uint8_t* input, size_t size, double alpha);

void fir_filter(uint8_t* input, size_t size);

void filter_bit_changes(uint8_t* input, size_t size);

void modify_signal(uint8_t* input, size_t size);
void add_one_bit(uint8_t* input, size_t size);

void average_filter(uint8* input, size_t size, size_t window_size);
 
//void scope_update_cursor_possition(void);
 
//void scope_preset_long_mode(void);

void scope_get_long_timebase_data(void);

void scope_check_long_trigger(void);



//----------------------------------------------------------------------------------------------------------------------------------

void scope_test_ADoffset_value(void);

void scope_test_TP(void);

void scope_test_sunlight(void);

void scope_test_longbase_value(void);

void scope_test_trigerposition_value(void);
void scope_test_calibration_value(void);

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
