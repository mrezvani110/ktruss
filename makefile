#DFLAGS=-Wall -g
DFLAGS=-Wall -O3
#LDFLAGS=-Wall -g -c
LDFLAGS=-Wall -O3 -c
OBJS=IntQueue.o Timestamp.o EvaluationResults.o TriangleGraph.o SimpleTriangleGraph.o BipartiteTriangleGraph.o Graph.o main.o

all: run

run: $(OBJS)
	g++ $(DFLAGS) $(OBJS) -o run

%.o: %.cpp %.h
	g++ $(LDFLAGS) -o $@ $< $(CFLAGS)

main.o: main.cpp
	g++ $(LDFLAGS) -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f run
