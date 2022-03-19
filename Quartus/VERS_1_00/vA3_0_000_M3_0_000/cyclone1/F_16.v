

module F_16(input DIR1, input _CS1, input DIR2, input _CS2,
				inout [7 : 0]A_1, inout [7 : 0]B_1, 
				inout [7 : 0]A_2, inout [7 : 0]B_2
				);

assign A_1 = ((DIR1 == 0) &  (_CS1 == 0))? B_1 : 8'bzzzzzzzz; 
assign B_1 = ((DIR1 == 1) &  (_CS1 == 0))? A_1 : 8'bzzzzzzzz; 

assign A_2 = ((DIR2 == 0) &  (_CS2 == 0))? B_2 : 8'bzzzzzzzz; 
assign B_2 = ((DIR2 == 1) &  (_CS2 == 0))? A_2 : 8'bzzzzzzzz; 
				
endmodule				