# ========================
# config.mk contains paths to ANTLR etc.
include config_PR.mk

CC = g++
CCFLAGS = -g -c -std=c++17 -I$(ANTLRINC) -Wno-attributes
LDFLAGS = -g
BUILDDIR = /home/$(USER)/work/PLD-COMP/PLD-COMP/compiler/build
SOURCEDIR = /home/$(USER)/work/PLD-COMP/PLD-COMP/compiler/src

# === Colors ===
GREEN := \033[1;32m
BLUE  := \033[1;34m
RED   := \033[1;31m
NC    := \033[0m

ANTLR_CPP_FILES = $(SOURCEDIR)/generated/ifccParser.cpp $(SOURCEDIR)/generated/ifccLexer.cpp $(SOURCEDIR)/generated/ifccVisitor.cpp $(SOURCEDIR)/generated/ifccBaseVisitor.cpp

default: all
all: ifcc

# ========================
# Generate ANTLR files
$(ANTLR_CPP_FILES): $(SOURCEDIR)/ifcc.g4
	@printf "$(GREEN)Generating ANTLR files...$(NC)\n"
	@cd $(SOURCEDIR) && mkdir -p generated && java -jar $(ANTLRJAR) -visitor -no-listener -Dlanguage=Cpp -o generated ifcc.g4

# ========================
# Automatically find all .cpp files under src/
CPP_FILES := $(shell find $(SOURCEDIR) -name '*.cpp')

# Replace src/abc.cpp -> build/abc.o
OBJECTS := $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o, $(CPP_FILES))

# Add generated ANTLR object files
ANTLR_OBJECTS := $(BUILDDIR)/generated/ifccBaseVisitor.o $(BUILDDIR)/generated/ifccLexer.o $(BUILDDIR)/generated/ifccVisitor.o $(BUILDDIR)/generated/ifccParser.o
OBJECTS += $(ANTLR_OBJECTS)

# ========================
# Final executable build
ifcc: $(ANTLR_CPP_FILES) $(OBJECTS)
	@printf "$(GREEN)\nLinking executable $@...$(NC)\n"
	@$(CC) $(LDFLAGS) $(OBJECTS) $(ANTLRLIB) -o $@
	@printf "$(GREEN)Executable $@ built successfully!$(NC)\n"
# ========================
# Build rule for any .cpp file found under src/ except generated
$(BUILDDIR)/$(notdir %.o): $(SOURCEDIR)/%.cpp $(ANTLR_CPP_FILES)
	@printf "$(GREEN)Compilation de $< -> $@$(NC)\n";
	@mkdir -p $(dir $@);
	@$(CC) $(CCFLAGS) -MMD -o $@ $<;

# ========================
# Clean everything
clean:
	@printf "$(RED)\nCleaning...$(NC)\n"
	@rm -rf build $(SOURCEDIR)/generated

# ========================
# Dependency management
-include $(BUILDDIR)/*.d
.PRECIOUS: $(SOURCEDIR)/generated/ifcc%.cpp
