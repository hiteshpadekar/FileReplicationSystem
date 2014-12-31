IDIR =../hdr
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=../lib
LDIR =../lib

LIBS=-lm

_DEPS = fwk_config.h fwk_error.h fwk_list.h fwk_ncm_layer.h fwk_rtos.h fwk_structures.h fwk_ConfigInterface.h fwk_file.h fwk_memory.h fwk_ncm_net.h fwk_statistics.h fwk_timer.h fwk_datatypes.h fwk_hash_table.h fwk_module_entry.h fwk_net.h fwk_string.h fwk_trace.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = fwk_config.o fwk_file.o fwk_list.o fwk_memory.o fwk_ncm_net.o fwk_rtos.o fwk_string.o fwk_trace.o fwk_error.o fwk_hash_table.o fwk_manager.o fwk_ncm_layer.o fwk_net.o fwk_statistics.o fwk_timer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

framework: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

