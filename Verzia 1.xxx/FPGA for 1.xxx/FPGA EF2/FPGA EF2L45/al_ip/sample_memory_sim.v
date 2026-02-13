// Verilog netlist created by TD v4.6.116866
// Wed Jan 28 13:13:14 2026

`timescale 1ns / 1ps
module sample_memory  // al_ip/sample_memory.v(14)
  (
  addra,
  addrb,
  cea,
  clka,
  clkb,
  dia,
  dob
  );

  input [12:0] addra;  // al_ip/sample_memory.v(23)
  input [12:0] addrb;  // al_ip/sample_memory.v(24)
  input cea;  // al_ip/sample_memory.v(25)
  input clka;  // al_ip/sample_memory.v(26)
  input clkb;  // al_ip/sample_memory.v(27)
  input [31:0] dia;  // al_ip/sample_memory.v(22)
  output [31:0] dob;  // al_ip/sample_memory.v(19)

  wire [0:1] addrb_piped;
  wire  \inst_dob_mux_b0/B0_0 ;
  wire  \inst_dob_mux_b0/B0_1 ;
  wire  \inst_dob_mux_b1/B0_0 ;
  wire  \inst_dob_mux_b1/B0_1 ;
  wire  \inst_dob_mux_b10/B0_0 ;
  wire  \inst_dob_mux_b10/B0_1 ;
  wire  \inst_dob_mux_b11/B0_0 ;
  wire  \inst_dob_mux_b11/B0_1 ;
  wire  \inst_dob_mux_b12/B0_0 ;
  wire  \inst_dob_mux_b12/B0_1 ;
  wire  \inst_dob_mux_b13/B0_0 ;
  wire  \inst_dob_mux_b13/B0_1 ;
  wire  \inst_dob_mux_b14/B0_0 ;
  wire  \inst_dob_mux_b14/B0_1 ;
  wire  \inst_dob_mux_b15/B0_0 ;
  wire  \inst_dob_mux_b15/B0_1 ;
  wire  \inst_dob_mux_b16/B0_0 ;
  wire  \inst_dob_mux_b16/B0_1 ;
  wire  \inst_dob_mux_b17/B0_0 ;
  wire  \inst_dob_mux_b17/B0_1 ;
  wire  \inst_dob_mux_b18/B0_0 ;
  wire  \inst_dob_mux_b18/B0_1 ;
  wire  \inst_dob_mux_b19/B0_0 ;
  wire  \inst_dob_mux_b19/B0_1 ;
  wire  \inst_dob_mux_b2/B0_0 ;
  wire  \inst_dob_mux_b2/B0_1 ;
  wire  \inst_dob_mux_b20/B0_0 ;
  wire  \inst_dob_mux_b20/B0_1 ;
  wire  \inst_dob_mux_b21/B0_0 ;
  wire  \inst_dob_mux_b21/B0_1 ;
  wire  \inst_dob_mux_b22/B0_0 ;
  wire  \inst_dob_mux_b22/B0_1 ;
  wire  \inst_dob_mux_b23/B0_0 ;
  wire  \inst_dob_mux_b23/B0_1 ;
  wire  \inst_dob_mux_b24/B0_0 ;
  wire  \inst_dob_mux_b24/B0_1 ;
  wire  \inst_dob_mux_b25/B0_0 ;
  wire  \inst_dob_mux_b25/B0_1 ;
  wire  \inst_dob_mux_b26/B0_0 ;
  wire  \inst_dob_mux_b26/B0_1 ;
  wire  \inst_dob_mux_b27/B0_0 ;
  wire  \inst_dob_mux_b27/B0_1 ;
  wire  \inst_dob_mux_b28/B0_0 ;
  wire  \inst_dob_mux_b28/B0_1 ;
  wire  \inst_dob_mux_b29/B0_0 ;
  wire  \inst_dob_mux_b29/B0_1 ;
  wire  \inst_dob_mux_b3/B0_0 ;
  wire  \inst_dob_mux_b3/B0_1 ;
  wire  \inst_dob_mux_b30/B0_0 ;
  wire  \inst_dob_mux_b30/B0_1 ;
  wire  \inst_dob_mux_b31/B0_0 ;
  wire  \inst_dob_mux_b31/B0_1 ;
  wire  \inst_dob_mux_b4/B0_0 ;
  wire  \inst_dob_mux_b4/B0_1 ;
  wire  \inst_dob_mux_b5/B0_0 ;
  wire  \inst_dob_mux_b5/B0_1 ;
  wire  \inst_dob_mux_b6/B0_0 ;
  wire  \inst_dob_mux_b6/B0_1 ;
  wire  \inst_dob_mux_b7/B0_0 ;
  wire  \inst_dob_mux_b7/B0_1 ;
  wire  \inst_dob_mux_b8/B0_0 ;
  wire  \inst_dob_mux_b8/B0_1 ;
  wire  \inst_dob_mux_b9/B0_0 ;
  wire  \inst_dob_mux_b9/B0_1 ;
  wire \and_Naddra[11]_Naddr_o ;
  wire \and_Naddra[11]_addra_o ;
  wire \and_Naddrb[11]_Naddr_o ;
  wire \and_Naddrb[11]_addrb_o ;
  wire \and_addra[11]_Naddra_o ;
  wire \and_addrb[11]_Naddrb_o ;
  wire \and_and_Naddra[11]_N_o ;
  wire \and_and_Naddra[11]_a_o ;
  wire \and_and_addra[11]_Na_o ;
  wire inst_dob_i0_000;
  wire inst_dob_i0_001;
  wire inst_dob_i0_002;
  wire inst_dob_i0_003;
  wire inst_dob_i0_004;
  wire inst_dob_i0_005;
  wire inst_dob_i0_006;
  wire inst_dob_i0_007;
  wire inst_dob_i0_008;
  wire inst_dob_i0_009;
  wire inst_dob_i0_010;
  wire inst_dob_i0_011;
  wire inst_dob_i0_012;
  wire inst_dob_i0_013;
  wire inst_dob_i0_014;
  wire inst_dob_i0_015;
  wire inst_dob_i0_016;
  wire inst_dob_i0_017;
  wire inst_dob_i0_018;
  wire inst_dob_i0_019;
  wire inst_dob_i0_020;
  wire inst_dob_i0_021;
  wire inst_dob_i0_022;
  wire inst_dob_i0_023;
  wire inst_dob_i0_024;
  wire inst_dob_i0_025;
  wire inst_dob_i0_026;
  wire inst_dob_i0_027;
  wire inst_dob_i0_028;
  wire inst_dob_i0_029;
  wire inst_dob_i0_030;
  wire inst_dob_i0_031;
  wire inst_dob_i1_000;
  wire inst_dob_i1_001;
  wire inst_dob_i1_002;
  wire inst_dob_i1_003;
  wire inst_dob_i1_004;
  wire inst_dob_i1_005;
  wire inst_dob_i1_006;
  wire inst_dob_i1_007;
  wire inst_dob_i1_008;
  wire inst_dob_i1_009;
  wire inst_dob_i1_010;
  wire inst_dob_i1_011;
  wire inst_dob_i1_012;
  wire inst_dob_i1_013;
  wire inst_dob_i1_014;
  wire inst_dob_i1_015;
  wire inst_dob_i1_016;
  wire inst_dob_i1_017;
  wire inst_dob_i1_018;
  wire inst_dob_i1_019;
  wire inst_dob_i1_020;
  wire inst_dob_i1_021;
  wire inst_dob_i1_022;
  wire inst_dob_i1_023;
  wire inst_dob_i1_024;
  wire inst_dob_i1_025;
  wire inst_dob_i1_026;
  wire inst_dob_i1_027;
  wire inst_dob_i1_028;
  wire inst_dob_i1_029;
  wire inst_dob_i1_030;
  wire inst_dob_i1_031;
  wire inst_dob_i2_000;
  wire inst_dob_i2_001;
  wire inst_dob_i2_002;
  wire inst_dob_i2_003;
  wire inst_dob_i2_004;
  wire inst_dob_i2_005;
  wire inst_dob_i2_006;
  wire inst_dob_i2_007;
  wire inst_dob_i2_008;
  wire inst_dob_i2_009;
  wire inst_dob_i2_010;
  wire inst_dob_i2_011;
  wire inst_dob_i2_012;
  wire inst_dob_i2_013;
  wire inst_dob_i2_014;
  wire inst_dob_i2_015;
  wire inst_dob_i2_016;
  wire inst_dob_i2_017;
  wire inst_dob_i2_018;
  wire inst_dob_i2_019;
  wire inst_dob_i2_020;
  wire inst_dob_i2_021;
  wire inst_dob_i2_022;
  wire inst_dob_i2_023;
  wire inst_dob_i2_024;
  wire inst_dob_i2_025;
  wire inst_dob_i2_026;
  wire inst_dob_i2_027;
  wire inst_dob_i2_028;
  wire inst_dob_i2_029;
  wire inst_dob_i2_030;
  wire inst_dob_i2_031;

  reg_ar_as_w1 addrb_pipe_b0 (
    .clk(clkb),
    .d(addrb[11]),
    .en(1'b1),
    .reset(1'b0),
    .set(1'b0),
    .q(addrb_piped[0]));
  reg_ar_as_w1 addrb_pipe_b1 (
    .clk(clkb),
    .d(addrb[12]),
    .en(1'b1),
    .reset(1'b0),
    .set(1'b0),
    .q(addrb_piped[1]));
  and \and_Naddra[11]_Naddr  (\and_Naddra[11]_Naddr_o , ~addra[11], ~addra[12]);
  and \and_Naddra[11]_addra  (\and_Naddra[11]_addra_o , ~addra[11], addra[12]);
  and \and_Naddrb[11]_Naddr  (\and_Naddrb[11]_Naddr_o , ~addrb[11], ~addrb[12]);
  and \and_Naddrb[11]_addrb  (\and_Naddrb[11]_addrb_o , ~addrb[11], addrb[12]);
  and \and_addra[11]_Naddra  (\and_addra[11]_Naddra_o , addra[11], ~addra[12]);
  and \and_addrb[11]_Naddrb  (\and_addrb[11]_Naddrb_o , addrb[11], ~addrb[12]);
  and \and_and_Naddra[11]_N  (\and_and_Naddra[11]_N_o , \and_Naddra[11]_Naddr_o , cea);
  and \and_and_Naddra[11]_a  (\and_and_Naddra[11]_a_o , \and_Naddra[11]_addra_o , cea);
  and \and_and_addra[11]_Na  (\and_and_addra[11]_Na_o , \and_addra[11]_Naddra_o , cea);
  EF2_PHY_CONFIG #(
    .DONE_PERSISTN("ENABLE"),
    .INIT_PERSISTN("ENABLE"),
    .JTAG_PERSISTN("DISABLE"),
    .PROGRAMN_PERSISTN("DISABLE"))
    config_inst ();
  // address_offset=0;data_offset=0;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_000000_000 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_Naddra[11]_N_o ),
    .csb(\and_Naddrb[11]_Naddr_o ),
    .dia(dia[15:0]),
    .dob({inst_dob_i0_015,inst_dob_i0_014,inst_dob_i0_013,inst_dob_i0_012,inst_dob_i0_011,inst_dob_i0_010,inst_dob_i0_009,inst_dob_i0_008,inst_dob_i0_007,inst_dob_i0_006,inst_dob_i0_005,inst_dob_i0_004,inst_dob_i0_003,inst_dob_i0_002,inst_dob_i0_001,inst_dob_i0_000}));
  // address_offset=0;data_offset=16;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_000000_016 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_Naddra[11]_N_o ),
    .csb(\and_Naddrb[11]_Naddr_o ),
    .dia(dia[31:16]),
    .dob({inst_dob_i0_031,inst_dob_i0_030,inst_dob_i0_029,inst_dob_i0_028,inst_dob_i0_027,inst_dob_i0_026,inst_dob_i0_025,inst_dob_i0_024,inst_dob_i0_023,inst_dob_i0_022,inst_dob_i0_021,inst_dob_i0_020,inst_dob_i0_019,inst_dob_i0_018,inst_dob_i0_017,inst_dob_i0_016}));
  // address_offset=2048;data_offset=0;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_002048_000 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_addra[11]_Na_o ),
    .csb(\and_addrb[11]_Naddrb_o ),
    .dia(dia[15:0]),
    .dob({inst_dob_i1_015,inst_dob_i1_014,inst_dob_i1_013,inst_dob_i1_012,inst_dob_i1_011,inst_dob_i1_010,inst_dob_i1_009,inst_dob_i1_008,inst_dob_i1_007,inst_dob_i1_006,inst_dob_i1_005,inst_dob_i1_004,inst_dob_i1_003,inst_dob_i1_002,inst_dob_i1_001,inst_dob_i1_000}));
  // address_offset=2048;data_offset=16;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_002048_016 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_addra[11]_Na_o ),
    .csb(\and_addrb[11]_Naddrb_o ),
    .dia(dia[31:16]),
    .dob({inst_dob_i1_031,inst_dob_i1_030,inst_dob_i1_029,inst_dob_i1_028,inst_dob_i1_027,inst_dob_i1_026,inst_dob_i1_025,inst_dob_i1_024,inst_dob_i1_023,inst_dob_i1_022,inst_dob_i1_021,inst_dob_i1_020,inst_dob_i1_019,inst_dob_i1_018,inst_dob_i1_017,inst_dob_i1_016}));
  // address_offset=4096;data_offset=0;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_004096_000 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_Naddra[11]_a_o ),
    .csb(\and_Naddrb[11]_addrb_o ),
    .dia(dia[15:0]),
    .dob({inst_dob_i2_015,inst_dob_i2_014,inst_dob_i2_013,inst_dob_i2_012,inst_dob_i2_011,inst_dob_i2_010,inst_dob_i2_009,inst_dob_i2_008,inst_dob_i2_007,inst_dob_i2_006,inst_dob_i2_005,inst_dob_i2_004,inst_dob_i2_003,inst_dob_i2_002,inst_dob_i2_001,inst_dob_i2_000}));
  // address_offset=4096;data_offset=16;depth=2048;width=16;num_section=1;width_per_section=16;section_size=32;working_depth=2048;working_width=16;address_step=1;bytes_in_per_section=1;
  EF2_PHY_BRAM32K #(
    .DATA_WIDTH_A("16"),
    .DATA_WIDTH_B("16"),
    .MODE("DP16K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .SRMODE("SYNC"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_6144x32_sub_004096_016 (
    .addra(addra[10:0]),
    .addrb(addrb[10:0]),
    .bytewea(1'b0),
    .byteweb(1'b0),
    .clka(clka),
    .clkb(clkb),
    .csa(\and_and_Naddra[11]_a_o ),
    .csb(\and_Naddrb[11]_addrb_o ),
    .dia(dia[31:16]),
    .dob({inst_dob_i2_031,inst_dob_i2_030,inst_dob_i2_029,inst_dob_i2_028,inst_dob_i2_027,inst_dob_i2_026,inst_dob_i2_025,inst_dob_i2_024,inst_dob_i2_023,inst_dob_i2_022,inst_dob_i2_021,inst_dob_i2_020,inst_dob_i2_019,inst_dob_i2_018,inst_dob_i2_017,inst_dob_i2_016}));
  AL_MUX \inst_dob_mux_b0/al_mux_b0_0_0  (
    .i0(inst_dob_i0_000),
    .i1(inst_dob_i1_000),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b0/B0_0 ));
  AL_MUX \inst_dob_mux_b0/al_mux_b0_0_1  (
    .i0(inst_dob_i2_000),
    .i1(inst_dob_i2_000),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b0/B0_1 ));
  AL_MUX \inst_dob_mux_b0/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b0/B0_0 ),
    .i1(\inst_dob_mux_b0/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[0]));
  AL_MUX \inst_dob_mux_b1/al_mux_b0_0_0  (
    .i0(inst_dob_i0_001),
    .i1(inst_dob_i1_001),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b1/B0_0 ));
  AL_MUX \inst_dob_mux_b1/al_mux_b0_0_1  (
    .i0(inst_dob_i2_001),
    .i1(inst_dob_i2_001),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b1/B0_1 ));
  AL_MUX \inst_dob_mux_b1/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b1/B0_0 ),
    .i1(\inst_dob_mux_b1/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[1]));
  AL_MUX \inst_dob_mux_b10/al_mux_b0_0_0  (
    .i0(inst_dob_i0_010),
    .i1(inst_dob_i1_010),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b10/B0_0 ));
  AL_MUX \inst_dob_mux_b10/al_mux_b0_0_1  (
    .i0(inst_dob_i2_010),
    .i1(inst_dob_i2_010),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b10/B0_1 ));
  AL_MUX \inst_dob_mux_b10/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b10/B0_0 ),
    .i1(\inst_dob_mux_b10/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[10]));
  AL_MUX \inst_dob_mux_b11/al_mux_b0_0_0  (
    .i0(inst_dob_i0_011),
    .i1(inst_dob_i1_011),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b11/B0_0 ));
  AL_MUX \inst_dob_mux_b11/al_mux_b0_0_1  (
    .i0(inst_dob_i2_011),
    .i1(inst_dob_i2_011),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b11/B0_1 ));
  AL_MUX \inst_dob_mux_b11/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b11/B0_0 ),
    .i1(\inst_dob_mux_b11/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[11]));
  AL_MUX \inst_dob_mux_b12/al_mux_b0_0_0  (
    .i0(inst_dob_i0_012),
    .i1(inst_dob_i1_012),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b12/B0_0 ));
  AL_MUX \inst_dob_mux_b12/al_mux_b0_0_1  (
    .i0(inst_dob_i2_012),
    .i1(inst_dob_i2_012),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b12/B0_1 ));
  AL_MUX \inst_dob_mux_b12/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b12/B0_0 ),
    .i1(\inst_dob_mux_b12/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[12]));
  AL_MUX \inst_dob_mux_b13/al_mux_b0_0_0  (
    .i0(inst_dob_i0_013),
    .i1(inst_dob_i1_013),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b13/B0_0 ));
  AL_MUX \inst_dob_mux_b13/al_mux_b0_0_1  (
    .i0(inst_dob_i2_013),
    .i1(inst_dob_i2_013),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b13/B0_1 ));
  AL_MUX \inst_dob_mux_b13/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b13/B0_0 ),
    .i1(\inst_dob_mux_b13/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[13]));
  AL_MUX \inst_dob_mux_b14/al_mux_b0_0_0  (
    .i0(inst_dob_i0_014),
    .i1(inst_dob_i1_014),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b14/B0_0 ));
  AL_MUX \inst_dob_mux_b14/al_mux_b0_0_1  (
    .i0(inst_dob_i2_014),
    .i1(inst_dob_i2_014),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b14/B0_1 ));
  AL_MUX \inst_dob_mux_b14/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b14/B0_0 ),
    .i1(\inst_dob_mux_b14/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[14]));
  AL_MUX \inst_dob_mux_b15/al_mux_b0_0_0  (
    .i0(inst_dob_i0_015),
    .i1(inst_dob_i1_015),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b15/B0_0 ));
  AL_MUX \inst_dob_mux_b15/al_mux_b0_0_1  (
    .i0(inst_dob_i2_015),
    .i1(inst_dob_i2_015),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b15/B0_1 ));
  AL_MUX \inst_dob_mux_b15/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b15/B0_0 ),
    .i1(\inst_dob_mux_b15/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[15]));
  AL_MUX \inst_dob_mux_b16/al_mux_b0_0_0  (
    .i0(inst_dob_i0_016),
    .i1(inst_dob_i1_016),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b16/B0_0 ));
  AL_MUX \inst_dob_mux_b16/al_mux_b0_0_1  (
    .i0(inst_dob_i2_016),
    .i1(inst_dob_i2_016),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b16/B0_1 ));
  AL_MUX \inst_dob_mux_b16/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b16/B0_0 ),
    .i1(\inst_dob_mux_b16/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[16]));
  AL_MUX \inst_dob_mux_b17/al_mux_b0_0_0  (
    .i0(inst_dob_i0_017),
    .i1(inst_dob_i1_017),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b17/B0_0 ));
  AL_MUX \inst_dob_mux_b17/al_mux_b0_0_1  (
    .i0(inst_dob_i2_017),
    .i1(inst_dob_i2_017),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b17/B0_1 ));
  AL_MUX \inst_dob_mux_b17/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b17/B0_0 ),
    .i1(\inst_dob_mux_b17/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[17]));
  AL_MUX \inst_dob_mux_b18/al_mux_b0_0_0  (
    .i0(inst_dob_i0_018),
    .i1(inst_dob_i1_018),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b18/B0_0 ));
  AL_MUX \inst_dob_mux_b18/al_mux_b0_0_1  (
    .i0(inst_dob_i2_018),
    .i1(inst_dob_i2_018),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b18/B0_1 ));
  AL_MUX \inst_dob_mux_b18/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b18/B0_0 ),
    .i1(\inst_dob_mux_b18/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[18]));
  AL_MUX \inst_dob_mux_b19/al_mux_b0_0_0  (
    .i0(inst_dob_i0_019),
    .i1(inst_dob_i1_019),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b19/B0_0 ));
  AL_MUX \inst_dob_mux_b19/al_mux_b0_0_1  (
    .i0(inst_dob_i2_019),
    .i1(inst_dob_i2_019),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b19/B0_1 ));
  AL_MUX \inst_dob_mux_b19/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b19/B0_0 ),
    .i1(\inst_dob_mux_b19/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[19]));
  AL_MUX \inst_dob_mux_b2/al_mux_b0_0_0  (
    .i0(inst_dob_i0_002),
    .i1(inst_dob_i1_002),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b2/B0_0 ));
  AL_MUX \inst_dob_mux_b2/al_mux_b0_0_1  (
    .i0(inst_dob_i2_002),
    .i1(inst_dob_i2_002),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b2/B0_1 ));
  AL_MUX \inst_dob_mux_b2/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b2/B0_0 ),
    .i1(\inst_dob_mux_b2/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[2]));
  AL_MUX \inst_dob_mux_b20/al_mux_b0_0_0  (
    .i0(inst_dob_i0_020),
    .i1(inst_dob_i1_020),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b20/B0_0 ));
  AL_MUX \inst_dob_mux_b20/al_mux_b0_0_1  (
    .i0(inst_dob_i2_020),
    .i1(inst_dob_i2_020),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b20/B0_1 ));
  AL_MUX \inst_dob_mux_b20/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b20/B0_0 ),
    .i1(\inst_dob_mux_b20/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[20]));
  AL_MUX \inst_dob_mux_b21/al_mux_b0_0_0  (
    .i0(inst_dob_i0_021),
    .i1(inst_dob_i1_021),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b21/B0_0 ));
  AL_MUX \inst_dob_mux_b21/al_mux_b0_0_1  (
    .i0(inst_dob_i2_021),
    .i1(inst_dob_i2_021),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b21/B0_1 ));
  AL_MUX \inst_dob_mux_b21/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b21/B0_0 ),
    .i1(\inst_dob_mux_b21/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[21]));
  AL_MUX \inst_dob_mux_b22/al_mux_b0_0_0  (
    .i0(inst_dob_i0_022),
    .i1(inst_dob_i1_022),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b22/B0_0 ));
  AL_MUX \inst_dob_mux_b22/al_mux_b0_0_1  (
    .i0(inst_dob_i2_022),
    .i1(inst_dob_i2_022),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b22/B0_1 ));
  AL_MUX \inst_dob_mux_b22/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b22/B0_0 ),
    .i1(\inst_dob_mux_b22/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[22]));
  AL_MUX \inst_dob_mux_b23/al_mux_b0_0_0  (
    .i0(inst_dob_i0_023),
    .i1(inst_dob_i1_023),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b23/B0_0 ));
  AL_MUX \inst_dob_mux_b23/al_mux_b0_0_1  (
    .i0(inst_dob_i2_023),
    .i1(inst_dob_i2_023),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b23/B0_1 ));
  AL_MUX \inst_dob_mux_b23/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b23/B0_0 ),
    .i1(\inst_dob_mux_b23/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[23]));
  AL_MUX \inst_dob_mux_b24/al_mux_b0_0_0  (
    .i0(inst_dob_i0_024),
    .i1(inst_dob_i1_024),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b24/B0_0 ));
  AL_MUX \inst_dob_mux_b24/al_mux_b0_0_1  (
    .i0(inst_dob_i2_024),
    .i1(inst_dob_i2_024),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b24/B0_1 ));
  AL_MUX \inst_dob_mux_b24/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b24/B0_0 ),
    .i1(\inst_dob_mux_b24/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[24]));
  AL_MUX \inst_dob_mux_b25/al_mux_b0_0_0  (
    .i0(inst_dob_i0_025),
    .i1(inst_dob_i1_025),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b25/B0_0 ));
  AL_MUX \inst_dob_mux_b25/al_mux_b0_0_1  (
    .i0(inst_dob_i2_025),
    .i1(inst_dob_i2_025),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b25/B0_1 ));
  AL_MUX \inst_dob_mux_b25/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b25/B0_0 ),
    .i1(\inst_dob_mux_b25/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[25]));
  AL_MUX \inst_dob_mux_b26/al_mux_b0_0_0  (
    .i0(inst_dob_i0_026),
    .i1(inst_dob_i1_026),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b26/B0_0 ));
  AL_MUX \inst_dob_mux_b26/al_mux_b0_0_1  (
    .i0(inst_dob_i2_026),
    .i1(inst_dob_i2_026),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b26/B0_1 ));
  AL_MUX \inst_dob_mux_b26/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b26/B0_0 ),
    .i1(\inst_dob_mux_b26/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[26]));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_0_0  (
    .i0(inst_dob_i0_027),
    .i1(inst_dob_i1_027),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b27/B0_0 ));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_0_1  (
    .i0(inst_dob_i2_027),
    .i1(inst_dob_i2_027),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b27/B0_1 ));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b27/B0_0 ),
    .i1(\inst_dob_mux_b27/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[27]));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_0_0  (
    .i0(inst_dob_i0_028),
    .i1(inst_dob_i1_028),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b28/B0_0 ));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_0_1  (
    .i0(inst_dob_i2_028),
    .i1(inst_dob_i2_028),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b28/B0_1 ));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b28/B0_0 ),
    .i1(\inst_dob_mux_b28/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[28]));
  AL_MUX \inst_dob_mux_b29/al_mux_b0_0_0  (
    .i0(inst_dob_i0_029),
    .i1(inst_dob_i1_029),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b29/B0_0 ));
  AL_MUX \inst_dob_mux_b29/al_mux_b0_0_1  (
    .i0(inst_dob_i2_029),
    .i1(inst_dob_i2_029),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b29/B0_1 ));
  AL_MUX \inst_dob_mux_b29/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b29/B0_0 ),
    .i1(\inst_dob_mux_b29/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[29]));
  AL_MUX \inst_dob_mux_b3/al_mux_b0_0_0  (
    .i0(inst_dob_i0_003),
    .i1(inst_dob_i1_003),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b3/B0_0 ));
  AL_MUX \inst_dob_mux_b3/al_mux_b0_0_1  (
    .i0(inst_dob_i2_003),
    .i1(inst_dob_i2_003),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b3/B0_1 ));
  AL_MUX \inst_dob_mux_b3/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b3/B0_0 ),
    .i1(\inst_dob_mux_b3/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[3]));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_0_0  (
    .i0(inst_dob_i0_030),
    .i1(inst_dob_i1_030),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b30/B0_0 ));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_0_1  (
    .i0(inst_dob_i2_030),
    .i1(inst_dob_i2_030),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b30/B0_1 ));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b30/B0_0 ),
    .i1(\inst_dob_mux_b30/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[30]));
  AL_MUX \inst_dob_mux_b31/al_mux_b0_0_0  (
    .i0(inst_dob_i0_031),
    .i1(inst_dob_i1_031),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b31/B0_0 ));
  AL_MUX \inst_dob_mux_b31/al_mux_b0_0_1  (
    .i0(inst_dob_i2_031),
    .i1(inst_dob_i2_031),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b31/B0_1 ));
  AL_MUX \inst_dob_mux_b31/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b31/B0_0 ),
    .i1(\inst_dob_mux_b31/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[31]));
  AL_MUX \inst_dob_mux_b4/al_mux_b0_0_0  (
    .i0(inst_dob_i0_004),
    .i1(inst_dob_i1_004),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b4/B0_0 ));
  AL_MUX \inst_dob_mux_b4/al_mux_b0_0_1  (
    .i0(inst_dob_i2_004),
    .i1(inst_dob_i2_004),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b4/B0_1 ));
  AL_MUX \inst_dob_mux_b4/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b4/B0_0 ),
    .i1(\inst_dob_mux_b4/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[4]));
  AL_MUX \inst_dob_mux_b5/al_mux_b0_0_0  (
    .i0(inst_dob_i0_005),
    .i1(inst_dob_i1_005),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b5/B0_0 ));
  AL_MUX \inst_dob_mux_b5/al_mux_b0_0_1  (
    .i0(inst_dob_i2_005),
    .i1(inst_dob_i2_005),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b5/B0_1 ));
  AL_MUX \inst_dob_mux_b5/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b5/B0_0 ),
    .i1(\inst_dob_mux_b5/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[5]));
  AL_MUX \inst_dob_mux_b6/al_mux_b0_0_0  (
    .i0(inst_dob_i0_006),
    .i1(inst_dob_i1_006),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b6/B0_0 ));
  AL_MUX \inst_dob_mux_b6/al_mux_b0_0_1  (
    .i0(inst_dob_i2_006),
    .i1(inst_dob_i2_006),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b6/B0_1 ));
  AL_MUX \inst_dob_mux_b6/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b6/B0_0 ),
    .i1(\inst_dob_mux_b6/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[6]));
  AL_MUX \inst_dob_mux_b7/al_mux_b0_0_0  (
    .i0(inst_dob_i0_007),
    .i1(inst_dob_i1_007),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b7/B0_0 ));
  AL_MUX \inst_dob_mux_b7/al_mux_b0_0_1  (
    .i0(inst_dob_i2_007),
    .i1(inst_dob_i2_007),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b7/B0_1 ));
  AL_MUX \inst_dob_mux_b7/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b7/B0_0 ),
    .i1(\inst_dob_mux_b7/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[7]));
  AL_MUX \inst_dob_mux_b8/al_mux_b0_0_0  (
    .i0(inst_dob_i0_008),
    .i1(inst_dob_i1_008),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b8/B0_0 ));
  AL_MUX \inst_dob_mux_b8/al_mux_b0_0_1  (
    .i0(inst_dob_i2_008),
    .i1(inst_dob_i2_008),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b8/B0_1 ));
  AL_MUX \inst_dob_mux_b8/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b8/B0_0 ),
    .i1(\inst_dob_mux_b8/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[8]));
  AL_MUX \inst_dob_mux_b9/al_mux_b0_0_0  (
    .i0(inst_dob_i0_009),
    .i1(inst_dob_i1_009),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b9/B0_0 ));
  AL_MUX \inst_dob_mux_b9/al_mux_b0_0_1  (
    .i0(inst_dob_i2_009),
    .i1(inst_dob_i2_009),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b9/B0_1 ));
  AL_MUX \inst_dob_mux_b9/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b9/B0_0 ),
    .i1(\inst_dob_mux_b9/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[9]));

endmodule 

