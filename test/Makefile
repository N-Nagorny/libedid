LIBEDID_INCDIR = ../include
LIBEDID_LIB = ../libedidgen.a

SRCDIR = src
OBJDIR = obj

GTEST_CFLAGS = $(shell pkg-config --cflags gtest)
GTEST_LFLAGS = $(shell pkg-config --libs gtest)

CXX = g++
INCFLAGS = -I$(LIBEDID_INCDIR)
CXXFLAGS = -std=c++17 $(INCFLAGS) $(GTEST_CFLAGS)
LFLAGS = $(GTEST_LFLAGS)

_DEPS = $(shell ls $(LIBEDID_INCDIR))
DEPS = $(patsubst %,$(LIBEDID_INCDIR)/%,$(_DEPS))

SRC = $(shell ls $(SRCDIR))
_OBJ = $(SRC:.cc=.o)
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

OUT = ./test_runner

default: $(OUT)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	mkdir -p $(OBJDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OUT): $(OBJ) $(LIBEDID_LIB)
	$(CXX) -o $(OUT) $(OBJ) $(LIBEDID_LIB) $(GTEST_LFLAGS)

clean:
	rm -rf $(OBJDIR) $(OUT)
