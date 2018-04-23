.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TARGET=train test

all: $(TARGET)
	g++ train.cpp -o train
	g++ test.cpp -o test
# type make/make all to compile test_hmm

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file
