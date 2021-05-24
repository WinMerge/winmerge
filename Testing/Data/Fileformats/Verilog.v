//Verilog Syntax Highlight Testfile

input                    // a Verilog Keyword
wire                     // a Verilog Keyword
WIRE                  // a unique name ( not a keyword)
Wire                    // a unique name (not a keyword)

/* This is a
  Multi line comment
  example */
module addbit (
a,
b,
ci,
sum,
co);

// Input Ports  Single line comment
input           a;
input           b;
input           ci;
// Output ports
output         sum;
output         co;
// Data Types      
wire            a;
wire            b;
wire            ci;
wire            sum;
wire            co; 

endmodule

// Compiler Directives...
module ifdef ();

initial begin
`ifdef FIRST
    $display("First code is compiled");
`else
  `ifdef SECOND 
    $display("Second code is compiled");
  `else
    $display("Default code is compiled");
  `endif
`endif
  $finish;
end

endmodule

// Numbers...
module signed_number;

reg [31:0]  a;

initial begin
  a = 14'h1234;
  $display ("Current Value of a = %h", a);
  a = 32'hDEAD_BEEF;
  $display ("Current Value of a = %h", a);
  a = 3.5E6;
  $display ("Current Value of a = %h", a);
  a = 16'bZ;
  $display ("Current Value of a = %h", a);
  #10 $finish;
end

endmodule

// Functions...
initial begin
  $monitor ("%g wr:%h wr_data:%h rd:%h rd_data:%h", 
    $time, wr_en, data_in,  rd_en, data_out);
  clk = 0;
  rst = 0;
  rd_en = 0;
  wr_en = 0;
  data_in = 0;
  #5 rst = 1;
  #5 rst = 0;
  @ (negedge clk);
  wr_en = 1;
  // We are causing over flow
  for (i = 0 ; i < 10; i = i + 1) begin
     data_in  = i;
     @ (negedge clk);
  end
  wr_en  = 0;
  @ (negedge clk);
  rd_en = 1;
  // We are causing under flow 
  for (i = 0 ; i < 10; i = i + 1) begin
     @ (negedge clk);
  end
  rd_en = 0;
  #100 $finish;
end

//Source: http://www.asic-world.com/verilog/