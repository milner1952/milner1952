
/*
При Binn == 1
Дано:
Частота сигнала Convect  1,045 Мгц
Частота Trig 71 кГц

Передача 140000 - 0 Гц
Подавление: 500000 - 12500000 Гц
Taps = 5
1342
21
21
21
1342
/*
wire [15 : 0]out_fit0;

fir #( .TAPS(5) ) fir_lp_inst(
  .clk(load_to_sort),
  .coefs( {
	 
32'd1342,
32'd21,
32'd21,
32'd21,
32'd1342	 
	 
	 
    } ),
  .in(w_s_buf0),
  ._out(out_fit0)
);
*/
*/
//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
module fir ( clk, coefs, in, _out );

parameter IWIDTH = 16;	//input data (signal) width
parameter CWIDTH = 16;	//tap coef data width (should be less then 32 bit)
parameter TAPS   = 25;	//number of filter taps
localparam MWIDTH = (IWIDTH+CWIDTH); //multiplied width
localparam RWIDTH = (MWIDTH+TAPS-1); //filter result width

input  wire clk;
input  wire [IWIDTH-1:0]in;
input  wire [TAPS*32-1:0]coefs; //all input coefficient concatineted
/*output*/ wire [RWIDTH-1:0]out; //output takes only top bits part of result
output reg  [15 : 0]_out;

genvar i;
generate
	for( i=0; i<TAPS; i=i+1 )
	begin:tap
		//make tap register chain
		reg [IWIDTH-1:0]r=0;
		if(i==0)
		begin
			//1st tap takes signal from input
			always @(posedge clk)
				r <= in;
		end
		else
		begin
			//tap reg takes signal from prev tap reg
			always @(posedge clk)
				tap[i].r <= tap[i-1].r;
		end

		//get tap multiplication constant coef
		wire [CWIDTH-1:0]c;
		assign c = coefs[((TAPS-1-i)*32+CWIDTH-1):(TAPS-1-i)*32];

		//calculate multiplication and fix result in register
		reg [MWIDTH-1:0]m;
		always @(posedge clk)
			m <= $signed(r) * $signed( c );
			
		//make combinatorial adders
		reg [MWIDTH-1+i:0]a;
		if(i==0)
		begin
			always @*
				tap[i].a = $signed(tap[i].m);
		end
		else
		begin
			always @*
				tap[i].a = $signed(tap[i].m)+$signed(tap[i-1].a);
		end
	end
endgenerate

//fix calculated taps summa in register
reg [RWIDTH-1:0]result;
always @(posedge clk)
	result <= tap[TAPS-1].a;

//deliver output
assign out = result;
//assign _out = out >> (RWIDTH - 16);
always @(posedge clk)
	_out <= out >> (RWIDTH - 16);
endmodule