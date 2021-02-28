-- VHDL Syntax Highlight Testfile

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity VHDL is
    generic(
        b1 : boolean := true;
        b2 : boolean := false
    );
    port (
        reset : in std_logic;
        clock : in std_logic;
        sig_o : out std_logic_vector(15 downto 0)
    );
end entity;

architecture arch of VHDL is

signal sig_vec : std_logic_vector(15 downto 0); -- lower case
SIGNAL SIG_vec : STD_LOGIC_VECTOR(15 DOWNTO 0); -- UPPER CASE
Signal Sig_vec : Std_Logic_Vector(15 Downto 0); -- Snake Case

signal sig_bit : std_logic := '0';

signal char : character := 'G'; -- this is character
signal str : string := "this is strings";

begin

identifier : process(reset, clock)
    variable float : real;
begin
    if reset = '1' then
        -- "'high" and "'low" are attributes
        sig_vec <= (sig'high downto sig'low => '0');
        -- numbers
        sig_bit <= 'X';
        float := 0.0;
    elsif rising_edge(clock) then
        -- numbers
        sig_vec <= X"0123";
        sig_vec <= X"5A5A";
        sig_vec <= x"ZXLH";
        sig_vec <= O"ZXLHwlhu";
        sig_vec <= o"0123_4567";
        sig_vec <= b"0000_zzzz_1111_0000";
        sig_vec <= "0101_0101_0101_0101";
        sig_bit <= '1';
        float := 0.1e-10;
        -- NOT numbers
        sig_vec <= "0101_0101_0101_ABCD";
        sig_bit <= 'A';
        float := 0.1f;
    end if;
end process;

end architecture;


