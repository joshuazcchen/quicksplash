# TODO: this  
CFLAGS = -Wall -Iinclude

.PHONY: all test

all: server client
test: clean server test_client server_test

# ik this is incorrect kai it was just so i could test builds lol
server_test: 
	gcc -Wall -Iinclude test/server_test.c src/server/cards.c src/server/gamehandler.c src/server/gamestates.c -o server_test

server:
	gcc -Wall -Iinclude src/server/init.c src/server/cards.c src/server/gamestates.c src/server/gamehandler.c src/server/server_comms.c src/server/lobby.c src/common/socket.c src/common/comms.c src/common/protocol.c -o server

test_client:
	gcc -Wall -Iinclude test/client_test.c src/common/socket.c src/common/comms.c src/client/output.c -o client_test

client:
	gcc -Wall -Iinclude src/client/init.c src/client/client_comms.c src/client/input.c src/client/output.c src/common/protocol.c src/common/comms.c src/common/socket.c -o client

clean:
	rm -f server client server_test test_client
