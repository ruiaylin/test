all:
	g++ -Wall -O3 -c logmon_reader.cc -I./
	g++ -Wall -O3 -c logmon.cc -I./
	g++ -o logmon logmon_reader.o logmon.o

clean:
	rm *.o
	rm logmon

