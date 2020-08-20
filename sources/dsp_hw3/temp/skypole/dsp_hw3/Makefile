SRIPATH?=../srilm-1.5.10

MACHINE_TYPE?=macosx

CXX=g++
CXXFLAGS=-O3 -I$(SRIPATH)/include

vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET=ngram
SRC = ngram.cpp
OBJ = $(SRC:.cpp=.o)

.PHONY: all clean

all: $(TARGET)


$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	perl separator_big5.pl corpus.txt > corpus_seg.txt
	perl separator_big5.pl testdata/1.txt > testdata/1_seg.txt
	perl separator_big5.pl testdata/2.txt > testdata/2_seg.txt
	perl separator_big5.pl testdata/3.txt > testdata/3_seg.txt
	perl separator_big5.pl testdata/4.txt > testdata/4_seg.txt
	perl separator_big5.pl testdata/5.txt > testdata/5_seg.txt
	perl separator_big5.pl testdata/6.txt > testdata/6_seg.txt
	perl separator_big5.pl testdata/7.txt > testdata/7_seg.txt
	perl separator_big5.pl testdata/8.txt > testdata/8_seg.txt
	perl separator_big5.pl testdata/9.txt > testdata/9_seg.txt
	perl separator_big5.pl testdata/10.txt > testdata/10_seg.txt

	$(CXX) $(LDFLAGS) -o $@ $^
run:
	./ngram 
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

map:
	python3 ZhuYinToBig5.py Big5-ZhuYin.map ZhuYin-Big5.map


clean:
	$(RM) $(OBJ) $(TARGET)


