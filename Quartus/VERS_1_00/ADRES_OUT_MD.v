`timescale 1ns/10 ps 

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

/*
 ������ ��������� ���� ������ ������������ ������� ����������: 

 input RES_HARD - ���������� �����
 input CLK      -����������� �� ��� ������� PLL_OSC1(PLL_OSC) - ������ PLL, ������������
				     ��� ������ ������������� ����� ��� ������� == 1
 input CS_READ_MTR - ���� ������ ������ ���������������� ������ ������ ������ ������, �� ����� ���������� ��� ������
 input 	INT_RESET_RE8 -  ������ ������ � ���������� ���������� ������ ����������(����� �� ������ PIC_SINXRO_RF12)
 input ALTCS, PMPRD, PMPWR,PMPA0 - ������� ������� ������, ������, ������, �������� ���� ���� ������
											  ��������������� PIC ��������������
				     
 output reg [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]ADRES_OUT - ���� ������ ������ ������ �������
 output MEM_EN_RD		- ������ ���������� ������ ������ ������ �������		
 output CLK_MEM_READ - ������ ������ ������ ������ ������ �������
 output  test_120
*/
module    ADRES_OUT_MD( input RES_HARD,input CLK, input CS_READ_MTR, input  	INT_RESET_RE8,
						input ALTCS, input PMPRD, input PMPWR, input PMPA0,
				     
				     output reg [`D_WIDTH_OF_ADRES_OUT_11 - 1  : 0]ADRES_OUT, output MEM_EN_RD,
				     output CLK_MEM_READ,output  test_120
					);
					

//��������� �� ���� � ��� ����� ������� �LK ��������������, ������ �������� ���� ���� ������ ��������������� PIC				
reg shift_pmpA0, shift_shift_pmpA0;

//change_pmpA0 - ������ ��������� ������ �������� ���� ���� ������ ��������������� PIC
//mtr_res - �� ������������
//res_adres_out - ����� ������� INT_RESET_RE8
wire change_pmpA0, mtr_res, res_adres_out ;


//������ ��������� ������ �������� ���� ���� ������ ��������������� PIC

assign change_pmpA0 = ((shift_pmpA0 ^ shift_shift_pmpA0) & CS_READ_MTR);


//- ������ ���������� ������ ������ ������ �������	
assign MEM_EN_RD = (~change_pmpA0 & CS_READ_MTR);

//����������
assign test_120 = ADRES_OUT == 2302;

//- ������ ������ ������ ������ ������ �������
assign CLK_MEM_READ = (~(PMPA0 ^ shift_shift_pmpA0)) & CS_READ_MTR;

assign res_adres_out = INT_RESET_RE8;


//-------------------------------------------------------------------------------------
//��������� �� ���� � ��� ����� ������� �LK ��������������, ������ �������� ���� ���� ������ ��������������� PIC	
//-------------------------------------------------------------------------------------
always @(posedge RES_HARD, negedge CLK)
begin
if(RES_HARD == 1)
begin
	shift_pmpA0 <= 0; shift_shift_pmpA0 <= 0;
end
else
begin
	if(CLK == 0)
	begin
		shift_pmpA0 <= PMPA0;
		shift_shift_pmpA0 <= shift_pmpA0;
	end
end 
end
	
	
//---------------------------------------------------------------------------------	
//[11 : 0]ADRES_OUT - ������� ���� ��� ������ ���������� �� ������ ����� � PIC
//������������ ��������:
//	- res_adres_out( INT_RESET_RE8); 
// - ��� ������� (ADRES_OUT == `D_END_ADRES_OUT_1152 )
//����������������: 
//	- change_pmpA0
//---------------------------------------------------------------------------------	
wire pmp0;
assign w_a = ADRES_OUT;

always @(posedge RES_HARD, posedge w_a, posedge res_adres_out )
begin
if(RES_HARD == 1)
begin
	ADRES_OUT = `D_WIDTH_OF_ADRES_OUT_11'h00;
end
else
begin

	if(res_adres_out == 1) 
	begin
				ADRES_OUT = `D_OFFS_ADRES_OUT_0; 				
	end		
	else
	begin

		if(w_a == 1)
		begin		
					if(ADRES_OUT == `D_END_ADRES_OUT_1152 )					
					begin							
						ADRES_OUT = `D_OFFS_ADRES_OUT_0; 
					end	
					else			
					ADRES_OUT = ADRES_OUT + `D_WIDTH_OF_ADRES_OUT_11'h1;
		end	
	end			
end
end		
	
//-----------------------------------------------------------------------------------								
endmodule	