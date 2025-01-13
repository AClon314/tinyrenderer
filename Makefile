SYSCONF_LINK = g++
CPPFLAGS = -O3
LDFLAGS = -O3
LIBS = -lm

DESTDIR = ./build/
TARGET  = main

$(shell mkdir -p $(DESTDIR))

OBJECTS := $(patsubst %.cpp,$(DESTDIR)%.o,$(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(DESTDIR)%.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(DESTDIR)$(TARGET)
	-rm -f *.tga