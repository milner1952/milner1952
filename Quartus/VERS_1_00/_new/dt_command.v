


`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

/*
	������ ��������� ����������������� ������� ��������������� �� 16-��������� ���� � ���������� �� � ���������� �����,
�� ������������������ ������-0�18

input RES_HARD_RF13 - ���������� ����� 
input [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD �������������������� ���� ���������������
input RD4_PMPWR, RB15_PMPA0, RF6_ALTCS - ������� ������, ������, ������� ������� ���������������
input M0, M1, M2 ,M3  - ���� �������������
input GET_VERSION - ������� get_version
						
output reg WR_COMAND - ������ ������ �� ��������������� ���� ������ ���������� ������
output reg MX_DATA -������ ������������������� ���� ������ ���������� ������ �� ����� ������� ������ ���������� ������
output reg TEST_GO - ������� ������ � ������ ��� ������
output reg BIN0, BIN1 - �������
output reg [4 : 0]ADRES_COMMAND - ���� ������ ���������� ������ ��� ������ ����������� �� ��������������� ���� ������ ���������� ������
output   [7 : 0]W_DT - ��������������� ���� ������ ���������� ������
output  [6 : 0]BINING	��������� �������� ������� Count32trigTooSilabs, ��� ������ ��������
*/

module DT_COMMAND(input RES_HARD_RF13, input [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]RE_AD,
						input RD4_PMPWR, input RB15_PMPA0, input RF6_ALTCS,

						input M0, input M1, input M2 ,input M3, input GET_VERSION,
						
						output reg WR_COMAND, output reg MX_DATA, output reg TEST_GO, 
						output reg BIN0, output reg BIN1, output reg [4 : 0]ADRES_COMMAND, output   [7 : 0]W_DT, output  [6 : 0]BINING
						);

//������� ��������������� ���� ������
reg [7 : 0]re_ad;

//������������������ ����� M0 �������  ��������� �������, ������ ��� ������, ��������� �������, ������������ �������� ������
reg send_binning, send_test_go, send_filter, send_gane; 

//�� ������������������ ������� ��������� �������, ������ ��� ������, ��������� �������, ������������ �������� ������
reg prev_send_binning, prev_send_test_go, prev_send_filter,prev_send_gane;

//����� �������� ������� TRIG
wire [2 : 0]w_t/* synthesis keep */;


assign w_t = {1'h0, BIN1, BIN0} + 3'h1; 
`ifndef _576_pix
//������ �������� ������� TRIG
assign BINING = 						w_t == 3'h1 ? 7'h0f :
											w_t == 3'h2 ? 7'h20 :
											w_t == 3'h3 ? 7'h2b :7'h40;
											
`else
assign BINING = 	 7'h1f;
`endif											
											
											
//��������������� ���� ������
assign  W_DT = re_ad;

//------------------------------------------------------------------------------------------------------------
//������� prev_send_binning, prev_send_test_go, prev_send_filter, prev_send_gane ������������ (RD4_PMPWR == 1) ��� �������
//(RB15_PMPA0 == 1) & RF6_ALTCS == 1)
//prev_send_test_go = RE_AD[2] ��� ������������ ���� ( RE_AD[10] == 1)
//prev_send_binning {RE_AD[4],RE_AD[3]}  ��� ������������ ���� ( RE_AD[11] == 1)
//prev_send_filter ��� ������������ ���� ( RE_AD[12] == 1)
//prev_send_gane  ��� ������������ ���� ( RE_AD[12] == 1)
//
//������������� ������ ������������ ����� M0: ��������� �������, ������ ��� ������, ��������� �������, ��������� ������������ �������� ������
//send_binning, send_test_go, send_filter, send_gane - ��������������
//-----------------------------------------------------------------------------
always @(posedge RES_HARD_RF13, posedge RD4_PMPWR, posedge M0)
begin
if(RES_HARD_RF13 == 1)
begin
   send_binning = 0; prev_send_binning = 0; prev_send_test_go = 0; send_test_go = 0; TEST_GO = 0;  prev_send_filter = 0; send_filter = 0;
	prev_send_gane = 0; send_gane = 0;
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
							
				        if( RE_AD[12] == 1)
				         begin		
									prev_send_filter = 1;																	
							end	
							
							if( RE_AD[13] == 1)
							begin
									prev_send_gane = 1;
							end
						end	
				end		
		  end				
		  else
		   if(M0 == 1)
			begin
					send_test_go = prev_send_test_go;
					send_binning = prev_send_binning;
					send_filter = prev_send_filter;
					send_gane = prev_send_gane;
					
					prev_send_test_go = 0;
					prev_send_binning = 0;
					prev_send_filter = 0;
					prev_send_gane = 0;
					
			end			  
end		  
end


//-------------------------------------------------------------------------------------
//�� ���� �0, ������� �������������� � ������� ��������������� ����, �� ���� ������ ���������� ������ ������������
//�����������e����� ����� 0�18, MX_DATA = 1;
//�� ���� M1 ��������������� ������ ������
//�� ���� M3 ������ ������-������������,8-�� ��������� ���� ����������� � ������ ���������
always @(posedge RES_HARD_RF13, posedge M0, posedge M1, posedge M3 )
begin
if(RES_HARD_RF13 == 1)
begin
	WR_COMAND = 0; MX_DATA = 0;  ADRES_COMMAND = 5'h0; re_ad <= 8'hzz;
end
else
	begin
	    if(M0 == 1)
		 begin
		      if(((send_test_go == 1) | (send_binning == 1) | (send_filter == 1) | (send_gane == 1)) & (GET_VERSION == 0))
				begin
			        MX_DATA = 1;ADRES_COMMAND = 5'h18; 								
					  
					  re_ad[0] = (({RE_AD[13], RE_AD[12],RE_AD[11],RE_AD[10]} == 4'h01) & (send_test_go == 1)); 				      	
					  re_ad[1] = RE_AD[2];	//test_go								
				     re_ad[2] = (({RE_AD[13], RE_AD[12],RE_AD[11],RE_AD[10]} == 4'h02) & (send_binning == 1));			         	
					  re_ad[3] = RE_AD[3]; 	//bin0
					  re_ad[4] = RE_AD[4]; 	//bin1	
					  
					  re_ad[5] = (({RE_AD[13], RE_AD[12],RE_AD[11],RE_AD[10]} == 4'h04) & (send_filter == 1));
			        re_ad[6] = RE_AD[5];    //filter	
					  
			       if(send_gane == 1)		  
					 begin
					 
					 `ifdef MAKET_1
							re_ad[7] = RE_AD[6]; //gane
					 `else				
				    		{re_ad[6],re_ad[4],re_ad[3],re_ad[1]} = {RE_AD[3], RE_AD[2],RE_AD[1],RE_AD[0]};	
							re_ad[7] = send_gane;							
					  `endif

					 end
					  
				end	
            else
				begin
               MX_DATA = 0;	re_ad <= 8'hzz;			
				end	
		 end

       if(M1 == 1)
		 begin
				if(((send_test_go == 1) | (send_binning == 1) | (send_filter == 1) | (send_gane == 1)) & (GET_VERSION == 0))
				begin
					WR_COMAND = 1;
				end	
		 end
		 else
		 if(M3 == 1)
		 begin
		   WR_COMAND = 0; re_ad <= 8'hzz;	
		 end
	end
end



endmodule
