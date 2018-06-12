-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Simon Stupinsky (xstupi00@stud.fit.vutbr.cz)

library ieee; 
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hour signal
   RESET : in std_logic;  -- asynchronous processor reset
   EN    : in std_logic;  -- enable processor operations
 
   -- synchronous memory ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adress to the memory
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] when CODE_EN='1'
   CODE_EN   : out std_logic;                     -- enable activities
   
   -- synchronous memory RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adress to the memory
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA when DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] when DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- read from the memory (DATA_RDWR='0') / write to the memory (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- enable activities
   
   -- input port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA it contains pressed keyboard characters if IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- valid data when IN_VLD='1'
   IN_REQ    : out std_logic;                     -- request to enter data from the keyboard
   
   -- output port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- written data
   OUT_BUSY : in std_logic;                       -- when OUT_BUSY='1', LCD is busy, can not be written, OUT_WE have to be'0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA when OUT_WE='1' and OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	-- PC register
	signal prg_reg : std_logic_vector(11 downto 0);	-- capacity memory of program is 4kB (2^12)
	signal prg_inc : std_logic;						-- increment program counter (prg_reg++)
	signal prg_dec : std_logic;						-- decrement program counter (prg_reg--)

	-- CNT register
	signal cnt_reg : std_logic_vector(7 downto 0);  -- capacity (max count) instructions in block while (2^12 / 8) / 2 -> right count left and right brackets
	signal cnt_inc : std_logic;						-- increment counter (for while and count of brackets) (cnt_reg++)
	signal cnt_dec : std_logic;						-- decrement counter (for while and count of brackets) (cnt_reg--)
	signal cnt_set : std_logic;						-- sets one to the cnt register in while (cnt_reg = 1)

	-- PTR register
	signal prt_reg : std_logic_vector(9 downto 0);	-- capacity memory of data is 1kB (2^10)
	signal prt_inc : std_logic;						-- increment RAM pointer counter (prt_reg++)
	signal prt_dec : std_logic;						-- decrement RAM pointer counter (prt_reg--)

	signal input_data : std_logic_vector(1 downto 0); -- select data, which will be written to the memory

	type inst_type is (								-- representing relevant instruction in language "BrainLove"
		prt_incr, 	-- increment value of RAM pointer
		prt_decr, 	-- decrement value of RAM pointer
		cell_inc, 	-- increment value of actual cell
		cell_dec, 	-- decrement value of actual cell
		beg_while, 	-- if value of actual cell zero, jump for command ], else continue to next symbol
		end_while,	-- if value of actual cell dont non-zero, jump for commmand [, else continue next symbol
		cell_write, -- print value of actual cell
		cell_load, 	-- load value and store it to the actual cell
		term_while, -- terminates actual running block of while
		term_prog,	-- terminates the program
		nope		-- unkown instruction and whatever other
	);
	signal inst_dec : inst_type;					-- representing decoded instruction in actual run

	type fsm_state is (								-- representing relevant state of finite state machine
		s_idle,			  	-- fsm is inactive
		s_inst_fetch,	  	-- loading instruction (fetch instruction)
		s_inst_decode, 	  	-- decoding relevant instruction
		s_prt_inc, 		  	-- execution increment value of RAM pointer
		s_prt_dec, 		  	-- execution decrement value of RAM pointer
		s_cell_inc_load,  	-- execution increment value of actual cell -> loading phase 
		s_cell_inc_store, 	-- execution increment value of actual cell -> storing phase
		s_cell_dec_load,  	-- execution decrement value of actual cell -> loading phase
		s_cell_dec_store, 	-- execution decrement value of acutal cell -> storing phase
		s_cell_write, 	  	-- execution sets needed signals and check output (have to be available)
		s_cell_write_exec,	-- execution print value of actual cell	
		s_cell_load,	  	-- check accessibility input
		s_cell_load_exec, 	-- execution load value and store it to the actual cell
		s_term_prog, 	  	-- execute terminate the program
		s_nope,			  	-- ignore unkown instruction and whatever other
		s_beg_while, 	  	-- initial setting at while
		s_while_cond,	  	-- evaluation condition in while
		s_while_jump,	  	-- check actual count of brackets
		s_while_repeat,   	-- skipping instruction
		s_end_while,	  	-- control at match with end of while
		s_end_cond,		  	-- repeat of the while (check conditon in while)
		s_end_cont,			-- check counts of brackets 		
		s_end_back,		  	-- repeat the while (back to the begin)
		s_end_finish,	  	-- increment or decrement count of brackets depending on the actual instruction
		s_end_last,			-- decision to change program counter
		s_term_while,		-- sets needed counters 
		s_term_enable,		-- permission to the memory
		s_term_wait			-- skkiping instruction in the while
	);
	signal present_state : fsm_state;				-- present state of finite state machine
	signal next_state : fsm_state;					-- next state of finite state machine

begin

	-- PC register
 	prg_cnt: process (RESET, CLK) 					-- program counter (pointer to memory of program)
		begin
			if (RESET = '1') then						-- asynchronous zeroing sign
				prg_reg <= (others => '0');
			elsif (CLK 'event) and (CLK = '1') then 	-- rising edge of the hour signal
				if (prg_inc = '1') then					-- increment program counter
					prg_reg <= prg_reg + 1;
				elsif (prg_dec = '1') then				-- decrement program counter
					prg_reg <= prg_reg - 1;		
				end if;
			end if;
			CODE_ADDR <= prg_reg;			
	end process;

	-- CNT REGISTER
	cnt_cnt: process (RESET, CLK)					-- counter (for while and count of brackets)
		begin
			if (RESET = '1') then						-- asynchronous zeroing sign
				cnt_reg <= (others => '0');
			elsif (CLK 'event) and (CLK = '1') then		-- rising edge of the hour signal
				if (cnt_inc = '1') then					-- incerement counter (for while and count of brackets)
					cnt_reg <= cnt_reg + 1;
				elsif (cnt_dec = '1') then				-- decrement counter (for while and count of brackets)
					cnt_reg <= cnt_reg - 1;
				end if;
				if (cnt_set = '1') then
					cnt_reg <= X"01";
				end if;
			end if;
	end process;

	-- PTR REGISTER
	ptr_cnt: process (RESET, CLK)					-- pointer counter (pointer to memory of data)
		begin	
			if (RESET = '1') then						-- asynchronous zeroing sign
				prt_reg <= (others => '0');
			elsif (CLK 'event) and (CLK = '1') then 	-- rising edge of the hour signal
				if (prt_inc = '1') then					-- increment pointer counter
					prt_reg <= prt_reg + 1;
				elsif (prt_dec = '1') then				-- decrement pointer counter
					prt_reg <= prt_reg - 1;
				end if;
				DATA_ADDR <= prt_reg;				
			end if;
	end process;

	-- MULTIPLEXER
	mpx_write: process (input_data)					-- multiplexer for select written data from memory or input
		begin	
			case (input_data) is
				when "00" => DATA_WDATA <= DATA_RDATA + 1;	-- cell in the memory (+1)
				when "01" => DATA_WDATA <= DATA_RDATA - 1;	-- cell in the memory (-1)
				when "10" => DATA_WDATA <= IN_DATA;			-- data from input
				when others => null;
			end case;		
	end process;

	-- DECODER
	decode_inst: process (CODE_DATA)
		begin
			case (CODE_DATA) is
				when X"3E" => inst_dec <= prt_incr; 	-- command '>'
				when X"3C" => inst_dec <= prt_decr;   	-- command '<'
				when X"2B" => inst_dec <= cell_inc;   	-- command '+'
				when X"2D" => inst_dec <= cell_dec;	 	-- command '-'
				when X"5B" => inst_dec <= beg_while; 	-- comamnd '['
				when X"5D" => inst_dec <= end_while; 	-- command ']'
				when X"2E" => inst_dec <= cell_write;	-- command '.'
				when X"2C" => inst_dec <= cell_load; 	-- command ','
				when X"7E" => inst_dec <= term_while;	-- command '~'
				when X"00" => inst_dec <= term_prog; 	-- command 'null'
				when others => inst_dec <= nope; 		-- else command
			end case;
	end process;

	-- present state of FSM
	fsm_present_state : process (RESET, CLK)
		begin
			if (RESET = '1') then	
				present_state <= s_idle;				-- initialization FSM
			elsif (CLK 'event) and (CLK = '1') then
				if (EN = '1') then						-- authorization of work processor
					present_state <= next_state;		-- sets new state of FSM
				end if;
			end if;
	end process;

	-- FSM next state logic, output logic
	next_state_logic: process (present_state, inst_dec, prt_reg, input_data, DATA_RDATA, OUT_BUSY, IN_VLD, IN_DATA) -- input signals
		begin
			-- initializations signals
			DATA_EN   <= '0';
			CODE_EN   <= '0';
			IN_REQ    <= '0';
			OUT_WE    <= '0';
			DATA_RDWR <= '0';
			
			prg_inc  <= '0';
			prt_inc  <= '0';
			cnt_inc  <= '0';
			prg_dec  <= '0';			
			prt_dec  <= '0';			
			cnt_dec  <= '0';
			cnt_set  <= '0';

			-- initializations input to the MPX
			input_data <= "10";

			case (present_state) is
				-- IDLE
				when s_idle =>
					next_state <= s_inst_fetch;		-- initilization finite state machine

				-- INSTRUCTION FETCH
				when s_inst_fetch =>
					next_state <= s_inst_decode;	-- decode actual instruction
					CODE_EN    <= '1';			    -- permission of activity of memory	ROM
					
				-- INSTRUCTION DECODE
				when s_inst_decode =>
					case (inst_dec) is
						when prt_incr   => next_state <= s_prt_inc; 
						when prt_decr   => next_state <= s_prt_dec;
						when cell_inc   => next_state <= s_cell_inc_load;
						when cell_dec   => next_state <= s_cell_dec_load;
						when beg_while  => next_state <= s_beg_while;
						when end_while  => next_state <= s_end_while;
						when cell_write => next_state <= s_cell_write;
						when cell_load  => next_state <= s_cell_load;
						when term_while => next_state <= s_term_while;
						when term_prog  => next_state <= s_term_prog;
						when nope       => next_state <= s_nope;
					end case;

				-- INCREMENT RAM POINTER
				when s_prt_inc =>
					next_state <= s_inst_fetch; 	 -- loading next instruction
					prt_inc    <= '1';				 -- increment RAM pointer (prt_reg++)
					prg_inc    <= '1';				 -- increment program counter (prg_reg++)

				-- DECREMENT RAM POINTER
				when s_prt_dec =>
					next_state <= s_inst_fetch;		 -- loading next instruction
					prt_dec    <= '1';				 -- decrement RAM pointer (prt_reg--)
					prg_inc    <= '1';				 -- increment program counter (prg_reg++)

				-- INCREMENT VALUE OF CELL (loading phase)
				when s_cell_inc_load =>
					next_state <= s_cell_inc_store;  -- storing incremented value
					DATA_RDWR  <= '0';				 -- permission of reading from memory
					DATA_EN    <= '1';				 -- permission of activity of memory
				
				-- INCREMENT VALUE OF CELL (storing phase)
				when s_cell_inc_store =>
					next_state <= s_inst_fetch;		 -- loading next instruction
					DATA_RDWR  <= '1';				 -- permission of writing to the memory
					DATA_EN    <= '1';				 -- permission of activity of memory
					input_data <= "00";				 -- select right cell in the memory											
					prg_inc    <= '1';				 -- increment program counter (prg_reg++)

				-- DECREMENT VALUE OF CELL (loading phase)
				when s_cell_dec_load =>
					next_state <= s_cell_dec_store;	 -- storing decremented value
					DATA_RDWR  <= '0';				 -- permission of reading from memory
					DATA_EN    <= '1';				 -- permission of activity of memory
				
				-- DECREMENT VALUE OF CELL (storing phase)
				when s_cell_dec_store =>
					next_state <= s_inst_fetch;		 -- loading next instruction
					DATA_RDWR  <= '1';				 -- permission of writing to the memory
					DATA_EN    <= '1';				 -- permission of activity of memory
					input_data <= "01";				 -- select right cell in the memory										
					prg_inc    <= '1';				 -- increment program counter (prg_reg++)

				-- PRINT CHAR (check accessibility of output)
				when s_cell_write =>
					if (OUT_BUSY = '1') then		 -- check accessibility LCD display		
						next_state <= s_cell_write;  -- waiting until LCD display will be ready
					else 
						next_state <= s_cell_write_exec; -- print value
						DATA_RDWR  <= '0';			 -- permission of reading from the memory
						DATA_EN    <= '1';			 -- permission of activity of memory
					end if;

				-- PRINT CHAR (print value of actual cell)
				when s_cell_write_exec =>
					next_state <= s_inst_fetch;		 -- loading next instruction
					OUT_DATA   <= DATA_RDATA;		 -- value of actual cell on the output
					OUT_WE     <= '1';				 -- permission to write
					prg_inc    <= '1';				 -- increment program counter (prg_reg++)

				-- LOAD CHAR (check accessibility input)
				when s_cell_load =>
					IN_REQ <= '1'; 					 -- sets read request
					if (IN_VLD = '0') then			 -- check accessibility input (input is valid)
						next_state <= s_cell_load;	 -- waiting until input will be valid
					else
						next_state <= s_cell_load_exec; -- load and store value
					end if;

				-- LOAD CHAR (load value and store it to the actual value
				when s_cell_load_exec =>
						next_state <= s_inst_fetch;	 -- loading next instruction
						input_data <= "10";			 -- select loading from the input
						DATA_RDWR  <= '1';			 -- permission writing to the the memory
						DATA_EN    <= '1';			 -- permission of activity of memmory
						prg_inc    <= '1';			 -- increment program counter (prg_reg++)

				-- START WHILE 
				-- BEGIN WHILE (initital setting at while)
				when s_beg_while =>
					next_state <= s_while_cond;	     -- check value in the actual cell
					DATA_RDWR  <= '0';		         -- permission reading to the memory
					DATA_EN    <= '1';		         -- permission of activity of memory
					prg_inc    <= '1';		         -- increment program counter (prg_reg++)

				-- BEGIN WHILE (evaluation the condition in the while)
				when s_while_cond =>
					next_state <= s_inst_fetch;  	 -- execution the instruction in the while				
					if (DATA_RDATA = X"00") then 	 -- the condition in the while is not valid
						next_state <= s_while_jump;  -- instruction in the while will be skipping 
						cnt_set    <= '1';			 -- sets the cnt register to one (cnt_reg = 1) 
					end if;

				-- BEGIN WHILE (check actual count of brackets) 
				when s_while_jump =>
					next_state <= s_inst_fetch;   	  -- loading next instruction				
					if (cnt_reg /= X"00") then	      -- check count of brackets
						next_state <= s_while_repeat; -- check type of instruction
						CODE_EN    <= '1';			  -- permission of activity of memory ROM					
					end if;

				-- BEGIN WHILE (skipping the instruciton in the while)
				when s_while_repeat =>
					next_state  <= s_while_jump;	  -- check potential finish of the while or continue in skipping the instruction
					if (inst_dec = beg_while) then	  -- start of the next while
						cnt_inc <= '1';				  -- increment count of brackets (cnt_reg++)
					elsif (inst_dec = end_while) then -- finish of the some while
						cnt_dec <= '1';			      -- decrement count of brackets (cnt_reg--)
					end if;
					prg_inc     <= '1';				  -- increment program counter (prg_reg++)

				-- END WHILE (match with end of while)
				when s_end_while => 
					next_state <= s_end_cond;		  -- check value in the actual cell
					DATA_RDWR  <= '0';				  -- permission writing to the memory
					DATA_EN    <= '1';				  -- permission of activity of memory

				-- END WHILE (evaluation the condition in the while)
				when s_end_cond =>
					next_state <= s_end_back;	  	  -- shift in the isntruction back to the begin of while				
					if (DATA_RDATA = X"00")	then	  -- check end of while
						next_state <= s_inst_fetch;	  -- loading next instruction
						prg_inc    <= '1';			  -- increment program counter (prg_reg++)
					end if;
				
				-- END WHILE (ensuring back to the begin of while)
				when s_end_back =>
					next_state <= s_end_cont;		  -- check actual counts of brackets
					cnt_set    <= '1'; 				  -- sets the cnt register to one (cnt_reg = 1)
					prg_dec    <= '1';				  -- decrement program counter (prg_reg--)
				
				-- END WHILE (continue or loading instruction)
				when s_end_cont =>
					next_state <= s_inst_fetch;	 	  -- loading next instruction				
					if (cnt_reg /= X"00") then		  -- check counts of brackets
						next_state <= s_end_finish; 
						CODE_EN    <= '1';			  -- permission of activity of memory ROM
					end if;	

				-- END WHILE (check type of instruction)
				when s_end_finish =>
					next_state  <= s_end_last;		   -- check count of while (brackets) and +/- program counter
					if (inst_dec = beg_while) then	   -- begin of the while
						cnt_dec <= '1';				   -- decrement counts of brackets (cnt_reg--)
					elsif (inst_dec = end_while) then  -- end of the while
						cnt_inc <= '1';				   -- increment coutns of brackets (cnt_reg++)
					end if;
				
				-- END WHILE (check actual counts of while (brackets))
				when s_end_last =>
					next_state  <= s_end_cont;		
					if (cnt_reg = X"00") then		   -- check counts of brackets
						prg_inc <= '1';				   -- continue after while
					else
						prg_dec <= '1';				   -- continue in while
					end if;

				-- BREAK (intial sets signals)
				when s_term_while =>
					next_state <= s_term_enable;	   -- sets enable to next actions
					cnt_set    <= '1';				   -- sets the cnt register to one (cnt_reg = 1)	
					prg_inc    <= '1';				   -- incerement program counter (prg_cnt++)

				-- BREAK (permission access)
				when s_term_enable =>
					next_state <= s_term_wait;		   -- skipping instruction in the while
					CODE_EN    <= '1';				   -- permission of activity of memory ROM
						
				-- BREAK (skipping instruction)
				when s_term_wait =>
   					next_state <= s_inst_fetch;	   	   -- outside of while
					if (cnt_reg /= X"00") then		   -- check counts of brackets
						next_state  <= s_term_enable;
						if (inst_dec = beg_while) then -- start of the next while
							cnt_inc <= '1';			   -- increment counts of brackets (cnt_reg++)
						elsif (inst_dec = end_while) then -- end of the while
							cnt_dec <= '1';			   -- decrement counts of brackets (cnt_reg--)
						end if;
						prg_inc     <= '1';			   -- increment program counter (prg_reg++)
					end if;

				-- TERMINATES THE PROGRAM
				when s_term_prog =>
					next_state <= s_term_prog; 		   -- terminates executing the program

				-- IGNORES ANYTHING LIKE INSTRUCTION
				when s_nope =>
					next_state <= s_inst_fetch; 	   -- loading next instruction
					prg_inc    <= '1';				   -- increment program counter (prg_reg++)

				when others => null;
			end case;
	end process;

end behavioral;
