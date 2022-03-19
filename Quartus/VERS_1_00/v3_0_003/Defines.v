
//`define TEST_AIN_WIHOT_MATRIX
//`define TEST_WITHOUT_MEMORY
//`define TEST_COUNT_GLOBAL_DATA
//v3.0.002 - ������ `define MAKET_1 ��� ����������� ������� ������ � �������� �������
//v3.0.003 - ��������� ������ ����� always@ ������������� � AIN_BRD
//`define _576_pix
//------------------------------------------------------//
//******************************************************//
//------------------------------------------------------//
`define _HARD_VERS_1152 8'h6 

`define _HARD_VERS_576  8'h7 

`ifndef _576_pix
	`define _HARD_VERS  `_HARD_VERS_1152
`else	
   `define _HARD_VERS  `_HARD_VERS_576
`endif

`ifdef TEST_WITHOUT_MEMORY
          `define TEST
`endif
`ifdef TEST_COUNT_GLOBAL_DATA
			 `define TEST 
`endif
//������ ��������������� ���� ���������� ������
`define  D_S_WIDTH_OF_DATA_8   8
//������ �������������������� ���� 
`define  D_S_WIDTH_OF_DATA_AD_16   16	

//������ ���������-���������� 
`define  D_S_WIDTH_OF_BUFER_AD_24  24//22

//����� �������� ������ ��� �������������� ��������� � ���������� ������
`define  D_S_ADRES_COMMAND_03 3'h03 

//������� - GetVersion		 
`define  D_S_COMMAND_GET_VERSION_001 8'h01 
//���������� ������������ ���� ������� GetVersion
`define  D_S_NUM_BYTE_GET_VERSION_017 17//20 

//��� ��������� ������� GetVersion
`define  D_VERSION_BIT_state_0 	state[0]
`define  D_VERSION_BIT_SET_state_0 		16'h01


//������� ��� ��������� � ���������� ������
`define D_GET_INFO_STATE   ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0)& (RB15_PMPA0 == 1) & (RD5_PMPRD == 1)) 
`define D_GET_INFO_VERSION ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0) & (RB15_PMPA0 == 0) & (RD5_PMPRD == 1))
`define D_GET_DATA_XRAY    ((RF6_ALTCS == 0) & (CS_READ_MTR_RF7 == 1) )

//����� �������� ����� ������� TRIG ����� ������������ MTR)RST � ������ ��� ������
`define  D_COUNT_WAITE_TRIG_GO_5 5'h18 //3'h5

//������ �������� MTR_RST
`define  D_WIDTH_OF_COUNTER_MTR_RST 5

//������������ ������� MTR_RST
`define  D_SZ_MTR_RST `D_WIDTH_OF_COUNTER_MTR_RST'h0b 
//`define  D_SZ_MTR_RST `D_WIDTH_OF_COUNTER_MTR_RST'h16 


//������ ���� adres_in ������ ������
`define  D_WIDTH_OF_ADRES_IN_11 11 
/*
//���������� ���� ������
`define  D_NUM_WORD_OF_ADRES_IN_1152  1152  //180 72
*/
//������ ���� adres_out ������ ������
`define  D_WIDTH_OF_ADRES_OUT_11 11

//�������� ������ ���������� ������� adres_out
`define  D_OFFS_ADRES_OUT_0  `D_WIDTH_OF_ADRES_OUT_11'h00 //01


`ifndef _576_pix
//���������� ���� ������
`define  D_NUM_WORD_OF_ADRES_IN_1152  1152  //180 72
`define  D_END_ADRES_OUT_1152   1151  
`define  D_NUM_COUNT_TEST_GLOBAL_DATA 1152
`else
//���������� ���� ������
`define  D_NUM_WORD_OF_ADRES_IN_1152  576  
`define  D_END_ADRES_OUT_1152   575
`define  D_NUM_COUNT_TEST_GLOBAL_DATA 576
`endif

`define D_USE_PLL_SYCLONE_1


//������� ����������� ��� ������������ ��������� ��������� ������ ���������� ������
`define PURE_SYCLE_CS_ADC  ((count_m10 != 7'h2) & (count_m10 != 7'h3) & (count_m10 != 7'h1) &  (count_m10 != 7'h0))
`define PURE_SYCLE_BYTESWAP_6  ((count_m10 != 7'h2) & (count_m10 != 7'h3))
`define PURE_SYCLE_CONVECT  ((count_m10 != 7'h1) & (count_m10 != 7'h2))

`define MAKET_1 

`ifdef MAKET_1
   `define _RESET 0
	`define _SET 1
`else
   `define _RESET 1
	`define _SET 0
`endif
