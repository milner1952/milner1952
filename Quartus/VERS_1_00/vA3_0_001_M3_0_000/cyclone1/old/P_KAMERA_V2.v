`include "../../SRC/Quartus/VERS_1_00/Defines.v"





`define  D_WIDTH_OF_DATA  16
`define  D_WIDTH_OF_ADRES_OUT  11
`define  D_WIDTH_OF_ADRES_IN  12
 


								
module P_KAMERA_V2( 
					/*input [`D_WIDTH_OF_DATA - 1:0]BAS_ADC,*/ input _TRIG, 
					input END, input PIC_SINXRO_RF12, input  TEST_GO_RD0, 
					input _RUN_RF8, input RES_HARD_RF13,
					input STRING_SIZE_RB0, input RD5_PMRD, input RA5_PMCS, 
					input RD4_PMWR, input CLK, input CS_READ_MTR_RF7, 
					input RB15_PMA0, input RX_P_S, input U1RX_RF2, 
					input U1RXALT, input CLK2,
 
					output _RST_ADC, output  _CS_ADC, 
					output reg INTERUPT_1_TOO_SILABS_P03, 
					
					inout [`D_WIDTH_OF_DATA - 1 : 0]BAS_AD,  
					output TEST_TRIG, output _TEST_END,  output ENC_RD, output MTR_CLK, 
					output ENC_WR,  
					output WR_BUF_ADC, output _RD_ADC, output INT_RESET_RE8, 
					output _CONVECT_ADC, output _MTR_RST, 
					output RX232, output _RD_BUF_ADC0, 
				/*	output _RD_BUF_ADC1, output _RD_BUF_ADC2, 
					output _RD_BUF_ADC3, output _RD_BUF_ADC4, 
					output _RD_BUF_ADC5, output _RD_BUF_ADC6, 
					output _RD_BUF_ADC7, output _RD_BUF_ADC8,
					*/
					input VERS_DT_RF6, output OUTPLL,
										
					output T_MEM1_OUT_134,
					output T_MEM2_OUT_139,
					output T_KM_125, 
					output T_KM_140,
					output T_AIN_119,
					output T_AOUT_120,
					output T_SINXRO_121,
					output T_TEST_122
				 ); 

				 

		 
reg _trig_0, _trig_1, _trig_2, end_0, end_1,end_2,   mx_board/*, pic_sinxro*/; 
reg [4:0]temp;																	 
wire [7 : 0]pd;
wire [7 : 0]pd0;
wire [7 : 0]pd1;
wire [7 : 0]pd2;

  

wire a0_board,	a1_board, a2_board, a3_board, a4_board, trig_board0, trig_board1, trig_board2, _rd_board, _wr_board ,  dir,_test_trig ;
wire cs0,cs1,cs2/* synthesis keep */; 
				 
wire _rst_adc0,  _convect_adc0,  end_board0,  LOCK0,  V_GANE0;
wire MX0, _0CS_ADC0, _1CS_ADC0, _2CS_ADC0, _BYTESWAP0, T01, T02, T03;				 
				 
wire _rst_adc1,  _convect_adc1,  end_board1,  LOCK1,  V_GANE1;
wire MX1, _0CS_ADC1, _1CS_ADC1, _2CS_ADC1, _BYTESWAP1, T11, T12, T13;

wire _rst_adc2,  _convect_adc2,  end_board2,  LOCK2,  V_GANE2;
wire MX2, _0CS_ADC2, _1CS_ADC2, _2CS_ADC2, _BYTESWAP2, T21, T22, T23;



wire _TEST_END_;
wire out_pll;

//reg [12 : 0]cnt_pic_sinxro;

assign _CS_ADC = 0;


wire  [`D_WIDTH_OF_DATA - 1:0]BAS_ADC;

assign T_KM_140 = T23;
assign T_MEM2_OUT_139 = T13;
assign T_MEM1_OUT_134 = T03; //out_pll;


assign T_AOUT_120 = T02;
assign T_SINXRO_121 = T12;
assign T_TEST_122 = T22;


assign _TEST_END = _TEST_END_;
assign TEST_TRIG =  _test_trig;//_trig_0;

assign _RD_BUF_ADC0 = LOCK0;

assign _CONVECT_ADC = _convect_adc0;
//------------------------------------------------------------
always @(posedge RES_HARD_RF13/*, posedge MTR_CLK*/)
begin
if(RES_HARD_RF13 == 1)
  begin
   _trig_0 = 0; _trig_1 = 0; _trig_2 = 0;
	end_0 = 0; end_1 = 0; end_2 = 0; /*pic_sinxro = 0;*/ /*cnt_pic_sinxro = 13'h0;*/
	mx_board = 0;  
  end
  /*
  else
	if(MTR_CLK == 1)
	begin
	
	  //if(_MTR_RST == 0)
	   //begin
	    // cnt_pic_sinxro = 13'h0;
		//end
		//else
		
		//begin
		  cnt_pic_sinxro = cnt_pic_sinxro + 13'h1;
		  if(cnt_pic_sinxro == 4620)
		  begin
				pic_sinxro = 1;
		  end
		  else
		  if(cnt_pic_sinxro == 4625)
		  begin
				pic_sinxro = 0;
				cnt_pic_sinxro = 13'h0;
		  end
		  
		//end
	end
  */
end	


//------------------------------------------------------------
/*
always @(posedge RES_HARD_RF13, posedge out_pll)
begin
if(RES_HARD_RF13 == 1)
  begin
 temp = 0;
  end
  
  else
	if(out_pll == 1)
	begin
	temp = temp + 5'h1;
	 
	end
  
end		
*/
		  
		
	
	
S_AIN_BRD 				   s_ain_brd0( BAS_ADC[7 : 0], 		MTR_CLK, 
												_MTR_RST,	_trig_0, 			end_0, 
												_trig_1, 	end_1, 				_trig_2, 
												end_2, 		_test_trig, 		   _TEST_END_, 
												1'h0,			mx_board, 			_rd_board, 
												_wr_board, 	end_board0,  	   a0_board,	
												a1_board, 	a2_board, 			a3_board, 
												a4_board,	RES_HARD_RF13, 	out_pll, 
												CLK,			2'h0,		
												pd0, 
												_rst_adc0,_convect_adc0, trig_board0, LOCK0, V_GANE0, MX0, 
												_0CS_ADC0, _1CS_ADC0,_2CS_ADC0,_BYTESWAP0,																							 
												T01, T02, T03//, komand_reg0
												);
												
												
S_AIN_BRD 				   s_ain_brd1( BAS_ADC[7 : 0], 		MTR_CLK, 
												_MTR_RST,	_trig_0, 			end_0, 
												_trig_1, 	end_1, 				_trig_2, 
												end_2, 		_test_trig, 	   	_TEST_END_, 
												1'h0,			mx_board, 			_rd_board, 
												_wr_board, 	end_board1,  	   a0_board,	
												a1_board, 	a2_board, 			a3_board, 
												a4_board,	RES_HARD_RF13, 	out_pll, 
												CLK,			2'h1,		
												pd1, 
												_rst_adc1,_convect_adc1, trig_board1, LOCK1, V_GANE1, MX1, 
												_0CS_ADC1, _1CS_ADC1,_2CS_ADC1,_BYTESWAP1,																							 
												T11, T12, T13//,komand_reg1
												);												
												
												
S_AIN_BRD 				   s_ain_brd2(  BAS_ADC[7 : 0], 		MTR_CLK, 
												_MTR_RST,	_trig_0, 			end_0, 
												_trig_1, 	end_1, 				_trig_2, 
												end_2, 		_test_trig, 	   	_TEST_END_, 
												1'h0,			mx_board, 			_rd_board, 
												_wr_board, 	end_board2,  	a0_board,	
												a1_board, 	a2_board, 			a3_board, 
												a4_board,	RES_HARD_RF13, 	out_pll, 
												CLK,			2'h2,		
												pd2, 
												_rst_adc2,_convect_adc2, trig_board2, LOCK2, V_GANE2, MX2, 
												_0CS_ADC2, _1CS_ADC2,_2CS_ADC2,_BYTESWAP2,																							 
												T21, T22, T23//,komand_reg2
												);													
												
												

S_MOTHER_BRD            s_mother_brd( pd, 
												  BAS_AD,
												  RES_HARD_RF13, 
												  RD5_PMRD, RD4_PMWR, PIC_SINXRO_RF12, _RUN_RF8, CS_READ_MTR_RF7, RB15_PMA0,
												  end_board0, end_board1, end_board2, TEST_GO_RD0, VERS_DT_RF6, CLK, CLK,	
												  trig_board0, trig_board1, trig_board2,
												  MTR_CLK, _MTR_RST, _rd_board, _wr_board, req_board_rd, 
												  a0_board,	a1_board, a2_board, a3_board, a4_board, dir, cs0, cs1, cs2, _test_trig , _TEST_END_,																													
                                      ENC_WR,ENC_RD, INT_RESET_RE8, 	T_AIN_119,  T_KM_125																												
												  );
												
									
								
F_16                    _0F_16(0,0,dir,cs0, A_0,C_0,pd0,pd);	
F_16                    _1F_16(0,0,dir,cs1, A_1,C_1,pd1,pd);
F_16                    _2F_16(0,0,dir,cs2, A_2,C_2,pd2,pd);
	


								
`ifdef D_USE_PLL_SYCLONE_1		
PLLMUL10  altpll(CLK2, out_pll);		
`endif
		

												  
												  
												 											  
												  
endmodule 

