
`include "../../../SRC/Quartus/VERS_1_00/Defines.v"


module BubbleSort(input HARD_RESET, input _MTR_RST, input OSC,input [15 : 0]DATA_IN,input  LOAD, 
                  input RUN, output  [15 : 0]DATA_OUT, output GET,
						output reg[15 : 0]temp, output  F_SHIFT_LOAD);
parameter WINDOW = 11;

//Регистры сортировки
reg [15 : 0]M[WINDOW];

//Входные, сдвиговые регистры
reg [15 : 0]D[WINDOW];

//Счетчик, заполнения входных сдвиговых регистров, после сброса в начале новой
//строки
reg [3 : 0]count_set; 

//Счетчик  сортировки
reg [3 : 0]count_i; 
//Счетчик под-циклов циклов сортировки
reg [3 : 0]count_j;

//Буфер для выходной шины
reg [15 : 0]data_out;

//Сигнал сдвинутый на один такт СLK относительно сигнала LOAD
reg s_load; 
//Сигнал сдвинутый на два такта СLK относительно сигнала LOAD
reg ss_load; 
//Флаг изменения переднего фронта сигнала LOAD
wire f_load;

//Сигнал сдвинутый на один такт СLK относительно сигнала f_load;
reg f_shift_load;

//Сигнал сдвинутый на один такт СLK относительно сигнала RUN;
reg s_run;
//Сигнал сдвинутый на два такта СLK относительно сигнала RUN
reg ss_run; 

//Флаг изменения переднего фронта сигнала RUN
wire f_run;

//Cигнал разрешения сортировки (D-регистры загружены)
reg sort_go;

//Сигнал цикла сортировки(в М-регистры переписано содержимое D-регистров 
//и сортировка их содержимого происходит...
reg sort_cikle;

//Сигнал синхронизации
wire  CLK;
wire _CLK;


assign _CLK = ~CLK;
assign CLK = OSC; 



//Тестовый вывод сигнала f_shift_load
assign F_SHIFT_LOAD = f_shift_load;

//Флаг изменения переднего фронта сигнала LOAD
assign f_load = ~s_load & ss_load;

//Флаг изменения переднего фронта сигнала RUN
assign f_run = ~s_run & ss_run;

//Пока загружаются D-регистры, на выход передаем не сортированную информацию
assign DATA_OUT = ((sort_go == 1)) ? data_out : DATA_IN;

//Тестовый выход
assign GET = sort_go == 1 ? (count_i == 4'h1 & count_j == 4'h1 & sort_go == 1) : 1; 
//assign rs_LOCK = s_LOCK & ~LOCK;

//----------------------------------------------------------------------------------------
//Сдвиг сигналов  s_load, ss_load, s_run, ss_run, f_shift_load
// по сигналу CLK относительно сигналов LOAD и RUN
//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1) 
begin
 s_load <= 0; ss_load <= 0;  s_run <= 0; ss_run <= 0; f_shift_load <= 0;
end
	else
		begin		
		s_load <= LOAD;
		ss_load <= s_load;	
	   s_run <= RUN;
      ss_run <= s_run;	
	   f_shift_load = f_load;	
		end
end

//-------------------------------------------------------------------------------------------------
//Отсчет счетчиком [3 : 0]count_set циклов заполнения и сдвига регистров D входными данными.  
//Отсчет производится по сигналу f_load, по переднему фронту CLK.
//Когда заполнены все D-регистры(count_set == WINDOW): устанавливаются сигналы sort_go  и sort_cikle
//Сигналом (f_run == 1)сигналы sort_go, sort_cikle и счетчик	 count_set обнуляются для подготовки 
//	к работе в последующей строке. Когда счетчик циклов count_i == WINDOW, по переднему фронту CLK, 
//обнуляется сигнал  sort_cikle
//--------------------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK) 
begin
if(HARD_RESET == 1)  
begin
	count_set <= 4'h0;  
   sort_go <= 0; sort_cikle <= 0; 
end
	else
	if(f_run == 1)
	begin	
			sort_go <= 0; sort_cikle <= 0;	 count_set <= 4'h0; 
   end
	else
		begin
			if(f_load == 1)
			begin
			      if(count_set < WINDOW)
					begin
						 count_set <= count_set + 4'h1;
					end
					else
					begin
						 sort_go <= 1; sort_cikle <= 1; 
					end
			end
			else
				if(count_i == WINDOW)
					sort_cikle <= 0;
       end 
end		 

//----------------------------------------------------------------------------------------
//счетчики циклов count_i и подциклов count_j по переднему фронту _СLK:
//		-при наличии sort_go == 1 и sort_cikl == 1 счетчик подциклов count_j отсчитывает
//количество тактов _CLK, пока его значение меньше WINDOW при f_load == 0, после чего сбрасывается в
//единицу и отсчитывает новый цикл.При этом на единицу увеличивается счетчик циклов count_i.
//Пока счетчик count_i < WINDOW, он увеличивается на единицу при каждом сбросе в 1 счетчика 
//count_j, после чего оба счетчика останавливаются 
//	- при наличии f_run == 1 оба счетчика сбрасываются в ноль, при этом обнуляется регистр data_out
// - по сигналу LOAD == 1, cерединный элемент из регистра M[WINDOW/2] записывается в регистр  data_out
//и передается на выход
//-----------------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge _CLK)
begin
if(HARD_RESET == 1)  
begin  
	count_i <= 4'h1;
	count_j <= 4'h1;  data_out <= 16'h0;   data_out <= 16'h0;

end
	else
	if(f_run == 1)
	begin  
	count_i <= 4'h1; 
	count_j <= 4'h1;  data_out <= 16'h0;
	
   end
	else
		begin
					if(sort_go == 1)
					begin
							if(sort_cikle == 1)
							begin
									if(count_i < WINDOW)
									 begin
											if(count_j < WINDOW)
											begin
													if(f_load == 0)
														count_j <= count_j + 4'h1;
											end
											else
											begin
												count_j <= 4'h1; 
												count_i <= count_i + 4'h1; 
											end
							       end
									 else
									 begin
									 count_i = 4'h1;  
									 end
							end
							if(LOAD == 1)
							begin
										data_out <= M[WINDOW/2];
							end					
					end	
					
		end

end
//----------------------------------------------------------------------------------------
// - по сигналу f_run == 1 все регистры D, М и регистр temp - обнуляются
// - по сигналу (f_load == 1) при (count_set < WINDOW) в регистпы D заносится
// входная информация с шины DATA_IN, в соответствии с состоянием счетчика (WINDOW - 1) - count_set]
// при при (count_set >= WINDOW) входная информация с шины DATA_IN последователтно сдвигается
// в регистры D по каждому сигналу f_load.
// - по сигналу   (f_shift_load == 1) при наличии (sort_cikle == 1), регистры D переписываются в регистры М
// - при наличии (sort_cikle == 1) и отсутствии  f_shift_load == 1, по каждому переднему фронту CLK
//происходит сравнение содержимого (M[count_j] < M[count_j - 1]) и если условие не выполняется, ячейки 
//(M[count_j] и M[count_j - 1]) обмениваются содержимым, через регистр temp
//---------------------------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge _CLK)
begin
if(HARD_RESET == 1)  
begin
  M[0] <= 16'h0;  M[1] <= 16'h0;  M[2] <= 16'h0;  M[3] <= 16'h0; 
  M[4] <= 16'h0;  M[5] <= 16'h0;  M[6] <= 16'h0;  M[7] <= 16'h0;  
  M[8] <= 16'h0;  M[9] <= 16'h0;
  M[10] <= 16'h0; temp = 16'h0;
  
  D[0] <= 16'h0;  D[1] <= 16'h0;  D[2] <= 16'h0;  D[3] <= 16'h0; 
  D[4] <= 16'h0;  D[5] <= 16'h0;  D[6] <= 16'h0;  D[7] <= 16'h0;  
  D[8] <= 16'h0;  D[9] <= 16'h0;
  D[10] <= 16'h0; temp = 16'h0;  

end
	else
	if(f_run == 1)
	begin	
   M[0] <= 16'h0;  M[1] <= 16'h0;  M[2] <= 16'h0;  M[3] <= 16'h0; 
	M[4] <= 16'h0;  M[5] <= 16'h0;  M[6] <= 16'h0;  M[7] <= 16'h0;  
   M[8] <= 16'h0;  M[9] <= 16'h0;	
	M[10] <= 16'h0; temp = 16'h0;

   D[0] <= 16'h0;  D[1] <= 16'h0;  D[2] <= 16'h0;  D[3] <= 16'h0; 
   D[4] <= 16'h0;  D[5] <= 16'h0;  D[6] <= 16'h0;  D[7] <= 16'h0;  
	D[8] <= 16'h0;  D[9] <= 16'h0;
	D[10] <= 16'h0; temp = 16'h0; 	

   end
	else
		begin
			if(f_load == 1)
			begin
			      if(count_set < WINDOW)
					begin

							D[(WINDOW - 1) - count_set] <= DATA_IN; 
					end
					else
					begin					
							D[10] <= D[9]; D[9] <= D[8];	
							D[8] <= D[7]; D[7] <= D[6]; D[6]	<= D[5]; D[5] <= D[4];
							D[4] <= D[3]; D[3] <= D[2]; D[2] <= D[1]; D[1] <= D[0];
							D[0] <= DATA_IN;								
					end
			end
				else
				if(sort_cikle == 1)  
				begin
				   if(f_shift_load == 1)
				   begin					
					M[0] <= D[0]; M[1] <= D[1]; M[2]	<= D[2]; M[3] <= D[3];
					M[4] <= D[4]; M[5] <= D[5]; M[6]	<= D[7]; M[7] <= D[7];													
					M[8]	<= D[8]; M[9] <= D[9];		
					M[10]	<= D[10]; 
				   end
					else
					begin
							if(M[count_j] > M[count_j - 1]) 
							begin
									temp = M[count_j];
									M[count_j] = M[count_j - 1];
									M[count_j - 1] = temp;																			
							end
					end		
				end 	
			end	

end

endmodule


//----------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------
module TEST_AIN(input HARD_RESET,input CLK, input LOCK ,input INC_COUNT, input LOAD, input _MTR_RST, output  [7 : 0]ADC_OUT, input add_sum_bufer);
reg [5 : 0]count_add_sum_bufer;
reg hi_lo;
reg [15 : 0]count;
reg s_hi_lo;
reg s_inc_count;

reg [15 : 0]adc_out;
reg [1 : 0]count3;


reg s_lock;
wire f_lock/* synthesis keep */;

wire f_hi_lo;
wire f_inc_count;
wire w_res;
wire w_res_count3;
wire w_res_hi_lo;

assign f_inc_count = ~s_inc_count & INC_COUNT;
assign f_hi_lo = ~s_hi_lo & hi_lo;
assign ADC_OUT = hi_lo == 1 ? adc_out[7 : 0] : adc_out[15 : 8];
assign w_res = HARD_RESET | ~ _MTR_RST;
assign w_res_hi_lo = HARD_RESET | f_lock; 
assign w_res_count3 = HARD_RESET | f_inc_count | (LOCK == 1);
assign f_lock = s_lock & ~LOCK;
//--------------------------------------------
always@( posedge CLK)
begin
   s_lock <= LOCK; 
end
//-----------------------------------------------------------------
always@(  posedge LOAD, posedge  w_res_hi_lo)
begin
if(w_res_hi_lo == 1)  
begin
  hi_lo <= 0;

end
  else
   if(LOAD == 1)
	begin
     hi_lo <= ~hi_lo;
 	
	 end 
end


//-----------------------------------------------------------------
always@(posedge HARD_RESET,   negedge f_hi_lo)
begin
if(HARD_RESET == 1)  
begin
 adc_out = 16'h0;
end
else
	if(f_hi_lo == 0)
 	begin
`ifndef _576_pix	

 if((count + ({14'h0,count3} * 16'h80)) == 16'ha1 &&
	 (count_add_sum_bufer == 6'h1) )
     adc_out <= 16'h3ff;
	  else
	  adc_out <= (count + ({14'h0,count3} * 16'h80));



/*
	  adc_out <= ((count + ({14'h0,count3} * 16'h80)) == 16'ha1 &&
	              count_add_sum_bufer == 6'h3) ? 16'h3ff : 
	             (count + ({14'h0,count3} * 16'h80));
					 */
`else
	  adc_out <= count + ({14'h0,count3} * 16'h40);
`endif	

  
	end
	
end

//-----------------------------------------------------------------
always@(posedge w_res_hi_lo, posedge CLK)
begin
if(w_res_hi_lo == 1)  
begin
 s_hi_lo <= 0;

end
  else
   s_hi_lo <= hi_lo; 

end

//-----------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1)  
begin
 s_inc_count <= 0;
end
  else 
	s_inc_count <= INC_COUNT; 
end
//-------------------------------------------------------------
always@(posedge w_res,   posedge f_inc_count)
begin
if(w_res == 1)  
begin
 count = 16'h0;
end
	else
	if(f_inc_count == 1)
 	begin
     count <= count + 16'h1;
	end
end
//-------------------------------------------------------------
always@(posedge w_res_count3,   posedge f_hi_lo)
begin
if(w_res_count3 == 1)  
begin
 count3 = 2'h2;
end
	else
 	begin
     count3 <= count3 + 2'h1;
	  if(count3 == 2'h2)
			count3 <= 2'h0; 
	end
end



//-------------------------------------------------------------
always@(posedge LOCK,   posedge add_sum_bufer)
begin
if(LOCK == 1)  
begin
count_add_sum_bufer <= 6'h0;
end
	else
 	begin
     count_add_sum_bufer <= count_add_sum_bufer + 1;
    end
end	 
	 
endmodule

/*
module BubbleSort(input HARD_RESET,input OSC,input [15 : 0]DATA_IN,input  LOAD, 
                  input RUN, output [15 : 0]DATA_OUT, output GET,
						output reg[15 : 0]temp, output  F_SHIFT_LOAD, input [6 : 0]COUNTER_DVDR, input strob_dvdr);
parameter WINDOW_MAX = 57;
reg [15 : 0]M[WINDOW_MAX];
reg [15 : 0]D[WINDOW_MAX];

reg [5 : 0]window;

reg [3 : 0]count_set; 
reg [3 : 0]count_i; 
reg [3 : 0]count_j;
reg [15 : 0]data_out;
reg s_load; 
reg ss_load; 
wire f_load;
reg s_run;
reg ss_run; 
wire f_run;
reg sort_go;
reg sort_cikle;
wire  CLK;
reg f_shift_load;
wire _CLK;
assign _CLK = ~CLK;
assign CLK = OSC; 
assign F_SHIFT_LOAD = f_shift_load;
assign f_load = ~s_load & ss_load;
assign f_run = ~s_run & ss_run;
assign DATA_OUT = sort_go == 1 ? data_out : DATA_IN;
assign GET = sort_go == 1 ? (count_i == 4'h1 & count_j == 4'h1 & sort_go == 1) : 1; 


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge strob_dvdr)
begin
if(HARD_RESET == 1) 
begin
 window <= 0;
end
	else
		begin	
				if(strob_dvdr == 1)
				begin	
					window  = COUNTER_DVDR[0] == 1 ? COUNTER_DVDR : COUNTER_DVDR - 7'h1;							
				end				
		end
end

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1) 
begin
 s_load <= 0; ss_load <= 0;  s_run <= 0; ss_run <= 0; f_shift_load <= 0;
end
	else
		begin		
		s_load <= LOAD;
		ss_load <= s_load;	
	   s_run <= RUN;
      ss_run <= s_run;	
	   f_shift_load = f_load;	
		end
end




//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK) 
begin
if(HARD_RESET == 1)  
begin
	count_set <= 4'h0;  
   sort_go <= 0; sort_cikle <= 0; 

end
	else
	if(f_run == 1)
	begin	
			sort_go <= 0; sort_cikle <= 0;	 count_set <= 4'h0; 
   end
	else
		begin
			if(f_load == 1)
			begin
			      if(count_set < window)
					begin
						 count_set <= count_set + 4'h1;
					end
					else
					begin
						 sort_go <= 1; sort_cikle <= 1; 
					end
			end
			else
				if(count_i == window)
					sort_cikle <= 0;
       end 
end		 

//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge _CLK)
begin
if(HARD_RESET == 1)  
begin  
	count_i <= 4'h1;
	count_j <= 4'h1;  data_out <= 16'h0;   data_out <= 16'h0;

end
	else
	if(f_run == 1)
	begin  
	count_i <= 4'h1; 
	count_j <= 4'h1;  data_out <= 16'h0;
	
   end
	else
		begin
					if(sort_go == 1)
					begin
							if(sort_cikle == 1)
							begin
									if(count_i < window)
									 begin
											if(count_j < window)
											begin
													if(f_load == 0)
														count_j <= count_j + 4'h1;
											end
											else
											begin
												count_j <= 4'h1; 
												count_i <= count_i + 4'h1; 
											end
							       end
									 else
									 begin
									 count_i = 4'h1;  
									 end
							end
							if(LOAD == 1)
							begin
										data_out <= M[window/2];
							end					
					end	
					
		end

end
//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge _CLK)
begin
if(HARD_RESET == 1)  
begin
  M[0] <= 16'h0;   M[1] <= 16'h0;   M[2] <= 16'h0;   M[3] <= 16'h0; 
  M[4] <= 16'h0;   M[5] <= 16'h0;   M[6] <= 16'h0;   M[7] <= 16'h0;  
  M[8] <= 16'h0;   M[9] <= 16'h0;   M[10] <= 16'h0;  M[11] <= 16'h0;  
  M[12] <= 16'h0;  M[13] <= 16'h0;  M[14] <= 16'h0;  M[15] <= 16'h0;  
  M[16] <= 16'h0;  M[17] <= 16'h0;  M[18] <= 16'h0;  M[19] <= 16'h0;  
  M[20] <= 16'h0;  M[21] <= 16'h0;   M[23] <= 16'h0;  M[24] <= 16'h0; 
  M[25] <= 16'h0;  M[26] <= 16'h0;   M[27] <= 16'h0;  M[28] <= 16'h0; 

  M[29] <= 16'h0;   M[30] <= 16'h0;   M[31] <= 16'h0; M[32] <= 16'h0; 
  M[33] <= 16'h0;   M[34] <= 16'h0;   M[35] <= 16'h0; M[36] <= 16'h0;  
  M[37] <= 16'h0;   M[38] <= 16'h0;   M[39] <= 16'h0; M[40] <= 16'h0;  
  M[41] <= 16'h0;   M[42] <= 16'h0;  M[43] <= 16'h0;  M[44] <= 16'h0;  
  M[45] <= 16'h0;   M[46] <= 16'h0;  M[47] <= 16'h0;  M[48] <= 16'h0;  
  M[49] <= 16'h0;   M[50] <= 16'h0;   M[51] <= 16'h0;  M[52] <= 16'h0; 
  M[53] <= 16'h0;   M[54] <= 16'h0;   M[55] <= 16'h0;  M[56] <= 16'h0; 
  
  D[0] <= 16'h0;   D[1] <= 16'h0;   D[2] <= 16'h0;   D[3] <= 16'h0; 
  D[4] <= 16'h0;   D[5] <= 16'h0;   D[6] <= 16'h0;   D[7] <= 16'h0;  
  D[8] <= 16'h0;   D[9] <= 16'h0;   D[10] <= 16'h0;  D[11] <= 16'h0;  
  D[12] <= 16'h0;  D[13] <= 16'h0;  D[14] <= 16'h0;  D[15] <= 16'h0;  
  D[16] <= 16'h0;  D[17] <= 16'h0;  D[18] <= 16'h0;  D[19] <= 16'h0;  
  D[20] <= 16'h0;  D[21] <= 16'h0;   D[23] <= 16'h0;  D[24] <= 16'h0; 
  D[25] <= 16'h0;  D[26] <= 16'h0;   D[27] <= 16'h0;  D[28] <= 16'h0; 

  D[29] <= 16'h0;   D[30] <= 16'h0;   D[31] <= 16'h0; D[32] <= 16'h0; 
  D[33] <= 16'h0;   D[34] <= 16'h0;   D[35] <= 16'h0; D[36] <= 16'h0;  
  D[37] <= 16'h0;   D[38] <= 16'h0;   D[39] <= 16'h0; D[40] <= 16'h0;  
  D[41] <= 16'h0;   D[42] <= 16'h0;  D[43] <= 16'h0;  D[44] <= 16'h0;  
  D[45] <= 16'h0;   D[46] <= 16'h0;  D[47] <= 16'h0;  D[48] <= 16'h0;  
  D[49] <= 16'h0;   D[50] <= 16'h0;   D[51] <= 16'h0;  D[52] <= 16'h0; 
  D[53] <= 16'h0;   D[54] <= 16'h0;   D[55] <= 16'h0;  D[56] <= 16'h0;
  temp = 16'h0;

end
	else
	if(f_run == 1)
	begin	
  M[0] <= 16'h0;   M[1] <= 16'h0;   M[2] <= 16'h0;   M[3] <= 16'h0; 
  M[4] <= 16'h0;   M[5] <= 16'h0;   M[6] <= 16'h0;   M[7] <= 16'h0;  
  M[8] <= 16'h0;   M[9] <= 16'h0;   M[10] <= 16'h0;  M[11] <= 16'h0;  
  M[12] <= 16'h0;  M[13] <= 16'h0;  M[14] <= 16'h0;  M[15] <= 16'h0;  
  M[16] <= 16'h0;  M[17] <= 16'h0;  M[18] <= 16'h0;  M[19] <= 16'h0;  
  M[20] <= 16'h0;  M[21] <= 16'h0;   M[22] <= 16'h0;   M[23] <= 16'h0;  
  M[24] <= 16'h0;   M[25] <= 16'h0;  M[26] <= 16'h0;   M[27] <= 16'h0;  
  M[28] <= 16'h0;   M[29] <= 16'h0;   M[30] <= 16'h0;   M[31] <= 16'h0; 
  M[32] <= 16'h0;  M[33] <= 16'h0;   M[34] <= 16'h0;   M[35] <= 16'h0; 
  M[36] <= 16'h0;  M[37] <= 16'h0;   M[38] <= 16'h0;   M[39] <= 16'h0; 
  M[40] <= 16'h0;  M[41] <= 16'h0;   M[42] <= 16'h0;  M[43] <= 16'h0;  
  M[44] <= 16'h0;  M[45] <= 16'h0;   M[46] <= 16'h0;  M[47] <= 16'h0;  
  M[48] <= 16'h0;  M[49] <= 16'h0;   M[50] <= 16'h0;   M[51] <= 16'h0;  
  M[52] <= 16'h0;   M[53] <= 16'h0;   M[54] <= 16'h0;   M[55] <= 16'h0;  
  M[56] <= 16'h0; 
  
  D[0] <= 16'h0;   D[1] <= 16'h0;   D[2] <= 16'h0;   D[3] <= 16'h0; 
  D[4] <= 16'h0;   D[5] <= 16'h0;   D[6] <= 16'h0;   D[7] <= 16'h0;  
  D[8] <= 16'h0;   D[9] <= 16'h0;   D[10] <= 16'h0;  D[11] <= 16'h0;  
  D[12] <= 16'h0;  D[13] <= 16'h0;  D[14] <= 16'h0;  D[15] <= 16'h0;  
  D[16] <= 16'h0;  D[17] <= 16'h0;  D[18] <= 16'h0;  D[19] <= 16'h0;  
  D[20] <= 16'h0;  D[21] <= 16'h0;   D[22] <= 16'h0; D[23] <= 16'h0;  
  D[24] <= 16'h0;  D[25] <= 16'h0;  D[26] <= 16'h0;   D[27] <= 16'h0;  
  D[28] <= 16'h0;  D[29] <= 16'h0;   D[30] <= 16'h0;   D[31] <= 16'h0; 
  D[32] <= 16'h0;  D[33] <= 16'h0;   D[34] <= 16'h0;   D[35] <= 16'h0; 
  D[36] <= 16'h0;  D[37] <= 16'h0;   D[38] <= 16'h0;   D[39] <= 16'h0; 
  D[40] <= 16'h0;  D[41] <= 16'h0;   D[42] <= 16'h0;  D[43] <= 16'h0;  
  D[44] <= 16'h0;  D[45] <= 16'h0;   D[46] <= 16'h0;  D[47] <= 16'h0;  
  D[48] <= 16'h0;  D[49] <= 16'h0;   D[50] <= 16'h0;   D[51] <= 16'h0;  
  D[52] <= 16'h0;  D[53] <= 16'h0;   D[54] <= 16'h0;   D[55] <= 16'h0;  
  D[56] <= 16'h0; 
	
	temp = 16'h0; 	

   end
	else
		begin
			if(f_load == 1)
			begin
			      if(count_set < window)
					begin

							D[(window - 1) - count_set] <= DATA_IN; 
					end
					else
					begin			
			
	
							D[56] <= D[55]; D[55] <= D[54];	
							D[54]	<= D[53]; D[53] <= D[52]; D[52] <= D[51]; D[51] <= D[50]; D[50] <= D[49]; 
							
							D[49] <= D[48]; D[48] <= D[47]; D[47] <= D[46];	D[46]	<= D[45]; D[45] <= D[44];
							D[44] <= D[43]; D[43] <= D[42]; D[42] <= D[41]; D[41] <= D[40]; D[40] <= D[39]; 
							
							D[39] <= D[38]; D[38] <= D[37]; D[37] <= D[36]; D[36] <= D[35]; D[35] <= D[34]; 
							D[34]	<= D[33]; D[33] <= D[32]; D[32] <= D[31]; D[31] <= D[30]; D[30] <= D[29]; 
							
							D[29] <= D[28]; D[28] <= D[27]; D[27] <= D[26]; D[26]	<= D[25]; D[25] <= D[24];							
							D[24] <= D[23]; D[23] <= D[22]; D[22] <= D[21]; D[21]	<= D[20]; D[20] <= D[19];
							
							D[19] <= D[18]; D[18] <= D[17]; D[17] <= D[16]; D[16] <= D[15]; D[15] <= D[14]; 
							D[14] <= D[13]; D[13] <= D[12]; D[12] <= D[11]; D[11] <= D[10]; D[10] <= D[9];  
							
							D[9]	<= D[8];  D[8] <= D[7];   D[7] <= D[6];   D[6] <= D[5];   D[5]	<= D[4];  
							D[4] <= D[3];   D[3] <= D[2];   D[2] <= D[1];   D[1] <= D[0];   D[0] <= DATA_IN;

					end
			end
				else
				if(sort_cikle == 1)  
				begin
				   if(f_shift_load == 1)
				   begin			
				
							M[56] <= D[56];  M[55] <= D[55]; 
							M[54] <= D[54]; M[53] <= D[53];  M[52] <= D[52]; M[51] <= D[51]; M[50] <= D[50]; 
							
							M[49] <= D[49]; M[48] <= D[48]; M[47] <= D[47];  M[46] <= D[46];  M[45] <= D[45];  
							M[44] <= D[44]; M[43] <= D[43];  M[42] <= D[42];  M[41] <= D[41];  M[40] <= D[40]; 	

							M[39] <= D[39];  M[38] <= D[38]; M[37] <= D[37];  M[36] <= D[36]; M[35] <= D[35];  
							M[34] <= D[34]; M[33] <= D[33]; M[32] <= D[32];  M[31] <= D[31]; M[30] <= D[30]; 
							
							M[29] <= D[29]; M[28] <= D[28]; M[27] <= D[27];  M[26] <= D[26];  M[25] <= D[25]; 
							M[24] <= D[24]; M[23] <= D[23]; M[22] <= D[22];   M[21] <= D[21]; M[20] <= D[20];  
							
							M[19] <= D[19]; M[18] <= D[18];  M[17] <= D[17];  M[16] <= D[16];  M[15] <= D[15]; 	
							M[14] <= D[14];  M[13] <= D[13]; M[12] <= D[12];  M[11] <= D[11]; M[10] <= D[10];  
							
							M[9] <= D[9];   M[8] <= D[8];   M[7] <= D[7];    M[6] <= D[6];  M[5] <= D[5]; 
							M[4] <= D[4];   M[3] <= D[3];   M[2] <= D[2];     M[1] <= D[1]; M[0] <= D[0];  
																												
				   end
					else
					begin
							if(M[count_j] > M[count_j - 1]) 
							begin
									temp = M[count_j];
									M[count_j] = M[count_j - 1];
									M[count_j - 1] = temp;																			
							end
					end		
				end 	
			end	

end

endmodule
*/