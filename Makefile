akscii: src/main.c include/helper.h src/helper.c include/log.h src/log.c include/decode.h src/decode.c include/marker.h src/marker.c include/marker_struct.h include/huffman_struct.h
	@echo "Compiling Akscii"
	gcc -std=gnu23 -Iinclude src/main.c src/helper.c src/decode.c src/log.c src/marker.c -o akscii
