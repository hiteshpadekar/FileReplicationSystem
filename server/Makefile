vpath %c server/main/:server/sync/src/:framework/src:access_nw/src:server/common/src:server/client_serv/src

INC_SYNC	= server/sync/hdr
INC_COMMON	= server/common/hdr
INC_FWK		= framework/hdr
INC_ACCESS_NW   = access_nw/hdr
INC_CLIENT_SERV = server/client_serv/hdr
INCDIRS		= -I${INC_SYNC} -I${INC_COMMON} -I${INC_FWK} -I${INC_ACCESS_NW} -I${INC_CLIENT_SERV}
CFLAGS          = -Wall -Wextra -ggdb -O0 -Wunreachable-code

LIBS = -lpthread
ODIR = obj

_OBJ = server.o sync_master.o sync_slave.o sync_proc.o dir_oper.o client_serv.o fwk_list.o fwk_queue.o fwk_string.o fwk_memory.o fwk_ncm_layer.o fwk_ncm_net.o access_nw.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS) ${INCDIRS}

#main : fwk_list.c fwk_queue.c fwk_string.c fwk_memory.c fwk_ncm_layer.c fwk_ncm_net.c server.c server_sync.c access_nw.c
minidb : $(OBJ)
	gcc -o $@ $^ ${INCDIRS} $(LIBS) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
	rm minidb
