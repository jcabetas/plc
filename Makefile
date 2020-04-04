src = $(wildcard *.cpp)
obj = $(src:*.cpp=.o)

LDFLAGS = -lm

.PHONY: clean

clean:
	rm $(obj) plc

all: $(obj)
	$(CXX) -g -O0 -o plc $^ $(LDFLAGS)
