CC = g++ -g


server.out : main.o webServer.o task.o
	$(CC) -o server.out $^ -lpthread

main.o : main.cpp webServer.h
	$(CC) -o main.o -c main.cpp

# webServer.a : webServer.o threadPool.o
# 	ar cr libwebServer.a $^

webServer.o : webServer.cpp webServer.h threadPool.h
	$(CC) -o webServer.o -c webServer.cpp

task.o : task.cpp task.h
	$(CC) -o task.o -c task.cpp


.PHONY: clean

clean:
	rm *.o