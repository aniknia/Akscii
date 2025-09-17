akscii: src/main.c src/log.c src/decode.c
	@echo "Compiling Akscii"
	gcc -Iinclude src/main.c src/decode.c src/log.c -o akscii
