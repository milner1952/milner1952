

module sortirovka(input RES_HARD_RF13, input CLK, output K8);

wire [15 : 0]BAS_AD/* synthesis keep */;
				 
reg [15 : 0]in_put_d[20];
reg [15 : 0] t_in_put_d;
wire [15 : 0]out_put_d[10];
reg [3 : 0]count;
reg run;
reg [15 : 0]count_cykle;
wire load/* synthesis keep */;
wire get/* synthesis keep */; 
wire [31 : 0]out; 
assign K8 = |BAS_AD;
assign load = count_cykle[3 : 0] ==4'h1;

//----------------------
always@(posedge CLK )
begin
if (RES_HARD_RF13)
begin
  in_put_d[0] = 300; in_put_d[1] = 201;in_put_d[2] = 502;
  in_put_d[3] = 403; in_put_d[4] = 204;in_put_d[5] = 705;
  in_put_d[6] = 306; in_put_d[7] = 807;in_put_d[8] = 908;
  in_put_d[9] = 609; 
  in_put_d[10] = 110; in_put_d[11] = 111;in_put_d[12] = 512;
  in_put_d[13] = 213; in_put_d[14] = 214;in_put_d[15] = 215;
  in_put_d[16] = 116; in_put_d[17] = 117;in_put_d[18] = 318;
  in_put_d[19] = 119;
  count = 4'h0; run  = 0; 
  count_cykle = 16'h0;
end
  else
  if(CLK == 1)
  begin
       if(get == 1)
		 begin
         count_cykle <= count_cykle + 16'h1;
			if(count_cykle[3 : 0] == 4'hf)
			begin			 
					t_in_put_d = out[15 : 0];
					//in_put_d[count]; 
					count = count + 4'h1;
					if(count == 4'ha)
					begin
						count = 4'h0; 
					end	
			end
		 end	
			if((count_cykle == 16'hffff) || (count_cykle == 16'h00) || (count_cykle == 16'h01))
			    run <= 1;
			else
	          run <= 0;		
  end
end
												 											  
BubbleSort    sort(RES_HARD_RF13, CLK, t_in_put_d, load, run, BAS_AD, get);												 											  
Random rand(RES_HARD_RF13,  CLK,  1, out, out_lb);


endmodule

