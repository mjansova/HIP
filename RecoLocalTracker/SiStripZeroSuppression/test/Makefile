ObjSuf        = o
SrcSuf        = cc
ExeSuf        = run
LogSuf        = log

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 

CXX           = g++
CXXFLAGS      = -std=c++0x
CXXFLAGS     += -O -Wall -fPIC $(DEFINES) -Wno-unused-result -Wshadow
CXXFLAGS     += $(ROOTCFLAGS) -I./

LD            = g++ 
LDFLAGS       = -g -O -Wall -fPIC -Wl,-undefined,error
LDFLAGS      += $(ROOTLIBS)  -lEG -lTMVA 
#LDFLAGS      += -l:`pwd`/.vectorDictionnary_C.so 
#LDFLAGS      += -l:/opt/sbg/cms/ui6_data1/mjansova/.vectorDictionnary_C.so 

SOFLAGS       = -shared
LIBS          =  

# Uncomment this line if you want to use a script to parse & colorize gcc output
# (You can also export this variable from your bashrc)
#GCCPARSER     = 2>&1 | python ~/littleScripts/colorGcc.py

#------------------------------------------------------------------------------

SOURCES       = $(wildcard *.$(SrcSuf))
OBJECTS       = $(SOURCES:.$(SrcSuf)=.$(ObjSuf))
EXECUTABLES   = $(SOURCES:.$(SrcSuf)=.$(ExeSuf))
LOGS          = $(SOURCES:.$(SrcSuf)=.$(LogSuf))

#------------------------------------------------------------------------------

#TARGET = ttMarketaStudy
TARGET = processTimeTree 

compile: $(TARGET).$(ExeSuf)

run:
	./$(TARGET).$(ExeSuf) | tee $(TARGET).$(LogSuf)

browse:
	export PLOTS_TO_BROWSE=`ls plots/*.root`; root -l .browse

clean:
	@echo "Cleaning..."
	@rm -f *.$(ObjSuf) *.$(ExeSuf) *.$(LogSuf)

#------------------------------------------------------------------------------

%.$(ExeSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LIBS) $(GCCPARSER)
