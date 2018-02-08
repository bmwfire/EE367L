Name: Brandon Wong
Assignment: EE 367L Lab 3

Instructions to compile:

	Simply type "make all" and this will compile all files ready to be executed.

Instructions to run:

	1. You will need to initially connect to server367.
	2. Type "./server367 &". The server will begin running in the background.
	3. Type "client367 wiliki.eng.hawaii.edu" in order to connect to client367.
	4. Initially execute ls by typing "ls". The client should list all files in directory.
	5. Test command list by typing in "list". This will do the same as "ls".
	6. Type "check". This will prompt you to type in a certain file to check.
	7. Type "display". This will once again prompt you for a certain file to display.
	8. Type "download". This will as well prompt you for a certain file to download. 
	9. Once all necessary commands have been tested, type "quit" to end the client.
		~ for any necessary commands, type "h" to view all possible commands.
	10. Lastly type "fg" then Ctrl-C to kill the server.
		
		- There is an alternative way to killing the server:

			~ type "ps" or "ps -a" then type "kill <server number>" where you type in the server number
				listed for server367.    
