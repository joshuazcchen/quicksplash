# TODO: this  
CFLAGS = -Wall -Iinclude

.PHONY: all test

all: server client
test: clean server test_client

# ik this is incorrect kai it was just so i could test builds lol 
server:
	gcc -Wall -Iinclude src/server/init.c src/server/lobby.c src/common/socket.c src/common/comms.c -o server

test_client:
	gcc -Wall -Iinclude test/client_test.c src/common/socket.c src/common/comms.c -o client

client:
	echo "TODO"

clean:
	rm -f server client
