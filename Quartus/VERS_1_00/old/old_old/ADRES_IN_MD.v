`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"






module 	ADRES_IN_MD(input RES_HARD, input F0, input F1, input F2,
							input M0, input M1, input M2,input  M3,  
							input MTR_CLK, input PIC_SINXRO_RF12, input _RUN, input  _TRIG, input END,
							input CHENGE_ADRES_IN, input _RD_SXR,
							
							
							output reg INT_RESET_RE8, output reg _MTR_RST, output reg COUNT_F_GO,
							output reg [`D_WIDTH_OF_ADRES_IN_11 - 1  : 0]ADRES_IN  , output _RD,output reg ADRES_IN_11
							);
		
		
reg flag_rdy/* synthesis noprune */;		
reg go_counter_mtr_rst;
reg [`D_WIDTH_OF_COUNTER_MTR_RST - 1:0]counter_mtr_rst;	
reg shift_trig;
reg end_of_end_adres_in;
reg [3 : 0]count_chenge_mtr_clk;
reg count_chenge_mtr_clk_go;
reg end_adres_in;
wire chenge_trig;

assign _RD = _RUN == 1'h0 ?_RD_SXR | (shift_trig & (count_chenge_mtr_clk == 4'h4)) : 1'h1;

assign chenge_trig = shift_trig & ~_TRIG & (_RUN == 0);


//----------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge M2)
begin
	if(RES_HARD == 1)
	begin
		end_adres_in = 0;
	end
	else
	begin
	if(M2 == 1)
		begin
				end_adres_in = ((`D_NUM_WORD_OF_ADRES_IN_1152 - 1) == ADRES_IN );	
		end
	end
end	

//----------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge M3)
begin
	if(RES_HARD == 1)
	begin
		flag_rdy = 0; 
	end
	else
      if(M3 == 1)
		begin
				if(PIC_SINXRO_RF12 == 1) 
				begin
				    if(_RUN == 0)
					 begin
						if(INT_RESET_RE8 == 1)
						begin
							flag_rdy = 1;					
						end
					end	
				end	
				else
				begin				
			         if(MTR_CLK == 0)
						begin
							flag_rdy = 0; 
						end	
				end	
		 end
end

//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge M3, posedge M1)
begin
	if(RES_HARD == 1)
	begin
		 INT_RESET_RE8 = 1;
	end
	else
	
	   if(M3 == 1)
		begin
				if( end_of_end_adres_in == 1)
				begin 					
						INT_RESET_RE8 = 1;
				end				
		end		
	   else
	   if(M1 == 1)
      begin		
					if(flag_rdy == 1)
					begin
							if(_RUN == 0)
							begin
								INT_RESET_RE8 = 0;  			 
							end
					end
      end	
	
end


//------------------------------------------------------------------------
always @(posedge RES_HARD,posedge M1, posedge M2)
begin
	if(RES_HARD == 1)
	begin
		 go_counter_mtr_rst = 0;  counter_mtr_rst <= `D_WIDTH_OF_COUNTER_MTR_RST'h0; 		 	
	end
	else		
		if((M1 == 1))
		begin
		         if(flag_rdy == 1)
					begin
							if(_RUN == 0)
							begin
							   if(MTR_CLK == 0)
									go_counter_mtr_rst = 1;
							end
					end		
		end
      else
	
		if(M2 == 1)
		begin

		         if(go_counter_mtr_rst == 1)
					begin
					      counter_mtr_rst <= counter_mtr_rst + `D_WIDTH_OF_COUNTER_MTR_RST'h1;
							if(counter_mtr_rst == `D_SZ_MTR_RST)
							begin
							counter_mtr_rst <= `D_WIDTH_OF_COUNTER_MTR_RST'h0;
							go_counter_mtr_rst = 0;
							end        
					end
		
		end

end




//----------------------------------------------------------
always @(posedge RES_HARD, posedge MTR_CLK)
begin
	if(RES_HARD == 1)
	begin
         _MTR_RST = 1;   
	end
	else
	   if(MTR_CLK == 1)
		begin
				if(go_counter_mtr_rst == 1)
				begin
					_MTR_RST = 0;
				end	
				else
				begin
					_MTR_RST = 1;			  
				end	
		end	  
end

//------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
always @(posedge RES_HARD, negedge M0)
begin
	if(RES_HARD == 1)
	begin
		shift_trig = 0; 
	end
	else
	begin
	
	   if(M0 == 0)
		begin
			shift_trig = _TRIG;
		end
	end
end	


//------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
always @(posedge RES_HARD, negedge _MTR_RST , negedge MTR_CLK,  posedge end_adres_in)
begin
	if(RES_HARD == 1)
	begin
		count_chenge_mtr_clk = 4'h0; count_chenge_mtr_clk_go = 0;
	end
	else
	begin//2
		if(_MTR_RST == 0) 
		begin
	   		count_chenge_mtr_clk = 4'h0; count_chenge_mtr_clk_go = 0;
		end			
		else
		if(end_adres_in == 1)
		begin
				count_chenge_mtr_clk = 4'h0; count_chenge_mtr_clk_go = 0;
		end			
      else //1
	   if(MTR_CLK == 0)
		begin
				if(_TRIG == 0)
				begin		    
				    count_chenge_mtr_clk_go = 1;
				end

				    if((count_chenge_mtr_clk < 4'h8) & (count_chenge_mtr_clk_go == 1))
						begin
							count_chenge_mtr_clk = count_chenge_mtr_clk + 4'h1;
						end		
		
		end //1
			
	end//2
end	



//------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge chenge_trig, negedge _MTR_RST, posedge end_adres_in)
begin
	if(RES_HARD == 1)
	begin
		COUNT_F_GO = 0; 
	end
	
	else
	
	if(_MTR_RST == 0) 
	begin
	   COUNT_F_GO = 0; 
	end
	else	
	if(end_adres_in == 1)
	begin
	    if(M0 == 1)
		 begin
			COUNT_F_GO = 0; 
		 end	
	end
	else
	if(chenge_trig == 1 )
	begin
	   begin
			if(count_chenge_mtr_clk == 4'h4) 
			begin
				COUNT_F_GO = 1;	
			end	
		end
	end	
	
end


//----------------------------------------------------------------------------
always @(posedge RES_HARD, posedge CHENGE_ADRES_IN, negedge COUNT_F_GO, posedge _RUN)
begin
	if(RES_HARD == 1)
	begin
		ADRES_IN = `D_WIDTH_OF_ADRES_IN_11'h0; ADRES_IN_11 = 0;
	end
	else
	begin
	   if(_RUN == 1)
		begin
		ADRES_IN_11 = 0;
		end
		else
	   if(COUNT_F_GO == 0)
		begin
			ADRES_IN = `D_WIDTH_OF_ADRES_IN_11'h0; 
	   end
		else
		
		if(CHENGE_ADRES_IN == 1)
		begin
			ADRES_IN = ADRES_IN + `D_WIDTH_OF_ADRES_IN_11'h1; 			
			if(ADRES_IN == `D_NUM_WORD_OF_ADRES_IN_1152)
			begin
				if(ADRES_IN[10] == 1)
				begin
					ADRES_IN_11 = 	~ADRES_IN_11;
				end
				ADRES_IN = `D_WIDTH_OF_ADRES_IN_11'h0;         				
			end
		end
	end	
end
//-----------------------------------------------------------
//------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
always @(posedge RES_HARD, posedge M0)
begin
	if(RES_HARD == 1)
	begin
		end_of_end_adres_in = 0; 
	end
	else
	begin
	
	   if(M0 == 1)
		begin
			end_of_end_adres_in = end_adres_in;
		end
	end
end
endmodule

