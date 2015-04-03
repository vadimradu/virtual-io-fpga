--------------------------------------------------------------------------------
-- SPI module adapter for implementing with virtual-io
-- SPI configuration register cfg_reg description
-- bit		|7|6|		5	4	3	|	2		|1				|0					|
-- function	| | |speed_setting|enable	|clk_phase	|clk_polarity	|
-- Speed setting is acheived by setting the 3bits in the corresponding format
--	|	000	|	001	|	010	|	011	|	100	|	101	|	110	|	111	|
--	|			|			|			|			|			|			|			|			|
--	Note: the speed settings are calculated for values matching a 50MHz
--		quartz connected to the fpga.
--
-- Author: Vadim Radu
-- Date: apr/2015
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;

ENTITY spi_interface IS
  PORT(
    clock   : IN     STD_LOGIC;                             --system clock
    tx_data : IN     STD_LOGIC_VECTOR(7 DOWNTO 0);          --data to transmit
    rx_data : OUT    STD_LOGIC_VECTOR(7 DOWNTO 0); 			--data received
    cfg_reg : IN     STD_LOGIC_VECTOR(7 downto 0);
    miso    : IN     STD_LOGIC;                             --master in, slave out
    sclk    : INOUT  STD_LOGIC;                             --spi clock
    ss_n    : INOUT  STD_LOGIC;                             --slave select
    mosi    : OUT    STD_LOGIC);                             --master out, slave in
END spi_interface;

ARCHITECTURE BEHAVIORAL of spi_interface is

--signals needed
  signal enable_spi   : std_logic;
  signal reset_n_spi  : std_logic;
  signal busy_spi     : std_logic; --should be linked to a global reg for status flags.
  signal cpol_spi     : std_logic;
  signal cpha_spi     : std_logic;
  signal cont_spi     : std_logic;
  signal clk_div      : integer;
  signal addr         : integer;
  
--components needed
  component spi_master is
    GENERIC(
      d_width : INTEGER := 8); --data bus width
    PORT(
      clock   : IN     STD_LOGIC;                             --system clock
      enable  : IN     STD_LOGIC;                             --initiate transaction
      cpol    : IN     STD_LOGIC;                             --spi clock polarity
      cpha    : IN     STD_LOGIC;                             --spi clock phase
      cont    : IN     STD_LOGIC;                             --continuous mode command
      clk_div : IN     INTEGER;                               --system clock cycles per 1/2 period of sclk
      tx_data : IN     STD_LOGIC_VECTOR(d_width-1 DOWNTO 0);  --data to transmit
      miso    : IN     STD_LOGIC;                             --master in, slave out
      sclk    : INOUT  STD_LOGIC;                             --spi clock
      ss_n    : INOUT  STD_LOGIC;                             --slave select
      mosi    : OUT    STD_LOGIC;                             --master out, slave in
      busy    : OUT    STD_LOGIC;                             --busy / data ready signal
      rx_data : OUT    STD_LOGIC_VECTOR(d_width-1 DOWNTO 0)); --data received
  END component;
  
  begin
    --link spi_config with proper register
    cpol_spi <= cfg_reg(0);
    cpha_spi <= cfg_reg(1);
    enable_spi <= cfg_reg(2);
    
    process (cfg_reg) is
    begin
      case cfg_reg(5 downto 3) is
        when "000"		=> clk_div	<= 25000;
        when "001"		=> clk_div	<= 2500;
        when "010"		=> clk_div	<= 500;
        when "011"		=> clk_div	<= 250;
        when "100"		=> clk_div	<= 50;
        when "101"		=> clk_div	<= 25;
        when "110"		=> clk_div	<= 12;
        when "111"		=> clk_div	<= 6;
        when others		=> clk_div	<= 0;
      end case;
    end process;
    --instantiation, port map and others
    spi_instance: spi_master port map (clock, enable_spi, cpol_spi, cpha_spi, cont_spi, clk_div,
                                      tx_data, miso, sclk, ss_n, mosi, busy_spi, rx_data);
end architecture;
  
  
