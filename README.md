**Tasks to do:**
- Game logic
    - Question/prompt bank.
    - Allow the user to enter an input (temporarily hold it on the user client before the actual socket sendoff to the host server).

- Host client
    - Must be able to take in input from several hosts.
    - Also needs to be able to initialize a server wherein people can join the game.
    - process socket painfully.
    - Server must never block (this is the server).
    - Server must handle client disconnection (remove them from the game).

- Player client
    - Must be able to join a game.
    - Once the game is joined, must be able to play the game.
    - process socket painfully.
    - Client may block.
    - Client must gracefully terminate if host disconnects.
        - Wrap the send into an error check s.t. if it does error, it can gracefully terminate.

FILE STRUCTURE
main folder
- include
    - gamestructs.h (header file for all game structs like the cards, players, game states (voting, prompt, etc).)
    - comms.h (how to decode the communication between the socket and client and vice versa).
    - sockets.h (just put every useful common socket thing into here so that we dont have to recode it, do namespaces exist in C?, even if not we can basically treat it like that).
    - protocol.h (manages the types ig? how to read a packet ig)
    - votes.h (voting substates if necessary and also potential necessary voting?)
- assets
    - prompts.txt
    - legal.md (so we dont get in trouble for using the CC CAH prompts)
- src
    - common (stuff used by both host and client)
        - sockets.c (manage all socket logic here?)
        - cards.c (handle prompts.txt)
        - votes.c (handles voting)
        - comms.c (basic send/receive)
        - protocol.c (manages typecasting of the Packets).
    - server (everything in this folder will be compiled to a runserver executable)
        - init.c (start up the server w/ necessary config)
        - server_comms.c (manages all the server specific send/receive)
        - lobby.c (once init ends, go here and have this screen be listening to clients trying to join, until the host decides theyre full and opens the proper game, thus closing the lobby).
        - game.c (handles the actual game and states)
    - client (everything in this folder will be compiled to a runclient executable)
        - init.c (this will handle the initial join).
        - output.c (handle rendering cool game visual)
        - client_comms.c (manages all the client specific send/receive)
        - input.c (handles user input for voting and prompts)
    - test (test cases in the future)
        - test_client.c (technically this should be a test case but right now its just that I'm using to test around with features to verify they work)
    - readme.md
    - Makefile
