CFLAGS  = -ggdb3 -std=c11
LDFLAGS = -Lintel_rt/lib
LDLIBS  = -liomp5
SOURCE_INTEL = $(shell find src -name "intel.c")
SOURCE_GNU = $(shell find src -name "gnu.c")
SOURCE_TEST = $(shell find test -name "test*.c")

OBJECTS_INTEL = $(SOURCE_INTEL:%.c=%)
OBJECTS_GNU = $(SOURCE_GNU:%.c=%)
OBJECTS_TEST_GCC = $(SOURCE_TEST:%.c=%.gcc)
OBJECTS_TEST_CLANG= $(SOURCE_TEST:%.c=%.clang)

all: $(OBJECTS_INTEL) $(OBJECTS_GNU) $(OBJECTS_TEST_GCC) $(OBJECTS_TEST_CLANG)

$(OBJECTS_INTEL): % : %.c
	gcc $(CFLAGS) -Iintel_rt/include -Iinclude $(LDFLAGS) $(LDLIBS) $< -o $@

$(OBJECTS_GNU): % : %.c
	gcc $(CFLAGS) -Iinclude -fopenmp $< -o $@

$(OBJECTS_TEST_GCC): %.gcc : %.c
	gcc -fopenmp $< -o $@

$(OBJECTS_TEST_CLANG): %.clang : %.c
	clang -fopenmp $< -o $@

clean:
	find src/ -type f -executable -delete
	find test/ -type f -executable -delete
