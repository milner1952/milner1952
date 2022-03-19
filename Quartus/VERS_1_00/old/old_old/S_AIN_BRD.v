

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

reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2L;


reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf0;
reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf1;
reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf2;

reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf0/* synthesis keep */; 
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf1/* synthesis keep */;
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf2/* synthesis keep */; 


reg s_trig/* synthesis noprune */;
reg s_cs_adc2;

reg [12 : 0]count_mtr_rst;
reg [7 : 0]count_trig;
reg [9 : 0]count_adres_board;

reg [5 : 0]counter_dvdr;
//reg r_change_bute_swap;

reg load_byte_from_adc;

wire addr_get_data_x_ray/* synthesis keep */;
wire broadcasting_adres/* synthesis keep */;

wire [3 : 0]count_f10;
wire sin/* synthesis keep */;
wire sin_go/* synthesis keep */;
//wire count_m10_next;

wire f0;

wire f1/* synthesis keep */;

wire f2,f3;
wire m0,m1,m2,m3,m4;
wire TRIG_OUT;


wire [5 : 0]count_m10/* synthesis keep */;

wire count_f10_next;

wire [15 : 0]w_s_buf0/* synthesis keep */;   
wire [15 : 0]w_s_buf1/* synthesis keep */;  
wire [15 : 0]w_s_buf2/* synthesis keep */;  



wire addr_board/* synthesis keep */; 
wire addr_comand/* synthesis keep */; 
wire chang_byte_swap/* synthesis keep */; 
//wire _rs_byte_swap6/* synthesis keep */; 
wire _add_sum_bufer/* synthesis keep */; 
wire fr_trig/* synthesis keep */;
wire res_sum_buf/* synthesis keep */;
//wire [5 : 0]dvdr_to_sin;
wire s_mx_board;
reg sh_mx_board;
wire _K7;
wire res_counter_dvdr/* synthesis keep */; 
wire no_add_sum;


assign res_counter_dvdr = (count_m10 == 6'h1);
assign MX = MX_BOARD;
/*
assign LOCK = ((count_m10 == 6'h1) | (count_m10 == 6'h0)) & ~END_BOARD & sin_go;
assign _CONVECT = ~(`PURE_SYCLE_CONVECT &  (count_f10 == 4'h1 ) & (~(END_BOARD & (~_TRIG_BOARD))) & (count_m10 != 6'h0));
assign CS_ADC_0 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 1 || count_f10 == 2) &  (~(END_BOARD & (~_TRIG_BOARD)))) ;
assign CS_ADC_1 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 3 || count_f10 == 4) &  (~(END_BOARD & (~_TRIG_BOARD))));
assign CS_ADC_2 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 5 || count_f10 == 6) &  (~(END_BOARD & (~_TRIG_BOARD)& (BYSY2 == 1) )
));
*/
assign LOCK = ((count_m10 == 6'h1) | (count_m10 == 6'h0)) /*& ~END_BOARD */& sin_go;
assign _CONVECT = ~(`PURE_SYCLE_CONVECT &  (count_f10 == 4'h1 )/* & (~(END_BOARD & (~_TRIG_BOARD)))*/ & (count_m10 != 6'h0));
assign CS_ADC_0 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 1 || count_f10 == 2) /*&  (~(END_BOARD & (~_TRIG_BOARD)))*/) ;
assign CS_ADC_1 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 3 || count_f10 == 4)/* &  (~(END_BOARD & (~_TRIG_BOARD)))*/);
assign CS_ADC_2 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 5 || count_f10 == 6) /*&  (~(END_BOARD & (~_TRIG_BOARD)& (BYSY2 == 1) ))*/);


assign _BYTESWAP = ((CS_ADC_0 == 0 || CS_ADC_1 == 0 || CS_ADC_2 == 0) & count_f10[0])/* & (~(END_BOARD & (~_TRIG_BOARD))) */;
assign chang_byte_swap = (s_byte_swap ^ _BYTESWAP) &  (count_m10 != 6'h1);
assign _add_sum_bufer = ~s_cs_adc2 & CS_ADC_2 & `PURE_SYCLE_BYTESWAP_6 & (count_m10 != 6'h1) & (~no_add_sum);



assign w_s_buf0 = {s_buf0L,s_buf0};
assign w_s_buf1 = {s_buf1L,s_buf1};
assign w_s_buf2 = {s_buf2L,s_buf2};

 
assign PD = ((_RD_BOARD == 0) & (addr_board == 1)) ? pr : 8'bzzzzzzzz;	
								
assign _TRIG_BOARD = comand_reg[1] == 1'h1 ? _TEST_TRIG :(_TRIG_0 | _TRIG_1 | _TRIG_2);   
assign END_BOARD = comand_reg[1] == 1'h1 ? TEST_END : ( END_0 |  END_1 | END_2);      


assign addr_board = (BA == {A4_BOARD, A3_BOARD}) ? 1'h1 : 1'h0; 
assign addr_comand =  ((A2_BOARD == 0) & (A1_BOARD == 1) & (A0_BOARD == 1));
assign addr_get_data_x_ray  =  (A0_BOARD == 0);

assign broadcasting_adres = ({A4_BOARD,A3_BOARD,A2_BOARD,A1_BOARD,A0_BOARD} == 5'h18);
assign K6 = count_mtr_rst[12] | count_trig[7] | count_adres_board[8] ;
	 

assign K8  = addr_board;


/*
assign K8 = (_rs_byte_swap6 | fr_trig) & ((out_bf_buf0 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
													 (out_bf_buf1 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
                                        (out_bf_buf2 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
													 (bf_buf0 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
													 (bf_buf1 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
													 (bf_buf2 != `D_S_WIDTH_OF_BUFER_AD_24'h0));
													 */
assign fr_trig = s_trig & (~TRIG_OUT);
assign res_sum_buf = (count_m10 == 6'h0) & (count_f10 == 4'h0) & (f0 == 1) & (count3_next == 1);
assign s_mx_board = (sh_mx_board & (~MX_BOARD));



//---------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge OSC1)
begin
if(HARD_RESET  == 1)
begin
load_byte_from_adc = 0; 
end
else
  load_byte_from_adc <= chang_byte_swap;
end

//---------------------------------------------------------------------------

always @(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET  == 1)
begin
sh_mx_board = 0; 
end
else
  sh_mx_board <= MX_BOARD;
end

//-------------------------------------------------------------------------
//********************************************************************
//----------------------------------------------------------------------------
always @(posedge HARD_RESET,  posedge addr_board, negedge RST)
begin
if(HARD_RESET  == 1)
begin
count_adres_board <= 9'h0; 
end
else
  begin
   if(RST == 0)
	begin
	         count_adres_board <= 9'h0;
	end			
   else
	if(TRIG_OUT == 1)
		begin
				count_adres_board <= count_adres_board + 9'h1;
		end
	end			
end
//----------------------------------------------------------------------------
//***********************************************************************
//------------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge RST, posedge s_mx_board)
begin
if(HARD_RESET  == 1)
begin
count_mtr_rst <= 13'h0; 
end
else
  begin
   if(s_mx_board == 1)
	begin
	         count_mtr_rst <= 13'h0;
	end			
   else
	if(RST == 0)
		begin
				count_mtr_rst <= count_mtr_rst + 13'h1;
		end
	end			
end

//-------------------------------------------------------------------------
//********************************************************************
//----------------------------------------------------------------------------
always @(posedge HARD_RESET,  posedge TRIG_OUT, negedge RST)
begin
if(HARD_RESET  == 1)
begin
count_trig <= 8'h0; 
end
else
  begin
   if(RST == 0)
	begin
	         count_trig <= 8'h0;
	end			
   else
	if(TRIG_OUT == 1)
		begin
				count_trig <= count_trig + 8'h1;
		end
	end			
end


/*
//-------------------------------------------------------------------------
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge _rs_byte_swap6, posedge res_counter_dvdr)
begin
if(HARD_RESET == 1)
begin
counter_dvdr = 6'h0;
end
else	
begin
         if(res_counter_dvdr == 1)
			begin
            counter_dvdr = 6'h0; 
			end
			else
         if(_rs_byte_swap6 == 1)
			begin
				counter_dvdr <= counter_dvdr + 6'h1; 			 
			end  
 
end
end
*/

//-------------------------------------------------------------------------
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge OSC1)
begin
if(HARD_RESET == 1)
begin
counter_dvdr <= 6'h0;
//strob_res_sin_go <= 0; 
end
else	
begin
         if(res_counter_dvdr == 1)
			begin
            counter_dvdr <= 6'h0; 
			//	strob_res_sin_go <= 1; 
			end
			else
			begin
			//strob_res_sin_go <= 0; 
         if(_add_sum_bufer == 1)
			begin
				counter_dvdr <= counter_dvdr + 6'h1; 			 
			end  
			end
 
end
end



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

			s_trig <= TRIG_OUT;
			s_cs_adc2 <= CS_ADC_2;
		end

end
//---------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge OSC1)
begin
if(HARD_RESET  == 1)
begin
	s_byte_swap <= 0;
end
else
	if(OSC1 == 1)
		begin
			s_byte_swap <= _BYTESWAP;
		end

end

//---------------------------------------------------------------------------

always @(posedge HARD_RESET, negedge /*chang_byte_swap*/ load_byte_from_adc)
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
			if(/*chang_byte_swap*/load_byte_from_adc == 0)
				begin					 
				    s_buf1L <= s_buf2; s_buf2 <= s_buf2L; s_buf2L <= ADC_D;
                s_buf0 <= s_buf0L; s_buf0L <= s_buf1; s_buf1 <= s_buf1L;					 
					 
				end	 					 
		end
end

//---------------------------------------------------------------------------

always @(posedge HARD_RESET, posedge fr_trig, posedge res_sum_buf, negedge _add_sum_bufer)
begin
if(HARD_RESET  == 1)
begin
 
	sum_buf0 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
	sum_buf1 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
	sum_buf2 <= `D_S_WIDTH_OF_BUFER_AD_24'h0; 
	
end 
else
   if(fr_trig == 1)
	begin
//24'he;  
/* 
      out_bf_buf0 <= sum_buf0/({18'h0,counter_dvdr} - 24'h1);  //(dvdr - `D_S_WIDTH_OF_BUFER_AD_24'h4); //3
		out_bf_buf1 <= sum_buf1/({18'h0,counter_dvdr} - 24'h1);   //(dvdr - `D_S_WIDTH_OF_BUFER_AD_24'h4);
		out_bf_buf2 <= sum_buf2/({18'h0,counter_dvdr} - 24'h1);   //    (dvdr - `D_S_WIDTH_OF_BUFER_AD_24'h4);		
		*/
      out_bf_buf0 <= sum_buf0/({18'h0,counter_dvdr});  
		out_bf_buf1 <= sum_buf1/({18'h0,counter_dvdr});   
		out_bf_buf2 <= sum_buf2/({18'h0,counter_dvdr});  
	end
	else
	  if(res_sum_buf == 1)
	  begin
		sum_buf0 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
		sum_buf1 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
		sum_buf2 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;			
		end
	else
	if(_add_sum_bufer == 0)
		begin
		sum_buf0 <= sum_buf0 + w_s_buf0; 
		sum_buf1 <= sum_buf1 + w_s_buf1; 
		sum_buf2 <= sum_buf2 + w_s_buf2; 
							 					 
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
						begin
							comand_reg[3] = PD[3];
							comand_reg[4] = PD[4];
						end
					end		
			end
   end
end



//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  _WR_BOARD )
begin
if(HARD_RESET == 1)
begin
   ret_version = 0;  
end
else
begin
     if(_WR_BOARD ==  1)
	  begin
			if(addr_board == 1)
			begin
			   if(addr_comand == 1)
				begin
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
										if(lo_hi == 0)
										begin
											pr = {A2_BOARD,A1_BOARD} == 2'h00 ? out_bf_buf0[7 : 0] :
											     {A2_BOARD,A1_BOARD} == 2'h01 ? out_bf_buf1[7 : 0] : 
												  {A2_BOARD,A1_BOARD} == 2'h02 ? out_bf_buf2[7 : 0] : 8'bzzzzzzzz ;
										end
										else
										begin
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



SINXRO_AIN_6 sinxro_ain_6(HARD_RESET, OSC1,_TRIG_BOARD, END_BOARD, 
								  CLK,//strob_res_sin_go, //  dvdr_to_sin, 
								  TRIG_OUT,  
							     f0,f1,f2,f3, 
							     m0, m1, m2, m3, m4,
							     count_f10, sin, K7, sin_go, 
								  count_m10, count_f10_next, count3_next, no_add_sum
								  );

//DEVIDR   devider({comand_reg[4], comand_reg[3]}, dvdr_to_sin);

endmodule
