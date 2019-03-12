
IDIR =-Isrc -Itest 
CPP=g++
CFLAGS=$(IDIR) --std=c++1y
SRC=src
TESTDIR=test
ODIR=obj


_OBJ = StateMachine.o ThreadPool.o main.o StateMachineImpl.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

stateMachine : $(OBJ)
	$(CPP) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: $(SRC)/%.cpp 
	@mkdir -p $(ODIR)
	$(CPP) -c -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: $(TESTDIR)/%.cpp 
	@mkdir -p $(ODIR)
	$(CPP) -c -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rmdir $(ODIR)
	rm stateMachine

