
module Sorting_Stack (input clk,input hold,input is_input,input [HBIT:0] data_in, output [HBIT:0]data_out	);

//	Числовые параметры
parameter HBIT= 15;				//	size of number in bits
parameter R_SZ= 7;				//	capacity, max sequence size
parameter _R_SZ= (R_SZ+1)/2;			//	not to modify

wire [HBIT:0] in_prev[_R_SZ ];
wire [HBIT:0] in_next[_R_SZ ];
wire [HBIT:0] out[_R_SZ];




  Cell_Compare #(HBIT)ribbon[_R_SZ]( clk, hold, is_input, in_prev[i], in_next[i], out[i] );	
  
  
  
generate
  genvar i;
  genvar j;
 
  for (i=0; i<_R_SZ-1; i=i+1) 
  begin : block_name01
		assign in_prev[i+1]= out[i];			
		assign in_next[i]=  out[i+1];			 
  end  
 
  assign in_prev[0]= data_in;
  assign data_out= out[0];			
  assign in_next[_R_SZ-1]= 0;
endgenerate


  
endmodule

//-------------------------------------------------------------------------
module Cell_Compare (input clk, input hold, input is_input, input [HBIT:0]in_prev, input [HBIT:0] in_next,
							output [HBIT:0]out );

parameter HBIT= 15;
assign out= is_input ? lower : higher;

reg [HBIT:0] higher;
reg [HBIT:0] lower;

wire [HBIT:0] cand_h= is_input ? higher : lower;
wire [HBIT:0] cand_l= is_input ? in_prev : in_next;

always@(posedge clk )
if (~hold)
begin
	//	Здесь мы наконец сравниваем два числа-кандидата.
	higher <= ( cand_h >= cand_l ) ? cand_h : cand_l;
	lower  <= ( cand_h >= cand_l ) ? cand_l : cand_h;
end
endmodule

//----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------

					
module BubbleSort(input HARD_RESET,input CLK,input [15 : 0]DATA_IN,input  LOAD, input RUN, output [15 : 0]DATA_OUT);
parameter WINDOW = 9;
reg [15 : 0]M[WINDOW];
reg [3 : 0]count_set; 
reg [3 : 0]count_i; 
reg [3 : 0]count_j;
reg [15 : 0]temp;
reg [15 : 0]data_out;

reg s_load; 
reg ss_load; 
wire f_load;

reg s_run;
reg ss_run; 
wire f_run;

reg sort_go;
reg sort_cikle;
reg i;
reg j;

assign f_load = ~s_load & ss_load;
assign f_run = ~s_run & ss_run;
assign DATA_OUT = data_out;

//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1) 
begin
 s_load = 0; ss_load = 0;  s_run = 0; ss_run = 0;
end
	else
		begin		
		s_load <= LOAD;
		ss_load <= s_load;	
	   s_run <= RUN;
      ss_run <= s_run;		
		end
end


//----------------------------------------------------------------------------------------
always@(posedge HARD_RESET, posedge CLK)
begin
if(HARD_RESET == 1)  
begin
	count_set <= 4'h0;  M[0] <= 16'h0;  M[1] <= 16'h0;  M[2] <= 16'h0;  M[3] <= 16'h0; 
							  M[4] <= 16'h0;  M[5] <= 16'h0;  M[6] <= 16'h0;  M[7] <= 16'h0;  M[8] <= 16'h0;
	count_i <= 4'h0; temp = 16'h0;
	count_j <= 4'h0; sort_go <= 0; sort_cikle <= 0; data_out <= 16'h0;
end
	else
	if(f_run == 1)
	begin
	
	count_set <= 4'h0;  M[0] <= 16'h0;  M[1] <= 16'h0;  M[2] <= 16'h0;  M[3] <= 16'h0; 
							  M[4] <= 16'h0;  M[5] <= 16'h0;  M[6] <= 16'h0;  M[7] <= 16'h0;  M[8] <= 16'h0;
	count_i <= 4'h0; temp = 16'h0;
	count_j <= 4'h0; sort_go <= 0; sort_cikle <= 0; data_out <= 16'h0;
	
	
   end
	else
		begin
			if(f_load == 1)
			begin
			      if(count_set < WINDOW)
					begin
						M[(WINDOW - 1) - count_set] <= DATA_IN; count_set <= count_set + 4'h1;
					end
					else
					begin
						M[count_set] <= M[count_set - 1]; M[0] <= DATA_IN; sort_go <= 1; sort_cikle <= 1; i <= 0; j <= 0; 
					end
			end
				else
					if(sort_go == 1)
					begin
							if(sort_cikle == 1)
							begin
									if(count_i < WINDOW)
									 begin
											if(count_j < WINDOW)
											begin
														if(M[j] > M[j - 1])
														begin
															temp = M[j];
															M[j] = M[j-1];
															M[j-1] = temp;
														end																							
														count_j <= count_j + 4'h1;
											end
											else
											begin
												count_j = 4'h0;
												count_i <= count_i + 4'h1;
											end
							       end
									 else
									 begin
									 count_i = 4'h0; sort_cikle <= 0; data_out <= M[WINDOW/2 +1];
									 end
							end					
					end	
					
		end

end


endmodule
