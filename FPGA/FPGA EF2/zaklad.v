//-------------------------------------------------------------------------------------
//                           Code for FNIRSI 1013D oscilloscope
//                                      Atlan and PECO
//=====================================================================================

//A simple clock divider module for which the width and the last count can be set

module clockdivider
(
  input  wire clk_in,
  output reg clk_out = 0
);
parameter WIDTH = 32;
parameter LAST  = (2 ** WIDTH) - 1;

reg [WIDTH-1:0] count = 0;

  always @(posedge clk_in)
    begin
      if(count == LAST)
        count <= 0;
      else
        count <= count + 1;
    end

  always @(posedge clk_in)
    begin
      if(count == LAST)
        clk_out <= ~clk_out;
    end

endmodule

//-------------------------------------------------------------------------------------
//A 16 bit pulse width counter module with the pulse width setting as an input

module pwmcounter
(
  input wire clk,

  input wire [15:0] pulsewidth,

  output reg pwm = 0
);
parameter LAST = 65535;

reg [15:0] count = 0;

  always @(posedge clk)
    begin
      if(count == LAST)
        count <= 0;
      else
        begin
          count <= count + 1;

          pwm <= count < pulsewidth;
        end
    end

endmodule

//-------------------------------------------------------------------------------------
//Not implemented in this version is the I2C interface part.

module fnirsi_1013D
(
  //Input signals
  input wire       i_xtal,       //50 MHz clock
  input wire       i_mcu_clk,    //Active low going pulse from the mcu to clock data
  input wire       i_mcu_rws,    //Read 0 / write 1
  input wire       i_mcu_dcs,    //Data 0 / command 1
  input wire [7:0] i_adc1A_d,    //ADC 1 databus
  input wire [7:0] i_adc1B_d,        
  input wire [7:0] i_adc2A_d,    //ADC 2 databus        
  input wire [7:0] i_adc2B_d,        

  //Bi-directional parallel data bus to / from the mcu        
  inout wire [7:0] io_mcu_data,

  //I2C port
  //inout wire io_i2c_sda,

  //Outputs
  output wire o_adc1_encA,       //ADC clock outputs   
  output wire o_adc1_encB,
  output wire o_adc2_encA,        
  output wire o_adc2_encB,        
  output wire o_offset_1,        //DC offset channnel 1
  output wire o_relay1_1,        //Relay channel 1
  output wire o_relay1_2,        
  output wire o_relay1_3,
  output wire o_ac_dc_1,         //AC / DC select channel 1        
  output wire o_offset_2,        //DC offset channnel 2
  output wire o_relay2_1,        //Relay channel 2
  output wire o_relay2_2,        
  output wire o_relay2_3,
  output wire o_ac_dc_2,         //AC / DC select channel 2  
  output wire o_pwm_display,     //Display brightness control
  output wire o_1khz_calib
);

//-------------------------------------------------------------------------------------
//Wires to connect parts together

  wire clk_200MHz;
  wire clk_50MHz;
  wire clk_1KHz;

  wire sample_clock_enable;

  wire sample_read_clock;

  wire sampling_enable;

  wire time_base_timeout;

  wire trigger_check_enable;
  wire trigger_rise_enable;
  wire trigger_fall_enable;

  wire cur_greater_equal_lev;      //Signal active when the current sample is less then or equal to the set trigger level
  wire prev_less_equal_lev;        //Signal active when the previous sample is less then or equal to the set trigger level

  wire [12:0] sample_read_address;
  
  wire [12:0] addr_delta;

  wire [7:0] o_adc1A_d;
  wire [7:0] o_adc1B_d;
  wire [7:0] o_adc2A_d;
  wire [7:0] o_adc2B_d;

//-------------------------------------------------------------------------------------
//Registers to store working data

  reg [7:0] mcu_command = 0;               //Current command set by the MCU

  reg [7:0] o_mcu_data = 0;                //Data set to be read by the MCU

  reg [3:0] mcu_state = 0;                 //Current state for writing or reading multiple bytes (Can be reduced to two bits)

  reg [15:0] ch1_offset = 0;               //Channel 1 DC offset setting
  reg [15:0] ch2_offset = 0;               //Channel 2 DC offset setting

  reg [15:0] display_brigthness = 0;       //Display brightness control

  reg [15:0] touch_panel_address = 0;      //Data written with command 0x3C and read with 0x41

  reg [31:0] sample_rate_divider = 0;      //For controlling the sample rate a large divider

  reg [31:0] time_base_set = 0;            //Set by the MCU a value to count up to before trigger can get valid?
  reg [31:0] time_base_cnt = 0;            //Counter running on the sample write clock to count up to the set value?

  reg relay1_1 = 0;                        //Relay control for channel 1
  reg relay1_2 = 0;
  reg relay1_3 = 0;
  reg ac_dc_1  = 0;

  reg relay2_1 = 0;                        //Relay control for channel 2
  reg relay2_2 = 0;
  reg relay2_3 = 0;
  reg ac_dc_2  = 0;

  reg [31:0] sample_rate_counter = 0;      //Counter for making the sample rate

  reg sample_write_clock = 0;              //Free running clock on which the samples are taken
  reg sample_write_enable = 0;             //Flag to allow writing the samples or not (Read with command 0x0A)
  reg sample_read_enable = 0;              //Flag to allow reading of the samples by the MCU. (Written with command 0x0F)
  reg sample_system_reset = 0;             //Set by the MCU to force a reset of the sampling system
  reg sample_system_state = 0;             //Flag read by the MCU to check the reset state

  reg [3:0] sample_system_reset_count;     //Delay counter for the sample system reset state to go high on a reset

  reg [12:0] sample_write_address = 0;     //Circulair address counter for storing the samples when enabled

  reg [12:0] sample_read_offset = 0;       //An address offset for the reading of the data set by the MCU command 0x1F

  reg sampling_triggered = 0;              //Set when a trigger has occured
  reg trigger_detect_enable = 0;           //Set when a falling edge trigger occurs
  reg sampling_half_way = 0;               //Set when half the memory has been filled with samples

  reg trigger_mode = 0;                    //0 == auto mode and 1 == normal mode.
  reg trigger_edge = 0;                    //0 == rising edge and 1 =  falling edge.
  reg trigger_channel = 0;                 //0 == channel 1 and 1 == channel 2.

  reg [7:0] trigger_level = 0;
  
  reg [7:0] trigger_data;
  
  reg [7:0] trigger_previous = 0;          //On reset set to the inverted trigger edge for detecting the trigger
  
  localparam BUFFER_BITS = 13;
  localparam BUFFER_SIZE = 1 << BUFFER_BITS;  // 8192

  reg [12:0] pretrigger_samples= 750;     // samples before trigger alias set_trigger_point
  reg [12:0] total_samples = 1500;        // total number of samples (pretrigger + posttrigger)

  reg [12:0] trigger_read_address;        //A counter that starts to count when the half way point has been passed (0x0320)

  reg [12:0] trigger_address = 0;         //The trigger read address is captured on the trigger signal and can be read on command 0x14

  reg adc1_encA = 0;                       //Clock signals for the ADC's
  reg adc1_encB = 0;
  reg adc2_encA = 0;
  reg adc2_encB = 0;

//-------------------------------------------------------------------------------------

  pll main_clock
  (
    .refclk   (i_xtal),
    .reset    (1'b0),
    .clk0_out (clk_200MHz),
    .clk1_out (clk_RAM)
  );

//-------------------------------------------------------------------------------------
//Clock divider for making the 50MHz clock
//The original uses 16 bits

  clockdivider #
  (
    .WIDTH (1),
    .LAST  (1)
  )
  peripheral_clock
  (
    .clk_in  (clk_200MHz),
    .clk_out (clk_50MHz)
  );

//-------------------------------------------------------------------------------------
//Clock divider for making the 1KHz calibration signal
//The original uses 32 bits and 25000 for last which gives the incorrect frequency

  clockdivider #
  (
    .WIDTH (16),
    .LAST  (24999)
  )
  calibration_clock
  (
    .clk_in  (clk_50MHz),
    .clk_out (clk_1KHz)
  );

  assign o_1khz_calib = clk_1KHz;

//-------------------------------------------------------------------------------------
//Channel 1 DC offset

  pwmcounter #
  (
    .LAST (1999)
  )
  channel1_dc_offset
  (
    .clk        (clk_50MHz),
    .pulsewidth (ch1_offset),
    .pwm        (o_offset_1)
  );

//-------------------------------------------------------------------------------------
//Channel 2 DC offset

  pwmcounter #
  (
    .LAST (1999)
  )
  channel2_dc_offset
  (
    .clk        (clk_50MHz),
    .pulsewidth (ch2_offset),
    .pwm        (o_offset_2)
  );

//-------------------------------------------------------------------------------------
//Display brightness control

  pwmcounter #
  (
    .LAST (59999)
  )
  display_brightness
  (
    .clk        (clk_50MHz),
    .pulsewidth (display_brigthness),
    .pwm        (o_pwm_display)
  );

//---------------------------------------------------------------------------
//Memory part

  sample_memory samples
  (
    .dia   ({ i_adc2B_d, i_adc2A_d, i_adc1B_d, i_adc1A_d }),
    .addra (sample_write_address),
    .clka  (sample_write_clock),
    .cea   (sample_write_enable),

    .dob   ({ o_adc2B_d, o_adc2A_d, o_adc1B_d, o_adc1A_d }),
    .addrb (sample_read_address),
    .clkb  (sample_read_clock)
    //.ceb   (1'b1)
  );

//-------------------------------------------------------------------------------------
//Sample write clock

  always @(negedge clk_RAM)
    begin
      if(sample_clock_enable == 1)
        begin
          adc1_encB <= ~adc1_encB;
          adc2_encB <= ~adc2_encB;

          //The A and B clocks are inverted versions of each other
          adc1_encA <= adc1_encB;
          adc2_encA <= adc2_encB;
        end
    end

  always @(negedge clk_200MHz)//clk_200MHz  clk_RAM
    begin
      if(sample_clock_enable == 1)
        sample_write_clock <= ~sample_write_clock;
    end

// ---------------------------------------------------
// Sample rate divider
// ---------------------------------------------------

  always @(negedge clk_200MHz)
    begin
      if(sample_clock_enable == 1)
        sample_rate_counter <= 0;
      else
        sample_rate_counter <= sample_rate_counter + 1;
    end

  //To allow the sample clock to range from 100MHz down to very low rates the plus one is needed.
  //Without it the maximum output would be 50MHz.
  assign sample_clock_enable = sample_rate_divider < (sample_rate_counter + 1);

//-------------------------------------------------------------------------------------
//Sampling write address counter
//This counter is reset when the reset is activated by the MCU

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 1)
        sample_write_address <= 0;
      else
        sample_write_address <= sample_write_address + 1;
    end

//-------------------------------------------------------------------------------------
//Time base counter
//This counter is reset when the reset is activated by the MCU
//The oscilloscope firmware does not give correct values ​​for this type of calculation, 
//so the assignment trigger address = 750 is uncommented. 
//This is caused by the meamder when the base is running freely in auto mode without a trigger.

  always @(posedge sample_write_clock) 
    begin
      if(sample_system_reset == 1)
        time_base_cnt <= 0;
      else
        time_base_cnt <= time_base_cnt + 1;
    end

  assign time_base_timeout = time_base_cnt > time_base_set;


//-------------------------------------------------------------------------------------
//Sample writing enable control

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 1)
        sample_write_enable <= 1;                //Enable the writing on reset
      else if(sampling_enable == 0)
        sample_write_enable <= 0;                //Disable it when enough samples have been gathered
    end

//-------------------------------------------------------------------------------------
// Sampling is stopped when a trigger has been found AND the post-trigger region is complete 
// or when in auto mode a time out occurred

// delta address (modulo 8192)
  assign  addr_delta = trigger_read_address - trigger_address;

  assign sampling_enable = ~(
    (sampling_triggered && (addr_delta >= total_samples - pretrigger_samples))//);  // post-trigger complete
         || (~trigger_mode && time_base_timeout)                                    // auto mode timeout
         				 );

// For testing purposes for trigger control
/*
assign sampling_enable = ~(
    (sampling_triggered )   // post-trigger complete
    || (~trigger_mode && time_base_timeout)                  // auto mode timeout
);
*/
  //Half way the sampling buffer cycle the check for a trigger can be done
  assign trigger_check_enable = ((~sampling_triggered & sampling_half_way));

//--------------------------------------------------------------------------------------------
//Sampling triggered detect

// Previous states
  assign prev_less_equal_lev = trigger_previous < trigger_level;
  assign cur_greater_equal_lev = trigger_data >= trigger_level;
  assign cur_less_equal_lev = trigger_data < trigger_level;

// Enable signals
  assign trigger_rise_enable = (~trigger_edge & prev_less_equal_lev) | trigger_detect_enable;
  assign trigger_fall_enable =  (trigger_edge & ~prev_less_equal_lev) | trigger_detect_enable;

// Rising edge: previous ≤ level, current ≥ level
  assign trigger_rise_detect = (~trigger_edge & prev_less_equal_lev & cur_greater_equal_lev);

// Falling edge: previous ≥ level, current ≤ level
  assign trigger_fall_detect = (trigger_edge & ~prev_less_equal_lev & cur_less_equal_lev); 


  always @(posedge clk_RAM) begin			//clk_RAM  sample_write_clock
    if(sample_system_reset == 1) begin
        sampling_triggered <= 0;
        trigger_detect_enable <= 0;
    end else if(trigger_check_enable == 1) begin
        sampling_triggered <= sampling_triggered | trigger_rise_detect | trigger_fall_detect;
        trigger_detect_enable <= trigger_rise_detect | trigger_fall_detect;
    end
  end

//------------------------------------------------

  always @(posedge clk_RAM) begin	//clk_RAM  sample_write_clock
    if (sample_system_reset) 
       begin
         trigger_previous <= 0;
         trigger_data     <= 0;
       end 
    else
       begin
         //sample from the previous cycle
         trigger_previous <= trigger_data;

         //current sample
	 trigger_data <= trigger_channel ? i_adc2A_d : i_adc1A_d;
       end
  end

//-------------------------------------------------------------------------------------
//Sampling half way detect

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 1)
        sampling_half_way <= 0;
      else
        sampling_half_way <= (sample_write_address >= pretrigger_samples) || sampling_half_way;
    end

//-------------------------------------------------------------------------------------
//An address counter that is preloaded and starts counting on the half way point

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 1)
        trigger_read_address <= pretrigger_samples;   //Starting point in memory
      else if(sampling_half_way == 1)
        trigger_read_address <= trigger_read_address + 1;
    end

//-------------------------------------------------------------------------------------
//Capture of the trigger address

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 1)
        trigger_address <= 0;
        
      else if (~trigger_mode && time_base_timeout) //Time base counter
        trigger_address <= pretrigger_samples;
        
      else if(sampling_triggered == 0)
        trigger_address <= trigger_read_address; 
    end

//=====================================================================================

  //Select the address and clock source based on the sample read enbale setting

  assign sample_read_address = sample_read_enable ? sample_read_offset : sample_read_offset;

  assign sample_read_clock   = sample_read_enable ? i_mcu_clk : i_mcu_clk;

//-------------------------------------------------------------------------------------
//Sampling reset state delay counter
//This counter is activated when the reset is activated by the MCU
//Only used to delay the software and on low sample rates it slows the system down

  always @(posedge sample_write_clock)
    begin
      if(sample_system_reset == 0)
        begin
          sample_system_reset_count <= 0;

          sample_system_state <= 0;
        end
      else
        begin
          sample_system_reset_count <= sample_system_reset_count + 1;

          //Flag remains high when count has been reached
          sample_system_state <= (sample_system_reset_count == 6) | sample_system_state;
        end
    end

//-------------------------------------------------------------------------------------
//Loading of the command register

  always @(posedge i_mcu_clk)
    begin
      if(i_mcu_dcs & i_mcu_rws)
        mcu_command <= io_mcu_data;
    end

//-------------------------------------------------------------------------------------
//State machine for the multiple byte write and read of data
//Only used when writing data for the commands 0x0D, 0x0E, 0x1F, 0x32, 0x35, 0x38 and 0x3C
//Or when reading data for the commands 0x06, 0x14 and 0x41

  always @(posedge i_mcu_clk)
    begin
      if(i_mcu_dcs == 1'b1)
        mcu_state <= 3'h0;
      else if((((mcu_command == 8'h0D) | (mcu_command == 8'h0E) | (mcu_command == 8'h1B) | (mcu_command == 8'h1C) | (mcu_command == 8'h1F)
              | (mcu_command == 8'h32) | (mcu_command == 8'h35) | (mcu_command == 8'h38) | (mcu_command == 8'h3C)) & i_mcu_rws) |
              (((mcu_command == 8'h06) | (mcu_command == 8'h14) | (mcu_command == 8'h41)) & ~i_mcu_rws))
        mcu_state <= mcu_state + 1;
    end

//-------------------------------------------------------------------------------------
//Updating of the sample read address on every read for one of the designated commands

  always @(posedge i_mcu_clk)
    begin
      if((i_mcu_dcs == 0) & (i_mcu_rws == 0) & ((mcu_command == 8'h20) | (mcu_command == 8'h21) | (mcu_command == 8'h22) | (mcu_command == 8'h23)))
        begin       
          sample_read_offset = sample_read_offset + 1;
        end
        
      else if((i_mcu_dcs == 0) & (i_mcu_rws == 1) & (mcu_command == 8'h1F))
        begin
          //Loading of the two bytes of the data read address offset high byte first
          case(mcu_state)
            4'h0:
              sample_read_offset[12:8] <= io_mcu_data[4:0];
            4'h1:
              sample_read_offset[7:0] <= io_mcu_data;
          endcase
        end
    end

//-------------------------------------------------------------------------------------
//Loading data into the registers based on the set command

  always @(posedge i_mcu_clk)
    begin
      if((i_mcu_dcs == 0) & i_mcu_rws)       //Only handled for data and write
        begin
          case(mcu_command)
            //Setting the sampling reset state
            8'h01:
              sample_system_reset <= io_mcu_data[0];

            //Loading of the four bytes of the sample rate divider high byte first
            8'h0D:
              begin
                case(mcu_state)
                  4'h0:
                    sample_rate_divider[31:24] <= io_mcu_data;
                  4'h1:
                    sample_rate_divider[23:16] <= io_mcu_data;
                  4'h2:
                    sample_rate_divider[15:8] <= io_mcu_data;
                  4'h3:
                    sample_rate_divider[7:0] <= io_mcu_data;
                endcase
              end

            //Loading of the four bytes of the time base counter high byte first
            8'h0E:
              begin
                case(mcu_state)
                  4'h0:
                    time_base_set[31:24] <= io_mcu_data;
                  4'h1:
                    time_base_set[23:16] <= io_mcu_data;
                  4'h2:
                    time_base_set[15:8] <= io_mcu_data;
                  4'h3:
                    time_base_set[7:0] <= io_mcu_data;
                endcase
              end

            //Setting the trigger channel
            8'h0F:
              sample_read_enable <= io_mcu_data[0];

            //Setting the trigger channel
            8'h15:
              trigger_channel <= io_mcu_data[0];

            //Setting the trigger edge
            8'h16:
              trigger_edge <= io_mcu_data[0];

            //Setting the trigger level
            8'h17:
              trigger_level <= io_mcu_data;

            //Setting the trigger mode
            8'h1A:
              trigger_mode <= io_mcu_data[0];

            //Loading of the two bytes of the pretrigger_samples (set_trigger_point) high byte first
            8'h1B:
              begin
                case(mcu_state)
                  4'h0:
                    pretrigger_samples[12:8] <= io_mcu_data[4:0];
                  4'h1:
                    pretrigger_samples[7:0] <= io_mcu_data;
                endcase
              end
              
            //Loading of the two bytes of the total number of sample high byte first
            8'h1C:
              begin
                case(mcu_state)
                  4'h0:
                    total_samples[12:8] <= io_mcu_data[4:0];
                  4'h1:
                    total_samples[7:0] <= io_mcu_data;
                endcase
              end

            //Loading of the two bytes of the channel 1 DC offset high byte first
            8'h32:
              begin
                case(mcu_state)
                  4'h0:
                    ch1_offset[15:8] <= io_mcu_data;
                  4'h1:
                    ch1_offset[7:0] <= io_mcu_data;
                endcase
              end

            //Controlling of the channel 1 input sensitivity relays
            8'h33:
              begin
                if(io_mcu_data < 8'h6)
                  begin
                    relay1_1 <=  io_mcu_data[0];
                    relay1_2 <= ~io_mcu_data[1];
                    relay1_3 <= ~io_mcu_data[2];
                  end
              end

            //Controlling of the channel 1 input for AC or DC
            8'h34:
              ac_dc_1 <= io_mcu_data[0];

            //Loading of the two bytes of the channel 2 DC offset high byte first
            8'h35:
              begin
                case(mcu_state)
                  4'h0:
                    ch2_offset[15:8] <= io_mcu_data;
                  4'h1:
                    ch2_offset[7:0] <= io_mcu_data;
                endcase
              end

            //Controlling of the channel 2 input sensitivity relays
            8'h36:
              begin
                if(io_mcu_data < 8'h6)
                  begin
                    relay2_1 <=  io_mcu_data[0];
                    relay2_2 <= ~io_mcu_data[1];
                    relay2_3 <= ~io_mcu_data[2];
                  end
              end

            //Controlling of the channel 2 input for AC or DC
            8'h37:
              ac_dc_2 <= io_mcu_data[0];

            //Loading of the two bytes of the display brightness control high byte first
            8'h38:
              begin
                case(mcu_state)
                  4'h0:
                    display_brigthness[15:8] <= io_mcu_data;
                  4'h1:
                    display_brigthness[7:0] <= io_mcu_data;
                endcase
              end

            //Loading of the two bytes of the touch panel read address high byte first
            8'h3C:
              begin
                case(mcu_state)
                  4'h0:
                    touch_panel_address[15:8] <= io_mcu_data;
                  4'h1:
                    touch_panel_address[7:0] <= io_mcu_data;
                endcase
              end
          endcase
        end
    end

//-------------------------------------------------------------------------------------
//For reading data the requested byte is loaded into a register on the positive edge
//of the i_mcu_clk signal. 

  always @(posedge i_mcu_clk)
    begin
      if((i_mcu_dcs == 0) & (i_mcu_rws == 0))    //Only handled when data and read
        begin
          case(mcu_command)
            //Reading of the sampling system resets state
            8'h05:
              o_mcu_data[0] <= sample_system_state;

            //Reading of the FPGA version needs loading of high byte first
            8'h06:
              begin
                case(mcu_state)
                  4'h0:
                    o_mcu_data <= 8'h14;
                  4'h1:
                    o_mcu_data <= 8'h32;
                endcase
              end

            //Reading of the sampling done state
            //When sampling finishes the writing is disabled. Enabled again with a reset.
            //Can't determine how the original does this, but for the software to work it needs to be inverted
            8'h0A:
              o_mcu_data[0] <= ~sample_write_enable;

            //Reading of the trigger address needs loading of high byte first
            8'h14:
              begin
                case(mcu_state)
                  4'h0:
                    o_mcu_data <= trigger_address[12:8];
                  4'h1:
                    o_mcu_data <= trigger_address[7:0];
                endcase
              end

            //Reading of the sampling triggered state
            //This is not used in the software and is basically the same as command 0x0A
            8'h18:
              o_mcu_data[0] <= sampling_triggered;

            //Reading of the sample data for channel 1 ADC 1 from memory
            8'h20:
              o_mcu_data <= o_adc1A_d;

            //Reading of the sample data for channel 1 ADC 2 from memory
            8'h21:
              o_mcu_data <= o_adc1B_d;

            //Reading of the sample data for channel 2 ADC 1 from memory
            8'h22:
              o_mcu_data <= o_adc2A_d;

            //Reading of the sample data for channel 2 ADC 2 from memory
            8'h23:
              o_mcu_data <= o_adc2B_d;

            //Reading of the sample data for channel 1 ADC 1 
            8'h24:
              o_mcu_data <= i_adc1A_d;

            //Reading of the sample data for channel 1 ADC 2
            8'h25:
              o_mcu_data <= i_adc1B_d;

            //Reading of the sample data for channel 2 ADC 1
            8'h26:
              o_mcu_data <= i_adc2A_d;

            //Reading of the sample data for channel 2 ADC 2
            8'h27:
              o_mcu_data <= i_adc2B_d;

            //Reading of the touch panel address needs loading of high byte first
            8'h41:
              begin
                case(mcu_state)
                  4'h0:
                    o_mcu_data <= touch_panel_address[15:8];
                  4'h1:
                    o_mcu_data <= touch_panel_address[7:0];
                endcase
              end
          endcase
        end
    end

//-------------------------------------------------------------------------------------
//Control of the bidirectional bus for input and output
//The bus direction is based on the read write signal

  assign io_mcu_data = i_mcu_rws ? 8'bZ : o_mcu_data;

//-------------------------------------------------------------------------------------
//Connect the relay control signals to the outside world

  assign o_relay1_1 = relay1_1;
  assign o_relay1_2 = relay1_2;
  assign o_relay1_3 = relay1_3;
  assign o_ac_dc_1  = ac_dc_1;

  assign o_relay2_1 = relay2_1;
  assign o_relay2_2 = relay2_2;
  assign o_relay2_3 = relay2_3;
  assign o_ac_dc_2  = ac_dc_2;

  assign o_adc1_encA = adc1_encA;
  assign o_adc1_encB = adc1_encB;
  assign o_adc2_encA = adc2_encA;
  assign o_adc2_encB = adc2_encB;

//-------------------------------------------------------------------------------------
endmodule
//-------------------------------------------------------------------------------------
