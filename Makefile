CFLAGS  = -m32 -std=c11
LDFLAGS = -Lintel_rt/lib
LDLIBS  = -liomp5
SOURCE_INTEL = $(shell find . -name "*intel*.c")
SOURCE_GNU = $(shell find . -name "*gnu*.c")
OBJECTS_INTEL = $(SOURCE_INTEL:%.c=%)
OBJECTS_GNU = $(SOURCE_GNU:%.c=%)

all: $(OBJECTS_INTEL) $(OBJECTS_GNU)

$(OBJECTS_INTEL): % : %.c
	gcc $(CFLAGS) -Iintel_rt/include -Iinclude $(LDFLAGS) $(LDLIBS) $< -o $@

$(OBJECTS_GNU): % : %.c
	gcc $(CFLAGS) -Iinclude -fopenmp $< -o $@
