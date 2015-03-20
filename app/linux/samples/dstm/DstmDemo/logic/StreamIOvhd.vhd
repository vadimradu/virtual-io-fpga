-----------------------------------------------------------------------------
-- Company: Digilent
-- Engineer: Aaron Odell
-- 
-- Create Date:    15:39:04 08/19/2010 
-- Design Name: StreamIO
-- Module Name: StreamIOvhd - Behavioral 
-- Description: Top level design for StreamIO project.
--		Instantiates StmCtrl and Memory modules.
-----------------------------------------------------------------------------
-- Revision History:
--  08/19/2010(AaronO): created
--	 01/30/2012(SamB)  : Changed signal names to conform with General UCFs 
--	 08/03/2012(JoshS) : Signals in UCFs updated, applied changes
-----------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity StreamIOvhd is
    Port ( DstmIFCLK  : in  STD_LOGIC;
           DstmSLCS   : in  STD_LOGIC;
           DstmFLAGA  : in  STD_LOGIC;
           DstmFLAGB  : in  STD_LOGIC;
           DstmSLRD   : out  STD_LOGIC;
           DstmSLWR   : out  STD_LOGIC;
           DstmSLOE   : out  STD_LOGIC;
           DstmPKTEND : out  STD_LOGIC;
           DstmADR    : out  STD_LOGIC_VECTOR (1 downto 0);
           DB         : inout  STD_LOGIC_VECTOR (7 downto 0));
end StreamIOvhd;

architecture Behavioral of StreamIOvhd is

	-- Component definitions
	COMPONENT StmCtrl
	PORT(
		IFCLK : IN std_logic;
		STMEN : IN std_logic;
		FLAGA : IN std_logic;
		FLAGB : IN std_logic;
		DOWNBSY : IN std_logic;
		DOWNACK : IN std_logic;
		UPBSY : IN std_logic;
		UPACK : IN std_logic;
		UPDATA : IN std_logic_vector(7 downto 0);    
		USBDB : INOUT std_logic_vector(7 downto 0);      
		SLRD : OUT std_logic;
		SLWR : OUT std_logic;
		SLOE : OUT std_logic;
		FIFOADR : OUT std_logic_vector(1 downto 0);
		PKTEND : OUT std_logic;
		DOWNWR : OUT std_logic;
		DOWNDATA : OUT std_logic_vector(7 downto 0);
		UPRD : OUT std_logic
		);
	END COMPONENT;

	COMPONENT Memory
	PORT(
		IFCLK : IN std_logic;
		RST : IN std_logic;
		DOWNWR : IN std_logic;
		DOWNDATA : IN std_logic_vector(7 downto 0);
		UPRD : IN std_logic;          
		DOWNBSY : OUT std_logic;
		DOWNACK : OUT std_logic;
		UPBSY : OUT std_logic;
		UPACK : OUT std_logic;
		UPDATA : OUT std_logic_vector(7 downto 0)
		);
	END COMPONENT;
	
	-- Internal connections between StmCtrl and Memory
	signal downbsy : std_logic;
	signal downwr : std_logic;
	signal downack : std_logic;
	signal downdata : std_logic_vector(7 downto 0);
	signal upbsy : std_logic;
	signal uprd : std_logic;
	signal upack : std_logic;
	signal updata : std_logic_vector(7 downto 0);

begin

	-- Component instantiation
	StmCtrlInst: StmCtrl PORT MAP(
		IFCLK => DstmIFCLK,
		STMEN => DstmSLCS,
		FLAGA => DstmFLAGA,
		FLAGB => DstmFLAGB,
		SLRD => DstmSLRD,
		SLWR => DstmSLWR,
		SLOE => DstmSLOE,
		FIFOADR => DstmADR,
		PKTEND => DstmPKTEND,
		USBDB => DB,
		DOWNBSY => downbsy,
		DOWNWR => downwr,
		DOWNACK => downack,
		DOWNDATA => downdata,
		UPBSY => upbsy,
		UPRD => uprd,
		UPACK => upack,
		UPDATA => updata
	);

	MemoryInst: Memory PORT MAP(
		IFCLK => DstmIFCLK,
		RST => DstmSLCS,
		DOWNBSY => downbsy,
		DOWNWR => downwr,
		DOWNACK => downack,
		DOWNDATA => downdata,
		UPBSY => upbsy,
		UPRD => uprd,
		UPACK => upack,
		UPDATA => updata
	);

end Behavioral;

