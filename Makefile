akscii: src/main.c include/log.h src/log.c include/decode.h src/decode.c include/marker.h src/marker.c include/marker_struct.h
	@echo "Compiling Akscii"
	gcc -Iinclude src/main.c src/decode.c src/log.c src/marker.c -o akscii
