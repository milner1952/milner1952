`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module 	TEST_MD(input RES_HARD, input M0,input M1,input M2,input M3,  
				input MTR_CLK, input _RUN, input _MTR_RES, input TEST_GO, 
				
				
				output  _TEST_TRIG, output _TEST_END, output reg K13 
				);
				
reg shift_mtr_res, count_wait_trig_go, trig_go, shift_end;				
reg [4 : 0]count_wait_trig ;
reg [1 : 0]count_mtr_clk;
reg [7 : 0]count_trig; 
reg test_trig;
wire change_mtr_res, change_end;

assign _TEST_TRIG = ~test_trig;
assign change_mtr_res = _MTR_RES & (~shift_mtr_res);
assign change_end = ~_TEST_END & shift_end; 
assign test_122 = count_wait_trig_go;
assign _TEST_END = (count_trig == ((`D_NUM_WORD_OF_ADRES_IN_1152/9) - 1'h1)); //1'h0
//----------------------------------------------------------------------------------			
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
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

