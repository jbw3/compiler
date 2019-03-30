NAME = unittests

INCLUDES = -I../compilerapi
SRCDIR = .
DEPDIR = .dep
OBJDIR = ../../obj/$(NAME)
LIBDIR = ../../lib
BINDIR = ../../bin

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$<.Td

LLVM_CONFIG=llvm-config-8

CXX = clang++-8
CXXFLAGS = `$(LLVM_CONFIG) --cxxflags` $(INCLUDES) -std=c++14 -Wall -Wextra $(DEPFLAGS)

LDLIBS = -L$(LIBDIR) -lcompilerapi
LDFLAGS = $(LDLIBS) `$(LLVM_CONFIG) --ldflags --system-libs --libs all`

POSTCOMPILE = @mv -f $(DEPDIR)/$<.Td $(DEPDIR)/$<.d && touch $@

release: CXXFLAGS += -O2
debug: CXXFLAGS += -O0 -g

SRC = $(notdir $(wildcard $(SRCDIR)/*.cpp))
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

TARGET = $(BINDIR)/$(NAME)

.PHONY: release
release: init librelease $(TARGET)

.PHONY: debug
debug: init libdebug $(TARGET)

.PHONY: librelease
librelease:
	cd ../compilerapi; make release

.PHONY: libdebug
libdebug:
	cd ../compilerapi; make debug

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
	cd ../compilerapi; make clean
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.gcno $(OBJDIR)/*.gcda $(TARGET) *.gcov $(DEPDIR)/*.d

# include dependency makefiles
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(SRC)))
