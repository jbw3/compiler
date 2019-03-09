NAME = comp

SRCDIR = .
DEPDIR = .dep
OBJDIR = ../obj/$(NAME)
BINDIR = ../bin
LLVMDIR = ~/opt/llvm/bin

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$<.Td

CXX = clang++
CXXFLAGS = `$(LLVMDIR)/llvm-config --cxxflags` -std=c++1z -Wall -Wextra $(DEPFLAGS)

LDFLAGS = `$(LLVMDIR)/llvm-config --ldflags --system-libs --libs core`

POSTCOMPILE = @mv -f $(DEPDIR)/$<.Td $(DEPDIR)/$<.d && touch $@

release: CXXFLAGS += -O2
debug: CXXFLAGS += -O0 -g

SRC = $(notdir $(wildcard $(SRCDIR)/*.cpp))
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

TARGET = $(BINDIR)/$(NAME)

.PHONY: release
release: init $(TARGET)

.PHONY: debug
debug: init $(TARGET)

.PHONY: init
init:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.cpp.d
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(POSTCOMPILE)

# rule for dependency files (this is here to keep make happy)
$(DEPDIR)/%.d: ;

# prevent make from deleting dependency files
.PRECIOUS: $(patsubst %,$(DEPDIR)/%.d,$(SRC))

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.gcno $(OBJDIR)/*.gcda $(TARGET) *.gcov $(DEPDIR)/*.d

# include dependency makefiles
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(SRC)))
