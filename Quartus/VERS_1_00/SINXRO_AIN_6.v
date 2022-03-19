`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

module  SINXRO_AIN_6(input HARD_RESET,input _out_pll, input TRIG_IN, input END, input MTR_CLK,
							output reg TRIG_OUT, 
							output reg F0,output reg F1,output reg F2, output reg F3, 
							output reg M0, output reg M1, output reg M2, output reg M3, output reg M4,
							output reg [3 : 0]count_f10, output  SIN, output  K20, output reg SIN_GO, 
							output [6 : 0]W_COUNT_M10,
							output  W_COUNT_F10_NEXT, output COUNT3_NEXT,
							output NO_ADD_SUMM, output DIV_FOR_END, output RES_SUM_FOR_END,
							output RES_COUNTER_DVDR_FOR_END
							);



//�������� �� ��� ������� �������						
reg [2 : 0]count3;
// ������� �������� count3;
reg count3_next;

//�������� �� ������ ������� �������
//reg [5 : 0]count_m10;
reg [6 : 0]count_m10;
// ������� �������� count_f10;	
reg count_f10_next;

//�������� ������������ ������������� ��� ������������� SignalTap2 
(*preserve = "true"*) reg [9 : 0]count_sin;

//������ ����������� ������������ ������� trig
reg s_trig;		
								

//������ ����������� ������������ ������� TRIG_IN	
reg sh3_trig;
//reg sh4_trig;

// ������� ����� ��������� {count_m10,count_f10} � ������ ������� res_count;
//reg [9 : 0]sz_trig;
reg [10 : 0]sz_trig;

// ������ ��������� �������� NO_ADD_SUMM ��� ������� ������� END � ������������
//�������� end_of_end_string
reg end_string;

//������ ��������������� ����� (sz_trig == {count_m10,count_f10} & (END == 0))
//��� end_string == 1, ������������ ��� end_string == 0
reg end_of_end_string;

//������� ����������� ���������� ������ out_pll �� ��������� ������, �� ������� 
//����������� ������������ ���������� � ���
reg [2 : 0]count_no_sum_of_end;

//��������������� � ������ (sz_trig == {count_m10,count_f10} & (END == 0))	
//������������ ���� (count_no_sum_of_end == 3'h7)
reg go_count_no_sum_of_end;

wire chenge_trig/* synthesis keep */;  
//wire chenge_end/* synthesis keep */; 
wire res_count/* synthesis keep */; 

wire out_pll;

assign out_pll = _out_pll;

//����������� ������������ ������������ ���������� � ���
// -  � ������ ������� ������� TRIG, ����������� ����������� ���������� ���������� ����� ������
//��� �� ����� ����������� ������� TRIG, �.� ������. � ���2-�� ����������.
// -  � ����� ������ ������, �. �.  ����� ������� END ��� ���������� TRIG ��. �������� ��� ����
//���� �������� countm10, �� ��������� �������� ����� ����������� ������������ ��-�� �� ������������� 
//���������� � ���2
assign NO_ADD_SUMM = (sh3_trig & ~TRIG_IN) | go_count_no_sum_of_end;
//assign NO_ADD_SUMM = ((sh4_trig | sh3_trig) & ~TRIG_IN) | go_count_no_sum_of_end;

//������� �������� COUNT_F10_NEXT 
assign W_COUNT_F10_NEXT = count_f10_next;

//������ �������� COUNT_M10 
assign W_COUNT_M10 = count_m10;

//��������� ��������� ������ TRIG_OUT
assign chenge_trig = (~s_trig & TRIG_OUT);

//��� ������� SIN_GO ������ ����� chenge_trig � ���������� �������� �ount3, count_f10,  
//count_m10 � ���� �� ��������� ������ TRIG_OUT, � ��������� ������ ���������� ��� �������� � ���� 
//�� ����� �� �������  � ������� sz_trig ��������� ������� ��������� 
//{count_m10,count_f10}, �� ���� ������ �������.
assign res_count = SIN_GO == 1'h1 ? chenge_trig : 1'h1;

assign K20 = count_sin[9] | end_string || sz_trig || count_no_sum_of_end;

`ifndef MAKET_1
	assign SIN = out_pll;
`else
	assign SIN = count_sin[9];
`endif

//assign SIN = out_pll;



//������� �������� count3
assign COUNT3_NEXT = count3_next;

//  ������ ������� ����� ���������� � ��� � ����� ������ ������ ����� 
//������� END, ��� ���������� ������� TRIG
assign DIV_FOR_END = count_no_sum_of_end == 3'h2;

//  ������ ������ ����� ���������� � ��� � ����� ������ ������ ����� 
//������� END, ��� ���������� ������� TRIG(���������� � ���������� �����)
assign RES_SUM_FOR_END = count_no_sum_of_end == 3'h5;

//  ������ ������ �������� ��� ������� ����� ���������� � ��� -�ount_dvdr, 
// �������������� � ����� ������ ������ ����� 
//������� END, ��� ���������� ������� TRIG(���������� � ���������� �����)
assign RES_COUNTER_DVDR_FOR_END = count_no_sum_of_end == 3'h6;


//----------------------------------------------------------------------------
//���� ������ [2 : 0]count_no_sum_of_end
//����������: ��� ������� ������� go_count_no_sum_of_end == 1
//������ � ����� � 0: ��� ���������� go_count_no_sum_of_end == 0
//---------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
	if(HARD_RESET == 1)
	begin
    count_no_sum_of_end = 3'h0;
	end
		else	
		begin
        if(go_count_no_sum_of_end == 1)
	           count_no_sum_of_end <= count_no_sum_of_end + 3'h1;
			else 
			     count_no_sum_of_end = 3'h0;
		end
end
//--------------------------------------------------------------------------
//������� go_count_no_sum_of_end � end_of_end_string
//���������: ��� end_string  == 1 � (sz_trig == {count_m10,count_f10} & (END == 0))	
//					end_of_end_string <= 1;
//					go_count_no_sum_of_end <= 1;
//�����: ��� end_string  == 0
//   				end_of_end_string <= 0;
//			��� end_string  == 0 � count_no_sum_of_end == 3'h7
//					go_count_no_sum_of_end <= 0
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
	if(HARD_RESET == 1)
	begin
	end_of_end_string <= 0; 
	go_count_no_sum_of_end <= 0;
	end
		else	
		begin
         if(end_string  == 1)
		   begin
                if(sz_trig == {count_m10,count_f10} & (END == 0))		   
					 begin
								end_of_end_string <= 1;
								go_count_no_sum_of_end <= 1;
					 end			
		   end						
			else			
					end_of_end_string <= 0;		  
					if(count_no_sum_of_end == 3'h7)
						      go_count_no_sum_of_end <= 0;
		end
end
//--------------------------------------------------------------------------
//������ end_string
//���������: ���  ((NO_ADD_SUMM  == 1) & (END == 1))   
//								end_string = 1;
//�����: ��� end_of_end_string == 1
//								end_string = 0;
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  NO_ADD_SUMM, posedge end_of_end_string)
begin
	if(HARD_RESET == 1)
	begin
	end_string = 0; 
	end
		else	
	begin
	     if(end_of_end_string == 1)
					end_string = 0;
		  else		 
        if(NO_ADD_SUMM  == 1)
		  begin
		       if(END == 1)
				 begin
	            end_string = 1;		  
				 end	
			end		
	end
end

//----------------------------------------------------------------------------
//�� ��������� ��. out_pll ��� ������� ������� res_count(�� ��������� �. TRIG) � ������� sz_trig 
//���������  ��������� {count_m10,count_f10} � ������ ������� res_count,
//�������  ����� ������ ��� ������ �������� BINNING
// ������� [9 : 0]sz_trig
// ���������: ��� (res_count == 1)
// 				sz_trig = {count_m10,count_f10};
//----------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
sz_trig = 10'h0; 
end
else	
begin
         if(res_count == 1)
			begin
			  sz_trig = {count_m10,count_f10};
			end  
end
end

//wire trig_in/* synthesis keep */;
//assign trig_in = TRIG_IN;
//--------------------------------------------------------
always @(posedge HARD_RESET, posedge  count_f10[2])
begin
if(HARD_RESET == 1)
begin
sh3_trig <= 0; 
end
else	
begin
        // if(count_f10[1] == 1)
			begin
			  sh3_trig <= TRIG_IN;			  			  
			end  
end
end


//------------------------------------------------------------------------
//�������� ������������ ������������� ��� ������������� SignalTap2 
//------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
count_sin = 10'h0;
end
else	
begin
         if(out_pll == 1)
			begin
			  count_sin <= count_sin + 10'h1; 			 
			end  
end
end

//-----------------------------------------------------------------------------------------------
//������ SIN_GO - ���������� ������ ����� �������������: ��������� count3, count_m10, count_f10 
//��������� �� ��������� ������ chenge_trig == 1
//					SIN_GO = 1;
//����� �� ��������� ������ (end_of_end_string  == 1)
//					SIN_GO = 0;
//-----------------------------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge end_of_end_string , posedge chenge_trig )
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
			if(end_of_end_string  == 1)
						SIN_GO = 0;                        			  
end
end

//--------------------------------------------------------------------------
//������ ����������� ������������ ������� TRIG_IN	�� ������ out_pll
//������  s_trig
//���������: �� ��������� ������ out_pll
//			TRIG_OUT <= s_trig;
//----------------------------------------------------------------------------
wire b_out_pll/* synthesis keep */;
assign b_out_pll = out_pll;
always @(posedge HARD_RESET, posedge  b_out_pll)
begin
if(HARD_RESET == 1)
begin
s_trig <= 0; TRIG_OUT <= 0; 
end
else	
begin
         if(b_out_pll == 1)
			begin
			  s_trig <= TRIG_IN;
			  TRIG_OUT <= s_trig;
			end  
end
end

//----------------------------------------------------------------------------
//���� ������ [2 : 0]count3 �� ��������� �. out_pll:
//		����������: ��� ���������� ������� res_count == 0
//		������ � ����� � 0: 
//				- ��� ������� ������� res_count == 0;
//				- ��� (count3 == 3'h2).
//������ count3_next: �� ��������� �. out_pll 
//		���������������: ���  (count3 == 3'h1)
//    ������������: 
//					��� (count3 != 3'h1)
//					��� res_count == 0 
//---------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//���� ������ [3 : 0]count_f10 �� ��������� �. out_pll:
//		����������: ��� ���������� ������� res_count == 0 � ������� ������� count3_next == 1
//		������ � ����� � 0: 
//				- ��� ������� ������� res_count == 1;
//				- ��� (count_f10 == 4'h7) � � ������� ������� count3_next == 1
//
//������ count_f10_next: �� ��������� �. out_pll 
//		���������������: ���  count_f10 == 4'h6 � (count3_next == 1)	
//    ������������: 
//					��� count_f10 != 4'h6) � (count3_next == 1)	
//             ��� res_count == 1 
//---------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//���� ������ [5 : 0]count_m10 �� ��������� �. out_pll:
//		����������: ��� ���������� ������� res_count == 0 
//						� ������� ������� count3_next == 1 	� (count_f10_next == 1
//		������ � ����� � 0: 
//						- ��� ������� ������� res_count == 1;
//---------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge  out_pll)
begin
if(HARD_RESET == 1)
begin
 count_m10 <= 7'h0; 

end
else	
begin	//0								
			if(res_count == 0)
				begin//2	
					if((count_f10_next == 1) & (count3_next  == 1))										
					begin//3
						count_m10 <= count_m10 + 7'h1;	
					end//3
            end//2				
	         else			
				begin//6
						count_m10 <= 7'h0; 
				end//6												     		
				
end//0
end
//---------------------------------------------------------------
//������� ������������� ������� ����������������� ��� F0-F3, �������������� ��������� count_f10 
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

//---------------------------------------------------------------
//������� ������������� ���� ����������������� ��� M0-M4, �������������� ��������� count_m10 
//��� ���������� ������� SIN_GO == 0, ������ � ����, 
// ��� ������� ������� SIN_GO == 1 - ������������ ���������� count_m10, 
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

							