start: threads.cpp
	gcc -D_REENTRANT threads.cpp -lm -lstdc++ -lpthread -o start

get_array: change_array.cpp
	gcc -D_REENTRANT change_array.cpp -lm -lstdc++ -lpthread -o change_array

server: server.cpp
	gcc -D_REENTRANT server.cpp -lm -lstdc++ -lpthread -o server

clean_start:
	rm start

clean_array:
	rm change_array

clean_server:
	rm server