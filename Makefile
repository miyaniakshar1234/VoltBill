# VoltBill Makefile
# Lead Architect: Akshar Miyani

CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS ?= -lm

SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/voltbill

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SOURCES))

.PHONY: all clean install run test

TEST_TARGET = $(BINDIR)/test_suite
TEST_SOURCES = tests/test_suite.c $(filter-out $(SRCDIR)/main.c, $(SOURCES))

all: $(TARGET)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) | $(BINDIR)
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o $@ $(LDFLAGS)

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
	@if [ -f man/voltbill.1 ]; then \
		install -d /usr/local/share/man/man1; \
		install -m 644 man/voltbill.1 /usr/local/share/man/man1/voltbill.1; \
		echo "✓ man page installed to /usr/local/share/man/man1/voltbill.1"; \
	fi
	@if [ -f completions/voltbill.bash ] && [ -d /etc/bash_completion.d ]; then \
		install -m 644 completions/voltbill.bash /etc/bash_completion.d/voltbill; \
		echo "✓ bash completion installed to /etc/bash_completion.d/voltbill"; \
	fi
	@echo "✓ voltbill installed to /usr/local/bin/voltbill"

uninstall:
	rm -f /usr/local/bin/voltbill
	rm -f /usr/local/share/man/man1/voltbill.1
	rm -f /etc/bash_completion.d/voltbill
	@echo "✓ voltbill uninstalled from system"

run: $(TARGET)
	./$(TARGET)
