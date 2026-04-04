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
