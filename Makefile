# Makefile
CC        :=gcc
CFLAGS    :=-Wall -Werror -m64 -mabi=ms -ffreestanding -DPRINT
LINKFLAGS :=-Wall -Werror -m64
UNAME_S   :=$(shell uname -s)
ext		  :=.exe

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
obj 	  :=$(patsubst %.c,%.o,$(call rwildcard,.,*.c))
includes  =$(subst ./Common-Code-For-C-Projects/example.o,,$(obj))

remove	  :=del $(subst /,\,$(includes)) *$(ext)

ifeq ($(UNAME_S),Linux)
CFLAGS    :=-Wall -Werror -ffreestanding -DPRINT
ext		  :=.out
remove	  :=rm -f $(obj) *$(ext)
endif

all: example$(ext)

%.o: %.c 
	@ echo !====== COMPILING $<
	$(CC) $(CFLAGS) -c $< -o $@

example$(ext): $(includes)
	@ echo !====== Linking with $^
	$(CC) $(LINKFLAGS) $^ -o example$(ext)

clean:
	@echo "Cleaning files...."
	$(remove)
	@echo "Done."

run:
	./example$(ext) test.json

.PHONY: clean
