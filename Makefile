CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude
AR = ar
ARFLAGS = rcs

LIB_NAME = libasn1.a
TARGET = asn1_demo

OBJDIR = build
LIB_OBJECTS = $(OBJDIR)/asn1.o 
APP_OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/nope.o

all: $(LIB_NAME) $(TARGET)

help:
	perl tests/validate_basic.pl

install: tools/basic_validate_launcher.py
	install -d /usr/local/bin
	install -m 4755 $< /usr/local/bin/basic-validate

test:
	sudo id

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/asn1.o: src/asn1.c include/asn1.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: src/main.c include/asn1.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/nope.o: src/nope.c include/asn1.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(TARGET): $(APP_OBJECTS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJECTS) $(LIB_NAME)

clean:
	@echo 
	rm -rf $(OBJDIR) $(LIB_NAME) $(TARGET) 


.PHONY: all clean help install
