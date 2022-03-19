
//----------------------------------------------------
//Узел синхронизации 4 фазы с делителем частоты 
//-----------------------------------------------------
`timescale 1ns/10 ps 

module   SINXRO(input RES_HARD,input  OSC, 
					 input [7 : 0]DELIMER,
					 output reg C0,output reg C1,output reg C2,output reg C3
					 );
					 
reg div_out;
reg [7 : 0]count_div_out;
reg [3 : 0]count_c4;

//-----------------------------------------------------------------
always @(posedge RES_HARD, negedge OSC)
begin
	if(RES_HARD == 1)
	begin
    div_out = 0;	
	 count_div_out = 8'h0;
	end
	else
	begin
	if(OSC == 0)
	begin 
		count_div_out = count_div_out + 8'h1;
		if(count_div_out == DELIMER)
			begin
			count_div_out = 8'h0;
			div_out = ~div_out;
			end		
	end	
	end
end

//----------------------------------------------------------------
always @(posedge RES_HARD, negedge div_out)
begin
	if(RES_HARD == 1)
	begin
    C0 = 0; C1 = 0; C2 = 0; C3 = 0;
	 count_c4 = 4'h0;
	end
	else
		begin
	
				case (count_c4)	
				4'h0:  		begin {C3,C2,C1,C0} <= 4'h1; end							
				4'h2:  		begin {C3,C2,C1,C0} <= 4'h2; end											
				4'h4:    	begin {C3,C2,C1,C0} <= 4'h4; end						
				4'h6:    	begin {C3,C2,C1,C0} <= 4'h8; end		
				default: 	begin {C3,C2,C1,C0} <= 4'h0; end 
				endcase	
	
				count_c4 = count_c4 + 4'h1;
				if(count_c4 == 4'h8)
				begin
					count_c4 = 4'h0;
				end	 
		end
end






endmodule
