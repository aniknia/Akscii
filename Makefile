akscii: src/main.c include/log.h src/log.c include/decode.h src/decode.c include/marker.h
	@echo "Compiling Akscii"
	gcc -Iinclude src/main.c src/decode.c src/log.c -o akscii
