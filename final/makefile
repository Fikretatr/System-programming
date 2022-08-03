all:
	gcc -Wall -o server server.c -lm -lrt -lpthread
	gcc -Wall -o client client.c -lm -lrt -lpthread
	gcc -Wall -o servant servant.c -lm -lrt -lpthread  
	
compile:
	gcc -Wall -o server server.c -lm -lrt -lpthread
	gcc -Wall -o client client.c -lm -lrt -lpthread
	gcc -Wall -o servant servant.c -lm -lrt -lpthread  

clean:
	rm -f *.o server
	rm -f *.o client
	rm -f *.o servant	