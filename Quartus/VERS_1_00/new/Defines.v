

//`define TEST_WITHOUT_MEMORY
//`define TEST_COUNT_GLOBAL_DATA

`ifdef TEST_WITHOUT_MEMORY
          `define TEST
`endif
`ifdef TEST_COUNT_GLOBAL_DATA
			 `define TEST 
`endif

`define  D_S_WIDTH_OF_DATA_8   8
`define  D_S_WIDTH_OF_DATA_AD_16   16	
`define  D_S_WIDTH_OF_BUFER_AD_20  20
`define  D_S_ADRES_COMMAND_03 3'h03 		 
`define  D_S_COMMAND_GET_VERSION_001 8'h01 

`define  D_S_NUM_BYTE_GET_VERSION_017 17//20 
`define  D_VERSION_BIT_state_0 	state[0]
`define  D_VERSION_BIT_SET_state_0 		16'h01



`define D_GET_INFO_STATE   ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0)& (RB15_PMPA0 == 1) & (RD5_PMPRD == 1)) 
`define D_GET_INFO_VERSION ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0) & (RB15_PMPA0 == 0) & (RD5_PMPRD == 1))
`define D_GET_DATA_XRAY    ((RF6_ALTCS == 0) & (CS_READ_MTR_RF7 == 1) )


`define  D_COUNT_WAITE_TRIG_GO_5 3'h5

`define  D_WIDTH_OF_COUNTER_MTR_RST 5
`define  D_SZ_MTR_RST `D_WIDTH_OF_COUNTER_MTR_RST'h0b 

`define  D_WIDTH_OF_ADRES_IN_11 11 
`define  D_NUM_WOPRD_OF_ADRES_IN_1152  1152  //180 72

`define  D_WIDTH_OF_ADRES_OUT_11 11
`define  D_OFFS_ADRES_OUT_0  `D_WIDTH_OF_ADRES_OUT_11'h00 //01
`define  D_END_ADRES_OUT_1152   1151  

`define D_USE_PLL_SYCLONE_1
