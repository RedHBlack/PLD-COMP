# config.mk contains the paths to antlr4 etc.
# Each student should have a config.mk corresponding to her system.
# Examples are  ubuntu.mk, DI.mk, fedora.mk
# Then config.mk should be in the .gitignore of your project
include config_PR.mk

CC=g++
CCFLAGS=-g -c -std=c++17 -I$(ANTLRINC) -Wno-attributes
LDFLAGS=-g
BUILDDIR=build

default: all
all: ifcc

##########################################
# Générer les fichiers ANTLR d'abord
generated/ifccParser.cpp generated/ifccLexer.cpp generated/ifccVisitor.cpp generated/ifccBaseVisitor.cpp: ifcc.g4
	@mkdir -p generated
	java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Cpp -o generated ifcc.g4

##########################################
# Lister explicitement les objets dont nous avons besoin
ANTLR_OBJECTS=build/ifccBaseVisitor.o build/ifccLexer.o build/ifccVisitor.o build/ifccParser.o
MAIN_OBJECTS=build/main.o build/CodeCheckVisitor.o build/IRVisitor.o build/SymbolsTable.o
IR_OBJECTS=build/CFG.o build/BasicBlock.o
IR_INSTR_OBJECTS=build/BaseIRInstr.o build/IRInstrLoadConst.o build/IRInstrClean.o build/IRInstrMove.o \
                build/IRInstrSet.o build/IRInstrUnaryOp.o build/IRInstrArithmeticOp.o build/IRInstrComp.o

OBJECTS=$(ANTLR_OBJECTS) $(MAIN_OBJECTS) $(IR_OBJECTS) $(IR_INSTR_OBJECTS)

##########################################
# Règle principale
ifcc: generated/ifccParser.cpp $(OBJECTS)
	@mkdir -p build
	$(CC) $(LDFLAGS) $(OBJECTS) $(ANTLRLIB) -o $@

##########################################
# Compile les fichiers dans le répertoire principal
build/%.o: %.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

##########################################
# Compile les fichiers dans le sous-répertoire IR
build/%.o: IR/%.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

##########################################
# Compile les fichiers dans le sous-répertoire IR/Instr
build/%.o: IR/Instr/%.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

##########################################
# Compile les fichiers ANTLR générés
build/ifccBaseVisitor.o: generated/ifccBaseVisitor.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

build/ifccLexer.o: generated/ifccLexer.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

build/ifccVisitor.o: generated/ifccVisitor.cpp | generated/ifccParser.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

build/ifccParser.o: generated/ifccParser.cpp 
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o $@ $<

# automagic dependency management
-include build/*.d
build/%.d:

# Empêcher le nettoyage automatique
.PRECIOUS: generated/ifcc%.cpp

##########################################
# GUI pour l'arbre d'analyse syntaxique
FILE ?= ../testfiles/arithmetic/5_multiple_operations_and_assignation.c

gui:
	@mkdir -p generated build
	java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Java -o generated ifcc.g4
	javac -cp $(ANTLRJAR) -d build generated/*.java
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# Nettoyage
clean:
	rm -rf build generated
	rm -f ifcc