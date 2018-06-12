-------------------------------------------------------
-- Course	: Design of Computer Systems
-- Name		: Simon Stupinsky
-- Email	: xtupi00@stud.fit.vutbr.cz 
-- Date		: 05-11-2017
-- Project 	: Controlling matrix display using FPGA
-------------------------------------------------------

-- Libraries 
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- Entity
entity ledc8x8 is
	port (
		-- input signals
		RESET: in std_logic;
		SMCLK: in std_logic;
		-- output signals
		ROW: out std_logic_vector (7 downto 0);
		LED: out std_logic_vector (7 downto 0)
	);
end ledc8x8;

-- Architecture
architecture main of ledc8x8 is

	signal clk_en: std_logic;
	signal counter: std_logic_vector (7 downto 0) := (others => '0');
	signal rows: std_logic_vector (7 downto 0) := (others => '0');

begin

	-- division of the SMCLK/256
	clk_en_div: process(RESET, SMCLK)
	begin
		if RESET = '1' then
			counter <= (others => '0');
		elsif (SMCLK'event) and (SMCLK = '1') then
			counter <= counter + 1;
		end if;
	end process clk_en_div;
	clk_en <= '1' when counter = X"FF" else '0';
	
	-- register which switchs rows
	switch_rows: process(RESET, SMCLK, clk_en)
	begin
		if RESET = '1' then
			rows <= X"80";
		elsif (SMCLK'event) and (SMCLK = '1') and (clk_en = '1') then
			rows <= rows(0) & rows(7 downto 1);
		end if;			
	end process switch_rows;

	ROW <= rows;

	-- views letters on the display
	decoder_of_letter: process(rows)		
	begin	
		case (rows) is
			when "10000000" => LED <= "10001111";
			when "01000000" => LED <= "01111111";
			when "00100000" => LED <= "10011111";
			when "00010000" => LED <= "11101000";
			when "00001000" => LED <= "00010111";
			when "00000100" => LED <= "11111001";
			when "00000010" => LED <= "10111110";
			when "00000001" => LED <= "01010001";
			when others => LED <= "11111111";
		end case;
	end process decoder_of_letter;	

end main;
