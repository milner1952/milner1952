`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"
//-----------------------------------------------------------------------------------------------------
//Модуль вырабатывает сигналы _TEST_TRIG, _TRST_END, при наличии TEST_GO == 1
//---------------------------------------------------------------------------------------------------- 
//RES_HARD - Аппаратный сброс схемы
//Mx - синхро серия неперекрывающихся фаз сигналов
//МТR_CLK-частота синхронизации матриц
//_RUN - строб старшего процессора на время сёмки кадра
//_MTR_RES -сигнал синфазирования строк матриц
//TEST_GO - схема работает в тестовом режиме без матриц
//TEST_TRIG - сигнал ТRIG в режиме без матриц
//_TEST_END - сигнал END в режиме без матриц
//--------------------------------------------------------------------------------
module 	TEST_MD(input RES_HARD, input M0,input M1,input M2,input M3,  
				input MTR_CLK, input _RUN, input _MTR_RES, input TEST_GO, 
				
				
				output  _TEST_TRIG, output _TEST_END, output reg K13 
				);
			
//------------------------------------------------------------------------------------		
//сигналы:
// -shift_mtr_res - cинхронизированный с фазой M2 сигнал _MTR_RES
// -count_wait_trig_go - cигнал(разрешение)  работы счетчику [4 : 0]count_wait_trig
// -trig_go - разрешение счетчику [7 : 0]count_trig и [1 : 0]count_mtr_clk сформировать и отсчитывать 
//сигнал _TEST_TRIG = ~test_trig
// - shift_end - cинхронизированный с фазой M3 сигнал _TEST_END
//-------------------------------------------------------------------------------------------------
reg shift_mtr_res, count_wait_trig_go, trig_go, shift_end;	

//счетчик [4 : 0]count_wait_trig, отсчитывает задержку сигналов TEST_TRIG от сигнала MTR_RST, в соответствии с временной
//диаграммой работы матриц.			
reg [4 : 0]count_wait_trig ;

//отсчитывает четыре периода MTR_CLK b формирует сигнал _TEST_TRIG = ~test_trig
reg [1 : 0]count_mtr_clk;

//отсчитывает 128 периодов сигнала TEST_TRIG и для формирования сигнала TEST_END
reg [7 : 0]count_trig; 

//сигнал test_trig - эмулирует сигнал TRIG в режиме без матриц(в тестовом режиме работы датчика)
reg test_trig;

//Сигналы изменения сигналов _MTR_RES и _TEST_END
wire change_mtr_res, change_end;

//Выод сигнала test_trig за пределы модуля TEST_MD
assign _TEST_TRIG = ~test_trig;

//Формирование заднего фронта при изменении сигнала _MTR_RES
assign change_mtr_res = _MTR_RES & (~shift_mtr_res);

//Формирование переднего фронта при изменении сигнала _TEST_END
assign change_end = ~_TEST_END & shift_end; 

//тестовый выход
assign test_122 = count_wait_trig_go;

//сигнал _TEST_END формируется, когда (count_trig == ((`D_NUM_WORD_OF_ADRES_IN_1152/9) - 1'h1))
assign _TEST_END = (count_trig == ((`D_NUM_WORD_OF_ADRES_IN_1152/9) - 1'h1)); 
//----------------------------------------------------------------------------------			
//-shift_mtr_res - cинхронизация с фазой M2 сигнал _MTR_RES
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		shift_mtr_res = 0;
	end
	else
		if(M2 == 1)
		begin
			shift_mtr_res = _MTR_RES;				
		end						
end	

//-------------------------------------------------------------------------------
//- по фазе М1 при изменении заднего фронта MTR_RST устанавливается сигнал разрешения отсчета 
//счетчика задержки count_wait_trig_go = 1;
//- по фазе М2 согда счетчик count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5, сигнал count_wait_trig_go = 0 
//сбрасывается
//-------------------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M1, posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		count_wait_trig_go = 0; 
	end
	else			
		if(M1 == 1)
		begin
				if(change_mtr_res == 1)
				begin
					count_wait_trig_go = 1;
				end	
		end		
		else										
		if(M2 == 1)			
		begin
				if(count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5)	//4	
				begin
					count_wait_trig_go = 0;  
				end		
		end	
end	

//-------------------------------------------------------------------------------
// - по фазе M1 когда счетчие count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5
// взводится сигнал  trig_go = 1 - разрешение формирования сигнала TEST_TRIG
//- по фазе M2 при change_end == 1, сбрасывается trig_go = 0
//--------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M1,  posedge M2 )
begin
	if(RES_HARD == 1)
	begin
		trig_go = 0;
	end
	else
		if(M1 == 1)
		begin
			if(count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5)
			        trig_go = 1;	
		end
		else
		if(M2 == 1)
		begin
			if(change_end == 1)
			begin
					trig_go = 0;
			end			
		end			
end


	
//-------------------------------------------------------------------------------
// при наличии сигнала count_wait_trig_go, счетчик [4 : 0]count_wait_trig
// увеличивается на единицу, при отсутствии этого сигнала - сбрасывается в ноль,
// формируя задержку выдачи сигналов TRIG относительно сигналов MTR_RST в соответствии
// с временной диаграммой матриц
//---------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		count_wait_trig = 5'h0; 
	end	
	else
		if(M2 == 1)
		begin			
			if(count_wait_trig_go == 1)
			begin
				count_wait_trig = count_wait_trig + 5'h1;					
			end	
			else
			    count_wait_trig = 5'h0;													
		end	
end				

//-------------------------------------------------------------------------
//счетчик [1 : 0]count_mtr_clk, отсчитывает четыре периода сигнала MTR_CLK и 
//в состоянии count_mtr_clk == 2'h01 формирует сигнал test_trig(~_TEST_TRIG), при trig_go == 0
//счетчик count_mtr_clk и сигнал test_trig(~_TEST_TRIG) сбрасываются в ноль
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge MTR_CLK)			
begin
	if(RES_HARD == 1)
	begin
		count_mtr_clk = 2'h0; test_trig = 0;
	end	
	else
		if(MTR_CLK == 1)
		begin
			if(trig_go == 1)
			begin
				
				count_mtr_clk = count_mtr_clk + 2'h1;
				if(count_mtr_clk == 2'h01)
					test_trig = 1;
				else
					test_trig = 0;	
			end	
			else
			begin
			count_mtr_clk = 2'h0; test_trig = 0;	
			end
		end
end	

//-------------------------------------------------------------------------
//при наличии trig_go == 1, счетчик [7 : 0]count_trig отсчитывает периоды
//MTR_CLK, при trig_go == 0 этот счетчик сбрасывается в ноль
//----------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge MTR_CLK)			
begin
	if(RES_HARD == 1)
	begin
		count_trig = 8'h0; 
	end	
	else
	begin
		if(MTR_CLK == 1)
		begin	
			if(trig_go == 1)
			begin
				if(test_trig == 1)
				begin
					count_trig = count_trig + 8'h1;	
				end
			end		
			else
			count_trig = 8'h0;			 
		end
	end	
end	

//----------------------------------------------------------------------------------			
//-shift_end - cинхронизация с фазой M3 сигнал _TEST_END
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M3)			
begin
	if(RES_HARD == 1)
	begin
		shift_end = 0; 
	end	
	else
		if(M3 == 1)
			shift_end = _TEST_END;	
end	
				
endmodule				

