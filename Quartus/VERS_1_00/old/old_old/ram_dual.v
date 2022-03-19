
`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module ram_dual(data, read_addr,  write_addr, write_en,  read_clock,  
					 write_clock, q, adr_out0 ,test, read_en);
					 
parameter num_words = (1152+ 1);
parameter wd_adres = `D_WIDTH_OF_ADRES_IN_11;
parameter wd_data = `D_S_WIDTH_OF_DATA_AD_16;

	input [wd_data - 1 : 0] data;
	input [wd_adres - 1 : 0] read_addr;
	input [wd_adres - 1 : 0]write_addr;
	input wire write_en, read_clock, adr_out0, read_en;
	input wire write_clock;
	output reg [wd_data - 1 : 0] q;
	reg [wd_data - 1 : 0] q_reg;

	output reg test;

	reg [wd_data - 1:0]ram[num_words - 1 : 0];

	always @ ( posedge write_clock)
	begin
         //Write
		if (write_en == 1)begin

			ram[write_addr] <= data; test = ~test;
			end						
	end
		
//-------------------------------------------------------		
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
