

`include "../../../SRC/Quartus/VERS_1_00/Defines.v"

/*

input [`D_S_WIDTH_OF_DATA_8 - 1 : 0]ADC_D - ��������������� ���� ������ ���� ���
input CLK   - ������ MTR_CLK		
input  RST - ������ MTR_RST    		
input _TRIG_0, END_0, _TRIG_1, END_1, _TRIG_2,END_2,	 - ������� TRIG � END  ���� ������  	
input _TEST_TRIG, TEST_END - ������� TRIG � END ��� ������ � �������� ������ ��� ������
input  T_R - ��������� ���������� LOAD ��� �������� ���������� ������         
input MX_BOARD - ������������ ����������� �������������� 
input _RD_BOARD,  _WR_BOARD 	 - ������ ������ � ������ �����  

output  END_BOARD   - ������ END �����

input  A0_BOARD, A1_BOARD, A2_BOARD, A3_BOARD, A4_BOARD - �������� ���� �����    							 
input HARD_RESET,
input OSC1 - ��������� ��������� 25 ��������  			
input OSC2 - ��������� ��������� 25 ��������     
input [1:0]BA - ����� ����� � �������       
input BYSY0, BYSY1 ,BYSY2  - ������ ��������� ���� ���
inout   [`D_S_WIDTH_OF_DATA_8 - 1 : 0]PD - ��������������� �������� ���� �����

//������� ������, ������ �������������� ���, ������ TRIG �����, 								 
output reg RESET_ADC,_CONVECT,_TRIG_BOARD, 
//������ ������������ ����� �������-�������� ���,
//������������ �������� ������
//C����� ���������� ���������� ��������������� 
output LOCK, V_GANE,	MX, 
output CS_ADC_0,CS_ADC_1,CS_ADC_2 ������� ������� ���� ���	
output _BYTESWAP - ������ �������/������� ���� �� ���� ADC_D
output K6,K7,K8 - �������� ������
*/

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
								 
								 output reg RESET_ADC, 	output  _CONVECT,	output _TRIG_BOARD,	output  LOCK,	output reg V_GANE,	output MX ,	output CS_ADC_0,	
								 output CS_ADC_1,	output CS_ADC_2,	output _BYTESWAP,
								 output K6, output  K7, output K8
								 );
	
//������� ������:
//  comand_reg[6] -  en_h_filtr 
//  comand_reg[4],comand_reg[3] - BINNING[1 : 0]
//  comand_reg[1] - With matrices/Without matrices
reg [7 : 0]comand_reg/* synthesis noprune */;

//������� ��������������:
//		-���������� ���� ������ � ������ ��������
//    - �������� � �������� ����� ����������
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]pr;								 

//������ � ������ ��������
reg [7 : 0]VersData[20];

//������� ������ ������ �������� ��� ����������
reg [5 : 0]count_vers_data_out;

//��� ������ �������������� �������� � �������� ����� ���������� ��� �� ����������
reg lo_hi;


//����������� ������� �� ���������� ������,
//����������  ���:
//		_WR_BOARD ==  1 �  addr_board == 1 � addr_comand == 1 � ret_version = PD[0]; 
//������������ ���:
// 	_WR_BOARD ==  1 �  (addr_board != 1 ��� addr_comand != 1 ��� ret_version != PD[0]); 
reg ret_version; 
reg /*_R_WR_BOARD,*/ K13;

//����� ����������� �� ���� ������ ������� OSC1 ������������ 
//������� BYTE_SWAP
reg s_byte_swap;

//������� � ������� ����� ������� ��������� ���� ��������������� ���� ���� A��
//���������� ���������������
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf0L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf1L;
reg [`D_S_WIDTH_OF_DATA_8 - 1 : 0]s_buf2L;

//��� 24 ��������� �������� ��������� ���� ���
reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf0;
reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf1;
reg [`D_S_WIDTH_OF_BUFER_AD_24 - 1 : 0]sum_buf2;

//��� �������� �������� �������� ��������� ���� ���
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf0/* synthesis keep */; 
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf1/* synthesis keep */;
reg [`D_S_WIDTH_OF_DATA_AD_16 - 1 : 0]out_bf_buf2/* synthesis keep */; 

//������, ����������� �� ���� ���� ������� OSC1 ������������ �������
// TRIG_OUT
reg s_trig/* synthesis noprune */;

//������, ����������� �� ���� ���� ������� OSC1 ������������ �������
//CS_ADC_2, ����� ������� CS_ADC_2
reg s_cs_adc2;

// ������� �����, ����������� ������������� ��� ������������� SignalTap2 
reg [12 : 0]count_mtr_rst;

// ������� �������� � ������, ����������� ������������� ��� ������������� SignalTap2
reg [7 : 0]count_trig;

// ������� ��������� ���. ����� � ���������� ����� � ������, ����������� 
//������������� ��� ������������� SignalTap2
reg [9 : 0]count_adres_board;

// ������� ���������� ���� ��� ��� ������ BINNING, �� �������� � ���� ������� 
//������� ����� � ��������� sum_buf� 
//reg [5 : 0]counter_dvdr;
reg [6 : 0]counter_dvdr;

// ������ �������� � ������ ����� �� ��� �� ��������������� ����� s_buf�L, s_buf��
//���� ���
reg load_byte_from_adc;
//������ ���������� ���������� ������� ������� ������ ������, ������ �������
// �����  frst_lock == 1� ������ ���������� � ��� ���������� �� ����� ������� ��� ����������
// ������ ��������������� �� RST == 0(MTR_RST) � ������������ �������� counter_dvdr_iz7 == 1 
reg frst_lock;


//������� �������������� �� ������ ����������� � ���. ����� �� ��������� �������� out_bf_buf�, 
//���������� ��� (A0_BOARD == 0);
wire addr_get_data_x_ray/* synthesis keep */;

//����������������� ����� ({A4_BOARD,A3_BOARD,A2_BOARD,A1_BOARD,A0_BOARD} == 5'h18), ��� ������� 
//������� ���. ����� ��������� �� ��� ���������� �����.
wire broadcasting_adres/* synthesis keep */;

//����� �������� ������ ������������� count_f10
wire [3 : 0]count_f10;

//����� ������ SIN ,������ ������������� - �� ������������ 
wire sin/* synthesis keep */;

//����� ������ SIN_GO ,������ �������������
wire sin_go/* synthesis keep */;

//����� ������ f0 ,������ �������������
wire f0;

//����� ������ f1 ,������ ������������� - �� ������������ 
wire f1/* synthesis keep */;

//����� ������� f2,f3 ,������ ������������� - �� ������������� 
wire f2,f3;

//����� ������� m0,m1,m2,m3,m4 ,������ ������������� - �� �������������
wire m0,m1,m2,m3,m4;

//����� ������ ������������� TRIG_OUT
wire TRIG_OUT;

//����� ������ [7 : 0]count_m10 ,������ �������������
//wire [5 : 0]count_m10/* synthesis keep */;
wire [6 : 0]count_m10/* synthesis keep */;

//����m ������ count_f10_next ,������ ������������� - �� �������������
wire count_f10_next;

//�������������������� ���� ���� ��� - w_s_buf� = {s_buf�H, s_buf�L}
wire [15 : 0]w_s_buf0/* synthesis keep */;   
wire [15 : 0]w_s_buf1/* synthesis keep */;  
wire [15 : 0]w_s_buf2/* synthesis keep */;  



//������� �����
wire addr_board/* synthesis keep */; 

//������� ������� �� �����
wire addr_comand/* synthesis keep */; 

//��������� ������� BYTE_SWAP - ���  (count_m10 != 7'h1);
wire chang_byte_swap/* synthesis keep */; 

//������: 
// - ������������ sum_buf� <= sum_buf� + w_sort_buf� 
// - ���������� counter_dvdr
// - �������� ������ ��������� ����������  load_to_sort ��� ������� 
//           ����������  load_to_sort = _add_sum_bufer & en_filter;
wire _add_sum_bufer/* synthesis keep */; 

//������ ������� �������� ����� sum_buf� �� ������� {17'h0,counter_dvdr} �
//��������� ���������� �  out_bf_buf�
//out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr});  
wire div_sum_buf/* synthesis keep */;

//����� �������� ����� sum_buf� � ���� ������ �������, ��� ���������� 
//� ���������� �����
wire res_sum_buf/* synthesis keep */;

//����� �������� ������� ��_BOARD, ������������ ��� ������ ��������� 
//� ��������� ������� ����������
wire s_mx_board;
//�������� �������� ������� ��_BOARD �� ���� ���� CLK
reg sh_mx_board;

wire _K7;

//������ ������ �������� ������ �������� counter_dvdr ����� ������� ������ 
// � ��������� sum_buf� � ���������� ���������� � out_bf_buf�
wire res_counter_dvdr/* synthesis keep */; 

//������ ������ ������������� no_add_sum - ������� ������� ��������� sum_buf� <= sum_buf� + w_sort_buf� 
// � ��������� ������ ������������� �������, ��-�� �� ���������������� ������ ���2 � ���� ������, �.�.
// � ��� ����� ���������� ������� out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr});
wire no_add_sum/* synthesis keep */; 

//������ ������ ������������� div_for_end - ������ ������� ����� � ��������� 
//out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr});  ����� ��������� ������� END, �.�. ������ TRIG � ���
//�����  �����������
wire div_for_end;

//������ ������ ������������� res_sum_for_end - �����a �������� ����� sum_buf� � ���� ����� 
//�������, ��� ���������� � ���������� ����� ��� ���������� ������� TRIG ����� ��������� ������� END
wire res_sum_for_end;


//������ ������ ������������� res_counter_dvdr_for_end - ������ ������ � ����, �������� ������ �������� counter_dvdr 
//����� ������� ������, ��� ���������� � ���������� ����� ��� ���������� ������� TRIG ����� ��������� ������� END
wire res_counter_dvdr_for_end;

// ����� ������ ��������� ���� ADC_D ������ ���
wire [7 : 0]_ADC_D/* synthesis keep */; 

// �������� ����� ������ PLL ��� ������������� ������� ���������� �������
wire pll_clk_to_sort;

wire rs_rst/* synthesis keep */;
reg start/* synthesis noprune */;
reg s_rst/* synthesis noprune */;
reg s_s_rst/* synthesis noprune */;
reg [2 : 0]count_start/* synthesis noprune */;

//������ ������������� ������� ���������� �������/ ���  - w_out_sort_buf0 / w_s_buf�
// �������������� �������� en_filter
wire [15 : 0]w_sort_buf0/* synthesis keep */; 
wire [15 : 0]w_sort_buf1/* synthesis keep */;
wire [15 : 0]w_sort_buf2/* synthesis keep */;

assign K7 = start;

reg r_div_sum_buf;

//������, ����������� ������ ��������� ��������
wire en_filter;
// ������ �������� � ��������� �������� counter_dvdr == 7
wire counter_dvdr_iz7/* synthesis keep */;

//���������� ������ ��������, ����������� ������ ��������
//comand_reg[6];
assign en_filter = comand_reg[6];
assign counter_dvdr_iz7 = counter_dvdr == 7'h7;


`ifdef TEST_AIN_WIHOT_MATRIX
wire [7 : 0]ADC_OUT_TEST/* synthesis keep */; 
//���� ���
assign _ADC_D = ADC_OUT_TEST;
`else
//���� ���
assign _ADC_D = ADC_D;
`endif

//������� counter_dvdr ������������ � ������ ��������� ��. count_m10(��� ������� ������� TRIG) ���
//�������� res_counter_dvdr_for_end ������ ������������� ��� ���������� ������� TRIG � ����� ������
//������ ����� ������� END
assign res_counter_dvdr = (count_m10 == 7'h1) | res_counter_dvdr_for_end;
//assign res_counter_dvdr = (count_m10 == 6'h1) | res_counter_dvdr_for_end;

//C����� ���������� ���������� ��������������� With matrices/Without matrices(������ ����������� ����������)
assign MX = MX_BOARD;

//������ ������������ ���������� ���������� � ���, �������� � ��������� �������� count_m10 == 0 | 1 ��� 
//������� ������� sin_go
assign LOCK = ((count_m10 == 7'h1) | (count_m10 == 7'h0)) & sin_go;
//assign LOCK = ((count_m10 == 6'h1) | (count_m10 == 6'h0)) & sin_go;

//������ ������ �������������� ���, �������� � ���������  ��. count_f10 == 1 ��� (count_m10 != 0) �
//((count_m10 != 7'h1) & (count_m10 != 7'h2))
assign _CONVECT = ~(`PURE_SYCLE_CONVECT &  (count_f10 == 4'h1 ) & (count_m10 != 7'h0));
//assign _CONVECT = ~(`PURE_SYCLE_CONVECT &  (count_f10 == 4'h1 ) & (count_m10 != 6'h0));

//������� ������� ��� ���������� ���������� �������� count_f10 ��� �������
//((count_m10 != 7'h2) & (count_m10 != 7'h3) & (count_m10 != 7'h1) &  (count_m10 != 7'h0))
assign CS_ADC_0 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 1 || count_f10 == 2)) ;
assign CS_ADC_1 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 3 || count_f10 == 4));
assign CS_ADC_2 = ~(`PURE_SYCLE_CS_ADC & (count_f10 == 5 || count_f10 == 6));

//������ _BYTESWAP �������� ������� �������� ��.  count_f10 ��� (CS_ADC_0 == 0 || CS_ADC_1 == 0 || CS_ADC_2 == 0)
assign _BYTESWAP = ((CS_ADC_0 == 0 || CS_ADC_1 == 0 || CS_ADC_2 == 0) & count_f10[0]);

//������ ��������� ������� _BYTESWAP ��� count_m10 != 7'h1
assign chang_byte_swap = (s_byte_swap ^ _BYTESWAP) &  (count_m10 != 7'h1);
//assign chang_byte_swap = (s_byte_swap ^ _BYTESWAP) &  (count_m10 != 6'h1);

//������ ������������ ����������� ��������������, �������� ���������� ������� ������ ������� CS_ADC_2 ���
// ((count_m10 != 7'h2) & (count_m10 != 7'h3)) � (count_m10 != 7'h1) � ���������� ������� ������� ����������� 
//no_add_sum ������ �������������
assign _add_sum_bufer = ~s_cs_adc2 & CS_ADC_2 & `PURE_SYCLE_BYTESWAP_6 & (count_m10 != 7'h1) & (~no_add_sum);
//assign _add_sum_bufer = ~s_cs_adc2 & CS_ADC_2 & `PURE_SYCLE_BYTESWAP_6 & (count_m10 != 6'h1) & (~no_add_sum);

//����������� ��������� ����, ����������� �� 8-��������� ��. � ��. ���� ���
assign w_s_buf0 = {s_buf0L,s_buf0};
assign w_s_buf1 = {s_buf1L,s_buf1};
assign w_s_buf2 = {s_buf2L,s_buf2};

//������ ��������� ���� �������� ���������� � ����������� �����, �������� ���������� �� �������
// (_RD_BOARD == 0) ��� ������� �������  (addr_board == 1), � ��������� ��. ���� ��������� �
//����������������� ���������
assign PD = ((_RD_BOARD == 0) & (addr_board == 1)) ? pr : 8'bzzzzzzzz;	

//������ _TRIG_BOARD � ����������� �� ��������� comand_reg[1] == 1'h1 �����, ������� TRIG ����
//������ ��� ������� _TEST_TRIG(��� ������ ��� ������, � ����� ������������)
assign _TRIG_BOARD = comand_reg[1] == 1'h1 ? _TEST_TRIG :(_TRIG_0 | _TRIG_1 | _TRIG_2);   

//������ END_BOARD � ����������� �� ��������� comand_reg[1] == 1'h1 �����, ������� END ����
//������ ��� ������� _TEST_END(��� ������ ��� ������, � ����� ������������)
assign END_BOARD = comand_reg[1] == 1'h1 ? TEST_END : ( END_0 |  END_1 | END_2);      

//������ ������� �����, �������� ��� ��������� ���� ��- ��� ������� � �������� {A4_BOARD, A3_BOARD}
assign addr_board = (BA == {A4_BOARD, A3_BOARD}) ? 1'h1 : 1'h0; 

//������ ����� �������, �������� ��� ((A2_BOARD == 0) & (A1_BOARD == 1) & (A0_BOARD == 1));
assign addr_comand =  ((A2_BOARD == 0) & (A1_BOARD == 1) & (A0_BOARD == 1));//- D_S_ADRES_COMMAND_03 3'h03 

//������ ������ ���������� �� A�� 
assign addr_get_data_x_ray  =  (A0_BOARD == 0);

//����������������� ����� - ������� �������� �� ��� ���������� ����� ������������
assign broadcasting_adres = ({A4_BOARD,A3_BOARD,A2_BOARD,A1_BOARD,A0_BOARD} == 5'h18);

//������������� SignalTap2 � �����������
assign K6 = /*HARD_RESET;*/count_mtr_rst[12] | count_trig[7] | count_adres_board[8] | addr_board || w_sort_buf0 || w_sort_buf1 || w_sort_buf2;

//������������� SignalTap2 � �����������, ��� ������ ���������� � XL	 
assign K8 =   ( _add_sum_bufer) &  ((out_bf_buf0 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (out_bf_buf1 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (out_bf_buf2 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (sum_buf0 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (sum_buf1 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (sum_buf2 != `D_S_WIDTH_OF_BUFER_AD_24'h0) &
																	  (count_trig != 8'h1) &
																	  (counter_dvdr != 7'h3) //&
																	//  (count_mtr_rst[12] | count_trig[7] | count_adres_board[8] | addr_board || w_sort_buf0 || w_sort_buf1 || w_sort_buf2)
																	  //(counter_dvdr != 6'h3) 
																	  );

//������ ������� ����� � ���������	out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr}) �������� � ����� ������� TRIG
//��� ��������  div_for_end ������ ������������� � ����� ������, ����� ������� END ��� ���������� TRIG
assign div_sum_buf = s_trig & (~TRIG_OUT) | div_for_end;

//������ ������ ����� � ��������� sum_buf� �������� ����� ������� out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr}) 
//� ���������  (count_m10 == 7'h0) � (count_f10 == 4'h0) �� ���� f1  ��� ��������  res_sum_for_end ������ ������������� 
//� ����� ������, ����� ������� END ��� ���������� TRIG, ��� ���������� � ���������� �����
assign res_sum_buf = ((count_m10 == 7'h0) & (count_f10 == 4'h0) & (f0 == 1) & (count3_next == 1)) | res_sum_for_end ;
//assign res_sum_buf = ((count_m10 == 6'h0) & (count_f10 == 4'h0) & (f0 == 1) & (count3_next == 1)) | res_sum_for_end ;

//������ ��������� ������� MX_BOARD
assign s_mx_board = (sh_mx_board & (~MX_BOARD));

//--------------------------------------------------------------------
//������ load_byte_from_adc, ��������
//�� ���-����� OSC1, ������������ 
//������� chang_byte_swap  
//---------------------------------------------------------------------------
/*
always @(posedge HARD_RESET, negedge OSC1)
begin
if(HARD_RESET  == 1)
begin
load_byte_from_adc <= 0; 
r_div_sum_buf <= 0;
end
else
if(OSC1 == 0)
begin
  load_byte_from_adc <= chang_byte_swap;
  r_div_sum_buf <= div_sum_buf;
end  
end
*/

always @(posedge HARD_RESET,  posedge OSC1)
begin
if(HARD_RESET  == 1)
begin
load_byte_from_adc <= 0; 
end
else
//if(OSC1 == 0)
begin
  load_byte_from_adc <= chang_byte_swap;
end  
end

always @(posedge HARD_RESET, negedge OSC1)
begin
if(HARD_RESET  == 1)
begin
r_div_sum_buf <= 0;
end
else
//if(OSC1 == 0)
begin
  r_div_sum_buf <= div_sum_buf;
end  
end

//--------------------------------------------------------------------
//������ sh_mx_board, ��������
//�� ���� ���� �LK, ������������ 
//������� MX_BOARD  
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
//������� ������������� ����������� � ����������� SignalTap2
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

//-------------------------------------------------------------------------
//������� ������������� ����������� � ����������� SignalTap2
//----------------------------------------------------------------------------
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
//������� ������������� ����������� � ����������� SignalTap2
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




//----------------------------------------------------------------------------
//������� [6 : 0]counter_dvdr, ���������� ���������� �������� _add_sum_bufer
//� ������ ����� ������� TRIG
//����� � 0: �������� res_counter_dvdr == 1
//---------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge OSC1)
begin
if(HARD_RESET == 1)
begin
counter_dvdr <= 7'h0;
 
end
else	
begin
         if(res_counter_dvdr == 1)
			begin
            counter_dvdr <= 7'h0; 

			end
			else
			begin

         if(_add_sum_bufer == 1)
			begin
				counter_dvdr <= counter_dvdr + 7'h1; 			 
			end  
			end
 
end
end


//--------------------------------------------------------------------
//������� s_cs_adc2 � s_trig, ���������
//�� ���� ���� OSC1, ������������ 
//������� CS_ADC_2 � TRIG_OUT ��������������   
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
//--------------------------------------------------------------------
//������ s_byte_swap, ��������
//�� ���� ���� OSC1, ������������ 
//������� _BYTESWAP   
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
//���������������� ���������� ����� ���������� ��� �� ���� _ACD � ���������������
// �������� s_buf� �� ������� load_byte_from_adc
//-------------------------------------------------------------------------------
always @(posedge HARD_RESET, negedge  load_byte_from_adc)
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
			if(load_byte_from_adc == 0)
				begin					 
				    s_buf1L <= s_buf2; s_buf2 <= s_buf2L; s_buf2L <= _ADC_D;
                s_buf0 <= s_buf0L; s_buf0L <= s_buf1; s_buf1 <= s_buf1L;					 
					 
				end	 					 
		end
end

//-------------------------------------------------------------------------------------------------
// ������� ���������  out_bf_buf� <= sum_buf�/({17'h0,counter_dvdr})  �� ������� (div_sum_buf == 1)
// ����� ��������� sum_buf0 �� ������� (res_sum_buf == 1)
// ������������ ��������� sum_buf0 <= sum_buf0 + w_sort_buf0 �� ������� (_add_sum_bufer == 0)
//-------------------------------------------------------------------------------------------------
always @(posedge HARD_RESET, posedge r_div_sum_buf, posedge res_sum_buf, negedge _add_sum_bufer)
begin
if(HARD_RESET  == 1)
begin
 
	sum_buf0 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
	sum_buf1 <= `D_S_WIDTH_OF_BUFER_AD_24'h0;  
	sum_buf2 <= `D_S_WIDTH_OF_BUFER_AD_24'h0; 
	
end 
else
   if(r_div_sum_buf == 1)
	begin
 		
      out_bf_buf0 <= sum_buf0/({17'h0,counter_dvdr});  
		out_bf_buf1 <= sum_buf1/({17'h0,counter_dvdr});   
		out_bf_buf2 <= sum_buf2/({17'h0,counter_dvdr});  
		
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
		sum_buf0 <= sum_buf0 + w_sort_buf0; 
		sum_buf1 <= sum_buf1 + w_sort_buf1; 
		sum_buf2 <= sum_buf2 + w_sort_buf2;		
		end
end

//-----------------------------------------------------------------------------
//���������� � ������ �����
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
//������ ������ ���, �������� ���� ��� ��� ��������� ������
//�����
//------------------------------------------------
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
	
//-----------------------------------------------------------------------------
//������ ������� � ������� ������ comand_reg:
//������ ���������� �� ������� _WR_BOARD == 0 ��� 
//������� broadcasting_adres == 1(������� ������������������ ������). 
//������ �������,����� ������������ �������� ������, �������������� ����� ����� "���������", �� ���� PD.
//������� ������������ �������� ������-�������������, ����� ��� ���� ��������� ����� ����.
//���� ������� ��������� (PD[0] == 0) & (PD[2] == 0) & (PD[5] == 0), �� ��� PD[7] � ��������
//������� ������������ �������� ������-��������� � comand_reg
//���� ��� ��������� � �������, �� ��������������� ��� ��� ���� PD, ��������� 
//� comand_reg
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
							comand_reg[1] = PD[1];// � ��������� ��� ���
						end
						if(PD[2] == 1)
						begin
							comand_reg[3] = PD[3];//�������
							comand_reg[4] = PD[4];
						end
						
						if(PD[5] == 1)
						   comand_reg[6] = PD[6];//����������\��������� ������
							
						if((PD[0] == 0) & (PD[2] == 0) & (PD[5] == 0))	//����������� �������� ������
						V_GANE = PD[7];
							
					end		
			end
   end
end

//----------------------------------------------------------------------------
//������� �������� ������.
//������ ������������ �������� _WR_BOARD
//��� ������� ��������������� ������ ����� (addr_board == 1) � (addr_comand == 1) 
//��������� ��� PD[0] ����, ����� �������� � �������  ret_version.
//��� ������� _WR_BOARD � ���������� (addr_board == 1) ��� (addr_comand == 1) 
//������� ret_version, ������������ 
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
//������ ���������� � ��������������� ���� ����������� �����
//������ ������������ �� ������� (_RD_BOARD == 0) ��� ������� (addr_board == 1)
// - ���� ��� ���� ���������� (addr_comand  == 1) � (ret_version == 1), �� ������������ 
//���������� � ������ ����� � ������������ � ���������� �������� count_vers_data_out.
// - ���� ���������� addr_get_data_x_ray - (A0_BOARD == 0), �� ��������� ���������� 
//�������������������� ��������� out_bf_buf�. ��� ������� lo_hi == 0 ��������� ������� ���� ����
//���������,� ��� lo_hi == 1 ��������� �� ������� ����.
//���� �������� out_bf_buf� ����������� � ������, � ������������ � ���������� ��������
//{A2_BOARD,A1_BOARD}
//-------------------------------------------------------------------------------
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



always@(posedge HARD_RESET, /*negedge RST,*/ posedge OSC1)
begin
if(HARD_RESET == 1) 
begin
  frst_lock <= 0;
end
else
begin

		if(OSC1 == 1)
		begin		
			if(RST == 0)
			begin
				frst_lock  <= 1;//1
			end
			else				
			if(counter_dvdr_iz7 == 1)
			begin
					frst_lock <= 0;
			end
		end

end
end








assign rs_rst = s_rst & ~s_s_rst;
//---------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1) 
begin
  s_rst <= 0;
  s_s_rst <= 0;
end
else
	begin
		s_rst <= RST;
		s_s_rst <= s_rst;
	end
end






//---------------------------------------------------------------
always@(posedge HARD_RESET, negedge CLK)
begin
if(HARD_RESET == 1) 
begin
  start <= 0; 
end
else
	begin
	  if(CLK == 0)
	  begin
			if(rs_rst == 1)
			begin				
							start <= 1;
			end	
			else
			begin
					if(count_start == 3'h3)
					begin
							start <= 0; 
					end		
			end			
		end						 
	end
end



//---------------------------------------------------------------
always@(posedge HARD_RESET, negedge CLK)
begin
if(HARD_RESET == 1) 
begin
 count_start <= 3'h0;
end
else
	begin
	  if(CLK == 0)
	  begin
         if(start  == 1)
					count_start <= count_start + 3'h1; 
			else		
					count_start <= 3'h0;			
		end						 
	end
end

//������ �������������
SINXRO_AIN_6 sinxro_ain_6(HARD_RESET, OSC1,_TRIG_BOARD, END_BOARD, 
								  CLK, 
								  TRIG_OUT,  
							     f0,f1,f2,f3, 
							     m0, m1, m2, m3, m4,
							     count_f10, sin, _K7, sin_go, 
								  count_m10, count_f10_next, count3_next, no_add_sum,
								  div_for_end, res_sum_for_end, res_counter_dvdr_for_end
								  );

wire [15 : 0]temp0;
wire f_s_load0;								  

wire [15 : 0]temp1;
wire f_s_load1;

wire [15 : 0]temp2;
wire f_s_load2;


//������� ���� ������� ����������(��������)
wire [15 : 0]w_out_sort_buf0;
wire [15 : 0]w_out_sort_buf1;
wire [15 : 0]w_out_sort_buf2;

//����������� � ���������� ������� ���������� � ����� ��������� w_sort_buf�
assign w_sort_buf0 = ((en_filter == 0) | (frst_lock == 1)) ? w_s_buf0 : w_out_sort_buf0;
assign w_sort_buf1 = ((en_filter == 0) | (frst_lock == 1)) ? w_s_buf1 : w_out_sort_buf1;
assign w_sort_buf2 = ((en_filter == 0) | (frst_lock == 1)) ? w_s_buf2 : w_out_sort_buf2;

//��������� � ���������� ������� ����������
wire load_to_sort;
assign  load_to_sort = _add_sum_bufer & en_filter;

//������ ����������

BubbleSort    sort0(HARD_RESET, RST, pll_CLK_to_sort, w_s_buf0, load_to_sort, ~s_mx_board ,w_out_sort_buf0, get, temp0, f_s_load0);					
BubbleSort    sort1(HARD_RESET, RST, pll_CLK_to_sort, w_s_buf1, load_to_sort, ~s_mx_board ,w_out_sort_buf1, get, temp1, f_s_load1);	
BubbleSort    sort2(HARD_RESET, RST, pll_CLK_to_sort, w_s_buf2, load_to_sort, ~s_mx_board ,w_out_sort_buf2, get, temp2, f_s_load2);	


/*
BubbleSort  sort0(HARD_RESET, pll_CLK_to_sort, w_s_buf0, load_to_sort, 
                  ~s_mx_board, w_out_sort_buf0, get,
						temp0, f_s_load0, counter_dvdr,  div_sum_buf);


BubbleSort  sort1(HARD_RESET, pll_CLK_to_sort, w_s_buf1, load_to_sort, 
                  ~s_mx_board, w_out_sort_buf1, get,
						temp0, f_s_load1, counter_dvdr,  div_sum_buf);						
						
						
BubbleSort  sort2(HARD_RESET, pll_CLK_to_sort, w_s_buf2, load_to_sort, 
                  ~s_mx_board, w_out_sort_buf2, get,
						temp0, f_s_load2, counter_dvdr,  div_sum_buf);						
	*/					

//������ PLL MUL = 6, DIV = 1, Fin = 25���, Fout = 150���
pll_ain1  pll(OSC1, pll_CLK_to_sort);

`ifdef TEST_AIN_WIHOT_MATRIX
TEST_AIN test_ain(HARD_RESET,pll_CLK_to_sort ,LOCK ,div_sum_buf ,load_byte_from_adc, RST, ADC_OUT_TEST);
`endif

endmodule
