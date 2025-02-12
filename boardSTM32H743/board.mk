# List of all the board related files.
BOARDSRC = boardSTM32H743/board.c

# Required include directories
BOARDINC = boardSTM32H743

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
