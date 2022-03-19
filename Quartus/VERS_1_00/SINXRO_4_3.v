
//----------------------------------------------------
//���� ������������� 4 ���� � ��������� ������� 
//-----------------------------------------------------


`timescale 1ns/10 ps 
//-----------------------------------------------------------------------------------------------------
//������ ������������ ������ ����� ����������������� ��� �������� �� � F�, ������������ MTR_CLK
//------------------------------------------------------------------------------------------------------
//FRC_IN - ������� ������� �����
//������ FRC2-������ ��� ������������� �����������
//RES_HARD - c����� ����������� ������
//TRIG - ������ ���������� ����� ���������� �� ������.
//COUNTER_F_GO - ������ ���������� ������ ��������� F � ��� Fx
//Mx � Fx - ����� �� 4 � 6 ����������������� ���, �������������� ������, ��� ������������� ����� �������
//COUNT_BOARD - ������� ��������� ���������� ������
//��R_CLK-������� ������������� ������
//CHENGE_ADRES_IN - ������ ���������������� ����� ������ ��� ������ ���������� �� ���������� ������
//_RD - ������ ������ ���������� �� ���������� ������
//CLK_BUFER - ������ ������ �������� � �������� ����� ���������� ��� �� ������ �� ���������� ������ 
//					�� 8-��������� ����.
//WRITE_BUFER - ������ ������ �������� � �������� ����� ��������� �������� �� ���������� ������ �� 
//					8-��������� ���� � 16 - ��������� �������    
//WRITE_TOO_MEM - ������ ������ � ������ ���������� �� 16-���������� ��������, �������� �� ���������� ������.
//COUNT_3P - ������� �������� �� ���
//-------------------------------------------------------------------------------------------------------
module    SINXRO_4_3(input FRC_IN, input FRC2, input RES_HARD,  input TRIG, input COUNTER_F_GO,

							output reg M0,output reg M1,output reg M2,output reg M3,
							output reg F0,output reg F1,output reg F2, 
							output reg F3,output reg F4,output reg F5, 
							output  [3 : 0]COUNT_BOARD, 
							output MTR_CLK, 
							output CHENGE_ADRES_IN, 
							output _RD, 
							output CLK_BUFER, 
							output WRITE_BUFER, 
							output reg WRITE_TOO_MEM,
							output reg [1:0]COUNT_3P
							);
							
//�������� ������� �������� [1 : 0]COUNT_3P							
reg [1 : 0]r_COUNT_3P;

//shift_trig - ���������� �� ���� ������� ������ div_out ������ TRIG
reg shift_trig;

//count_m9 - �������� ������� div_out �� 9
reg [3 : 0]count_m9;

//[1 : 0]count_m4 - �������� �������� �������� count_m9 ��4
reg [1 : 0]count_m4;

//count_f12 - �������� ������� div_out �� 12
reg [3 : 0]count_f12;

//[2 : 0]cnt_brd - ��� ������� ������� ���� ������� [1 : 0]count_f3_brd ���������� �������, �������
//�������� ���������� ����� �������
reg [2 : 0]cnt_brd;

//[2 : 0]count_f8_brd - �������� ������� div_out �� 8 ������ ����� �������� 
//����������� �������� COUNTER_F_GO
reg [2 : 0]count_f8_brd;

//[1 : 0]count_f3_brd; - �������� �������� �������� count_f8_brd �� 3
reg [1 : 0]count_f3_brd;

//����������� �� ���� ������ ��������� �������� ������ ���������� ������ COUNT_BOARD
reg [3 : 0]shift_count_board;

// �������� �� ��� ������� ������� FRC_IN
reg FRC1;

// �������� ������� div_out = ~COUNT_3P[1];
wire div_out;

//����� ������� TRIG
wire chenge_trig;
assign chenge_trig = TRIG & ~shift_trig;

//������ ������������� ������
assign MTR_CLK = ~count_m4[0];

//������� ���������� ������ �������
assign COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd};

//��������� ��������� �������� ���������� ������
assign CHENGE_ADRES_IN = shift_count_board != COUNT_BOARD; 

//������ ���������� �������� � �������� ����������������  ����� ���������� ����������� �� ���������� ������
// � �������� 16-��������� �������
assign WRITE_BUFER = count_f8_brd == 3'h7;

//������ ������ ���������� ������
assign _RD  = COUNTER_F_GO == 1'h1 ? (count_f12 == 4'h3 || count_f12 == 4'h7 ||  count_f12 == 4'hb) : 1'h1;

//������ ������ �������� � �������� ����������������  ����� ���������� ����������� �� ���������� ������
assign CLK_BUFER = (count_f12 == 4'h2 || count_f12 == 4'h6 ||  count_f12 == 4'ha);

//������� ��� ������ ��������� m � f
assign div_out = ~COUNT_3P[1];
//----------------------------------------------
//�������� ������� ������� �� ���
//------------------------------------------------
always @(posedge RES_HARD, posedge FRC_IN)
begin
	if(RES_HARD == 1)
	begin	
	FRC1 = 0;
	end
	else
	if(FRC_IN == 1)
	begin
		FRC1 = ~FRC1;
	end
end	
//---------------------------------------------------------------------------
//���������� ��������� �������� COUNT_3P � �������� ��������
//� ��������� �� ���� ������� FRC1
//-------------------------------------------------------------------------
always @(posedge RES_HARD, /*negedge*/posedge FRC1) //??????????  �� TimeQuest ��� negedge
begin

	if(RES_HARD == 1)
	begin	
		r_COUNT_3P <= 2'h0;	
	end	
	else			
		r_COUNT_3P <= COUNT_3P;
end
	
//---------------------------------------------------------------------------
//		�������� �������� ������� FRC1 �� 3. 
//����������� � �������� r_COUNT_3P ��������� ��������
//������������   ��� ������������ ������ ��������, ��� ������� ������������.
//---------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge FRC1)
begin
	if(RES_HARD == 1)
	begin	
	COUNT_3P <= 2'h0;	
	end
	else
	begin
			if(FRC1 == 1)
			begin
					COUNT_3P <= COUNT_3P + 2'h1;
					if(r_COUNT_3P == 2'h2)
							COUNT_3P <= 2'h0;
			end	
	end
end	
//----------------------------------------------
//�������� �������� ���������� ������ COUNT_BOARD �� 
//���� ���� ������� div_out. � ���������� ������������
// ��� ������������ ������� ��������� ��������� ������� 
// COUNT_BOARD
//----------------------------------------------------
always @(posedge RES_HARD, posedge div_out)
begin
	if(RES_HARD == 1)
	 shift_count_board = 4'h0;
	else
		if(div_out == 1)
		begin
			shift_count_board = COUNT_BOARD;
		end	
end
//-----------------------------------------------------------------------------
//������������ ����������������� ��� �00-F3 � ����������� � ��������� �������� 
//�� 9-��������� [3 : 0]count_m9. 
//������� count_m9 ���������������� ������� �������� div_out
//�� �������� �������� (count_m9 == 4'h8) ���������������� �������  count_m4, 
//�������� ������� �������� MTR_CLK = ~count_m4[0]; ������������� ������� �������������
//������ MTR_CLK.
//----------------------------------------------------------------------------------
//------------------------------------------------------------------------------							
always @(posedge RES_HARD, posedge div_out)
begin
	if(RES_HARD == 1)
	begin
		count_m9 <= 4'h0; count_m4 <= 2'h0;     M0 = 0; M1 = 0; M2 = 0; M3 = 0; 
	end
	else
	begin							
		
		if(div_out == 1)
		begin
			count_m9 <= count_m9 + 4'h1;
			if(count_m9 == 4'h8)
			begin
				count_m9 <= 4'h0; 
				count_m4 <= count_m4 + 2'h1;
			end
			
			
				case (count_m9)	
				4'h0:  		begin {M3,M2,M1,M0} <= 4'h1; end							
				4'h2:  		begin {M3,M2,M1,M0} <= 4'h2; end											
				4'h4:    	begin {M3,M2,M1,M0} <= 4'h4; end						
				4'h6:    	begin {M3,M2,M1,M0} <= 4'h8; end	
				
				default: 	begin {M3,M2,M1,M0} <= 4'h0; end 				
				endcase
			
			
			
		end	

   end
end


//-----------------------------------------------------------------------------
//������������ ����������������� ��� F0-F5 � ����������� � ��������� �������� 
//�� 12-��������� [3 : 0]count_f12. ���� ����������� ��� ������� ������� (COUNTER_F_GO == 1),
//��� ���������� ����� �������,  ������� count_f12 � ���� ������������ � ����. 
//�������� chenge_trig, ������� ��� �� ������������ � ����, � ���� F0 ��������������� 
//� �������:F0 <= 1.
//������� count_f12 ���������������� ������� �������� div_out
//----------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge div_out)
begin
	if(RES_HARD == 1)
	begin
		 count_f12 = 4'h0;   
		F0 = 0; F1 = 0; F2 = 0; F3 = 0; F4 = 0; F5 = 0; 		
	end
	else
	begin
		
				if(div_out == 1)
				begin	
				
					if(COUNTER_F_GO == 1)
					begin
								

							count_f12 = count_f12 + 4'h1;
							if(count_f12 == 4'hC)
							begin
								count_f12 = 4'h0;   
							end
						
													
						case (count_f12)	
						4'h0:  		begin {F5,F4,F3,F2,F1,F0} <= 6'h1; end						
						4'h2:  		begin {F5,F4,F3,F2,F1,F0} <= 6'h2; end											
						4'h4:    	begin {F5,F4,F3,F2,F1,F0} <= 6'h4; end						
						4'h6:  		begin {F5,F4,F3,F2,F1,F0} <= 6'h8; end						
						4'h8:  		begin {F5,F4,F3,F2,F1,F0} <= 6'h10; end											
						4'hA:    	begin {F5,F4,F3,F2,F1,F0} <= 6'h20; end				
		
						default: 	begin {F5,F4,F3,F2,F1,F0} <= 6'h0; end 					
						endcase	

						if(chenge_trig == 1)
					   begin
						 count_f12 = 4'h0;  F0 <= 1;
						end					
						
				   end
					
					else					
					begin
							count_f12 = 4'h0; 
							{F5,F4,F3,F2,F1,F0} = 	6'h0;;	

					end
					
				end	
		
	end
end


//------------------------------------------------------------------------------------------------------
//�������� [2 : 0]count_f8_brd � [1 : 0]count_f3_brd ����������������
//�������� div_out � ������������� ��� ������� ������� (COUNTER_F_GO == 1)
//������� ����� ������ div_out �� 8
//������� count_f3_brd ����� ������ �������� �������� count_f8_brd (count_f8_brd == 3'h7)
//�� ���.
// � ���������� �������� count_f8_brd == 3'h3 � count_f8_brd == 3'h6 ����������� 
//��������� WRITE_TOO_MEM = 1 �  �������  WRITE_TOO_MEM = 0(��������������) ��������� 
//������� ������ � ������. ������� count_f3_brd, ���������
//��� ������� ������� ����� ��������� ������ COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd};
//�������� (chenge_trig == 1) ��� ��� ���������� ������� COUNTER_F_GO == 1 ��� �������� ������������
//� ����
//----------------------------------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge div_out)
begin
	if(RES_HARD == 1)
	begin		
		count_f8_brd = 3'h0; WRITE_TOO_MEM = 0;
      count_f3_brd = 2'h0;		
	end
	else
	begin
		
				if(div_out == 1)
				begin	
				
					if(COUNTER_F_GO == 1)
					begin						
						count_f8_brd = count_f8_brd + 3'h1;					
						if(count_f8_brd == 3'h7)
						begin
								count_f3_brd = count_f3_brd + 2'h1;
								if(count_f3_brd == 2'h3)
									begin
										count_f3_brd = 2'h0;
									end
						end
						else
						if(count_f8_brd == 3'h3)
						begin
									WRITE_TOO_MEM = 1;
						end
						else
						if(count_f8_brd == 3'h6)//3'h5
						begin
						         WRITE_TOO_MEM = 0;
						end
						
				
						if(chenge_trig == 1)
					   begin
						 		count_f8_brd = 3'h0;
								count_f3_brd = 2'h0;
						end					
						
				   end
					
					else					
					begin		
							count_f8_brd = 3'h0;
							count_f3_brd = 2'h0;					
					end
					
		  end		
	end
end




//----------------------------------------------------------------------
//������� [2 : 0]cnt_brd �������� ��� ������� ������� 
//COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd}; ������ ���������� 
//������ �������. 
//������� ������������� �������� F5,���� ��� ������� �� 6,
//�������� chenge_trig ������� cnt_brd ������������ � ����
//------------------------------------------------------------------------
always @(posedge RES_HARD, negedge F5, posedge chenge_trig)
begin
	if(RES_HARD == 1)
	begin
    cnt_brd = 3'h0;
	end
	else
	begin
	
	
	   if(chenge_trig == 1)
			begin
			cnt_brd = 3'h0;
			end
	   else
		 if(F5 == 0)
		
			begin
			   cnt_brd =  cnt_brd + 3'h1;
				if(cnt_brd == 3'h6)
				begin
					cnt_brd = 3'h0;
				end
			end


				
	end
	
end	


//---------------------------------------------------------------
//shift_trig - ���������� �� ���� ������� ������ div_out ������ TRIG
//--------------------------------------------------------------------
always @(posedge RES_HARD, posedge div_out)
begin		
	if(RES_HARD == 1)
	begin
    shift_trig = 0;
	end
	else
	begin
		if(div_out == 1)
		begin
				shift_trig = TRIG;
		end		
	end
end

endmodule

