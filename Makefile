all:
	g++ -I src/include -L src/lib -o main main2.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
