NAME = libcompilerapi

SRCDIR = .
DEPDIR = .dep
OBJDIR = ../../obj/$(NAME)
LIBDIR = ../../lib

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$<.Td

LLVM_CONFIG=llvm-config-8

CXX = clang++-8
CXXFLAGS = `$(LLVM_CONFIG) --cxxflags` -std=c++14 -Wall -Wextra $(DEPFLAGS)

LD = ar
LDFLAGS = -rcs

POSTCOMPILE = @mv -f $(DEPDIR)/$<.Td $(DEPDIR)/$<.d && touch $@

release: CXXFLAGS += -O2
debug: CXXFLAGS += -O0 -g

SRC = $(notdir $(wildcard $(SRCDIR)/*.cpp))
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

TARGET = $(LIBDIR)/$(NAME).a

.PHONY: release
release: init $(TARGET)

.PHONY: debug
debug: init $(TARGET)

.PHONY: init
init:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIBDIR)

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $(TARGET) $(OBJ)

$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.cpp.d
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(POSTCOMPILE)

# rule for dependency files (this is here to keep make happy)
$(DEPDIR)/%.d: ;

# prevent make from deleting dependency files
.PRECIOUS: $(patsubst %,$(DEPDIR)/%.d,$(SRC))

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(TARGET) $(DEPDIR)/*.d

# include dependency makefiles
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(SRC)))
