akscii: src/main.c include/helper.h src/helper.c include/log.h src/log.c include/decode.h src/decode.c include/marker.h src/marker.c include/marker_struct.h include/quantization.h src/quantization.c include/quantization_struct.h include/huffman.h src/huffman.c include/huffman_struct.h
	@echo "Compiling Akscii"
	gcc -Iinclude src/main.c src/helper.c src/decode.c src/log.c src/marker.c src/quantization.c src/huffman.c -o akscii
