INCDIR = include
SRCDIR = src
OBJDIR = obj

CXX = g++
INCFLAGS = -I$(INCDIR)
CXXFLAGS = -std=c++17 $(INCFLAGS)

_DEPS = $(shell ls $(INCDIR))
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

SRC = $(shell ls $(SRCDIR))
_OBJ = $(SRC:.cc=.o)
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

OUT = ./libedidgen.a

default: $(OUT)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -rf $(OBJDIR) $(OUT)
