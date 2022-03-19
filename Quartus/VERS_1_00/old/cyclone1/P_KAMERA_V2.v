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
					output  INTERUPT_1_TOO_SILABS_P03, 
					
					inout [`D_WIDTH_OF_DATA - 1 : 0]BAS_AD,  
					output TEST_TRIG, output _TEST_END,  output ENC_RD, output MTR_CLK, 
					output ENC_WR,  
					output WR_BUF_ADC, output _RD_ADC, output INT_RESET_RE8, 
					output _CONVECT_ADC, output _MTR_RST, 
					output RX232, output _RD_BUF_ADC0, 

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

				 

		 																 
wire [7 : 0]pd;
wire end_board0,end_board1,end_board2;
wire trig_board0, trig_board1, trig_board2;  
wire a0_board,	a1_board, a2_board, a3_board, a4_board, dir, cs0, cs1, cs2, _test_trig, test_end;


assign end_board0 = test_end;
assign end_board1 = test_end;
assign end_board2 = test_end;


assign trig_board0 = _test_trig;
assign trig_board1 = _test_trig;
assign trig_board2 = _test_trig;



S_MOTHER_BRD            s_mother_brd( pd, 
												  BAS_AD,
												  RES_HARD_RF13, 
												  RD5_PMRD, RD4_PMWR, PIC_SINXRO_RF12, _RUN_RF8, CS_READ_MTR_RF7, RB15_PMA0,
												  end_board0, end_board1, end_board2, TEST_GO_RD0, VERS_DT_RF6, CLK, CLK,	
												  trig_board0, trig_board1, trig_board2,
												  MTR_CLK, _MTR_RST, _rd_board, _wr_board, INTERUPT_1_TOO_SILABS_P03, 
												  a0_board,	a1_board, a2_board, a3_board, a4_board, dir, cs0, cs1, cs2, _test_trig , test_end,																												
                                      ENC_WR,ENC_RD, INT_RESET_RE8, 	T_AIN_119,  T_KM_125																												
												  );
												
									
										 											  
												  
endmodule 

