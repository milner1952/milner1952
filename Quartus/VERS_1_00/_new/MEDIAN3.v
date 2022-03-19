

module MEDIAN3(input HARD_RESET, input [15 : 0]IN_DATA, input LOAD, input RST,  
					output  [15 : 0]OUT_DATA, output reg GO);
					
					
reg [15 : 0]R0;
reg [15 : 0]R1;
reg [15 : 0]R2;

reg [1 : 0]count_full;
reg [15 : 0]_OUT_DATA;
assign OUT_DATA = GO == 1 ?  IN_DATA : _OUT_DATA;
//--------------------------------------------------------------------------
always @(posedge HARD_RESET,  posedge LOAD, posedge RST)
begin
if(HARD_RESET == 1)
begin
	count_full = 2'h0;
	GO = 0;
end	
else
			if(RST == 1)
			begin
				count_full = 2'h0; GO = 0;
			end	
			else
				if(LOAD == 1)
				begin
					if(count_full < 2'h3)
					begin
					   count_full = count_full + 2'h1;	GO = 0;					
					end
					else
					   GO = 1;
				end	
end			

//--------------------------------------------------------------------------
always @(posedge HARD_RESET,  posedge LOAD, posedge RST)
begin
if(HARD_RESET == 1)
begin
	R0 <=0;
	R1 <=0;
	R2 <=0;
end	
else
			if(RST == 1)
			begin
			R0 <=0;
			R1 <=0;
			R2 <=0;
			end	
			else
				if(LOAD == 1)
				begin
               R2 <= R1;
					R1 <= R0;
					R0 <= IN_DATA; 
				end	
end
//--------------------------------------------------------------------------
always @(posedge HARD_RESET,  posedge LOAD )
begin
if(HARD_RESET == 1)
	_OUT_DATA = IN_DATA;
else
begin
				if(LOAD == 1)
				begin

					if ((R0 < R1) && (R0 < R2))
					begin
						_OUT_DATA <= (R1 < R2) ? R1 : R2;
					end
					else 
					if ((R1 < R0) && (R1 < R2))
					begin
						_OUT_DATA <= (R0 < R2) ? R0 : R2;
					end
					else
					begin
						_OUT_DATA <= (R0 < R1) ? R0 : R1;
					end
				end
/*				
				 if ((a <= b) && (a <= c))
					begin
							middle = (b <= c) ? b : c;
					end
					else if ((b <= a) && (b <= c))
					begin
							middle = (a <= c) ? a : c;
					end
					else
					begin
							middle = (a <= b) ? a : b;
					end
*/
				
end
end
	
endmodule					