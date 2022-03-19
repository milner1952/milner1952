

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"




module 			S_AIN_BRD(input [`D_S_WIDTH_OF_DATA_8 - 1 : 0]ADC_D, input CLK,    		
								 input  RST,    		input _TRIG_0,       input   END_0,       
								 input  _TRIG_1,     input END_1,  		   input _TRIG_2,			
								 input  END_2,	   	input _TEST_TRIG,	   input TEST_END,	   
								 input  T_R,         input MX_BOARD,      input  _RD_BOARD,    
								 input _WR_BOARD,	   output  END_BOARD   , input  A0_BOARD,	   
								 input  A1_BOARD,	   input  A2_BOARD, 		input A3_BOARD,     
								 input  A4_BOARD,    input HARD_RESET,	   input OSC1,				
								 input OSC2,   		input [1:0]BA,       input BYSY0, 
								 input BYSY1 , input BYSY2 ,
								 inout   [`D_S_WIDTH_OF_DATA_8 - 1 : 0]PD,
								 
								 output reg RESET_ADC, 	output  _CONVECT,	output _TRIG_BOARD,	output  LOCK,	output V_GANE,	output MX ,	output CS_ADC_0,	
								 output CS_ADC_1,	output CS_ADC_2,	output _BYTESWAP,
								 output K6, output  K7, output K8
								 );
	

reg [7 : 0]comand_reg/* synthesis noprune */;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]pr;								 
reg [7 : 0]VersData[20];
reg [5 : 0]count_vers_data_out;
reg lo_hi;
reg ret_version; 
reg _R_WR_BOARD, K13;
reg s_byte_swap;
//reg s_s_byte_swap;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2L;


reg [`D_S_WIDTH_OF_BUFER_AD_20 - 1 : 0]bf_buf0;
reg [`D_S_WIDTH_OF_BUFER_AD_20 - 1 : 0]bf_buf1;
reg [`D_S_WIDTH_OF_BUFER_AD_20 - 1 : 0]bf_buf2;

reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf0/* synthesis keep */; 
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf1/* synthesis keep */;
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf2/* synthesis keep */; 


reg s_trig/* synthesis noprune */;
reg s_cs_adc2;


wire addr_get_data_x_ray/* synthesis keep */;
wire broadcasting_adres/* synthesis keep */;

wire [3 : 0]count_f10;
wire sin/* synthesis keep */;
wire sin_go/* synthesis keep */;
wire count_m10_next;

wire f0;

wire f1/* synthesis keep */;

wire f2,f3;
wire m0,m1,m2,m3,m4;
wire TRIG_OUT;


wire [3 : 0]count_m10/* synthesis keep */;

wire count_f10_next;


wire addr_board/* synthesis keep */; 
wire addr_comand/* synthesis keep */; 
wire chang_byte_swap/* synthesis keep */; 
wire _rs_byte_swap6/* synthesis keep */; 
wire fr_trig/* synthesis keep */;
wire res_sum_buf/* synthesis keep */;
//wire chenge_cs_adc2;

assign _CONVECT = (count_f10 == 4'h1 );
assign LOCK = ((count_f10 == 4'h0) & (count_m10 == 4'h0));

assign MX = MX_BOARD;
assign CS_ADC_0 = ~(count_f10 == 1 || count_f10 == 2);
assign CS_ADC_1 = ~(count_f10 == 3 || count_f10 == 4);
assign CS_ADC_2 = ~(count_f10 == 5 || count_f10 == 6);

assign _BYTESWAP = (CS_ADC_0 == 0 || CS_ADC_1 == 0 || CS_ADC_2 == 0) & count_f10[0];


 
assign PD = ((_RD_BOARD == 0) & (addr_board == 1)) ? pr : 8'bzzzzzzzz;	
								
assign _TRIG_BOARD = _TEST_TRIG;    //TRIG_OUT;
assign END_BOARD = TEST_END;       //TEST_END;



assign addr_board = (BA == {A4_BOARD, A3_BOARD}) ? 1'h1 : 1'h0; 
assign addr_comand =  ((A2_BOARD == 0) & (A1_BOARD == 1) & (A0_BOARD == 1));
assign addr_get_data_x_ray  =  (A0_BOARD == 0);

assign broadcasting_adres = ({A4_BOARD,A3_BOARD,A2_BOARD,A1_BOARD,A0_BOARD} == 5'h18);
assign K6 = chang_byte_swap;//broadcasting_adres;	 
assign chang_byte_swap = s_byte_swap ^ _BYTESWAP;
assign _rs_byte_swap6 = ~s_cs_adc2 & CS_ADC_2;//chenge_cs_adc2;//~((s_byte_swap & ~_BYTESWAP ) & (~CS_ADC_2));
assign K8 = _rs_byte_swap6;
assign fr_trig = s_trig & (~TRIG_OUT);//_TRIG_BOARD;
assign res_sum_buf = (count_m10 == 4'h0) & (count_f10 == 4'h0) & (f0 == 1) & (count3_next == 1)/* & (OSC1 == 1)*/;
//assign chenge_cs_adc2 = ~s_cs_adc2 & CS_ADC2;



//---------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge OSC1)
begin
if(HARD_RESET  == 1)
begin
   s_cs_adc2 <= 0;
	s_trig <= 0;
end
else
	if(OSC1 == 1)
		begin

			s_trig <= TRIG_OUT;//_TRIG_BOARD;
			s_cs_adc2 <= CS_ADC_2;
		end

end
//---------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge OSC1)
begin
if(HARD_RESET  == 1)
begin
	s_byte_swap <= 0;
	//s_s_byte_swap <= 0;
end
else
	if(OSC1 == 1)
		begin
			s_byte_swap <= _BYTESWAP;
			//s_s_byte_swap <= s_byte_swap;
		end

end

//---------------------------------------------------------------------------

always @(posedge HARD_RESET, negedge chang_byte_swap)
begin
if(HARD_RESET  == 1)
begin
	s_buf0 <= `D_S_WIDTH_OF_DATA_8'h0; 
	s_buf1 <= `D_S_WIDTH_OF_DATA_8'h0; 
	s_buf2 <= `D_S_WIDTH_OF_DATA_8'h0; 

	s_buf0L <= `D_S_WIDTH_OF_DATA_8'h0; 
	s_buf1L <= `D_S_WIDTH_OF_DATA_8'h0; 
	s_buf2L <= `D_S_WIDTH_OF_DATA_8'h0;	
end
else
		begin
			if(chang_byte_swap == 0)
				begin					 
				    s_buf1L <= s_buf2; s_buf2 <= s_buf2L; s_buf2L <= ADC_D;
                s_buf0 <= s_buf0L; s_buf0L <= s_buf1; s_buf1 <= s_buf1L;					 
					 
				end	 					 
		end
end

//---------------------------------------------------------------------------

always @(posedge HARD_RESET, posedge fr_trig, posedge res_sum_buf, negedge _rs_byte_swap6)
begin
if(HARD_RESET  == 1)
begin
 
	bf_buf0 <= `D_S_WIDTH_OF_BUFER_AD_20'h0;  
	bf_buf1 <= `D_S_WIDTH_OF_BUFER_AD_20'h0;  
	bf_buf2 <= `D_S_WIDTH_OF_BUFER_AD_20'h0; 
	
end 
else
   if(fr_trig == 1)
	begin
	
      out_bf_buf0 <= bf_buf0/12; 
		out_bf_buf1 <= bf_buf1/12;
		out_bf_buf2 <= bf_buf2/12;
	end
	else
	  if(res_sum_buf == 1)
	  begin
		bf_buf0 <= `D_S_WIDTH_OF_BUFER_AD_20'h0;  
		bf_buf1 <= `D_S_WIDTH_OF_BUFER_AD_20'h0;  
		bf_buf2 <= `D_S_WIDTH_OF_BUFER_AD_20'h0;			
		end
	else
	if(_rs_byte_swap6 == 0)
		begin
		/*
                bf_buf0 <= bf_buf0 + {s_buf0,s_buf0L}; 
					 bf_buf1 <= bf_buf1 + {s_buf1,s_buf1L};  
					 bf_buf2 <= bf_buf2 + {s_buf2,s_buf2L}; 			 					 
					 */
                bf_buf0 <= bf_buf0 + {s_buf0L,s_buf0}; 
					 bf_buf1 <= bf_buf1 + {s_buf1L,s_buf1};  
					 bf_buf2 <= bf_buf2 + {s_buf2L,s_buf2}; 					 
		end
end

//-----------------------------------------------------------------------------
always @(posedge HARD_RESET)
begin

if(HARD_RESET == 1)
begin  
  VersData[0] =  ",";
  VersData[1] =  " ";
  VersData[2] = "A";  
  VersData[3] = "B";
  VersData[4] = "R";
  VersData[5] = "D";
  VersData[6] = {4'h3,2'h0,BA};
  VersData[7] = ":";
  VersData[8] = " ";
  VersData[9] = "v";
  VersData[10] = "3";
  VersData[11] = ".";
  VersData[12] = "0";
  VersData[13] = ".";
  VersData[14] = "0";
  VersData[15] = "0";
  VersData[16] = "0";
  VersData[17] = " ";
  VersData[18] = " ";
  VersData[19] = " ";
 
end    
end

//-------------------------------------------------
always @(posedge HARD_RESET, posedge CLK)
begin
 if(HARD_RESET == 1)
	RESET_ADC = 0;
 else
   if((CLK  == 1) & (HARD_RESET == 0))
	begin 
		RESET_ADC = 1;  
	end		
end	
	


//----------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge _WR_BOARD)
begin
if(HARD_RESET == 1)
begin
   comand_reg = 8'h0;  
end
else
	begin
		if(_WR_BOARD == 0)
		begin
				if(broadcasting_adres == 1)
				begin
				if(PD[0] == 1)
				begin
				    comand_reg[1] = PD[1];
				end
				if(PD[2] == 1)
					comand_reg[3] = PD[3];
					comand_reg[4] = PD[4];
				end
		end
   end
end



//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  _WR_BOARD )
begin
if(HARD_RESET == 1)
begin
   ret_version = 0; //K8 = 0; 
end
else
begin
     if(_WR_BOARD ==  1)
	  begin
			if(addr_board == 1)
			begin
			   if(addr_comand == 1)
				begin
							//				K8 = ~K8;
							ret_version = PD[0]; 		   					
				end			
			end
			else
			ret_version = 0; 	
      end
end
end

//----------------------------------------------------------------------------
always @(posedge HARD_RESET,  negedge _RD_BOARD)
begin
if(HARD_RESET == 1)
begin
lo_hi = 0; count_vers_data_out = 6'h0; 
end
else	
	 if(_RD_BOARD == 0) 			 
	 begin
				if(addr_board == 1)
				 begin
				 
						if(addr_comand  == 1)
						begin	
								if(ret_version == 1)
								begin								
										pr = count_vers_data_out == 5'h6 ? {4'h3,2'h0,BA} : VersData[count_vers_data_out];	 											
										count_vers_data_out = count_vers_data_out +5'h1; 										
								end				         
						end		
						else
						if((addr_get_data_x_ray  == 1))
						begin
										//if(lo_hi == 1)
										if(lo_hi == 0)
										begin
											//pr  = {4'h0,BA,A2_BOARD,A1_BOARD};
											pr = {A2_BOARD,A1_BOARD} == 2'h00 ? out_bf_buf0[7 : 0] :
											     {A2_BOARD,A1_BOARD} == 2'h01 ? out_bf_buf1[7 : 0] : 
												  {A2_BOARD,A1_BOARD} == 2'h02 ? out_bf_buf2[7 : 0] : 8'bzzzzzzzz ;
										end
										else
										begin
											//pr  =  8'h0;
											pr = {A2_BOARD,A1_BOARD} == 2'h00 ? out_bf_buf0[15 : 8] :
											     {A2_BOARD,A1_BOARD} == 2'h01 ? out_bf_buf1[15 : 8] :
												  {A2_BOARD,A1_BOARD} == 2'h02 ? out_bf_buf2[15 : 8] : 8'bzzzzzzzz ;											
										end
										lo_hi = ~ lo_hi;
						end
						else
						begin
						lo_hi = 0;	
						end
						
				end
		end	
	 
end	



SINXRO_AIN_6 sinxro_ain_6(HARD_RESET, OSC1,_TEST_TRIG,END_BOARD, res_sum_buf , /*chang_byte_swap*/LOCK, TRIG_OUT, 
							     f0,f1,f2,f3, 
							     m0, m1, m2, m3, m4,
							     count_f10, sin, K7, sin_go, count_m10_next,
								  count_m10, count_f10_next, count3_next
								  );


endmodule
