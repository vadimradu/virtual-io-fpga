--
--	Package File Template
--
--	Purpose: This package defines supplemental types, subtypes, 
--		 constants, and functions 
--
--   To use any of the example code shown below, uncomment the lines and modify as necessary
--

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.numeric_std.all;
package reg_spec is

	type data_regs_type is record
      data : std_logic_vector(7 downto 0);
   end record data_regs_type;

   -- unconstrained array of regs_data_type
   type data_regs_array is array (integer range <>) of data_regs_type;

end reg_spec;

package body reg_spec is
 
end reg_spec;
