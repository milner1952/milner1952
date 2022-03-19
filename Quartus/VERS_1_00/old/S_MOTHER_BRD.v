
`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

// [`D_S_WIDTH_OF_DATA_8 -1 : 0]B_PD - Восьмиразрядная двунаправленная шина данных аналоговых блоков
// [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD, -Шестнадцатиразрядная двунаправленная шина данных микропроцессора PIC
// RES_HARD_RF13 - вход аппаратного сброса
// RD5_PMPRD, RD4_PMPWR - входы сигналов записи и чтения микропроцессора
// PIC_SINXRO_RF12 - вход сигнала микропроцессора о готовности к приеиу строки
// CS_READ_MTR_RF7 -	вход строба приема микропроцессором одного пакета данных строки, на стоке помещаются два пакета
// RUN_RF8 - требование микропроцессора о передачи1 кадра изображения
// RB15_PMPA0, - вход сигнала младшего адреса шины адреса микропрцессора
// END_BOARD0, END_BOARD1, END_BOARD2 - входы сигналов END матриц трех аналоговых блоков, об окончании строки.
// TEST_GO_RD0 - вход сигнала микропроцессора о переходе в режим работы без матриц, с получением информ. от тестового ЦАП
// RF6_ALTCS - вход сигнала выборки АЛЬТЕРЫ от микропроцессора
// OSC1,	OSC2 - два входа кваревого генератора 25мГц
// _TRIG_BOARD0, _TRIG_BOARD1, _TRIG_BOARD2 - входы сигналов TERG матриц трех аналоговых блоков
// MTR_CLK,  MTR_RST, - входы сигналов синхроницации и синфазирования всех матриц для всех блоков
// _RD, _WR - выходы чтения и записи информации аналоговых блоков по восьмиразрядной шине данных аналоговых блоков
// INTERUPT_1_TOO_SILABS_P03 выход сигнала о необходимости ЦАП-у микропроцессора С51F017 сформировать новую ступеньку
// {A0, A1, A2, A3, A4} - выходы шина адреса восьмиразрядной шины данных аналоговых блоков	
// DIR - выход сигнала переключения направления передачи шинных формирователей, восьмиразрядной шины данных аналоговых блоков	
//_CS0, _CS1, _CS2 - выходы сигналов выборки аналоговых блоков
//_TEST_TRIG, 	TEST_END - выходы для формирования аналоговыми блоками сигналов END и TRIG в режиме работы без матриц
// ENC_WR, ENC_RD - сигналы записи и чтения контроллера Ethernet ENC624J600
// INT_RESET_RE8 - сигнал Алтеры о готовности передавать строку информации(ответ на сигнал PIC_SINXRO_RF12)
// K113, K111 - контрольные точки для тестирования сх.
module S_MOTHER_BRD(inout [`D_S_WIDTH_OF_DATA_8 -1 : 0]B_PD,
								  inout [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD,
								  
								  input RES_HARD_RF13 , 
								  input RD5_PMPRD,	input RD4_PMPWR,			input PIC_SINXRO_RF12,	input RUN_RF8,			input CS_READ_MTR_RF7,	
								  input RB15_PMPA0,  input END_BOARD0,       input END_BOARD1,		   input END_BOARD2,		input TEST_GO_RD0,	
								  input RF6_ALTCS,	input OSC1,	input OSC2,	input _TRIG_BOARD0,     input _TRIG_BOARD1,  input _TRIG_BOARD2,
									
								  output  MTR_CLK,  	output  MTR_RST,  	   output _RD,  				output _WR,  			output reg INTERUPT_1_TOO_SILABS_P03,
								  output  A0, 			output  A1,  				output  A2,  				output  A3, 			output  A4,									
								  
								  output DIR, 			output _CS0, 				output _CS1,				output _CS2, 			output _TEST_TRIG, 		
								  output TEST_END,	output ENC_WR, 		   output ENC_RD, 			output INT_RESET_RE8, 	output  K113, 			
								  output K111);
								
					
					
/*(* noprune *) */
// сигнал об окончании приема информации о версии прошивки всех аналоговых блоков
reg end_vers;

// не используется
reg [7 : 0]delimer;

//Синхронизированный фазой М0 бит команды prev_get_version микропроцессора о передачи информации о 
//версии прошивок АЛТЕРЫ и аналоговых блоков
reg get_version;

//бит RE_AD[0] шины микропроцессора при установленом бите (RE_AD[8] == 1)команды 
//микропроцессора о передачи информации о версии прошивок АЛТЕРЫ и аналоговых блоков
reg prev_get_version;

//Стоб записи информации о версии аналоговых блоков в буфер материнской платы VersData
//при состоянии регистра  state в состоянии (`D_VERSION_BIT_state_0 == 0)
reg set;

//регистр состояния материнской платы. Пока используется только нолевой бит.
//`D_VERSION_BIT_state_0 == 0 - происходит запись информации о версии прошивки аналоговых блоков в буфер VersData
//`D_VERSION_BIT_state_0 == 1 - происходит чтение информации из буфера  VersData в микропроцессор
reg [15 : 0]state;

//буфер памяти о версии прошивок аналоговых блоков и материнской платы
reg [7 : 0]VersData[80];

//[6 : 0]count_vers_data_out - счетчик байт иформации о версии прошивок при передачи ее из материнской платы
//в микропроцессор
reg [6 : 0]count_vers_data_out;

//[1 : 0]count_board - счетчик, для перечисления аналоговых блоков при обращении к ним
reg [1 : 0]count_board;

//[7: 0]count_vers_data_in  - счетчик байт иформации о версии прошивок аналоговых блоков при 
//передачи ее в буфер VersData материнской платы
reg [7: 0]count_vers_data_in;

// инверсия бита ~RE_AD[1] шины микропроцессора - команда о передачи кадра информации с АЦП
//действительна при установленом бите ( RE_AD[9] == 1) этой шины
reg _run;


//Второй шестнадцати разрядный буферный регистр, двойного буфирирования, используемый 
//при составлении 16 битного слова  из двух восьми разрядных байт (младшего и старшего) 
//восьмиразрядной шины, для записиего в шестнадцати разрядную память
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_bas_in;

//Первый шестнадцати разрядный буферный регистр, двойного буфирирования, используемый 
//при составлении 16 битного слова  из двух восьми разрядных байт (младшего и старшего) 
//восьмиразрядной шины, для записиего в шестнадцати разрядную память
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]buf_reg;

//Счетчик сигналов TRIG для формирования сигнала INTERUPT_1_TOO_SILABS_P03 в С51F017
reg [6 : 0]Count32trigTooSilabs/* synthesis preserve */;

//c_osc - делитель на два сигнала PLL_OSC1(PLL_OSC) - выхода PLL, используется
//как сигнал синхронизации схемы при биннинг == 1
reg c_osc; 

//делитель на 4 сигнала PLL_OSC1(PLL_OSC) - выхода PLL, c_osc4[0] используется как
//сигнал синхронизации схемы при биннинг == 2, а c_osc4[1] используется как
//сигнал синхронизации схемы при биннинг == 4
reg [1 : 0]c_osc4;

//делитель на 3 сигнала PLL_OSC1(PLL_OSC) - выхода PLL, c_osc3[1] используется как
//сигнал синхронизации схемы при биннинг == 3
reg [1 : 0]c_osc3;

//Счетчик чтений аналоговых блоков - необходим для синхронизации логического анализатора
reg [8 : 0]counter_adres_board;

//счетчик используется для синхронизации осцилографа
reg [12 : 0]counter_string;

//mem_en_rd - не используется
//clk_mem_read - синхроимпульсы чтения памяти
//write_too_mem `- импульс запяси в память
wire mem_en_rd;
wire clk_mem_read/* synthesis keep */;
wire  write_too_mem/* synthesis keep */;

//синхроимпульсы сдвига восьми битовой информации по первому шестнадцатиразрядному буферному регистру
//во второй шестнадцатиразрядный регистр.
wire w_clk_bufer/* synthesis keep */;

//связи счетчика байт иформации о версии прошивок аналоговых блоков при 
//передачи ее в буфер VersData материнской платы
wire [7:0]w_count_vers_data_in;

//связи выводов Mx модуля синхронизации
//M0 - используется для синхронизации приема команд с шестнадцатиразрядной шины микропроцессора и для синхронизации
//приема информации от аналоговых плат при считывании их версий.
//M1 - используется для синхронизации счетчиков count_board при приеме информации от аналоговых плат при считывании их версий.
//M2,M3 -   используется для синхронизации установки и сброса стоба записи информации set о версии аналоговых блоков 
//в буфер материнской платы VersData
wire M0,M1,M2,M3/* synthesis keep */;

//связи выводов fx модуля синхронизации
wire f0;
wire f1;
wire f2;
wire f3;
wire f4;
wire f5;

//связи выводов [1 : 0]count_3P модуля синхронизации
wire [1 : 0]count_3P;


///wire [3 : 0]cnt_ch_trig;
//выводы для тестирования
wire _K13, _K1;

//[3:0]adres_board- выоды модуля синхронизации 
wire [3:0]adres_board;

//Логическая сумма по и инверсий входов _TRIG_BOARD всех аналоговых блоков
//_TRIG = ~((~_TRIG_BOARD0) & (~_TRIG_BOARD1) & (~_TRIG_BOARD2)); 
wire _TRIG/* synthesis keep */;

//Выводы модуля GET_VERS_AIN_BRD, формирующие шину адреса аналоговых блоков при
//приеме информации об их версии
wire a4_vers, a3_vers, a2_vers, a1_vers, a0_vers;

//Выоды сигнала о необходимости изменения адреса записи в память модуля SINXRO_4_3
wire chenge_adres_in;

//Выоды адреса adres_in модуля ADRES_IN
wire [`D_WIDTH_OF_ADRES_IN_11 - 1 : 0]adres_in;

//Выоды адреса adres_out модуля ADRES_OUT
wire [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]adres_out;

//вывод сигнала COUNTER_F_GO модуля ADRES_IN
wire counter_f_go;

//_rd_snx - вывод сигнала чтения _RD модуля синхронизации 
wire _rd_snx;

//_rd_data - вывод сигнала _RD модуля ADRES_IN о необходимости чтения данных из аналоговых блоков
wire _rd_data;

//_rd_vers - вывод сигнала _RD модуля GET_VERS_AIN_BRD о необходимости чтения версии из аналоговых блоков
wire _rd_vers;

//флаг init_vers_go устанавливается после чтения версий аналоговых блоков и говорит о готовности 
//материнской платы передавать информацию о версии прошивок в микропроцессор PIC
reg  init_vers_go;

//вывод сигнала ~(_run == 0 &  init_vers_go == 1); о получении команды _run == 0, при установленом init_vers_go == 1
//является стробом работы схемы при снятии кадра информации с матриц.
wire t_run;

//вывод сигнала одиннадцатого адреса модуля ADRES_IN - сигнал переключает модули памяти через строку на запись/чтение
//информации
wire adres_in_11/* synthesis keep */;

//вывод сигнала WRITE_BUFER модуля синхронизации, записи информации в регистр шестнадцатиразрядный регистр mem_bas_in
//двойного буферирования
wire w_write_bufer/* synthesis keep */;

//вывод flag_init модуля GET_VERS_AIN_BRD
wire flag_init;

//wr_comand - сигнал записи WR_COMAND команды модуля DT_COMMAND
//mx_data -сигнал мультиплексирования MX_DATA шины адреса аналоговых блоков и команды записи модуля DT_COMMAND
//test_go - вывод TEST_GO модуля DT_COMMAND
// bin0, bin1 - выводы BIN0, BIN1 модуля DT_COMMAND
wire wr_comand, mx_data, test_go,  bin0, bin1 ;

//[6 : 0]w_binning - выводы [6 : 0]BINING модуля DT_COMMAND
wire [6 : 0]w_binning/* synthesis keep */;

// [4 : 0]adres_command - выводы [4 : 0]ADRES_COMMAND  модуля DT_COMMAND
wire [4 : 0]adres_command; 

//инверсия сигнала ~TRIG
wire trig;

//END = END_BOARD0 & END_BOARD0 & END_BOARD0;
wire END/* synthesis keep */;

`ifdef TEST_WITHOUT_MEMORY
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out;
`else
//Выходы мультиплексированной шины данных при чтении информации из памяти
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out/* synthesis keep */;
//Выходы q первого блока памяти ram_dual
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out1/* synthesis keep */;
//Выходы q первого блока памяти ram_dual
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out2/* synthesis keep */;
`endif
//сигнал синхронизации схемы, частота которого меняется с изменением биннинга
wire _OSC1/* synthesis keep */; 

//Выход модуля PLL - PLL_OSC1 = PLL_OSC;
wire PLL_OSC1;

//Выход модуля PLL - PLL_OSC1 = PLL_OSC;
wire PLL_OSC;
//wire test_sin;

//счетчик используется при тестировании прошивки и мат. платы без аналоговых блоков
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]test_count_global_data;

//reg r_INTERUPT_1_TOO_SILABS_P03;

//сигнал синхронизации схемы, частота которого меняется с изменением биннинга
assign _OSC1 = ~(({bin1,bin0} == 2'h0 & c_osc == 1) |
 					 ({bin1,bin0} == 2'h1 & c_osc4[0] == 1) |
					 ({bin1,bin0} == 2'h2 & c_osc3[1] == 1) |
					 ({bin1,bin0} == 2'h3 & c_osc4[1] == 1));

//Контрольный вывод для тестирования
assign K113 = MTR_CLK | counter_adres_board[8] | counter_string[12];					

//Контрольный вывод для тестирования
assign K111 = PLL_OSC1;

//Выход модуля PLL - PLL_OSC1 = PLL_OSC;
`ifndef _576_pix
assign PLL_OSC1 = PLL_OSC;
`else

reg [1 : 0]r_PLL_OSC_576;
wire  PLL_OSC_576;

assign PLL_OSC1 = r_PLL_OSC_576[1];
`endif


//инверсия сигнала TRIG
assign trig = ~_TRIG;

//мультиплексор выходов памяти
assign mem_out = adres_in_11 == 1 ? mem_out1 : mem_out2;

//сьемка кадра возможна только после передачи информации о версии прошивок
assign t_run = ~(_run == 0 &  init_vers_go == 1);

//сигнал чтения аналоговых блоков - мультиплексированные сигналы _rd_vers \ _rd_data;
assign _RD = (get_version == 1) ? _rd_vers : _rd_data;

//сумма по и инверсий сигналов _TRIG_BOARD аналоговых плат 
assign _TRIG = ~((~_TRIG_BOARD0) & (~_TRIG_BOARD1) & (~_TRIG_BOARD2));  

//сумма по и сигналов END_BOARD аналоговых плат 
assign END = END_BOARD0 & END_BOARD0 & END_BOARD0;

//Шестнадцатиразрядная шина микропроцессора PIC, на которую выводится информация:
// - регистра состояний state;
// - буфера VersData;
// - содержимое памяти(информ. X-RAY ). 
// при отсутствии сигналов D_GET_INFO_STATE == 1 или D_GET_INFO_VERSION == 1 или D_GET_DATA_XRAY == 1 шина находится 
// в третьем состоянии.
assign RE_AD = `D_GET_INFO_STATE == 1 ? state : 
					`D_GET_INFO_VERSION == 1 ?  {/*8'h06*/`_HARD_VERS , VersData[count_vers_data_out]}:
					`D_GET_DATA_XRAY == 1 ? mem_out : 16'bzzzzzzzzzzzzzzzz;
				
//пятиразрядная шина адреса аналоговых блоков, которая мультиплексирует адреса:
//- версии аналоговых блоков;
//- адреса команд аналоговых блоков;
//- адреса плат аналоговых блоков.
assign {A4,A3,A2,A1,A0} = get_version == 1 ?{a4_vers, a3_vers, a2_vers, a1_vers, a0_vers} :
								  (mx_data == 1) ? adres_command : {adres_board,1'h0};

// _WR - cигнал записи аналоговых блоков мультиплексирует:
// сигнал _wr - (_WR)модуля GET_VERS_AIN_BRD;
// и сигнал записи  команд wr_comand(WR_COMAND) модуля DT_COMMAND
// при отсутствии сигналов  get_version == 1'h1 и mx_data == 1'h1 сигнал _WR, находится
// в состоянии единицы  								  
assign _WR = get_version == 1'h1 ? _wr : mx_data == 1'h1 ? ~wr_comand : 1'h1;

//ENC_WR - сигнал записии в ENC624J600 - при наличии RF6_ALTCS == 0  
//зависит от состояния сигнала CS_READ_MTR_RF7: при CS_READ_MTR_RF7 == 1 :  
//	- определяется состоянием сигнала чтения микропроцессором RD5_PMPRD, который используется в этом случае 
//как сигнал записи в ENC624J600 информации буфера памяти АЛТЕРЫ по шине RE_AD (микропроцессор при
//чтении свою шину устанавливает в третье состояние) 
//при CS_READ_MTR_RF7 == 0 :
// - определяется состоянием сигнала RD4_PMPWR - записи микропроцессора.
// При наличии выборки микросхемы АЛТЕРА RF6_ALTCS == 0  сигнал устанавливается в 0
assign ENC_WR = RF6_ALTCS == 0 ? (CS_READ_MTR_RF7 == 1 ? RD5_PMPRD : RD4_PMPWR) : 1'h0;


//ENC_RD - сигнал чтения ENC624J600 - при наличии RF6_ALTCS == 0  
//зависит от состояния сигнала CS_READ_MTR_RF7: при CS_READ_MTR_RF7 == 1 :  
// - сбрасывается в ноль
//при CS_READ_MTR_RF7 == 0 -   определяется сигналом чтения RD5_PMPRD микропроцессора
//при  RF6_ALTCS == 1 сбрасывается в ноль 
assign ENC_RD = RF6_ALTCS == 0 ? (CS_READ_MTR_RF7 == 1 ? 1'h0 : RD5_PMPRD) : 1'h0;

//DIR - cигнал переключения шинных формирователей аналоговых блоков:
//при get_version == 0 - равен сигналу counter_f_go в противном случае
// определяется состоянием w_count_vers_data_in.
assign DIR = get_version == 1'h1 ?  (w_count_vers_data_in == 8'h0 ? 0 : 1'h1) : counter_f_go;

//сигналы выборк аналоговых блоков определяется состоянием двух старших разрядов
//шины адреса или выдачей широковещательного адреса.
assign _CS0 = ~(({A4,A3} == 2'h0) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));
assign _CS1 = ~(({A4,A3} == 2'h1) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));
assign _CS2 = ~(({A4,A3} == 2'h2) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));
//assign INTERUPT_1_TOO_SILABS_P03 = r_INTERUPT_1_TOO_SILABS_P03 | END | (~MTR_RST);
//---------------------------------------------------------------------------
//Счетчик переданныз строк - необходим для синхронизации логического анализатора
//----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge MTR_RST, posedge t_run)
begin
	if(RES_HARD_RF13 == 1)
		counter_string = 13'h0;
	else	
	begin			
				if(t_run == 1)
				begin
					counter_string = 13'h0;
				end
				else
				if(MTR_RST == 0)
				begin
				counter_string = counter_string+ 13'h1;
				end
   end
end
//---------------------------------------------------------------------------
//Счетчик чтений аналоговых блоков - необходим для синхронизации логического анализатора
//----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge MTR_RST, negedge _CS0)
begin
	if(RES_HARD_RF13 == 1)
		counter_adres_board = 9'h0;
	else	
	begin			
				if(MTR_RST == 0)
				begin
					counter_adres_board = 9'h0;
				end
				else
				if(_CS0 == 0)
				begin
				counter_adres_board = counter_adres_board + 9'h1;
				end
   end
end

//-------------------------------------------------------------------------
//c_osc - делитель на два сигнала PLL_OSC1(PLL_OSC) - выхода PLL, используется
//как сигнал синхронизации схемы при биннинг == 1				
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc <= 0;
	end
		else
		if(PLL_OSC1 == 0)
		begin
				c_osc <= ~c_osc;
		end		
end

//-------------------------------------------------------------------------------------------
//делитель на 4 сигнала PLL_OSC1(PLL_OSC) - выхода PLL, c_osc4[0] используется как
//сигнал синхронизации схемы при биннинг == 2, а c_osc4[1] используется как
//сигнал синхронизации схемы при биннинг == 4
//-----------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc4 <= 2'h0;
	end
		else
		if(PLL_OSC1 == 0)
	   begin	
			if(c_osc == 0)
			begin
				c_osc4 <= c_osc4 + 2'h1;
			end		
		end	
end

//----------------------------------------------------------------------------------------
//делитель на 3 сигнала PLL_OSC1(PLL_OSC) - выхода PLL, c_osc3[1] используется как
//сигнал синхронизации схемы при биннинг == 3
//-----------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc3 <= 2'h0;
	end
		else
		if(PLL_OSC1 == 0)
	   begin
			if(c_osc == 0)
			begin
				c_osc3 <= c_osc3 + 2'h1;
				if(c_osc3 == 2'h2)
				begin
					c_osc3 <= 2'h0;
				end
			end		
		end	
end	 

//-------------------------------------------------------
//		INTERUPT_1_TOO_SILABS_P03 сигнал о необходимости ЦАП-у микропроцессора С51F017 сформировать 
//новую ступеньку
//сбрасывается в 0 - сигналом PIC_SINXRO_RF12 == 1 и состоянием (Count32trigTooSilabs == w_binning)
//устанавливается в 1 - сигналом MTR_RST == 0 и состоянием (Count32trigTooSilabs != w_binning)
//		Count32trigTooSilabs  - счетчик сигналов trig, 
//сбрасывается сигналом MTR_RST = 0 и состоянием (Count32trigTooSilabs == w_binning)
//инкрементируется сигналом trig
//--------------------------------------------------------
always @( posedge RES_HARD_RF13, posedge trig, negedge MTR_RST, posedge PIC_SINXRO_RF12)
begin
	if(RES_HARD_RF13 == 1) 
	begin
		INTERUPT_1_TOO_SILABS_P03 <= 1;   Count32trigTooSilabs <= 7'h0;  
	end   
	else
	   if(PIC_SINXRO_RF12 == 1)
	   begin
       INTERUPT_1_TOO_SILABS_P03 <= 0;
      end		
	   else
		if(MTR_RST  == 0) 
		begin
			INTERUPT_1_TOO_SILABS_P03 <= 1;   Count32trigTooSilabs <= 7'h00; 
		end
		else
		if(trig == 1)
		begin
			Count32trigTooSilabs <= Count32trigTooSilabs + 7'h1;
			if(Count32trigTooSilabs == w_binning) 
			begin
				INTERUPT_1_TOO_SILABS_P03 <= 0;
				Count32trigTooSilabs <= 7'h0;
			end                 
			else
				INTERUPT_1_TOO_SILABS_P03 <= 1;                  
				
		end
		
end


/*
always @( posedge RES_HARD_RF13, posedge trig, //negedge MTR_RST,
											posedge PIC_SINXRO_RF12)
begin
	if(RES_HARD_RF13 == 1) 
	begin
		r_INTERUPT_1_TOO_SILABS_P03 <= 0;   Count32trigTooSilabs <= 7'h0;  
	end   
	else
	   if(PIC_SINXRO_RF12 == 1)
	   begin
       r_INTERUPT_1_TOO_SILABS_P03 <= 0;
		 			Count32trigTooSilabs <= 7'h00; 
      end		
	   else
		
	//	if(MTR_RST  == 0) 
	//	begin
	//		INTERUPT_1_TOO_SILABS_P03 <= 1;//1;   
	//	//	Count32trigTooSilabs <= 7'h00; 
	//	end
	//	else
		
		
		if(trig == 1)
		begin
			Count32trigTooSilabs <= Count32trigTooSilabs + 7'h1;
			if(Count32trigTooSilabs == w_binning) 
			begin
			if(END == 0)
				r_INTERUPT_1_TOO_SILABS_P03 <= 1;//0
				Count32trigTooSilabs <= 7'h0;
			end                 
			else
				r_INTERUPT_1_TOO_SILABS_P03 <= 0;   //1               
				
		end
		
end
*/
//-------------------------------------------------------------------------------------------------------------------------------
//	   prev_get_version -команда микропроцессора PIC по шине данных RE_AD[0], при наличии RE_AD[8] == 1 - возвратить 
//информацию о версиях прошивок  аналоговых блоков и материнского блоков  
// 	get_version - синхронизированная по M0 команда микропроцессора по шине данных RE_AD[0] возвратить информацию о версиях прошивок 
//аналоговых блоков и материнского блока  
//	   _run -команда микропроцессора PIC по шине данных RE_AD[1], при наличии RE_AD[9] == 1, о начале сьемки кадра
//    Все команды должны сопровождаться сигналом  RF6_ALTCS == 1.
//    Запись производится сигналом (RD4_PMPWR == 1) при состоянии сигнала адреса, шины адреса микропроцессора (RB15_PMPA0 == 1)
//-------------------------------------------------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge RD4_PMPWR, posedge M0)
begin
if(RES_HARD_RF13 == 1)
begin
	get_version = 1'h0;	prev_get_version = 0; _run = 1; 
end
else
begin
        if(RD4_PMPWR == 1)
		  begin
				if(RB15_PMPA0 == 1)
				begin
						if(RF6_ALTCS == 1)
						begin
						  if(RE_AD[8] == 1)
						   begin							    
									prev_get_version =  RE_AD[0]; 
							end	
				        if( RE_AD[9] == 1)
				         begin		
									_run =  ~RE_AD[1];
							end	
							
						end	
				end		
		  end				
		  else
		   if(M0 == 1)
			begin
		      	get_version = prev_get_version; 
			end			  
end		  
end




//-----------------------------------------------------------------------------
//VersData Init - буфер данных о версии прошивок.
//Байты с 18 по 69 - предназначены для записи версии прошивок трех аналоговых блоков
//При отсутствии блока, в микропроцессор должна быть выведена фраза: "NO CONNECT" 
//Версии прошивок аналоговых блоков считываются в этот буфер при инициализации датчика 
//побайтно в соответствии с состоянием счетчика count_vers_data_in, запись производится
// сигналом set при наличии сигнала D_VERSION_BIT_state_0 == 0
//после включения питания. После этого содержимое всего буфера передается по шине RE_AD
//в микропроцессор.
//-----------------------------------------------------------------------
always @(posedge RES_HARD_RF13,    posedge set)
begin
if(RES_HARD_RF13 == 1)
begin
  delimer = 6'h1; 

`ifndef _576_pix  
  VersData[0] = "1";  
  VersData[1] = "1";
  VersData[2] = "5";
  VersData[3] = "2";
`else
  VersData[0] = "0";  
  VersData[1] = "5";
  VersData[2] = "7";
  VersData[3] = "6";
`endif 
 
  VersData[4] = " ";
  VersData[5] = "A";
  VersData[6] = "L";
  VersData[7] = "T";
  VersData[8] = ":";
  VersData[9] = " ";
  VersData[10] = "v";
  VersData[11] = "3";
  VersData[12] = ".";
  VersData[13] = "0";
  VersData[14] = ".";
  VersData[15] = "0";
  VersData[16] = "0";
  VersData[17] = 8'h0;
  
  VersData[18] = "N";
  VersData[19] = "O";
  VersData[20] = " ";
  VersData[21] = "C";
  VersData[22] = "O";
  VersData[23] = "N";
  VersData[24] = "N";
  VersData[25] = "E";
  VersData[26] = "C";
  VersData[27] = "T";
  VersData[28] = " ";
  VersData[29] = " ";
  VersData[30] = " ";
  VersData[31] = " ";
  VersData[32] = " ";
  VersData[33] = " ";
  VersData[34] = 8'h0;
  
  VersData[35] = "N";
  VersData[36] = "O";
  VersData[37] = " ";
  VersData[38] = "C";
  VersData[39] = "O";
  VersData[40] = "N";
  VersData[41] = "N";
  VersData[42] = "E";
  VersData[43] = "C";
  VersData[44] = "T";
  VersData[45] = " ";
  VersData[46] = " ";
  VersData[47] = " ";
  VersData[48] = " ";
  VersData[49] = " ";
  VersData[50] = " ";
  VersData[51] = 8'h0;  

  VersData[52] = "N";
  VersData[53] = "O";
  VersData[54] = " ";
  VersData[55] = "C";
  VersData[56] = "O";
  VersData[57] = "N";
  VersData[58] = "N";
  VersData[59] = "E";
  VersData[60] = "C";
  VersData[61] = "T";
  VersData[62] = " ";
  VersData[64] = " ";
  VersData[65] = " ";
  VersData[66] = " ";
  VersData[67] = " ";
  VersData[68] = " ";
  VersData[69] = 8'h0;   
end
else
begin
	
	if((set == 1) & (`D_VERSION_BIT_state_0 == 0))
		begin
			VersData[ count_vers_data_in + 8'h11] = B_PD;
		end	

end		
end

//-----------------------------------------------------------------
//[6 : 0]count_vers_data_in - счетчик байтов при записи информвции 
//о версии прошивок аналоговых блоков в буфер VersData
//инкрементируется сигналом M0 при наличии 
// - D_VERSION_BIT_state_0 == 0) и
// - (get_version == 1) и (flag_init == 1)
//-----------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge   M0)
begin
if(RES_HARD_RF13 == 1)
begin
  count_vers_data_in = 7'h0;  
end
else
	     if((M0 == 1) && ((`D_VERSION_BIT_state_0 == 0)) && 
		  (get_version == 1)  & (flag_init == 1))
		  begin
				count_vers_data_in = count_vers_data_in + 7'h1;
		  end		        	 
end
//-----------------------------------------------------------------------------------
//Счетчик чтения информации из буфера VersData при ее передаче по шине RE_AD в микропроцессор PIC
//Инкрементирование происходит по сигналу (RD5_PMPRD == 0), при наличии:
//(RF6_ALTCS == 1) & (RB15_PMPA0 == 0) & (get_version == 1) & (`D_VERSION_BIT_state_0 == 1)
//-----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13,   negedge RD5_PMPRD )
begin
if(RES_HARD_RF13 == 1)
begin
  count_vers_data_out = 7'h0; 
end
else
	   if(RD5_PMPRD == 0)
		begin
		if((RF6_ALTCS == 1) & (RB15_PMPA0 == 0) & (get_version == 1) & 
		   ((`D_VERSION_BIT_state_0 == 1)))
		  begin
		   count_vers_data_out = count_vers_data_out + 7'h1;  
		  end
		end
end

//-----------------------------------------------------------------------------------
//[2 : 0]count_board  - счетчик адресующий аналоговые платы, при считывании 
//информации о версии их прошивок в буфер  VersData
//count_board - инкрементируется фазой М1 при наличии:
// - (w_count_vers_data_in == (`D_S_NUM_BYTE_GET_VERSION_017 - 1)
// - (`D_VERSION_BIT_state_0 == 0)
// в состоянии  count_board == 3'h3 взводится бит end_vers = 1.
//count_board - обнуляется следующей фазой M1 при наличии end_vers = 1. В
//этом состоянии устанавливаются в единицу бит  init_vers_go и бит D_VERSION_BIT_SET_state_0)
//регистра state 
//-----------------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge M1)
begin
if(RES_HARD_RF13 == 1)
begin
count_board = 2'h0; state = 16'h00; init_vers_go = 0;end_vers = 0;
end
else
	if(M1 == 1)
	begin
			if((w_count_vers_data_in == (`D_S_NUM_BYTE_GET_VERSION_017 - 1)) & 
			     (`D_VERSION_BIT_state_0 == 0))
			begin
				count_board = count_board + 2'h1;
				if(count_board == 3'h3)
				begin
				end_vers = 1;
				end				
			end
		   else	
				if(end_vers == 1)
				begin
				      count_board = 2'h0;
						state = (state | `D_VERSION_BIT_SET_state_0); 	
						init_vers_go = 1;						            				
				end
				
			
	end

end


//--------------------------------------------------------------------------
//set - сигнал записи информации о версии аналоговых блоков
// устанавливается по фазе M2 при наличии:
// (get_version == 1) & (`D_VERSION_BIT_state_0 == 0) & (flag_init == 1) 
//сбрасывается по фазе M3
//--------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge M2, posedge M3)
begin
	if(RES_HARD_RF13 == 1)
	begin
		set = 0; 
	end
	else
	if(M2 == 1) 
    begin
		if((get_version == 1)& (`D_VERSION_BIT_state_0 == 0) & (flag_init == 1) )
		begin
		   set = 1;
		end
    end
	 else 
	 if(M3 == 1)
	 begin
	      set = 0;
	 end
end

`ifdef TEST_WITHOUT_MEMORY
//-----------------------------------------------------------------------
//проверка прошивки без памяти
// mem_out - инкрементируется сигналом RD5_PMPRD при наличии (CS_READ_MTR_RF7 == 1)
// mem_out - сбрасывается (MTR_RST == 0)
//-----------------------------------------------------------------------
always @(posedge RES_HARD_RF13,negedge MTR_RST ,negedge RD5_PMPRD)
begin
	if(RES_HARD_RF13 == 1)
	begin
		mem_out = `D_S_WIDTH_OF_DATA_AD_16'h00;
	end
	else
	begin
	   if(MTR_RST == 0)
		begin
			mem_out = `D_S_WIDTH_OF_DATA_AD_16'h00; 	
		end
		else
		if(RD5_PMPRD == 0)
		begin
			if(CS_READ_MTR_RF7 == 1)
			begin
		         mem_out = mem_out + `D_S_WIDTH_OF_DATA_AD_16'h01;   	
			end
		end	
	end
end		
`endif	
//-----------------------------------------------------------------

`ifdef TEST_COUNT_GLOBAL_DATA
//--------------------------------------------------------------------------------------
//Проверка прошивки без аналоговых блоков
// счетчик test_count_global_data инкрементируеися от 0 до 1152 сигналом w_write_bufer
//при наличии сигнала counter_f_go
//счетчик test_count_global_data сбрасывается t_run == 1 или MTR_RST == 0
//-------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13,  posedge w_write_bufer, posedge t_run, negedge MTR_RST)
begin
	if(RES_HARD_RF13 == 1)
		begin
			test_count_global_data <=  16'h0;
		end
	else
	
	begin		
      	
		if(t_run == 1)	
		begin
			test_count_global_data <= 16'h0; 
		end
		else
		if(MTR_RST == 0)
		begin
		   test_count_global_data <= 16'h0; 
		end
		else
			if(w_write_bufer  == 1)
			begin
				if(counter_f_go == 1)
					begin	
					test_count_global_data <= test_count_global_data + 16'h01;	
                          if(test_count_global_data == (`D_NUM_COUNT_TEST_GLOBAL_DATA / {bin1,bin0}))
											test_count_global_data <= 16'h00;												
					end
			end			
	 end	
end
//------------------------------------------------------------------------
//Два блока памяти при подключении при проверки без аналоговых блоков
//------------------------------------------------------------------------
ram_dual   mem1_(test_count_global_data,
					 adres_out, adres_in,  adres_in_11 , clk_mem_read,   
					 write_too_mem , mem_out2 ,RB15_PMPA0 ,K13, CS_READ_MTR_RF7);
					 
			   
ram_dual   mem2_(test_count_global_data,
					 adres_out, adres_in, ~adres_in_11, clk_mem_read,   
					 write_too_mem , mem_out1,RB15_PMPA0 ,K1, CS_READ_MTR_RF7);	
					 

`endif


//-------------------------------------------------------------------------------
`ifndef TEST
//-------------------------------------------------------------------------------
//Двойное буферирование шины аналоговых блоков.
//Информация по восьмиразрядной шине B_PD записывается по байтно в старшую и мдадшую
//часть регистра buf_reg передним фронтом w_clk_bufer == 1
//По заднему фронту сигнала w_clk_bufer при налмчии сигнала w_write_bufer, эта информация 
//переписывается в шестнадцатиразрядный регистр - буфер памяти mem_bas_in 
//-------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13,  posedge  w_clk_bufer, 
			posedge w_write_bufer)
begin
	if(RES_HARD_RF13 == 1)
		begin
			mem_bas_in <=  16'h0; buf_reg <= 16'h0;
		end
	else	
	begin		 
	
	      if(w_write_bufer == 1) 		
			begin
			       if(w_clk_bufer == 0) 	
					 begin
							mem_bas_in <= buf_reg;
					 end		
			end	
			else	
			if(w_clk_bufer == 1) 
			begin						

							buf_reg[7 : 0] <= buf_reg[15 : 8];		
							buf_reg[15 : 8] <= B_PD;				
										
			end 

	 end	
end
//------------------------------------------------------------------------
//Два блока памяти при подключении в рабочем режиме
//------------------------------------------------------------------------
ram_dual   mem1_(mem_bas_in,
					 adres_out, adres_in,  adres_in_11 , clk_mem_read,   
					 write_too_mem , mem_out2 ,RB15_PMPA0 ,K13, CS_READ_MTR_RF7);
					 			   
ram_dual   mem2_(mem_bas_in,
					 adres_out, adres_in, ~adres_in_11, clk_mem_read,   
					 write_too_mem , mem_out1,RB15_PMPA0 ,K1, CS_READ_MTR_RF7);		
`endif
//------------------------------------------------------------------------

//Модуль ADRES_IN_MD
ADRES_IN_MD addres_in(RES_HARD_RF13, f0,f1,f2,
							M0,M1,M2,M3,  
							MTR_CLK, PIC_SINXRO_RF12, t_run ,_TRIG, END,chenge_adres_in,_rd_snx,

							
							INT_RESET_RE8, MTR_RST, counter_f_go, adres_in, _rd_data,
							adres_in_11
							);
		
//Модуль ADRES_OUT_MD
`ifndef _576_pix
ADRES_OUT_MD  addres_out(RES_HARD_RF13,c_osc, CS_READ_MTR_RF7,  INT_RESET_RE8,
								 RF6_ALTCS, RD5_PMPRD, RD4_PMPWR, RB15_PMPA0,
				     
								 adres_out, mem_en_rd, clk_mem_read , _K1
								);		
`else
ADRES_OUT_MD  addres_out(RES_HARD_RF13,/*PLL_OSC_576*/r_PLL_OSC_576[0], CS_READ_MTR_RF7,  INT_RESET_RE8,
								 RF6_ALTCS, RD5_PMPRD, RD4_PMPWR, RB15_PMPA0,
				     
								 adres_out, mem_en_rd, clk_mem_read , _K1
								);
`endif								
		
	
//Модуль TEST_MD
TEST_MD  Test_md(RES_HARD_RF13, M0,M1,M2,M3,  
				     MTR_CLK, t_run,MTR_RST, test_go, 	
					  
				     _TEST_TRIG,  TEST_END, _K13 
				     );
				



//Модуль Синхронизации
SINXRO_4_3    sinxro_i(_OSC1,  _OSC1, RES_HARD_RF13, ~_TRIG, counter_f_go,

									M0,M1,M2,M3, f0,f1,f2,f3,f4,f5, adres_board, 
									MTR_CLK, chenge_adres_in,_rd_snx, 
									w_clk_bufer, w_write_bufer, write_too_mem,count_3P);

							

//Модуль GET_VERS_AIN_BRD
GET_VERS_AIN_BRD get_vers(RES_HARD_RF13, get_version,
								  count_board,  
								 M0, M1, M3, `D_VERSION_BIT_state_0, count_3P,	
								 
						       B_PD,	
						       _rd_vers, _wr,  
								 w_count_vers_data_in, 
								 a4_vers, a3_vers, a2_vers, a1_vers, a0_vers, flag_init);
								 

								 
//Модуль DT_COMMAND
DT_COMMAND dt_COMMAND (RES_HARD_RF13,RE_AD,
						     RD4_PMPWR, RB15_PMPA0, RF6_ALTCS,
							  M0 ,M1 ,M2 ,M3, get_version,
						
						     wr_comand, mx_data, test_go, 
						     bin0, bin1, adres_command,B_PD, w_binning 
							);								 


//Модуль  PLL
`ifndef _576_pix
PLL_5 pll_brd(OSC1,PLL_OSC);	//div = 9, mul = 22, f =  61,111мГц, при f_in = 25мГц  						
`else
PLL_5 pll_brd(OSC1,PLL_OSC_576);	//div = 9, mul = 22, f =  61,111мГц, при f_in = 25мГц  						

always @(posedge RES_HARD_RF13,  posedge  PLL_OSC_576)
begin
	if(RES_HARD_RF13 == 1)
	begin
	r_PLL_OSC_576 = 2'h0;
	
	end
	else
	   begin
			//r_PLL_OSC_576 = ~r_PLL_OSC_576;
			r_PLL_OSC_576 = r_PLL_OSC_576 + 2'h1;
		end	
end
`endif




//------------------------------------------------------------------------							
endmodule
