
`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

/*
input RES_HARD - аппаратный сборос 
input GET_VERSION - синхронизированная команда микропроцессора по шине RE_AD
input [1 : 0]ADRES_BOARD - входной сигнал счетчика перебора аналоговых плпт датчика
input F0,F1,F3 - три фазы синхроимпульсов
input STATE - бит слстояния D_VERSION_BIT_state_0
input [1 : 0]COUNT_3P - счетчик 	[1 : 0]count3p модуля синхронизации 

output  reg [7 : 0]DT - Восьмиразрядная двунаправленная шина данных аналоговых блоков
output  _RD - выход чтения информации аналоговых блоков по восьмиразрядной шине данных аналоговых блоков 
output reg _WR - выход записи информации аналоговых блоков по восьмиразрядной шине данных аналоговых блоков  
output  [7 : 0]CNT - связи счетчика w_count_vers_data_in - адресация байт при их чтении в буфер данных о версии из аналоговых блоков 
output reg A4,A3,A2,A1,A0 - шина адреса аналоговых блоков при обмене информации по восьмиразрядной шине данных аналоговых блоков  
output reg flag_init - flag_init == 1 означает, что команда D_S_COMMAND_GET_VERSION_001 передана певому аналоговому блоку и информацию
                                      о версии блоков можно считывать. 

*/
module GET_VERS_AIN_BRD( input RES_HARD, input GET_VERSION,
								 input [1 : 0]ADRES_BOARD,  
								 input F0,input  F1,input F3, input STATE, input [1 : 0]COUNT_3P,	
						       output  reg [7 : 0]DT,	
						       output  _RD, output reg _WR,  
								 output  [7 : 0]CNT, 
								 output reg A4, output reg A3, output reg A2,output reg A1,output reg A0,
								 output w_flag_init/* synthesis keep */
								 );		
	
reg flag_init;
assign w_flag_init = flag_init;	
//адресация байт при их чтении из аналоговых блоков, в буфер данных VersData  	
assign CNT = COUNT;

//сигнал чтения при выводе информации из аналоговых блоков, в буфер данных VersData  
//при (STATE == 0) & (flag_init == 1) равен _rd,в противном случае равен единице	
assign _RD = ((STATE == 0) & (flag_init == 1))? _rd : 1'h1;

//сигнал чтения при выводе информации из аналоговых блоков, в буфер данных VersData
reg _rd;

//при GET_VERSION == 1, flag_get == 0 - чтение информации о версии прошивок из аналоговых блоков в в буфер VersData 
//при GET_VERSION == 1, flag_get == 1 - чтение информации о версии прошивок из буфера VersData в микропроцессор PIC по шине RE_AD
reg flag_get;

//адресация байт при их чтении из аналоговых блоков, в буфер данных VersData  
reg [7 : 0]COUNT;


//---------------------------------------------------------------------------------------------
// Модуль реализует выдачу команды о считывании версии в аналоговый блок и выдачу сигнала чтения при приеме
//информации о версии прошивки блоков и их адресацию при этом
// По фазе F0 при 
//    - (flag_get == 0):  _rd = 1; _WR = 1;  {A4,  A3,  A2,  A1, A0} = 5'h0; DT = 8'bzzzzzzzz;
//        освобождая тем самым указанные шины и сигналы
//		- (flag_get == 1)  и (COUNT == 8'h0): 
//          -на шину адреса выводится  {ADRES_BOARD,`D_S_ADRES_COMMAND_03}- адрес платы
//		и адрес команды аналогового блока. 
//          -на шину данных DT - команда D_S_COMMAND_GET_VERSION_001
//          - _WR = ~COUNT_3P[0] 
// По фазе F1 анализируется состояние сигнала flag_get:
//     - при (flag_get == 1) 			DT = 8'bzzzzzzzz;  _rd = 0; _WR = 1;
//             освобождая тем самым шину DT и сигнал _WR и устанавливая сигнал чтения 
//					в рабояее состояние.
// По фазе F3 при ((flag_get == 1) & (flag_init == 1))
//              изменяется состояние счетчика COUNT
//       если  ((flag_get == 1) & (flag_init == 1)) - не выполняктся то:
//       COUNT = 0, 
//			сигнал в любом случае: _rd = 1
//-----------------------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge F0, posedge F1, posedge F3)
begin
		if(RES_HARD == 1)
		begin
				COUNT = 8'h0; _rd = 1; _WR = 1;  flag_get = 0; flag_init = 0;
		end
		
		else
		
		begin		
		if(F0 == 1)
		begin
		   if(flag_get == 1)
			begin
				if(COUNT == 8'h0) 
				begin	
					{A4,  A3,  A2,  A1, A0} =  {ADRES_BOARD,`D_S_ADRES_COMMAND_03};
					DT =  `D_S_COMMAND_GET_VERSION_001; _WR = ~COUNT_3P[0]; flag_init = 1;
				end					
		        _rd = 1;								
			end	
			else
			begin			 
			    _rd = 1; _WR = 1;  {A4,  A3,  A2,  A1, A0} = 5'h0; DT = 8'bzzzzzzzz;
			end			
	   end
	  
	   else
	  
	   if(F1 == 1) 
 	   begin
				flag_get = GET_VERSION & (STATE == 0);
				if(flag_get == 1)
				begin
				DT = 8'bzzzzzzzz; 
				_rd = 0; _WR = 1;
				end
	   end	
	  
	   else
	    
	   if(F3 == 1)
	   begin
				
				if((flag_get == 1) & (flag_init == 1))
				begin				
					      COUNT = COUNT + 8'h1;					   
							if(COUNT == `D_S_NUM_BYTE_GET_VERSION_017)
							begin
								COUNT <= 8'h0; 
							end							
			   end
				else
				begin
				      COUNT = 8'h0; 
				end
				_rd = 1;
	   end	
	 
	   end	
		
end
	
endmodule



	





