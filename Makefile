# VoltBill Makefile
# Lead Architect: Akshar Miyani (MCA 1st Sem, MUJ)

CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS ?= -lm

SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/voltbill

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SOURCES))

.PHONY: all clean install run

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "⚡ VoltBill built successfully: $@"

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR) data/*.dat

install: $(TARGET)
	install -d /usr/local/bin
	install -m 755 $(TARGET) /usr/local/bin/voltbill
	@echo "✓ voltbill installed to /usr/local/bin/voltbill"

uninstall:
	rm -f /usr/local/bin/voltbill
	@echo "✓ voltbill uninstalled from /usr/local/bin"

run: $(TARGET)
	./$(TARGET)
