# ——— Toolchain ———
CXX      := g++
CXXFLAGS := -std=c++20 -Wall \
             -I/usr/local/include/mongocxx/v_noabi \
			 -I/usr/local/include/bsoncxx/v_noabi \
             -Isrc/include -Isrc
LDFLAGS := -ldpp -lmongocxx -lbsoncxx -lssl -lcrypto -lz -pthread

# ——— Project Layout ———
SRCDIR   := src
BUILDDIR := build
TARGET   := hildabot

# find all .cc under src/, map them to .o under build/
SRCS  := $(shell find $(SRCDIR) -name '*.cc')
OBJS  := $(patsubst $(SRCDIR)/%.cc,$(BUILDDIR)/%.o,$(SRCS))

# ——— Default target ———
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
