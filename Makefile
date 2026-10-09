TARGET               := nova3_nx
PORT_CFLAGS += -DRT_OPENSLES=1 -DRT_STDIO_READ_BUF=65536
PORT_NPDM_PROGRAM_ID := 0x0100777777777000

# Include android32 base rules
include runtime/runtime.mk
