--------------------------------------------------------------------------------
-- SPI module adapter for implementing with virtual-io
--------------------------------------------------------------------------------

ENTITY spi_interface IS
  PORT(
    clock   : IN     STD_LOGIC;                             --system clock
    tx_data : IN     STD_LOGIC_VECTOR(7 DOWNTO 0);          --data to transmit
    rx_data : OUT    STD_LOGIC_VECTOR(d_width-1 DOWNTO 0) --data received
    miso    : IN     STD_LOGIC;                             --master in, slave out
    sclk    : INOUT  STD_LOGIC;                             --spi clock
    ss_n    : INOUT  STD_LOGIC;                             --slave select
    mosi    : OUT    STD_LOGIC);                             --master out, slave in
END spi_master;

ARCHITECUTRE BEHAVIORAL oft spi_interface is

--signals needed
  signal enable_spi : std_logic;
  signal reset_n_spi: std_logic;
  signal busy_spi: std_logic;
  signal cpol_spi: std_logic;
  signal cpha_spi: std_logic;
  singal cont_spi: std_logic;
  signal clk_div: integer;
  signal addr: integer;
  
--components needed
  component spi_master is
    GENERIC(
    slaves  : INTEGER := 4;  --number of spi slaves
    d_width : INTEGER := 2); --data bus width
  PORT(
    clock   : IN     STD_LOGIC;                             --system clock
    reset_n : IN     STD_LOGIC;                             --asynchronous reset
    enable  : IN     STD_LOGIC;                             --initiate transaction
    cpol    : IN     STD_LOGIC;                             --spi clock polarity
    cpha    : IN     STD_LOGIC;                             --spi clock phase
    cont    : IN     STD_LOGIC;                             --continuous mode command
    clk_div : IN     INTEGER;                               --system clock cycles per 1/2 period of sclk
    addr    : IN     INTEGER;                               --address of slave
    tx_data : IN     STD_LOGIC_VECTOR(d_width-1 DOWNTO 0);  --data to transmit
    miso    : IN     STD_LOGIC;                             --master in, slave out
    sclk    : BUFFER STD_LOGIC;                             --spi clock
    ss_n    : BUFFER STD_LOGIC_VECTOR(slaves-1 DOWNTO 0);   --slave select
    mosi    : OUT    STD_LOGIC;                             --master out, slave in
    busy    : OUT    STD_LOGIC;                             --busy / data ready signal
    rx_data : OUT    STD_LOGIC_VECTOR(d_width-1 DOWNTO 0)); --data received
END component;
  
  begin
    --instantiation, port map and others
  
  
