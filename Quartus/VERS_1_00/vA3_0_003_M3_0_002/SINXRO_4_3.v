
//----------------------------------------------------
//Узел синхронизации 4 фазы с делителем частоты 
//-----------------------------------------------------


`timescale 1ns/10 ps 
//-----------------------------------------------------------------------------------------------------
//Модуль выробатывает синхро серии неперекрывающихся фаз сигналов Мх и Fх, синхросигнал MTR_CLK
//------------------------------------------------------------------------------------------------------
//FRC_IN - входная частота блока
//сигнал FRC2-служит для синхронизации анализатора
//RES_HARD - cигнал аппаратного сброса
//TRIG - Сигнал готовности видео информации от матриц.
//COUNTER_F_GO - сигнал разрешения работы счетчиков F и фаз Fx
//Mx и Fx - серии из 4 и 6 неперекрывающихся фаз, вырабатываемых блоком, для синхронизации схемы датчика
//COUNT_BOARD - счетчик адресации аналоговых блоков
//МТR_CLK-частота синхронизации матриц
//CHENGE_ADRES_IN - сигнал инкрементировать адрес памяти при приеме информации от аналоговых блоков
//_RD - сигнал чтения информации из аналоговых блоков
//CLK_BUFER - сигнал сдвига младшего и старшего байта информации при ее приеме из аналоговых блоков 
//					по 8-разрядной шине.
//WRITE_BUFER - сигнал записи младшего и старшего байта инфомации принятой из аналоговых блоков по 
//					8-разрядной шине в 16 - разрядный регистр    
//WRITE_TOO_MEM - сигнал записи в память информации из 16-разрядного регистра, принятой из аналоговых блоков.
//COUNT_3P - счетчик делитель на три
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
							
//Буферный регистр счетчика [1 : 0]COUNT_3P							
reg [1 : 0]r_COUNT_3P;

//shift_trig - задержаный на один переиод сигнал div_out сигнал TRIG
reg shift_trig;

//count_m9 - делитель сигнала div_out на 9
reg [3 : 0]count_m9;

//[1 : 0]count_m4 - делитель переноса счетчика count_m9 на4
reg [1 : 0]count_m4;

//count_f12 - делитель сигнала div_out на 12
reg [3 : 0]count_f12;

//[2 : 0]cnt_brd - два старших разряда плюс счетчик [1 : 0]count_f3_brd составляют счетчик, который
//адресует аналоговые блоки датчика
reg [2 : 0]cnt_brd;

//[2 : 0]count_f8_brd - делитель сигнала div_out на 8 работа этого счетчика 
//разрешается сигналом COUNTER_F_GO
reg [2 : 0]count_f8_brd;

//[1 : 0]count_f3_brd; - делитель переноса счетчика count_f8_brd на 3
reg [1 : 0]count_f3_brd;

//задержанное на один период состояние счетчика адреса аналоговых блоков COUNT_BOARD
reg [3 : 0]shift_count_board;

// делитель на два входной частоты FRC_IN
reg FRC1;

// инверсия разряда div_out = ~COUNT_3P[1];
wire div_out;

//фронт сигнала TRIG
wire chenge_trig;
assign chenge_trig = TRIG & ~shift_trig;

//Сигнал синхронизации матриц
assign MTR_CLK = ~count_m4[0];

//Счетчик аналоговых блоков датчика
assign COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd};

//изменение состояния счетчика аналоговых блоков
assign CHENGE_ADRES_IN = shift_count_board != COUNT_BOARD; 

//Сигнал перезаписи младшего и старшего восьмиразрядного  байта информации поступающей из аналоговых блоков
// в буферный 16-разрядный регистр
assign WRITE_BUFER = count_f8_brd == 3'h7;

//Сигнал чтения аналоговых блоков
assign _RD  = COUNTER_F_GO == 1'h1 ? (count_f12 == 4'h3 || count_f12 == 4'h7 ||  count_f12 == 4'hb) : 1'h1;

//Сигнал сдвига младшего и старшего восьмиразрядного  байта информации поступающей из аналоговых блоков
assign CLK_BUFER = (count_f12 == 4'h2 || count_f12 == 4'h6 ||  count_f12 == 4'ha);

//частота для работы счетчиков m и f
assign div_out = ~COUNT_3P[1];
//----------------------------------------------
//Делитель входной частоты на два
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
//Сохранение состояния счетчика COUNT_3P в буферном регистре
//с задержкой на пол-такта сигнала FRC1
//-------------------------------------------------------------------------
always @(posedge RES_HARD, negedge FRC1)
begin
	if(RES_HARD == 1)
	begin	
		r_COUNT_3P <= 2'h0;	
	end
	else
   r_COUNT_3P <= COUNT_3P;
end
	
//---------------------------------------------------------------------------
//		Делитель входного сигнала FRC1 на 3. 
//Сохраненное в регистре r_COUNT_3P состояние счетчика
//используется   при формировании сброса счетчика, для большей устойчивости.
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
//Задержка счетчика аналоговых блоков COUNT_BOARD на 
//один такт сигнала div_out. В дальнейшем используется
// для формирования сигнала изменения состояния сигнала 
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
//Формирование неперекрывающихся фаз М00-F3 в сответствии с состояним делителя 
//на 9-счетчиком [3 : 0]count_m9. 
//Счетчик count_m9 синхронизируется входной частотой div_out
//По импульсу переноса (count_m9 == 4'h8) инкрементируется счетчик  count_m4, 
//инверсия разряда которого MTR_CLK = ~count_m4[0]; присваивается сигналу синхронизации
//матриц MTR_CLK.
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
//Формирование неперекрывающихся фаз F0-F5 в сответствии с состояним делителя 
//на 12-счетчиком [3 : 0]count_f12. Фазы формируются при наличии сигнала (COUNTER_F_GO == 1),
//при отсутствии этого сигнала,  счетчик count_f12 и фазы сбрасываются в ноль. 
//Сигналом chenge_trig, счетчик так же сбрасывается в ноль, а фаза F0 устанавливается 
//в единицу:F0 <= 1.
//Счетчик count_f12 синхронизируется входной частотой div_out
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
//Счетчики [2 : 0]count_f8_brd и [1 : 0]count_f3_brd синхронизируются
//сигналом div_out и переключаются при наличии сигнала (COUNTER_F_GO == 1)
//счетчик делит сигнал div_out на 8
//счетчик count_f3_brd делит сигнал переноса счетчика count_f8_brd (count_f8_brd == 3'h7)
//на три.
// В состояниях счетчика count_f8_brd == 3'h3 и count_f8_brd == 3'h6 формируется 
//единичное WRITE_TOO_MEM = 1 и  нолевое  WRITE_TOO_MEM = 0(соответственно) состояния 
//сигнала записи в помять. Счетчик count_f3_brd, формирует
//два младших разряда адрес аналговых блоков COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd};
//Сигналом (chenge_trig == 1) или при отсутствии сигнала COUNTER_F_GO == 1 оба счетчика сбрасываются
//в ноль
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
//Счетчик [2 : 0]cnt_brd образует два старших разряда 
//COUNT_BOARD = {cnt_brd[2],cnt_brd[1],count_f3_brd}; адреса аналоговых 
//блоков датчика. 
//Счетчик переключается сигналом F5,деля его частоту на 6,
//Сигналом chenge_trig счетчик cnt_brd сбрасывается в ноль
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
//shift_trig - задержаный на один переиод сигнал div_out сигнал TRIG
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

