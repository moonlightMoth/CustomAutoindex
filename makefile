clean:
	rm -rf target/

compile: src/custom_autoindex.c
	mkdir target
	gcc -g -o target/custom_autoindex.out src/custom_autoindex.c

test: target/custom_autoindex.out
	./target/custom_autoindex.out test >> target/out.html
	cat target/out.html
