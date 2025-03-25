# config.mk contains the paths to antlr4 etc.
# Each student should have a config.mk corresponding to her system.
# Examples are  ubuntu.mk, DI.mk, fedora.mk
# Then config.mk should be in the .gitignore of your project
include config_PR.mk

CC=g++
CCFLAGS=-g -c -std=c++17 -I$(ANTLRINC) -Wno-attributes # -Wno-defaulted-function-deleted -Wno-unknown-warning-option
LDFLAGS=-g
BUILDDIR=build

default: all
all: ifcc

##########################################
# link together all pieces of our compiler

SOURCES = $(shell find . -type f -name '*.cpp')
OBJECTS = $(SOURCES:%.cpp=$(BUILDDIR)/%.o)

ifcc: $(OBJECTS)
	@mkdir -p build
	$(CC) $(LDFLAGS) $(addprefix build/, $(notdir $(OBJECTS))) $(ANTLRLIB) -o ifcc


##########################################
# compile our hand-writen C++ code: main(), CodeGenVisitor, etc.
$(BUILDDIR)/%.o: %.cpp generated/ifccParser.cpp main.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o  build/$(@F) $< 

##########################################
# compile all the antlr-generated C++
build/%.o: generated/%.cpp
	@mkdir -p build
	$(CC) $(CCFLAGS) -MMD -o build/$(@F) $<


# automagic dependency management: `gcc -MMD` generates all the .d files for us
-include build/*.d
build/%.d:

##########################################
# generate the C++ implementation of our Lexer/Parser/Visitor
generated/ifccLexer.cpp: generated/ifccParser.cpp
generated/ifccVisitor.cpp: generated/ifccParser.cpp
generated/ifccBaseVisitor.cpp: generated/ifccParser.cpp
generated/ifccParser.cpp: ifcc.g4
	@mkdir -p generated
	java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Cpp -o generated ifcc.g4

# prevent automatic cleanup of "intermediate" files like ifccLexer.cpp etc
.PRECIOUS: generated/ifcc%.cpp   

##########################################
# view the parse tree in a graphical window

# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= ../testfiles/arithmetic/5_multiple_operations_and_assignation.c

gui:
	@mkdir -p generated build
	java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Java -o generated ifcc.g4
	javac -cp $(ANTLRJAR) -d build generated/*.java
	java -cp $(ANTLRJAR):build org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)

##########################################
# delete all machine-generated files
clean:
	rm -rf build generated
	rm -f ifcc
