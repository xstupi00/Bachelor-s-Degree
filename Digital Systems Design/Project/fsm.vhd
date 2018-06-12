-- fsm.vhd: Finite State Machine
-- Author: Šimon Stupinský, xstupi00@stud.fit.vutbr.cz

library ieee;
use ieee.std_logic_1164.all;
-- -----------------------------------------------------------------------------
--                        Entity declaration
-- -----------------------------------------------------------------------------
entity fsm is
port(
   CLK          : in  std_logic;
   RESET        : in  std_logic;

   -- Input signals
   KEY          : in  std_logic_vector(15 downto 0);
   CNT_OF       : in  std_logic;

   -- Output signals
   FSM_CNT_CE   : out std_logic;
   FSM_MX_MEM   : out std_logic;
   FSM_MX_LCD   : out std_logic;
   FSM_LCD_WR   : out std_logic;
   FSM_LCD_CLR  : out std_logic
);
end entity fsm;

-- -----------------------------------------------------------------------------
--                      Architecture declaration
-- -----------------------------------------------------------------------------
architecture behavioral of fsm is
    type t_state is (
    FIRST_NUMBER,   SECOND_NUMBER,    NUMBER_A3,      NUMBER_A4, 
    NUMBER_A5,      NUMBER_A6,        NUMBER_A7,      NUMBER_A8, 
    NUMBER_A9,      NUMBER_B3,        NUMBER_B4,      NUMBER_B5, 
    NUMBER_B6,      NUMBER_B7,        NUMBER_B8,      NUMBER_B9, 
    NUMBER_B10,     ENABLE_TEST,      
    WRONG_PASSWORD, ACCESS_ALLOWED,   ACCESS_DENIED,  FINISH
    );
                    
    signal present_state, next_state : t_state;

begin
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
sync_logic : process(RESET, CLK)
begin
    if (RESET = '1') then
        present_state <= FIRST_NUMBER;
    elsif (CLK'event AND CLK = '1') then
        present_state <= next_state;
    end if;
end process sync_logic;

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
next_state_logic : process(present_state, KEY, CNT_OF)
begin
    case (present_state) is
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when FIRST_NUMBER =>
        next_state <= FIRST_NUMBER; 
            if (KEY(9) = '1') then
                next_state <= SECOND_NUMBER;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when SECOND_NUMBER =>
        next_state <= SECOND_NUMBER;
            if (KEY(8) = '1') then
                next_state <= NUMBER_A3;
            elsif (KEY(3) = '1') then
                next_state <= NUMBER_B3;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A3 =>
        next_state <= NUMBER_A3;
            if (KEY(6) = '1') then
                next_state <= NUMBER_A4;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A4 =>
        next_state <= NUMBER_A4;
            if (KEY(2) = '1') then
              next_state <= NUMBER_A5;
            elsif (KEY(15) = '1') then
              next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
              next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A5 =>
        next_state <= NUMBER_A5;
            if (KEY(9) = '1') then
                next_state <= NUMBER_A6;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A6 =>
        next_state <= NUMBER_A6;
            if (KEY(7) = '1') then
                next_state <= NUMBER_A7;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A7 =>
        next_state <= NUMBER_A7;
            if (KEY(1) = '1') then
                next_state <= NUMBER_A8;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A8 =>
        next_state <= NUMBER_A8;
            if (KEY(0) = '1') then
                next_state <= NUMBER_A9;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_A9 =>
        next_state <= NUMBER_A9;
            if (KEY(9) = '1') then
                next_state <= ENABLE_TEST;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if; 
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B3 =>
        next_state <= NUMBER_B3;
            if (KEY(0) = '1') then
                next_state <= NUMBER_B4;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B4 =>
        next_state <= NUMBER_B4;
            if (KEY(1) = '1') then
                next_state <= NUMBER_B5;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B5 =>
        next_state <= NUMBER_B5;
            if (KEY(3) = '1') then
                next_state <= NUMBER_B6;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B6 =>
        next_state <= NUMBER_B6;
            if (KEY(5) = '1') then
                next_state <= NUMBER_B7;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B7 =>
        next_state <= NUMBER_B7;
            if (KEY(9) = '1') then
                next_state <= NUMBER_B8;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when NUMBER_B8 =>
        next_state <= NUMBER_B8;
            if (KEY(9) = '1') then
                next_state <= NUMBER_B9;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    when NUMBER_B9 =>
        next_state <= NUMBER_B9;
            if (KEY(2) = '1') then
                next_state <= NUMBER_B10;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    when NUMBER_B10 =>
        next_state <= NUMBER_B10;
            if (KEY(2) = '1') then
                next_state <= ENABLE_TEST;
            elsif (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -          
  when ENABLE_TEST =>
      next_state <= ENABLE_TEST;
          if (KEY(15) = '1') then
              next_state <= ACCESS_ALLOWED;
          elsif (KEY(14 downto 0) /= "000000000000000") then
              next_state <= WRONG_PASSWORD;
          end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when WRONG_PASSWORD =>
        next_state <= WRONG_PASSWORD;
            if (KEY(15) = '1') then
                next_state <= ACCESS_DENIED;
            elsif (KEY(14 downto 0) /= "000000000000000") then
                next_state <= WRONG_PASSWORD;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when ACCESS_ALLOWED =>
        next_state <= ACCESS_ALLOWED;
            if (CNT_OF = '1') then
                next_state <= FINISH;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when ACCESS_DENIED =>
        next_state <= ACCESS_DENIED;
            if (CNT_OF = '1') then
                next_state <= FINISH;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when FINISH =>
        next_state <= FINISH;
            if (KEY(15) = '1') then
                next_state <= FIRST_NUMBER;
            end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when others =>
        next_state <= FIRST_NUMBER;
    end case;
    
end process next_state_logic;
-------------------------------------------------------------------------------- 
output_logic : process(present_state, KEY)
begin
        FSM_CNT_CE      <= '0';
        FSM_MX_MEM      <= '0';
        FSM_MX_LCD      <= '0';
        FSM_LCD_WR      <= '0';
        FSM_LCD_CLR     <= '0';

    case (present_state) is
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    when ACCESS_DENIED =>
        FSM_MX_MEM      <= '0';
        FSM_CNT_CE      <= '1';
        FSM_MX_LCD      <= '1';
        FSM_LCD_WR      <= '1';
        FSM_LCD_CLR     <= '0';
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    when ACCESS_ALLOWED =>
        FSM_MX_MEM      <= '1';
        FSM_CNT_CE      <= '1';
        FSM_MX_LCD      <= '1';
        FSM_LCD_WR      <= '1';
        FSM_LCD_CLR     <= '0';
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    when FINISH =>
        if (KEY(15) = '1') then
            FSM_LCD_CLR     <= '1';
        end if;
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    when others =>
        if (KEY(14 downto 0) /= "000000000000000") then
            FSM_LCD_WR      <= '1';
        end if;
        if (KEY(15) = '1') then
            FSM_LCD_CLR     <= '1';
        end if;
    end case;
end process output_logic;

end architecture behavioral;