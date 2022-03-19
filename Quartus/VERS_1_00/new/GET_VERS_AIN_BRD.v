
`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module GET_VERS_AIN_BRD( input RES_HARD, input GET_VERSION,
								 input [1 : 0]ADRES_BOARD,  
								 input F0,input  F1,input F3, input STATE, input [1 : 0]COUNT_3P,	
						       output  reg [7 : 0]DT,	
						       output  _RD,output reg _WR,  
								 output  [7 : 0]CNT, 
								 output reg A4, output reg A3, output reg A2,output reg A1,output reg A0,
								 output reg flag_init
								 );		
	
assign CNT = COUNT;

assign _RD = ((STATE == 0) & (flag_init == 1))? _rd : 1'h1;
reg _rd;
reg flag_get;
reg [7 : 0]COUNT;
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



	





