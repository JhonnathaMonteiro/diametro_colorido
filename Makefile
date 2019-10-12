
# ---------------------------------------------------------------------
#  make all      : para compilar.
#  make execute  : para compilar e executar.
#  make clean    : para limpar
# ---------------------------------------------------------------------

# Executavel
CPP_EX = caminho_colorido

# Codigo fonte .cpp
CPP_SOURCE = $(wildcard ./src/*.cpp)

# Headers
H_SOURCE = $(wildcard ./src/*.h)

# Objetos
OBJ = $(subst .cpp,.o,$(subst src,objects,$(CPP_SOURCE)))

# Informacoes do sistema e cplex
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

# ---------------------------------------------------------------------
# Diretorios do Cplex e Concert
# ---------------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio128/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio128/concert

# ---------------------------------------------------------------------
# Compilador
# ---------------------------------------------------------------------

CCC = g++ -O0

# ---------------------------------------------------------------------
# Opcoes do compilador
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Linkando opcoes e bibliotecas
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread -ldl



all: objFolder modelFolder $(CPP_EX)

execute: all
	./$(CPP_EX)

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

# Flags
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

objFolder:
	@ mkdir -p objects

modelFolder:
	@ mkdir -p model

$(CPP_EX): $(CPP_EX).o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o $@ ./objects/$< $(CCLNFLAGS)

$(CPP_EX).o: ./src/$(CPP_EX).cpp
	$(CCC) -c $(CCFLAGS) $< -o ./objects/$@

clean :

	@ rm -rf ./objects/*.o
	@ rmdir -p objects
	@ rm -rf ./model/*.lp
	@ rmdir -p model
	/bin/rm -rf $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp *.o
