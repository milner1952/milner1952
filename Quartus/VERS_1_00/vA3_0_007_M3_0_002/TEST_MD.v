`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"
//-----------------------------------------------------------------------------------------------------
//������ ������������ ������� _TEST_TRIG, _TRST_END, ��� ������� TEST_GO == 1
//---------------------------------------------------------------------------------------------------- 
//RES_HARD - ���������� ����� �����
//Mx - ������ ����� ����������������� ��� ��������
//��R_CLK-������� ������������� ������
//_RUN - ����� �������� ���������� �� ����� ���� �����
//_MTR_RES -������ �������������� ����� ������
//TEST_GO - ����� �������� � �������� ������ ��� ������
//TEST_TRIG - ������ �RIG � ������ ��� ������
//_TEST_END - ������ END � ������ ��� ������
//--------------------------------------------------------------------------------
module 	TEST_MD(input RES_HARD, input M0,input M1,input M2,input M3,  
				input MTR_CLK, input _RUN, input _MTR_RES, input TEST_GO, 
				
				
				output  _TEST_TRIG, output _TEST_END, output reg K13 
				);
			
//------------------------------------------------------------------------------------		
//�������:
// -shift_mtr_res - c����������������� � ����� M2 ������ _MTR_RES
// -count_wait_trig_go - c�����(����������)  ������ �������� [4 : 0]count_wait_trig
// -trig_go - ���������� �������� [7 : 0]count_trig � [1 : 0]count_mtr_clk ������������ � ����������� 
//������ _TEST_TRIG = ~test_trig
// - shift_end - c����������������� � ����� M3 ������ _TEST_END
//-------------------------------------------------------------------------------------------------
reg shift_mtr_res, count_wait_trig_go, trig_go, shift_end;	

//������� [4 : 0]count_wait_trig, ����������� �������� �������� TEST_TRIG �� ������� MTR_RST, � ������������ � ���������
//���������� ������ ������.			
reg [4 : 0]count_wait_trig ;

//����������� ������ ������� MTR_CLK b ��������� ������ _TEST_TRIG = ~test_trig
reg [1 : 0]count_mtr_clk;

//����������� 128 �������� ������� TEST_TRIG � ��� ������������ ������� TEST_END
reg [7 : 0]count_trig; 

//������ test_trig - ��������� ������ TRIG � ������ ��� ������(� �������� ������ ������ �������)
reg test_trig;

//������� ��������� �������� _MTR_RES � _TEST_END
wire change_mtr_res, change_end;

//���� ������� test_trig �� ������� ������ TEST_MD
assign _TEST_TRIG = ~test_trig;

//������������ ������� ������ ��� ��������� ������� _MTR_RES
assign change_mtr_res = _MTR_RES & (~shift_mtr_res);

//������������ ��������� ������ ��� ��������� ������� _TEST_END
assign change_end = ~_TEST_END & shift_end; 

//�������� �����
assign test_122 = count_wait_trig_go;

//������ _TEST_END �����������, ����� (count_trig == ((`D_NUM_WORD_OF_ADRES_IN_1152/9) - 1'h1))
assign _TEST_END = (count_trig == ((`D_NUM_WORD_OF_ADRES_IN_1152/9) - 1'h1)); 
//----------------------------------------------------------------------------------			
//-shift_mtr_res - c������������ � ����� M2 ������ _MTR_RES
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		shift_mtr_res = 0;
	end
	else
		if(M2 == 1)
		begin
			shift_mtr_res = _MTR_RES;				
		end						
end	

//-------------------------------------------------------------------------------
//- �� ���� �1 ��� ��������� ������� ������ MTR_RST ��������������� ������ ���������� ������� 
//�������� �������� count_wait_trig_go = 1;
//- �� ���� �2 ����� ������� count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5, ������ count_wait_trig_go = 0 
//������������
//-------------------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M1, posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		count_wait_trig_go = 0; 
	end
	else			
		if(M1 == 1)
		begin
				if(change_mtr_res == 1)
				begin
					count_wait_trig_go = 1;
				end	
		end		
		else										
		if(M2 == 1)			
		begin
				if(count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5)	//4	
				begin
					count_wait_trig_go = 0;  
				end		
		end	
end	

//-------------------------------------------------------------------------------
// - �� ���� M1 ����� ������� count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5
// ��������� ������  trig_go = 1 - ���������� ������������ ������� TEST_TRIG
//- �� ���� M2 ��� change_end == 1, ������������ trig_go = 0
//--------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M1,  posedge M2 )
begin
	if(RES_HARD == 1)
	begin
		trig_go = 0;
	end
	else
		if(M1 == 1)
		begin
			if(count_wait_trig == `D_COUNT_WAITE_TRIG_GO_5)
			        trig_go = 1;	
		end
		else
		if(M2 == 1)
		begin
			if(change_end == 1)
			begin
					trig_go = 0;
			end			
		end			
end


	
//-------------------------------------------------------------------------------
// ��� ������� ������� count_wait_trig_go, ������� [4 : 0]count_wait_trig
// ������������� �� �������, ��� ���������� ����� ������� - ������������ � ����,
// �������� �������� ������ �������� TRIG ������������ �������� MTR_RST � ������������
// � ��������� ���������� ������
//---------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M2)			
begin
	if(RES_HARD == 1)
	begin
		count_wait_trig = 5'h0; 
	end	
	else
		if(M2 == 1)
		begin			
			if(count_wait_trig_go == 1)
			begin
				count_wait_trig = count_wait_trig + 5'h1;					
			end	
			else
			    count_wait_trig = 5'h0;													
		end	
end				

//-------------------------------------------------------------------------
//������� [1 : 0]count_mtr_clk, ����������� ������ ������� ������� MTR_CLK � 
//� ��������� count_mtr_clk == 2'h01 ��������� ������ test_trig(~_TEST_TRIG), ��� trig_go == 0
//������� count_mtr_clk � ������ test_trig(~_TEST_TRIG) ������������ � ����
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge MTR_CLK)			
begin
	if(RES_HARD == 1)
	begin
		count_mtr_clk = 2'h0; test_trig = 0;
	end	
	else
		if(MTR_CLK == 1)
		begin
			if(trig_go == 1)
			begin
				
				count_mtr_clk = count_mtr_clk + 2'h1;
				if(count_mtr_clk == 2'h01)
					test_trig = 1;
				else
					test_trig = 0;	
			end	
			else
			begin
			count_mtr_clk = 2'h0; test_trig = 0;	
			end
		end
end	

//-------------------------------------------------------------------------
//��� ������� trig_go == 1, ������� [7 : 0]count_trig ����������� �������
//MTR_CLK, ��� trig_go == 0 ���� ������� ������������ � ����
//----------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge MTR_CLK)			
begin
	if(RES_HARD == 1)
	begin
		count_trig = 8'h0; 
	end	
	else
	begin
		if(MTR_CLK == 1)
		begin	
			if(trig_go == 1)
			begin
				if(test_trig == 1)
				begin
					count_trig = count_trig + 8'h1;	
				end
			end		
			else
			count_trig = 8'h0;			 
		end
	end	
end	

//----------------------------------------------------------------------------------			
//-shift_end - c������������ � ����� M3 ������ _TEST_END
//----------------------------------------------------------------------------------
always @(posedge RES_HARD,  posedge M3)			
begin
	if(RES_HARD == 1)
	begin
		shift_end = 0; 
	end	
	else
		if(M3 == 1)
			shift_end = _TEST_END;	
end	
				
endmodule				

