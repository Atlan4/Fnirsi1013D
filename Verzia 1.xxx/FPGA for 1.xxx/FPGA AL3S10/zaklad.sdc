create_clock -name clk_200MHz -period 5 -waveform {0 2} [get_nets clk_200MHz]
create_clock -name i_xtal -period 20 -waveform {0 10}  [get_ports i_xtal]
set_clock_latency  -source 1 [get_clocks {i_xtal}]

#create_generated_clock -name clk_ADC0   -source [get_ports i_xtal] [get_ports o_adc1_encA]
#create_generated_clock -name clk_ADC90  -source [get_ports i_xtal] [get_ports o_adc1_encB]
#create_generated_clock -name clk_ADC2_0 -source [get_ports clk_200MHz] [get_ports o_adc2_encA]
#create_generated_clock -name clk_ADC2_90 -source [get_ports clk_200MHz] [get_ports o_adc2_encB]
