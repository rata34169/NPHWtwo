all:
	gcc OXserver.c -o OXserver
	gcc player.c -o player
clean:
	rm OXserver
	rm player
