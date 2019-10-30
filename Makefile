
# ---------------------------------------------------------------------
#  make all      : para compilar e gerar o executavel.
#  make test     : para testar o executavel.
#  make debug    : para gerar o executavel debugavel.
#  make release  : para gerar versao final do executavel.
#  make clean    : para limpar
# ---------------------------------------------------------------------

###################### CONFIGURACOES ###########################
# Executavel
TARGET   = PMCC

# diretorios com os source files e com os objs files
SRCDIR      = src
BUILD_DIR   = build
MODELDIR    = model
OBJDIR      = $(BUILD_DIR)/objects
APPDIR      = $(BUILD_DIR)/apps
INCLUDE     = -Iinclude/

# lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# Informacoes do sistema e cplex
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

# Diretorios do Cplex e Concert
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio128/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio128/concert

# Compilador
CCC = g++
################################################################

########################## FLAGS ###############################
# flags de compilacao
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

# flags do linker
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lconcert -lilocplex -lcplex -L$(CONCERTLIBDIR)\
 -lm -lpthread -ldl
################################################################

############# REGRAS PARA GERAR O EXECUTAVEL ###################
all: dirs $(APPDIR)/$(TARGET)

dirs:
	@mkdir -p $(MODELDIR)
	@mkdir -p $(APPDIR)
	@mkdir -p $(OBJDIR)


$(APPDIR)/$(TARGET): $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CCC) $(CCFLAGS) $(OBJS) -o $(APPDIR)/$(TARGET) $(CCLNFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CCC) $(CCFLAGS) $(INCLUDE) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CCC) $(CCFLAGS) $(INCLUDE) -std=c++0x  -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp 
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp
################################################################


########################  TESTES ###############################
test: all
	./$(APPDIR)/$(TARGET) ./data/instancia_teste_mini
################################################################


################### REGRA PARA DEBUGAVEL #######################
debug: CCFLAGS += -DDEBUG -g
debug: all
################################################################

#################### REGRA PARA RELEASE ########################
release: CCFLAGS += -O2
release: all
################################################################

########################  LIMPEZA  #############################
clean :
	-@rm -rvf $(OBJDIR)/*
	-@rm -rvf $(APPDIR)/*
	-@rm -rf ./model/*
################################################################
