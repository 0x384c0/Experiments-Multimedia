all:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	
build:
	gcc test_fonts.c -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lcapstone

test:
	./a.out

clean:
	rm -f a.out