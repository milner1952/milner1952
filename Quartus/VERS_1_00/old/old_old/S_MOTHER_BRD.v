
`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module S_MOTHER_BRD(inout [`D_S_WIDTH_OF_DATA_8 -1 : 0]B_PD,
								  inout [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD,
								  
								  input RES_HARD_RF13 , 
								  input RD5_PMPRD,	input RD4_PMPWR,			input PIC_SINXRO_RF12,	input RUN_RF8,			input CS_READ_MTR_RF7,	
								  input RB15_PMPA0,  input END_BOARD0,       input END_BOARD1,		   input END_BOARD2,		input TEST_GO_RD0,	
								  input RF6_ALTCS,	input OSC1,	input OSC2,	input _TRIG_BOARD0,     input _TRIG_BOARD1,  input _TRIG_BOARD2,
									
								  output  MTR_CLK,  	output  MTR_RST,  	   output _RD,  				output _WR,  			output reg INTERUPT_1_TOO_SILABS_P03,
								  output  A0, 			output  A1,  				output  A2,  				output  A3, 			output  A4,									
								  
								  output DIR, 			output _CS0, 				output _CS1,				output _CS2, 			output _TEST_TRIG, 		
								  output TEST_END,	output ENC_WR, 		   output ENC_RD, 			output INT_RESET_RE8, 	output  K113, 			
								  output K111);
								
					
					
/*(* noprune *) */

reg end_vers;
reg [7 : 0]delimer;
reg get_version;
reg prev_get_version;
reg set;
reg [15 : 0]state;
reg [7 : 0]VersData[80];
reg [6 : 0]count_vers_data_out;
reg [1 : 0]count_board;
reg [7: 0]count_vers_data_in;
reg _run;

reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_bas_in;
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]buf_reg;

//reg [3 : 0]Count32trigTooSilabs;
reg [6 : 0]Count32trigTooSilabs;

reg c_osc; 
reg [1 : 0]c_osc4;
reg [1 : 0]c_osc3;

reg [8 : 0]counter_adres_board;
reg [12 : 0]counter_string;

wire mem_en_rd, clk_mem_read, write_too_mem;
wire w_clk_bufer/* synthesis keep */;
wire [7:0]w_count_vers_data_in;

wire M0,M1,M2,M3;

wire f0;
wire f1;
wire f2;
wire f3;
wire f4;
wire f5;
wire [1 : 0]count_3P;
wire [3 : 0]cnt_ch_trig;
wire _K13, _K1;
wire [3:0]adres_board;
wire _TRIG/* synthesis keep */;
wire a4_vers, a3_vers, a2_vers, a1_vers, a0_vers;
wire chenge_adres_in;
wire [`D_WIDTH_OF_ADRES_IN_11 - 1 : 0]adres_in;
wire [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]adres_out;
wire counter_f_go;

wire _rd_snx;
wire _rd_data;
wire _rd_vers;

reg  init_vers_go;
wire t_run;
wire adres_in_11;
wire w_write_bufer/* synthesis keep */;
wire flag_init;

wire wr_comand, mx_data, test_go,  bin0, bin1 ;
wire [6 : 0]w_binning/* synthesis keep */;

wire [4 : 0]adres_command; 
wire trig;
wire END/* synthesis keep */;

`ifdef TEST_WITHOUT_MEMORY
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out;
`else
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out/* synthesis keep */;
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out1/* synthesis keep */;
wire [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]mem_out2/* synthesis keep */;
`endif

wire _OSC1/* synthesis keep */; 
//reg  _OSC1;

wire PLL_OSC1;
wire PLL_OSC;
wire test_sin;

reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]test_count_global_data;

assign _OSC1 = ~(({bin1,bin0} == 2'h0 & c_osc == 1) |
 					 ({bin1,bin0} == 2'h1 & c_osc4[0] == 1) |
					 ({bin1,bin0} == 2'h2 & c_osc3[1] == 1) |
					 ({bin1,bin0} == 2'h3 & c_osc4[1] == 1));


assign K113 = MTR_CLK | counter_adres_board[8] | counter_string[12];					



assign K111 = PLL_OSC1;
assign PLL_OSC1 = PLL_OSC;
assign trig = ~_TRIG;
assign mem_out = adres_in_11 == 1 ? mem_out1 : mem_out2;
assign t_run = ~(_run == 0 &  init_vers_go == 1);
assign _RD = (get_version == 1) ? _rd_vers : _rd_data;


assign _TRIG = ~((~_TRIG_BOARD0) & (~_TRIG_BOARD1) & (~_TRIG_BOARD2));  
assign END = END_BOARD0 & END_BOARD0 & END_BOARD0;




assign RE_AD = `D_GET_INFO_STATE == 1 ? state : 
					`D_GET_INFO_VERSION == 1 ?  {8'h06 , VersData[count_vers_data_out]}:
					`D_GET_DATA_XRAY == 1 ? mem_out : 16'bzzzzzzzzzzzzzzzz;
				


assign {A4,A3,A2,A1,A0} = get_version == 1 ?{a4_vers, a3_vers, a2_vers, a1_vers, a0_vers} :
								  (mx_data == 1) ? adres_command : {adres_board,1'h0};

assign _WR = get_version == 1'h1 ? _wr : mx_data == 1'h1 ? ~wr_comand : 1'h1;


assign ENC_WR = RF6_ALTCS == 0 ? (CS_READ_MTR_RF7 == 1 ? RD5_PMPRD : RD4_PMPWR) : 1'h0;
assign ENC_RD = RF6_ALTCS == 0 ? (CS_READ_MTR_RF7 == 1 ? 1'h0 : RD5_PMPRD) : 1'h0;

assign DIR = get_version == 1'h1 ?  (w_count_vers_data_in == 8'h0 ? 0 : 1'h1) : counter_f_go;

assign _CS0 = ~(({A4,A3} == 2'h0) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));
assign _CS1 = ~(({A4,A3} == 2'h1) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));
assign _CS2 = ~(({A4,A3} == 2'h2) || ({DIR,A4,A3,A2,A1,A0} == 6'h18));

//---------------------------------------------------------------------------
//**************************************************************************
//----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge MTR_RST, posedge t_run)
begin
	if(RES_HARD_RF13 == 1)
		counter_string = 13'h0;
	else	
	begin			
				if(t_run == 1)
				begin
					counter_string = 13'h0;
				end
				else
				if(MTR_RST == 0)
				begin
				counter_string = counter_string+ 13'h1;
				end
   end
end
//---------------------------------------------------------------------------
//**************************************************************************
//----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge MTR_RST, negedge _CS0)
begin
	if(RES_HARD_RF13 == 1)
		counter_adres_board = 9'h0;
	else	
	begin			
				if(MTR_RST == 0)
				begin
					counter_adres_board = 9'h0;
				end
				else
				if(_CS0 == 0)
				begin
				counter_adres_board = counter_adres_board + 9'h1;
				end
   end
end

//-------------------------------------------------------------------------
//--------------------------------------------------------------				
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc <= 0;
	end
		else
		if(PLL_OSC1 == 0)
		begin
				c_osc <= ~c_osc;
		end		
end

//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc4 <= 2'h0;
	end
		else
		if(PLL_OSC1 == 0)
	   begin	
			if(c_osc == 0)
			begin
				c_osc4 <= c_osc4 + 2'h1;
			end		
		end	
end
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc3 <= 2'h0;
	end
		else
		if(PLL_OSC1 == 0)
	   begin
			if(c_osc == 0)
			begin
				c_osc3 <= c_osc3 + 2'h1;
				if(c_osc3 == 2'h2)
				begin
					c_osc3 <= 2'h0;
				end
			end		
		end	
end	 
/*
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		 _OSC1 <= 0; 
	end
	else
	begin	
						case ({bin1,bin0})																						
						2'h0:  		begin _OSC1 <= c_osc;     end						
						2'h1:  		begin _OSC1 <= c_osc4[0]; end											
						2'h2:    	begin _OSC1 <= c_osc3[1]; end						
						2'h3:  		begin _OSC1 <= c_osc4[1]; end									
						endcase						
	end
	
end	
*/
//-------------------------------------------------------
//INTERUPT_1_TOO_SILABS <= 1;
//--------------------------------------------------------
always @( posedge RES_HARD_RF13, posedge trig, negedge MTR_RST, posedge PIC_SINXRO_RF12)
begin
	if(RES_HARD_RF13 == 1) 
	begin
		INTERUPT_1_TOO_SILABS_P03 <= 1;   Count32trigTooSilabs <= 7'h0;  
	end   
	else
	   if(PIC_SINXRO_RF12 == 1)
	   begin
       INTERUPT_1_TOO_SILABS_P03 <= 0;
      end		
	   else
		if(MTR_RST  == 0) 
		begin
			INTERUPT_1_TOO_SILABS_P03 <= 1;   Count32trigTooSilabs <= 7'h00; 
		end
		else
		if(trig == 1)
		begin
			Count32trigTooSilabs <= Count32trigTooSilabs + 7'h1;
			if(Count32trigTooSilabs == w_binning)  //(4'h0f /** {bin1,bin0})*/) 
			begin
				INTERUPT_1_TOO_SILABS_P03 <= 0;
				Count32trigTooSilabs <= 7'h0;
			end                 
			else
				INTERUPT_1_TOO_SILABS_P03 <= 1;                  
				
		end
		
end

//-------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge RD4_PMPWR, posedge M0)
begin
if(RES_HARD_RF13 == 1)
begin
	get_version = 1'h0;	prev_get_version = 0; _run = 1; //K111 = 0;
end
else
begin
        if(RD4_PMPWR == 1)
		  begin
				if(RB15_PMPA0 == 1)
				begin
						if(RF6_ALTCS == 1)
						begin
						//  K111 = ~K111;
						  if(RE_AD[8] == 1)
						   begin							    
									prev_get_version =  RE_AD[0]; 
							end	
				        if( RE_AD[9] == 1)
				         begin		
									_run =  ~RE_AD[1];
							end	
							
						end	
				end		
		  end				
		  else
		   if(M0 == 1)
			begin
		      	get_version = prev_get_version; 
			end			  
end		  
end




//-----------------------------------------------------------------------------
//VersData Init
//-----------------------------------------------------------------------
always @(posedge RES_HARD_RF13,    posedge set)
begin
if(RES_HARD_RF13 == 1)
begin
  delimer = 6'h1; 

  
  VersData[0] = "1";  
  VersData[1] = "1";
  VersData[2] = "5";
  VersData[3] = "2";
  VersData[4] = " ";
  VersData[5] = "A";
  VersData[6] = "L";
  VersData[7] = "T";
  VersData[8] = ":";
  VersData[9] = " ";
  VersData[10] = "v";
  VersData[11] = "3";
  VersData[12] = ".";
  VersData[13] = "0";
  VersData[14] = ".";
  VersData[15] = "0";
  VersData[16] = "0";
  VersData[17] = 8'h0;
  
  VersData[18] = "N";
  VersData[19] = "O";
  VersData[20] = " ";
  VersData[21] = "C";
  VersData[22] = "O";
  VersData[23] = "N";
  VersData[24] = "N";
  VersData[25] = "E";
  VersData[26] = "C";
  VersData[27] = "T";
  VersData[28] = " ";
  VersData[29] = " ";
  VersData[30] = " ";
  VersData[31] = " ";
  VersData[32] = " ";
  VersData[33] = " ";
  VersData[34] = 8'h0;
  
  VersData[35] = "N";
  VersData[36] = "O";
  VersData[37] = " ";
  VersData[38] = "C";
  VersData[39] = "O";
  VersData[40] = "N";
  VersData[41] = "N";
  VersData[42] = "E";
  VersData[43] = "C";
  VersData[44] = "T";
  VersData[45] = " ";
  VersData[46] = " ";
  VersData[47] = " ";
  VersData[48] = " ";
  VersData[49] = " ";
  VersData[50] = " ";
  VersData[51] = 8'h0;  

  VersData[52] = "N";
  VersData[53] = "O";
  VersData[54] = " ";
  VersData[55] = "C";
  VersData[56] = "O";
  VersData[57] = "N";
  VersData[58] = "N";
  VersData[59] = "E";
  VersData[60] = "C";
  VersData[61] = "T";
  VersData[62] = " ";
  VersData[64] = " ";
  VersData[65] = " ";
  VersData[66] = " ";
  VersData[67] = " ";
  VersData[68] = " ";
  VersData[69] = 8'h0;   
end
else
begin
	
	if((set == 1) & (`D_VERSION_BIT_state_0 == 0))
		begin
			VersData[ count_vers_data_in + 8'h11] = B_PD;
		end	

end		
end

//-----------------------------------------------------------------

always @(posedge RES_HARD_RF13, posedge   M0)
begin
if(RES_HARD_RF13 == 1)
begin
  count_vers_data_in = 7'h0;  
end
else
	     if((M0 == 1) && ((`D_VERSION_BIT_state_0 == 0)) && 
		  (get_version == 1)  & (flag_init == 1))
		  begin
				count_vers_data_in = count_vers_data_in + 7'h1;
		  end		        	 
end
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13,   negedge RD5_PMPRD )
begin
if(RES_HARD_RF13 == 1)
begin
  count_vers_data_out = 7'h0; 
end
else
	   if(RD5_PMPRD == 0)
		begin
		if((RF6_ALTCS == 1) & (RB15_PMPA0 == 0) & (get_version == 1) & 
		   ((`D_VERSION_BIT_state_0 == 1)))
		  begin
		   count_vers_data_out = count_vers_data_out + 7'h1;  
		  end
		end
end
//-----------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge M1)
begin
if(RES_HARD_RF13 == 1)
begin
count_board = 2'h0; state = 16'h00; init_vers_go = 0;end_vers = 0;
end
else
	if(M1 == 1)
	begin
			if((w_count_vers_data_in == (`D_S_NUM_BYTE_GET_VERSION_017 - 1)) & 
			     (`D_VERSION_BIT_state_0 == 0))
			begin
				count_board = count_board + 2'h1;
				if(count_board == 3'h3)
				begin
				end_vers = 1;
				end
			end
		   else	
				if(end_vers == 1)
				begin
				      count_board = 2'h0;
						state = (state | `D_VERSION_BIT_SET_state_0); 	
						init_vers_go = 1;						            				
				end
				
			
	end

end


//--------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge M2, posedge M3)
begin
	if(RES_HARD_RF13 == 1)
	begin
		set = 0; 
	end
	else
	if(M2 == 1) 
    begin
		if((get_version == 1)& (`D_VERSION_BIT_state_0 == 0) & (flag_init == 1) )
		begin
		   set = 1;
		end
    end
	 else 
	 if(M3 == 1)
	 begin
	      set = 0;
	 end
end
/*
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge PLL_OSC1)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc <= 0;
	end
		else
		if(PLL_OSC1 == 0)
		begin
				c_osc <= ~c_osc;
		end		
end

//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge c_osc)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc4 <= 2'h0;
	end
		else
		if(c_osc == 0)
		begin
				c_osc4 <= c_osc4 + 2'h1;
		end		
end
//----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge c_osc)
begin

	if(RES_HARD_RF13 == 1)
	begin
		c_osc3 <= 2'h0;
	end
		else
		if(c_osc == 0)
		begin
				c_osc3 <= c_osc3 + 2'h1;
				if(c_osc3 == 2'h2)
				begin
					c_osc3 <= 2'h0;
				end
		end		
end
*/
`ifdef TEST_WITHOUT_MEMORY
//-----------------------------------------------------------------------
always @(posedge RES_HARD_RF13,negedge MTR_RST ,negedge RD5_PMPRD)
begin
	if(RES_HARD_RF13 == 1)
	begin
		mem_out = `D_S_WIDTH_OF_DATA_AD_16'h00;
	end
	else
	begin
	   if(MTR_RST == 0)
		begin
			mem_out = `D_S_WIDTH_OF_DATA_AD_16'h00; 	
		end
		else
		if(RD5_PMPRD == 0)
		begin
			if(CS_READ_MTR_RF7 == 1)
			begin
		         mem_out = mem_out + `D_S_WIDTH_OF_DATA_AD_16'h01;   	
			end
		end	
	end
end		
//------------------------------------------------------------------
`endif	
//-----------------------------------------------------------------

`ifdef TEST_COUNT_GLOBAL_DATA
always @(posedge RES_HARD_RF13,  posedge w_write_bufer, posedge t_run, negedge MTR_RST)
begin
	if(RES_HARD_RF13 == 1)
		begin
			test_count_global_data <=  16'h0;
		end
	else
	
	begin		
      	
		if(t_run == 1)	
		begin
			test_count_global_data <= 16'h0; 
		end
		else
		if(MTR_RST == 0)
		begin
		   test_count_global_data <= 16'h0; 
		end
		else
			if(w_write_bufer  == 1)
			begin
				if(counter_f_go == 1)
					begin	
					test_count_global_data <= test_count_global_data + 16'h01;	
                          if(test_count_global_data == 1152)
											test_count_global_data <= 0;					
					end
			end			
	 end	
end


//------------------------------------------------------------------------
ram_dual   mem1_(test_count_global_data,
					 adres_out, adres_in,  adres_in_11 , clk_mem_read,   
					 write_too_mem , mem_out2 ,RB15_PMPA0 ,K13, CS_READ_MTR_RF7);
					 
			   
ram_dual   mem2_(test_count_global_data,
					 adres_out, adres_in, ~adres_in_11, clk_mem_read,   
					 write_too_mem , mem_out1,RB15_PMPA0 ,K1, CS_READ_MTR_RF7);	
					 

`endif
//--------------------------------------------------------------------------


`ifndef TEST
always @(posedge RES_HARD_RF13,  posedge  w_clk_bufer, 
			posedge w_write_bufer)
begin
	if(RES_HARD_RF13 == 1)
		begin
			mem_bas_in <=  16'h0; buf_reg <= 16'h0;
		end
	else	
	begin		 
	
	      if(w_write_bufer == 1) 		
			begin
			       if(w_clk_bufer == 0) 	
					 begin
							mem_bas_in <= buf_reg;
					 end		
			end	
			else	
			if(w_clk_bufer == 1) 
			begin						

							buf_reg[7 : 0] <= buf_reg[15 : 8];		
							buf_reg[15 : 8] <= B_PD;				
										
			end 

	 end	
end

ram_dual   mem1_(mem_bas_in,
					 adres_out, adres_in,  adres_in_11 , clk_mem_read,   
					 write_too_mem , mem_out2 ,RB15_PMPA0 ,K13, CS_READ_MTR_RF7);
					 			   
ram_dual   mem2_(mem_bas_in,
					 adres_out, adres_in, ~adres_in_11, clk_mem_read,   
					 write_too_mem , mem_out1,RB15_PMPA0 ,K1, CS_READ_MTR_RF7);		
`endif
//------------------------------------------------------------------------


ADRES_IN_MD addres_in(RES_HARD_RF13, f0,f1,f2,
							M0,M1,M2,M3,  
							MTR_CLK, PIC_SINXRO_RF12, t_run ,_TRIG, END,chenge_adres_in,_rd_snx,

							
							INT_RESET_RE8, MTR_RST, counter_f_go, adres_in, _rd_data,
							adres_in_11
							);
		

ADRES_OUT_MD  addres_out(RES_HARD_RF13,c_osc, CS_READ_MTR_RF7,  INT_RESET_RE8,
								 RF6_ALTCS, RD5_PMPRD, RD4_PMPWR, RB15_PMPA0,
				     
								 adres_out, mem_en_rd, clk_mem_read , _K1
								);		
		
	

TEST_MD  Test_md(RES_HARD_RF13, M0,M1,M2,M3,  
				     MTR_CLK, t_run,MTR_RST, test_go, 								
				     _TEST_TRIG,  TEST_END, _K13 
				     );
				




SINXRO_4_3    sinxro_i(_OSC1,  _OSC1, RES_HARD_RF13, ~_TRIG, counter_f_go,
									M0,M1,M2,M3, f0,f1,f2,f3,f4,f5, adres_board, 
									MTR_CLK, chenge_adres_in,_rd_snx, 
									w_clk_bufer, w_write_bufer, write_too_mem,count_3P);

							


GET_VERS_AIN_BRD get_vers(RES_HARD_RF13, get_version,
								  count_board,  
								 M0, M1, M3, `D_VERSION_BIT_state_0, count_3P,	
						       B_PD,	
						       _rd_vers, _wr,  
								 w_count_vers_data_in, 
								 a4_vers, a3_vers, a2_vers, a1_vers, a0_vers, flag_init);
								 

								 
								 
DT_COMMAND dt_COMMAND (RES_HARD_RF13,RE_AD,
						     RD4_PMPWR, RB15_PMPA0, RF6_ALTCS,
							  M0 ,M1 ,M2 ,M3, get_version,
						
						     wr_comand, mx_data, test_go, 
						     bin0, bin1, adres_command,B_PD, w_binning 
							);								 
//------------------------------------------------------------------------
//assign PLL_OSC = OSC1;

PLL_5 pll_brd(OSC1,PLL_OSC);	//div = 9, mul = 22, f =  61,111м√ц, при f_in = 25м√ц  						

//------------------------------------------------------------------------							
endmodule
