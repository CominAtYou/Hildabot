# ——— Toolchain ———
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Werror -Wpedantic \
             -I/usr/local/include/mongocxx/v_noabi \
			 -I/usr/local/include/bsoncxx/v_noabi \
             -Isrc/include -Isrc
LDFLAGS := -ldpp -lmongocxx -lbsoncxx -lssl -lcrypto -lz -pthread
MAKEFLAGS += -j$(shell echo $(shell nproc) / 2 | bc)

# ——— Project Layout ———
SRCDIR   := src
BUILDDIR := build
TARGET   := hildabot

# find all .cc under src/, map them to .o under build/
SRCS  := $(shell find $(SRCDIR) -name '*.cc')
OBJS  := $(patsubst $(SRCDIR)/%.cc,$(BUILDDIR)/%.o,$(SRCS))

# ——— Default target ———
all: CXXFLAGS += -O3
all: $(TARGET)

# link
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean up
.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# debug builds add the -g flag
.PHONY: debug
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)
