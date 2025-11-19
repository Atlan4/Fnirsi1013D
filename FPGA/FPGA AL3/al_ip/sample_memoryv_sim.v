// Verilog netlist created by TD v4.6.116866
// Sat Jan 25 08:18:34 2025

`timescale 1ns / 1ps
module sample_memory  // al_ip/sample_memoryv.v(14)
  (
  addra,
  addrb,
  cea,
  ceb,
  clka,
  clkb,
  dia,
  dob
  );

  input [11:0] addra;  // al_ip/sample_memoryv.v(23)
  input [11:0] addrb;  // al_ip/sample_memoryv.v(24)
  input cea;  // al_ip/sample_memoryv.v(25)
  input ceb;  // al_ip/sample_memoryv.v(26)
  input clka;  // al_ip/sample_memoryv.v(27)
  input clkb;  // al_ip/sample_memoryv.v(28)
  input [31:0] dia;  // al_ip/sample_memoryv.v(22)
  output [31:0] dob;  // al_ip/sample_memoryv.v(19)

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
  wire inst_dob_i3_000;
  wire inst_dob_i3_001;
  wire inst_dob_i3_002;
  wire inst_dob_i3_003;
  wire inst_dob_i3_004;
  wire inst_dob_i3_005;
  wire inst_dob_i3_006;
  wire inst_dob_i3_007;
  wire inst_dob_i3_008;
  wire inst_dob_i3_009;
  wire inst_dob_i3_010;
  wire inst_dob_i3_011;
  wire inst_dob_i3_012;
  wire inst_dob_i3_013;
  wire inst_dob_i3_014;
  wire inst_dob_i3_015;
  wire inst_dob_i3_016;
  wire inst_dob_i3_017;
  wire inst_dob_i3_018;
  wire inst_dob_i3_019;
  wire inst_dob_i3_020;
  wire inst_dob_i3_021;
  wire inst_dob_i3_022;
  wire inst_dob_i3_023;
  wire inst_dob_i3_024;
  wire inst_dob_i3_025;
  wire inst_dob_i3_026;

  reg_ar_as_w1 addrb_pipe_b0 (
    .clk(clkb),
    .d(addrb[10]),
    .en(ceb),
    .reset(1'b0),
    .set(1'b0),
    .q(addrb_piped[0]));
  reg_ar_as_w1 addrb_pipe_b1 (
    .clk(clkb),
    .d(addrb[11]),
    .en(ceb),
    .reset(1'b0),
    .set(1'b0),
    .q(addrb_piped[1]));
  // address_offset=0;data_offset=0;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_000 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n0,addra[11:10]}),
    .csb({open_n1,addrb[11:10]}),
    .dia(dia[8:0]),
    .dob({inst_dob_i0_008,inst_dob_i0_007,inst_dob_i0_006,inst_dob_i0_005,inst_dob_i0_004,inst_dob_i0_003,inst_dob_i0_002,inst_dob_i0_001,inst_dob_i0_000}));
  // address_offset=0;data_offset=9;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_009 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n26,addra[11:10]}),
    .csb({open_n27,addrb[11:10]}),
    .dia(dia[17:9]),
    .dob({inst_dob_i0_017,inst_dob_i0_016,inst_dob_i0_015,inst_dob_i0_014,inst_dob_i0_013,inst_dob_i0_012,inst_dob_i0_011,inst_dob_i0_010,inst_dob_i0_009}));
  // address_offset=0;data_offset=18;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_018 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n52,addra[11:10]}),
    .csb({open_n53,addrb[11:10]}),
    .dia(dia[26:18]),
    .dob({inst_dob_i0_026,inst_dob_i0_025,inst_dob_i0_024,inst_dob_i0_023,inst_dob_i0_022,inst_dob_i0_021,inst_dob_i0_020,inst_dob_i0_019,inst_dob_i0_018}));
  // address_offset=0;data_offset=27;depth=4096;width=2;num_section=1;width_per_section=2;section_size=32;working_depth=4096;working_width=2;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("1"),
    .CSA1("1"),
    .CSA2("1"),
    .CSB0("1"),
    .CSB1("1"),
    .CSB2("1"),
    .DATA_WIDTH_A("2"),
    .DATA_WIDTH_B("2"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_027 (
    .addra({addra,1'b1}),
    .addrb({addrb,1'b1}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .dia({open_n84,open_n85,open_n86,dia[28],open_n87,open_n88,dia[27],open_n89,open_n90}),
    .dob({open_n115,open_n116,open_n117,open_n118,open_n119,open_n120,open_n121,inst_dob_i0_028,inst_dob_i0_027}));
  // address_offset=0;data_offset=29;depth=4096;width=2;num_section=1;width_per_section=2;section_size=32;working_depth=4096;working_width=2;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("1"),
    .CSA1("1"),
    .CSA2("1"),
    .CSB0("1"),
    .CSB1("1"),
    .CSB2("1"),
    .DATA_WIDTH_A("2"),
    .DATA_WIDTH_B("2"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_029 (
    .addra({addra,1'b1}),
    .addrb({addrb,1'b1}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .dia({open_n128,open_n129,open_n130,dia[30],open_n131,open_n132,dia[29],open_n133,open_n134}),
    .dob({open_n159,open_n160,open_n161,open_n162,open_n163,open_n164,open_n165,inst_dob_i0_030,inst_dob_i0_029}));
  // address_offset=0;data_offset=31;depth=4096;width=1;num_section=1;width_per_section=1;section_size=32;working_depth=4096;working_width=2;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("1"),
    .CSA1("1"),
    .CSA2("1"),
    .CSB0("1"),
    .CSB1("1"),
    .CSB2("1"),
    .DATA_WIDTH_A("2"),
    .DATA_WIDTH_B("2"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_000000_031 (
    .addra({addra,1'b1}),
    .addrb({addrb,1'b1}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .dia({open_n172,open_n173,open_n174,open_n175,open_n176,open_n177,dia[31],open_n178,open_n179}),
    .dob({open_n204,open_n205,open_n206,open_n207,open_n208,open_n209,open_n210,open_n211,inst_dob_i0_031}));
  // address_offset=1024;data_offset=0;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_001024_000 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n212,addra[11:10]}),
    .csb({open_n213,addrb[11:10]}),
    .dia(dia[8:0]),
    .dob({inst_dob_i1_008,inst_dob_i1_007,inst_dob_i1_006,inst_dob_i1_005,inst_dob_i1_004,inst_dob_i1_003,inst_dob_i1_002,inst_dob_i1_001,inst_dob_i1_000}));
  // address_offset=1024;data_offset=9;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_001024_009 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n238,addra[11:10]}),
    .csb({open_n239,addrb[11:10]}),
    .dia(dia[17:9]),
    .dob({inst_dob_i1_017,inst_dob_i1_016,inst_dob_i1_015,inst_dob_i1_014,inst_dob_i1_013,inst_dob_i1_012,inst_dob_i1_011,inst_dob_i1_010,inst_dob_i1_009}));
  // address_offset=1024;data_offset=18;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("INV"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("INV"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_001024_018 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n264,addra[11:10]}),
    .csb({open_n265,addrb[11:10]}),
    .dia(dia[26:18]),
    .dob({inst_dob_i1_026,inst_dob_i1_025,inst_dob_i1_024,inst_dob_i1_023,inst_dob_i1_022,inst_dob_i1_021,inst_dob_i1_020,inst_dob_i1_019,inst_dob_i1_018}));
  // address_offset=2048;data_offset=0;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_002048_000 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n290,addra[11:10]}),
    .csb({open_n291,addrb[11:10]}),
    .dia(dia[8:0]),
    .dob({inst_dob_i2_008,inst_dob_i2_007,inst_dob_i2_006,inst_dob_i2_005,inst_dob_i2_004,inst_dob_i2_003,inst_dob_i2_002,inst_dob_i2_001,inst_dob_i2_000}));
  // address_offset=2048;data_offset=9;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_002048_009 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n316,addra[11:10]}),
    .csb({open_n317,addrb[11:10]}),
    .dia(dia[17:9]),
    .dob({inst_dob_i2_017,inst_dob_i2_016,inst_dob_i2_015,inst_dob_i2_014,inst_dob_i2_013,inst_dob_i2_012,inst_dob_i2_011,inst_dob_i2_010,inst_dob_i2_009}));
  // address_offset=2048;data_offset=18;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("INV"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("INV"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_002048_018 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n342,addra[11:10]}),
    .csb({open_n343,addrb[11:10]}),
    .dia(dia[26:18]),
    .dob({inst_dob_i2_026,inst_dob_i2_025,inst_dob_i2_024,inst_dob_i2_023,inst_dob_i2_022,inst_dob_i2_021,inst_dob_i2_020,inst_dob_i2_019,inst_dob_i2_018}));
  // address_offset=3072;data_offset=0;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_003072_000 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n368,addra[11:10]}),
    .csb({open_n369,addrb[11:10]}),
    .dia(dia[8:0]),
    .dob({inst_dob_i3_008,inst_dob_i3_007,inst_dob_i3_006,inst_dob_i3_005,inst_dob_i3_004,inst_dob_i3_003,inst_dob_i3_002,inst_dob_i3_001,inst_dob_i3_000}));
  // address_offset=3072;data_offset=9;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_003072_009 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n394,addra[11:10]}),
    .csb({open_n395,addrb[11:10]}),
    .dia(dia[17:9]),
    .dob({inst_dob_i3_017,inst_dob_i3_016,inst_dob_i3_015,inst_dob_i3_014,inst_dob_i3_013,inst_dob_i3_012,inst_dob_i3_011,inst_dob_i3_010,inst_dob_i3_009}));
  // address_offset=3072;data_offset=18;depth=1024;width=9;num_section=1;width_per_section=9;section_size=32;working_depth=1024;working_width=9;address_step=1;bytes_in_per_section=1;
  AL_PHY_BRAM #(
    .CSA0("SIG"),
    .CSA1("SIG"),
    .CSA2("1"),
    .CSB0("SIG"),
    .CSB1("SIG"),
    .CSB2("1"),
    .DATA_WIDTH_A("9"),
    .DATA_WIDTH_B("9"),
    .MODE("DP8K"),
    .OCEAMUX("0"),
    .OCEBMUX("0"),
    .REGMODE_A("NOREG"),
    .REGMODE_B("NOREG"),
    .RESETMODE("SYNC"),
    .RSTAMUX("0"),
    .RSTBMUX("0"),
    .WEAMUX("1"),
    .WEBMUX("0"),
    .WRITEMODE_A("NORMAL"),
    .WRITEMODE_B("NORMAL"))
    inst_4096x32_sub_003072_018 (
    .addra({addra[9:0],3'b111}),
    .addrb({addrb[9:0],3'b111}),
    .cea(cea),
    .ceb(ceb),
    .clka(clka),
    .clkb(clkb),
    .csa({open_n420,addra[11:10]}),
    .csb({open_n421,addrb[11:10]}),
    .dia(dia[26:18]),
    .dob({inst_dob_i3_026,inst_dob_i3_025,inst_dob_i3_024,inst_dob_i3_023,inst_dob_i3_022,inst_dob_i3_021,inst_dob_i3_020,inst_dob_i3_019,inst_dob_i3_018}));
  AL_MUX \inst_dob_mux_b0/al_mux_b0_0_0  (
    .i0(inst_dob_i0_000),
    .i1(inst_dob_i1_000),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b0/B0_0 ));
  AL_MUX \inst_dob_mux_b0/al_mux_b0_0_1  (
    .i0(inst_dob_i2_000),
    .i1(inst_dob_i3_000),
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
    .i1(inst_dob_i3_001),
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
    .i1(inst_dob_i3_010),
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
    .i1(inst_dob_i3_011),
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
    .i1(inst_dob_i3_012),
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
    .i1(inst_dob_i3_013),
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
    .i1(inst_dob_i3_014),
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
    .i1(inst_dob_i3_015),
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
    .i1(inst_dob_i3_016),
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
    .i1(inst_dob_i3_017),
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
    .i1(inst_dob_i3_018),
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
    .i1(inst_dob_i3_019),
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
    .i1(inst_dob_i3_002),
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
    .i1(inst_dob_i3_020),
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
    .i1(inst_dob_i3_021),
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
    .i1(inst_dob_i3_022),
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
    .i1(inst_dob_i3_023),
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
    .i1(inst_dob_i3_024),
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
    .i1(inst_dob_i3_025),
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
    .i1(inst_dob_i3_026),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b26/B0_1 ));
  AL_MUX \inst_dob_mux_b26/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b26/B0_0 ),
    .i1(\inst_dob_mux_b26/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[26]));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_0_0  (
    .i0(inst_dob_i0_027),
    .i1(inst_dob_i0_027),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b27/B0_0 ));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_0_1  (
    .i0(inst_dob_i0_027),
    .i1(inst_dob_i0_027),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b27/B0_1 ));
  AL_MUX \inst_dob_mux_b27/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b27/B0_0 ),
    .i1(\inst_dob_mux_b27/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[27]));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_0_0  (
    .i0(inst_dob_i0_028),
    .i1(inst_dob_i0_028),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b28/B0_0 ));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_0_1  (
    .i0(inst_dob_i0_028),
    .i1(inst_dob_i0_028),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b28/B0_1 ));
  AL_MUX \inst_dob_mux_b28/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b28/B0_0 ),
    .i1(\inst_dob_mux_b28/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[28]));
  AL_MUX \inst_dob_mux_b29/al_mux_b0_0_0  (
    .i0(inst_dob_i0_029),
    .i1(inst_dob_i0_029),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b29/B0_0 ));
  AL_MUX \inst_dob_mux_b29/al_mux_b0_0_1  (
    .i0(inst_dob_i0_029),
    .i1(inst_dob_i0_029),
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
    .i1(inst_dob_i3_003),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b3/B0_1 ));
  AL_MUX \inst_dob_mux_b3/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b3/B0_0 ),
    .i1(\inst_dob_mux_b3/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[3]));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_0_0  (
    .i0(inst_dob_i0_030),
    .i1(inst_dob_i0_030),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b30/B0_0 ));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_0_1  (
    .i0(inst_dob_i0_030),
    .i1(inst_dob_i0_030),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b30/B0_1 ));
  AL_MUX \inst_dob_mux_b30/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b30/B0_0 ),
    .i1(\inst_dob_mux_b30/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[30]));
  AL_MUX \inst_dob_mux_b31/al_mux_b0_0_0  (
    .i0(inst_dob_i0_031),
    .i1(inst_dob_i0_031),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b31/B0_0 ));
  AL_MUX \inst_dob_mux_b31/al_mux_b0_0_1  (
    .i0(inst_dob_i0_031),
    .i1(inst_dob_i0_031),
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
    .i1(inst_dob_i3_004),
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
    .i1(inst_dob_i3_005),
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
    .i1(inst_dob_i3_006),
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
    .i1(inst_dob_i3_007),
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
    .i1(inst_dob_i3_008),
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
    .i1(inst_dob_i3_009),
    .sel(addrb_piped[0]),
    .o(\inst_dob_mux_b9/B0_1 ));
  AL_MUX \inst_dob_mux_b9/al_mux_b0_1_0  (
    .i0(\inst_dob_mux_b9/B0_0 ),
    .i1(\inst_dob_mux_b9/B0_1 ),
    .sel(addrb_piped[1]),
    .o(dob[9]));

endmodule 

