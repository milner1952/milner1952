`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module  SINXRO_AIN_6(input HARD_RESET,input out_pll, input TRIG_IN, input END, 
							MTR_CLK,//	input [5 : 0]DVDR,
							output reg TRIG_OUT, 
							output reg F0,output reg F1,output reg F2, output reg F3, 
							output reg M0, output reg M1, output reg M2, output reg M3, output reg M4,
							output reg [3 : 0]count_f10, output  SIN, output  K20, output reg SIN_GO, 
							output [5 : 0]W_COUNT_M10, output  W_COUNT_F10_NEXT, output COUNT3_NEXT,
							output NO_ADD_SUMM);



//Делитель на три входной частоты						
reg [2 : 0]count3;
// Перенос счетчика count3;
reg count3_next;

//Делитель на десять входной частоты
reg [5 : 0]count_m10;
// Перенос счетчика count_f10;	
reg count_f10_next;

//делитель используемый исключительно для синхронизации SignalTap2 
(*preserve = "true"*) reg [4 : 0]count_sin;

//сигнал задержанный отгосительно сигнала trig
reg s_trig;										

//сигнал задержанный относительно сигнала END				
reg s_end; 

//сигнал дважды задержанный относительно сигнала END	
reg s_s_end; 
reg sh3_trig;



wire chenge_trig/* synthesis keep */;  
wire chenge_end/* synthesis keep */; 
wire res_count/* synthesis keep */; 

assign NO_ADD_SUMM = sh3_trig & ~TRIG_IN;
assign W_COUNT_F10_NEXT = count_f10_next;
assign W_COUNT_M10 = count_m10;
assign chenge_end = (~s_end & s_s_end);
assign chenge_trig = (~s_trig & TRIG_OUT);
assign res_count = SIN_GO == 1'h1 ? chenge_trig : 1'h1;

assign K20 = count_sin[4];
assign SIN = count_sin[0]; 
assign COUNT3_NEXT = count3_next;
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  count_f10[1])
begin
if(HARD_RESET == 1)
begin
sh3_trig <= 0; 
end
else	
begin
         if(count_f10[1] == 1)
			begin
			  sh3_trig <= TRIG_IN;
			end  
end
end


//--------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
count_sin = 5'h0;
//r_res_count <= 0;
end
else	
begin
         if(out_pll == 1)
			begin
			  count_sin <= count_sin + 5'h1; 			 
			end  
end
end

//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge chenge_end , posedge chenge_trig )
begin
if(HARD_RESET == 1)
begin
SIN_GO = 0;
end
else	
begin		
	     if(chenge_trig == 1)
               SIN_GO = 1;
		  else
			if(chenge_end  == 1)
						SIN_GO = 0;                        			  
end
end

//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
s_trig <= 0; TRIG_OUT <= 0; 
end
else	
begin
         if(out_pll == 1)
			begin
			  s_trig <= TRIG_IN;
			  TRIG_OUT <= s_trig;
			end  
end
end

//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
s_end <= 0; s_s_end <= 0; 
end
else	
begin
         if(out_pll == 1)
			begin
			  s_end <= END;
			  s_s_end <= s_end;
			end  
end
end

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
count3  <= 3'h0; 
count3_next <= 0; 
end
else	
begin
         if(out_pll == 1)
			begin
			
				if(res_count == 0)
				begin	
				   count3 <= count3 + 3'h1; 			
					if(count3 == 3'h2) //3
					begin
						count3 <= 3'h0; 
					end
						
					if(count3 == 3'h1) //3
					begin
						count3_next <= 1'h1;
					end
					else
					   count3_next <= 1'h0;						
						
						
	
				end
					else
					begin
					count3 <= 3'h0; count3_next <= 1'h0;
					end
					
			end			
end
end

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
 count_f10 <= 4'h0; 
 count_f10_next <= 0; 
end
else	
begin					
			if(out_pll == 1)
			begin
			if(res_count == 0)
				begin	
					if(count3_next == 1)										
					begin
						count_f10 <= count_f10 + 4'h1;	

						
						if(count_f10 == 4'h7) 
						begin
							count_f10 <= 4'h0;							
						end	
						
												

						if(count_f10 == 4'h6) 
						begin
							count_f10_next <= 1; 							
						end							
						else						
						   count_f10_next <= 0;
							
					end	
				end
			   else
				begin
				count_f10 <= 4'h0;	count_f10_next <= 0;
				end				
			end		
end
end

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
 count_m10 <= 6'h0; 

end
else	
begin	//0								
			if(res_count == 0)
				begin//2	
					if((count_f10_next == 1) & (count3_next  == 1))										
					begin//3
						count_m10 <= count_m10 + 6'h1;	
					end//3
            end//2				
	         else			
				begin//6
						count_m10 <= 6'h0; 
				end//6												     		
				
end//0
end
//---------------------------------------------------------------
//
//-----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge out_pll)
begin
	if(HARD_RESET == 1)
	begin  
		F0 = 0; F1 = 0; F2 = 0;  	F3 = 0;	
	end
	else
	begin		
																							
						case (count_f10)	
						4'h0:  		begin {F3,F2,F1,F0} = 4'h1; end									
						4'h2:  		begin {F3,F2,F1,F0} = 4'h2; end						
						4'h4:  		begin {F3,F2,F1,F0} = 4'h4; end											
						4'h6:    	begin {F3,F2,F1,F0} = 4'h8; end						
						default: 	begin {F3,F2,F1,F0} = 4'h0; end 					
						endcase										
						
	end
end

//--------------------------------------------------------------------
//-----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge out_pll)
begin
	if(HARD_RESET == 1)
	begin  
		M0 = 0; M1 = 0; M2 = 0;  M3 = 0;	M4 =0;	
	end
	else
	begin		
				if(out_pll == 1)
				begin	
				
				
					if(SIN_GO == 1)
					begin
					
						case (count_m10)												
						
						4'h1:  		begin {M4,M3,M2,M1,M0} = 5'h1; end						
						4'h3:  		begin {M4,M3,M2,M1,M0} = 5'h2; end											
						4'h5:    	begin {M4,M3,M2,M1,M0} = 5'h4; end						
						4'h7:    	begin {M4,M3,M2,M1,M0} = 5'h8; end								
						4'h9:    	begin {M4,M3,M2,M1,M0} = 5'h10; end							
						
						default: 	begin {M4,M3,M2,M1,M0} = 5'h0; end												
						endcase										
						
					end
					
					else
					
						begin {M4,M3,M2,M1,M0} = 5'h0; 
						end
				
				end		
	end
end


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
`ifndef D_USE_PLL_SYCLONE_1
PLLMUL10  altpll(CLK2, out_pll);	
`endif



endmodule

							