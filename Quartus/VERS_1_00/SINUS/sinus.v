

module sinus(input RES_HARD, input CLK1,input CLK2, output test);

reg [9 : 0]count = 10'h0;
reg  [15 : 0]mem_reg;
reg  [32 : 0]sin_reg;
reg  [15 : 0]out_reg;
reg flag;
reg flag_flag;
wire [15 : 0]sin/* synthesis keep */;
reg [15:0] mem[0 : 989] /* synthesis ram_init_file = "D:\MAP\S_KAMERA\SRC\Quartus\VERS_1_00\SINUS\sinus.mif" */;
assign sin = out_reg;
assign test = |sin|flag_flag;

//----------------------------------------------------------------------------------
always @(posedge RES_HARD, posedge CLK1)
begin
	if(RES_HARD == 1)
	begin

	end
	else
	begin
	if(CLK1  == 1)
		begin
				if(flag_flag == 0)
				begin
				sin_reg <= mem_reg * 16'h7fff;
				out_reg <= (sin_reg/16'h2710 + 16'h7fff);
				end
				else
				begin
				sin_reg <= mem_reg * 16'h7fff;
				out_reg <= (16'h7fff - sin_reg/16'h2710);				
				end
		end
	end
end


//----------------------------------------------------------------------------------
always @(posedge RES_HARD, negedge CLK1)
begin
	if(RES_HARD == 1)
	begin
		mem_reg = 16'h0;
	end
	else
	begin
	if(CLK1  == 0)
		begin
			mem_reg <=	mem[count];
		end
	end
end
//------------------------------------------------------------
always @(posedge RES_HARD, posedge CLK1)
begin
	if(RES_HARD == 1)
	begin
		count = 10'h0; flag <= 0; flag_flag <= 0;
	end
	else
	begin
	if(CLK1  == 1)
		begin
			if(flag == 0)
			begin
				count <=	count + 10'h1;
				if(count == 983)
				begin
				flag <= 1;
				end
			end
		   else
			begin
				count <=	count - 10'h1;
				if(count == 0)
				begin
				flag <= 0; flag_flag <= ~flag_flag;
				end						
			end		
		end
	end
end



endmodule

