


`include "../../../SRC/Quartus/VERS_1_00/Defines.v"




module DT_COMMAND(input RES_HARD_RF13,input [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD,
						input RD4_PMPWR, input RB15_PMPA0, input RF6_ALTCS,

						input M0, input M1, input M2 ,input M3, input GET_VERSION,
						
						output reg WR_COMAND, output reg MX_DATA, output reg TEST_GO, 
						output reg BIN0, output reg BIN1, output reg [4 : 0]ADRES_COMMAND, output  reg [7 : 0]DT
						);



reg send_binning, send_test_go; 
reg prev_send_binning, prev_send_test_go;


always @(posedge RES_HARD_RF13, posedge RD4_PMPWR, posedge M0)
begin
if(RES_HARD_RF13 == 1)
begin
   send_binning = 0; prev_send_binning = 0; prev_send_test_go = 0; send_test_go = 0; TEST_GO = 0;
	BIN0 = 0; BIN1 = 0; 	
end
else
begin
        if(RD4_PMPWR == 1)
		  begin
				if(RB15_PMPA0 == 1)
				begin
						if(RF6_ALTCS == 1)
						begin							
				        if( RE_AD[10] == 1)
				         begin		
									TEST_GO =  RE_AD[2];
									prev_send_test_go = 1;
							end	
				        if( RE_AD[11] == 1)
				         begin		
									BIN0 =  RE_AD[3];
									BIN1 =  RE_AD[4];
									prev_send_binning = 1;
							end
							
						end	
				end		
		  end				
		  else
		   if(M0 == 1)
			begin
					send_test_go = prev_send_test_go;
					send_binning = prev_send_binning;
					prev_send_test_go = 0;
					prev_send_binning = 0;
			end			  
end		  
end


//-------------------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge M0, posedge M1, posedge M3 )
begin
if(RES_HARD_RF13 == 1)
begin
	WR_COMAND = 0; MX_DATA = 0; DT = 8'hzz; ADRES_COMMAND = 5'h0; 
end
else
	begin
	    if(M0 == 1)
		 begin
		      if(((send_test_go == 1) | (send_binning == 1)) & (GET_VERSION == 0))
				begin
			        MX_DATA = 1;ADRES_COMMAND = 5'h18; 								
					  DT[0] = RE_AD[10]; 				      	
					  DT[1] = RE_AD[2];	//test_go								
				     DT[2] = RE_AD[11];				         	
					  DT[3] = RE_AD[3]; 	//bin0
					  DT[4] = RE_AD[4]; 	//bin1					
				end	
            else
				begin
               MX_DATA = 0;	DT = 8'hzz;			
				end	
		 end

       if(M1 == 1)
		 begin
				if(((send_test_go == 1) | (send_binning == 1)) & (GET_VERSION == 0))
				begin
					WR_COMAND = 1;
				end	
		 end
		 else
		 if(M3 == 1)
		 begin
		   WR_COMAND = 0; DT = 8'hzz;	
		 end
	end
end



endmodule
