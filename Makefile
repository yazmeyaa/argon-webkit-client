DIST_DIR ?= client/dist
RESOURCE_XML_IN ?= resources.xml.in
RESOURCE_XML_OUT = argon.resources.xml
GRESOURCE_NAME = client.gresource
APP_HOME ?= $(HOME)/.argon_client
GRESOURCE_PATH = $(APP_HOME)/$(GRESOURCE_NAME)
TARGET = argon_webkit_client
GENERATE_SCRIPT = ./scripts/generate_resources.sh

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -D_POSIX_C_SOURCE=200809L
PKG_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.1 javascriptcoregtk-4.0 gio-2.0)
PKG_LIBS   = $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.1 javascriptcoregtk-4.0 gio-2.0)

SRC = $(wildcard src/*.c)

update-submodule:
	@echo "Updating frontend submodule..."
	git submodule update --init --recursive --remote
	@echo "Frontend submodule updated."

$(APP_HOME):
	@echo "Creating application resource directory: $(APP_HOME)"
	mkdir -p $(APP_HOME)

$(RESOURCE_XML_OUT): update-submodule
	@echo "Generating XML..."
	$(GENERATE_SCRIPT) $(DIST_DIR) $(RESOURCE_XML_IN)
	@echo "XML generated: $(RESOURCE_XML_OUT)"

$(GRESOURCE_PATH): $(RESOURCE_XML_OUT) | $(APP_HOME)
	@echo "Compiling GResource to $(GRESOURCE_PATH)..."
	glib-compile-resources $(RESOURCE_XML_OUT) --target=$@ --sourcedir=$(DIST_DIR)
	@if [ -f $@ ]; then \
		echo "SUCCESS: GResource created at $@"; \
	else \
		echo "ERROR: GResource not created!"; \
		exit 1; \
	fi

$(TARGET): $(SRC)
	@echo "Building target $(TARGET)..."
	$(CC) $(CFLAGS) $(SRC) $(PKG_CFLAGS) $(PKG_LIBS) -o $@
	@echo "Build complete: $@"

all: $(TARGET) $(GRESOURCE_PATH)

embed: $(GRESOURCE_PATH)

clean:
	rm -f $(RESOURCE_XML_OUT)
	rm -f $(TARGET)
	rm -f $(GRESOURCE_PATH)

.PHONY: all embed clean update-submodule
