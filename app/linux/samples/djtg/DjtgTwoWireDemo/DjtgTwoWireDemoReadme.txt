Module Description:
    The DjtgTwoWireDemo demonstrates how to read the IDCODEs of one or more
    IEEE 1149.1-2011 targets that are connected in series to an IEEE
    1149.7-2009 ADTAPC that supports the following scan formats: JScan0 -
    JScan3, MSCan, OScan0, and OScan1. The application functions as
    follows:
        1. Reset the tap controller.
        
        2. Read IDCODE's in four wire mode using the default configuration
           of the DJTG port (JScan0, compatible with JScan0-JScan3
           targets). The received IDCODE's are output to stdout.
           
        3. Configure the target and DJTG port with the RDY bit count. This
           bit count can be specified using the "-rdyc" option. If no ready
           bit count is specified then a default bit count of 1 is used.
           
        4. Configure the target and DJTG port with the DLY bit count. This
           bit count can be specified using the "-dlyc" option. If no delay
           bit count is specified then a default bit count of 0 is used.
           
        5. Configure the target and DJTG port with the specified scan
           format. The scan format can be specified using the "-sfmt"
           option. If no scan format is specified then a default scan
           format of OScan1 is used.
           
        6. Read IDCODE's in two-wire mode and output any received IDCODE's
           to stdout.


Required Hardware:
    A Digilent device that supports the MScan, OScan0, and OScan1 formats
    is required. At the time of writing, the JTAG-HS2 and JTAG-SMT2 were
    the only Digilent device's supporting all 3 of these formats.

    A device that exposes an IEEE 1149.7-2009 ADTAPC that supports the
    MScan, OScan0, and OScan1 scan formats and is connected to one or more
    IEEE 1149.1-2001 targets is required. This application was tested
    against a Synopsys DesignWare ARCompact core with JTAG 1149.7 support
    running on a XUPV5-LX110T (ML509) board.


Supported Command Line Options:
    -d           Specify the device user name or alias.
    
    -dlyc        Specify the delay bit count used for advanced scan
                 formats.
                 
    -f           Specify the device TCK frequency in Hz.
    
    -rdyc        Specify the count of executed RDY='1' bits (1-4).
    
    -sfmt        Specify the JTAG scan format used. Accepted scan formats
                 are "MScan", "OScan0", and "OScan1".
                 
    -?, -help    Display usage, supported arguments, and options.


Example Usage:
    Executing "DjtgTwoWireDemo -d JtagHs2" will result in the scan format
    being set to OScan1. The TCK frequency, RDY bit count, and count will
    not be set.
       
    Executing "DjtgTwoWireDemo -d JtagHs2 -sfmt MScan -rdyc 3 -dlyc 400"
    will result in the scan format being set to MScan, the RDY='1' bit
    count to 3, and the delay bit count to 400.

    Executing will "DjtgTwoWireDemo -d JtagHs2 -sfmt OScan0 -rdyc 3" result
    in the scan format being set to OScan0 and the RDY='1' bit count to 3.

    Executing "DjtgTwoWireDemo -d JtagHs2 -sfmt OScan1 -f 6000000" will
    result in the scan format being set to OScan1 and the TCK frequency to
    6 MHz.

