build:
	# ZLIB
	# gcc main.c src/* -o main -I include -DZLIB -lz
	# LIBDEFLATE
	# gcc main.c src/* -o main -I include -DLIBDEFLATE -ldeflate
	# ISA-L with "isal.h" include
	gcc main.c src/* -o main -I include -DISAL -lisal

run: build
	./main ./test_data/in1.png ./test_data/out1.pnm

memcheck:
	valgrind --leak-check=full --leak-resolution=high --track-origins=yes --vgdb=no ./main ./test_data/in0.png ./test_data/out0.pnm
