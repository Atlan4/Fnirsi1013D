create_clock -name clk_200MHz -period 5 -waveform {0 2} [get_nets clk_200MHz]
create_clock -name i_xtal -period 20 -waveform {0 10}  [get_ports i_xtal]
set_clock_latency  -source 1 [get_clocks {i_xtal}]