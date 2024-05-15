build:
	mkdir -p ./bin/
	cc -o ./bin/app.out -std=gnu99 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 src/*.c

win:
	mkdir -p ./bin/
	x86_64-w64-mingw32-gcc -o ./bin/winapp.exe -lraylib src/*.c

test: build
	./bin/app.out
