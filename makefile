CC = g++ -g


server.out : server.o webServer.o task.o
	$(CC) -o server.out $^ -lpthread

server.o : server.cpp webServer.h
	$(CC) -o server.o -c server.cpp

# webServer.a : webServer.o threadPool.o
# 	ar cr libwebServer.a $^

webServer.o : webServer.cpp webServer.h threadPool.h
	$(CC) -o webServer.o -c webServer.cpp

task.o : task.cpp task.h
	$(CC) -o task.o -c task.cpp


.PHONY: clean

clean:
	rm *.o *.out