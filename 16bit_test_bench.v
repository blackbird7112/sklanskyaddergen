module skadd_testb();
	reg[15:0] a, b;
	reg ci;
	wire[15:0] s;	wire co;
	sklansky g1(ci, a[15:0], b[15:0], s[15:0], co);
	initial begin
		a = 16'b0101100111011111; b=16'b1011110011010110; ci = 1'b0;
		#5 a = 16'b1100111101000010; b=16'b0100100010001101; ci = 1'b0;
		#5 a = 16'b1101010100111100; b=16'b1100000111000000; ci = 1'b1;
		#5 a = 16'b0001110111110110; b=16'b1111110101111010; ci = 1'b1;
	end
endmodule