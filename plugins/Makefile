GCCFLAGS=-Wall -g
ROOTFLAGS=`root-config --cflags --libs`


TOOLSFLAGS=$(GCCFLAGS) -I$(ROOTFLAGS) -c 
SELECTORFLAGS=$(GCCFLAGS) $(ROOTFLAGS) -lProof
FIGURESFLAGS=$(GCCFLAGS) $(ROOTFLAGS) 
FITFLAGS=$(GCCFLAGS) $(ROOTFLAGS) -lRooFit -lRooFitCore -lMinuit

all: sel fit
	@echo "Full build successful."


tools.o: tools.cc tools.h 
	g++ $(TOOLSFLAGS) $<

sel: SingleBsToPhiMuMuSelector.cc
	g++ $(SELECTORFLAGS) $< -o $@


fit: SingleBsToPhiMuMuFitters.cc tools.o
	g++ $(FITFLAGS) $^ -o $@	


clean:
	rm -rf  tools.o sel fit  

