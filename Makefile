
# ---------------------------------------------------------------------
#  make all      : para compilar.
#  make execute  : para compilar e executar.
#  make clean    : para limpar
# ---------------------------------------------------------------------

# Executavel
CPP_EX = App

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = objects
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

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

CCC := g++ -O0

# ---------------------------------------------------------------------
# Flags
# ---------------------------------------------------------------------

# flags de compilacao
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

# flags do linker
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lconcert -lilocplex -lcplex -L$(CONCERTLIBDIR) -lm -lpthread -ldl

all: objFolder modelFolder $(CPP_EX)

#### regra principal, gera o executavel
PMCC: $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CCC) $(CCFLAGS) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CCC) -c $(CCFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CCC) $(CCFLAGS) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CCC) $(CCFLAGS) -std=c++0x  -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp 
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

teste: PMCC
	./PMCC ./data/instancia_teste_mini


clean :

	@ rm -rf ./objects/*.o ./objects/*.d
	@ rm -rf ./model/*.lp
	/bin/rm -rf $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp *.o *.d
