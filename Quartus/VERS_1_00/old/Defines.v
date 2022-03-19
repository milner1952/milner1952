
//`define TEST_AIN_WIHOT_MATRIX
//`define TEST_WITHOUT_MEMORY
`define TEST_COUNT_GLOBAL_DATA

`define _576_pix
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
//Ширина восьмиразрядной шины аналоговых блоков
`define  D_S_WIDTH_OF_DATA_8   8
//Ширина шестнадцатиразрядной шины 
`define  D_S_WIDTH_OF_DATA_AD_16   16	

//Ширина регистров-сумматоров 
`define  D_S_WIDTH_OF_BUFER_AD_24  24//22

//Адрес регистра команд при индивидуальном обращении к аналоговым блокам
`define  D_S_ADRES_COMMAND_03 3'h03 

//Команда - GetVersion		 
`define  D_S_COMMAND_GET_VERSION_001 8'h01 
//Количество возвращаемых байт команды GetVersion
`define  D_S_NUM_BYTE_GET_VERSION_017 17//20 

//Бит состояния команды GetVersion
`define  D_VERSION_BIT_state_0 	state[0]
`define  D_VERSION_BIT_SET_state_0 		16'h01


//Макросы при обращении к аналоговым блокам
`define D_GET_INFO_STATE   ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0)& (RB15_PMPA0 == 1) & (RD5_PMPRD == 1)) 
`define D_GET_INFO_VERSION ((RF6_ALTCS == 1) & (CS_READ_MTR_RF7 == 0) & (RB15_PMPA0 == 0) & (RD5_PMPRD == 1))
`define D_GET_DATA_XRAY    ((RF6_ALTCS == 0) & (CS_READ_MTR_RF7 == 1) )

//Время ожидания перед выдачей TRIG после формирования MTR)RST в режиме без матриц
`define  D_COUNT_WAITE_TRIG_GO_5 5'h18 //3'h5

//Ширина счетчика MTR_RST
`define  D_WIDTH_OF_COUNTER_MTR_RST 5

//Длительность сигнала MTR_RST
`define  D_SZ_MTR_RST `D_WIDTH_OF_COUNTER_MTR_RST'h0b 
//`define  D_SZ_MTR_RST `D_WIDTH_OF_COUNTER_MTR_RST'h16 


//Ширина шины adres_in блоков памяти
`define  D_WIDTH_OF_ADRES_IN_11 11 
/*
//Количество слов памяти
`define  D_NUM_WORD_OF_ADRES_IN_1152  1152  //180 72
*/
//Ширина шины adres_out блоков памяти
`define  D_WIDTH_OF_ADRES_OUT_11 11

//Смещение начала считывания адресом adres_out
`define  D_OFFS_ADRES_OUT_0  `D_WIDTH_OF_ADRES_OUT_11'h00 //01


`ifndef _576_pix
//Количество слов памяти
`define  D_NUM_WORD_OF_ADRES_IN_1152  1152  //180 72
`define  D_END_ADRES_OUT_1152   1151  
`define  D_NUM_COUNT_TEST_GLOBAL_DATA 1152
`else
//Количество слов памяти
`define  D_NUM_WORD_OF_ADRES_IN_1152  576  
`define  D_END_ADRES_OUT_1152   575
`define  D_NUM_COUNT_TEST_GLOBAL_DATA 576
`endif

`define D_USE_PLL_SYCLONE_1


//Макросы применяемые при формирования временной диаграммы работы аналоговых блокрв
`define PURE_SYCLE_CS_ADC  ((count_m10 != 7'h2) & (count_m10 != 7'h3) & (count_m10 != 7'h1) &  (count_m10 != 7'h0))
`define PURE_SYCLE_BYTESWAP_6  ((count_m10 != 7'h2) & (count_m10 != 7'h3))
`define PURE_SYCLE_CONVECT  ((count_m10 != 7'h1) & (count_m10 != 7'h2))
/*
`define PURE_SYCLE_CS_ADC  ((count_m10 != 6'h2) & (count_m10 != 6'h3) & (count_m10 != 6'h1) &  (count_m10 != 6'h0))
`define PURE_SYCLE_BYTESWAP_6  ((count_m10 != 6'h2) & (count_m10 != 6'h3))
`define PURE_SYCLE_CONVECT  ((count_m10 != 6'h1) & (count_m10 != 6'h2))
*/

