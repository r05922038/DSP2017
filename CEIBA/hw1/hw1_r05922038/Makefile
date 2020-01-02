.PHONY: all clean

#CFLAGS+=
#LDFLAGS=-lm     # link to math library

TARGET=train test

all: $(TARGET)
# type make/make all to compile test_hmm

train: train.cpp
	g++ -O3 -lm train.cpp -o train

test: test.cpp
	g++ -O3 -lm test.cpp -o test

clean:
	rm $(TARGET)   # type make clean to remove the compiled file
