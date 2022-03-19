

`include "../../../SRC/Quartus/S_TEST_MOTHER_BRD/Defines.v"

module S_TEST_MOTHER_BRD(inout [`D_S_WIDTH_OF_DATA_8 -1 : 0]B_PD,
								  inout [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD,
								  
								  input RES_HARD_RF13 , 
								  input RD5_PMPRD,	input RD4_PMPWR,			input PIC_SINXRO_RF12,	input RUN_RF8,			input CS_READ_MTR_RF7,	
								  input RB15_PMPA0,  input END_BOARD0,       input END_BOARD1,		   input END_BOARD2,		input TEST_GO_RD0,	
								  input RF6_ALTCS,	input OSC1,	input OSC2,	input _TRIG_BOARD0,     input _TRIG_BOARD1,  input _TRIG_BOARD2,
									
								  output  MTR_CLK,  	output  MTR_RST,  	   output _RD,  				output _WR,  			output  INTERUPT_1_TOO_SILABS_P03,
								  output  A0, 			output  A1,  				output  A2,  				output  A3, 			output  A4,									
								  
								  output DIR, 			output _CS0, 				output _CS1,				output _CS2, 			output _TEST_TRIG, 		
								  output TEST_END,	output ENC_WR, 		   output ENC_RD, 			output INT_RESET_RE8, 	
								  								  								  							
								  output  K113, 		output K111);
								  
reg [2 : 0]count_8;
reg [2 : 0]C;
								  
reg [23 : 0]R;


wire r_sum/* synthesis noprune */;
wire c_sum/* synthesis noprune */;

				


assign r_sum = (~R[23] & ~R[22] & ~R[21] & ~R[20] & ~R[19] & ~R[18] & ~R[17] & ~R[16] &
					~R[15] & ~R[14] & ~R[13] & ~R[12] & ~R[11] & ~R[10] & ~R[9] & ~R[8] &
					~R[7] & ~R[6] & ~R[5] & ~R[4] & ~R[3] & ~R[2] & ~R[1] & ~R[0]);	
					
assign c_sum = (/*~C[2] &*/ ~C[1] & ~C[0]) ;
	
	
wire strob;

	
	
assign K111 = r_sum;
assign K113 = c_sum;
	
assign B_PD[0] = R[0];								  
assign B_PD[1] = R[1];	
assign B_PD[2] = R[2];	
assign B_PD[3] = R[3];	

assign B_PD[4] = R[4];	
assign B_PD[5] = R[5];	
assign B_PD[6] = R[6];	
assign B_PD[7] = R[7];	

assign MTR_CLK = R[8];								  
assign MTR_RST = R[9];	
assign _RD = R[10];	
assign _WR = R[11];	

assign INTERUPT_1_TOO_SILABS_P03 = R[12];	
assign A0 = R[13];	
assign A1 = R[14];	
assign A2 = R[15];
assign A3 = R[16];
assign A4 = R[17];


assign DIR = R[18];

assign _CS0 = ~C[0];								  
assign _CS1 = ~C[1];	
assign _CS2 = ~C[2];

assign _TEST_TRIG = R[19]; 		
assign TEST_END = R[20];	
assign ENC_WR = R[21]; 		   
assign ENC_RD = R[22]; 			
assign INT_RESET_RE8 = R[23]; 	

//-----------------------------------------------------------------
always @(posedge RES_HARD_RF13, negedge OSC1)		
begin
	if(RES_HARD_RF13 == 1)
	begin
		count_8 <= 3'h0;
	end
	else
	begin
		count_8 <= count_8 + 3'h1;
	end


end
	

assign strob = count_8[2] & r_sum & count_8[1];	
	
//-----------------------------------------------------------------	  
always @(posedge RES_HARD_RF13, negedge count_8[2])								  
begin
	if(RES_HARD_RF13 == 1)
	begin
		R <= 24'h0;
	end
	else
	begin
		R <= {R[22 : 0],r_sum};
	end
end


//-----------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge /*r_sum*/ strob)								  
begin
	if(RES_HARD_RF13 == 1)
	begin
		C <= 3'h0;
	end
	else
	begin
	if(r_sum == 1)
		C <= {C[1 : 0],c_sum};
	end
end

								  
endmodule


