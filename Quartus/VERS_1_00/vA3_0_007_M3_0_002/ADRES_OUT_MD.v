`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

/*
 Модуль формирует шину адреса двухпортовых буферов информации: 

 input RES_HARD - аппаратный сброс
 input CLK      -разделенный на два сигнала PLL_OSC1(PLL_OSC) - выхода PLL, используемый
				     как сигнал синхронизации схемы при биннинг == 1
 input CS_READ_MTR - вход строба приема микропроцессором одного пакета данных строки, на стоке помещаются два пакета
 input 	INT_RESET_RE8 -  сигнал Алтеры о готовности передавать строку информации(ответ на сигнал PIC_SINXRO_RF12)
 input ALTCS, PMPRD, PMPWR,PMPA0 - сигналы выборки Алтеры, чтения, записи, младшего бита шины адреса
											  микропроцессора PIC соответственно
				     
 output reg [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]ADRES_OUT - шина адреса буфера строки Альтеры
 output MEM_EN_RD		- сигнал разрешения чтения буфера строки Альтеры		
 output CLK_MEM_READ - синхро сигнал чтения буфера строки Альтеры
 output  test_120
*/
module    ADRES_OUT_MD( input RES_HARD,input CLK, input CS_READ_MTR, input  	INT_RESET_RE8,
						input ALTCS, input PMPRD, input PMPWR, input PMPA0,
				     
				     output reg [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]ADRES_OUT, output MEM_EN_RD,
				     output CLK_MEM_READ,output  test_120
					);
					

//сдвинутые на один и два такта сигнала СLK соответственно, сигнал младшего бита шины адреса микропроцессора PIC				
reg shift_pmpA0, shift_shift_pmpA0;

//change_pmpA0 - сигнал изменения фронта младшего бита шины адреса микропроцессора PIC
//mtr_res - не садействован
//res_adres_out - связь сигнала INT_RESET_RE8
wire change_pmpA0, mtr_res; 
wire res_adres_out/* synthesis noprune */;


//сигнал изменения фронта младшего бита шины адреса микропроцессора PIC
assign change_pmpA0 = ((shift_pmpA0 ^ shift_shift_pmpA0) & CS_READ_MTR);

//- сигнал разрешения чтения буфера строки Альтеры	
assign MEM_EN_RD = (~change_pmpA0 & CS_READ_MTR);

//Отладочный
assign test_120 = ADRES_OUT == 2302;

//- синхро сигнал чтения буфера строки Альтеры
assign CLK_MEM_READ = (~(PMPA0 ^ shift_shift_pmpA0)) & CS_READ_MTR;

assign res_adres_out = INT_RESET_RE8;


//-------------------------------------------------------------------------------------
//сдвинутые на один и два такта сигнала СLK соответственно, сигнал младшего бита шины адреса микропроцессора PIC	
//-------------------------------------------------------------------------------------
always @(posedge RES_HARD, negedge CLK)
begin
if(RES_HARD == 1)
begin
	shift_pmpA0 <= 0; shift_shift_pmpA0 <= 0;
end
else
begin
	if(CLK == 0)
	begin
		shift_pmpA0 <= PMPA0;
		shift_shift_pmpA0 <= shift_pmpA0;
	end
end 
end
	
	
//---------------------------------------------------------------------------------	
//[11 : 0]ADRES_OUT - счетчик слов при выводе информации из буфера строк в PIC
//сбрасывается сигналом:
//	- res_adres_out( INT_RESET_RE8); 
// - при условии (ADRES_OUT == `D_END_ADRES_OUT_1152 )
//инкрементируется: 
//	- change_pmpA0
//---------------------------------------------------------------------------------	
always @(posedge RES_HARD, posedge change_pmpA0, posedge res_adres_out )
begin
if(RES_HARD == 1)
begin
	ADRES_OUT = `D_WIDTH_OF_ADRES_OUT_11'h00;
end
else
begin

	if(res_adres_out == 1) 
	begin
				ADRES_OUT = `D_OFFS_ADRES_OUT_0; 				
	end		
	else

	begin
		if(change_pmpA0 == 1) 
		begin
		
					if(ADRES_OUT == `D_END_ADRES_OUT_1152 )					
					begin							
						ADRES_OUT = `D_OFFS_ADRES_OUT_0; 
					end	
					else
					
					ADRES_OUT = ADRES_OUT + `D_WIDTH_OF_ADRES_OUT_11'h1;
		end	
	end			
end
end		
	
//-----------------------------------------------------------------------------------								
endmodule	