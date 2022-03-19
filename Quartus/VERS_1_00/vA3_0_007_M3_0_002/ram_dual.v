
`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"
//----------------------------------------------------------------------------------------------
//Двупортовый блок памяти, с разными шинами для записи чтени информации
// параметр: 
//			-num_words - емкость памяти в словах
//			-wd_adres - ширина шины адреса
//			-wd_data - ширина шины данных
//-----------------------------------------------------------------------------------------------
module ram_dual(data, read_addr,  write_addr, write_en,  read_clock,  
					 write_clock, q, adr_out0 ,test, read_en);
					 
parameter num_words = (1152+ 1);
parameter wd_adres = `D_WIDTH_OF_ADRES_IN_11;
parameter wd_data = `D_S_WIDTH_OF_DATA_AD_16;
	// Входная шина
	input [wd_data - 1 : 0] data;
	//Адрес чтнгия
	input [wd_adres - 1 : 0] read_addr;
	//Адрес записи
	input [wd_adres - 1 : 0]write_addr;
	//Разрешение записи, синхросигнал чтения, младший разряд шины адреса, разрешение чтения
	input wire write_en, read_clock, adr_out0, read_en;
	//синхросигнал записи
	input wire write_clock;
	//выходной регистр
	output reg [wd_data - 1 : 0] q;
	
	//буфер выходного регистра
	reg [wd_data - 1 : 0] q_reg;
	
   //сигнал для тестирования
	output reg test;
	
   //Блок памяти
	reg [wd_data - 1:0]ram[num_words - 1 : 0];

//-------------------------------------------------------------------------------------
//Запись в память по адресу write_addr сигналом write_clock при write_en == 1
//--------------------------------------------------------------------------------------
	always @ ( posedge write_clock)
	begin
         //Write
		if (write_en == 1)begin

			ram[write_addr] <= data; test = ~test;
			end						
	end
		
//-------------------------------------------------------	
// Чтение из памяти в буферный регистр q_reg по сигналу	read_clock по
// адресу read_addr при наличии write_en == 0
	always @ (posedge read_clock)
	
	begin
		if(read_clock == 1)
		begin
			if(write_en == 0)
			begin    
				q_reg = ram[read_addr];   
			end
		end	
   end
	
//-----------------------------------------------------
//Перезапись содержимого буферного регистра q_reg в выходной регистр
//q при любом изменении adr_out0, рпри (read_clock == 1) и ((write_en == 0) & (read_en == 1)) 
//если условие  ((write_en == 0) & (read_en == 1))   не соблюдается,
//то регистр q переходит в третье состояние
	always @ (adr_out0)	
	begin
		if((write_en == 0) & (read_en == 1))
		begin 
         if(read_clock == 1)
			begin    
				q <= q_reg; 
			end
		end	
			else
		begin	
			  q <= 16'bzzzzzzzzzzzzzzzz;	
		end	  				
   end
	
		
endmodule
