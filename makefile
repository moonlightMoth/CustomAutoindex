clean:
	rm -rf target/

compile: src/custom_autoindex.c
	mkdir target
	gcc -g -o target/custom_autoindex.out src/custom_autoindex.c

install: target/custom_autoindex.out
	cp template/header.html target/header.html
	cp template/footer.html target/footer.html

test: target/custom_autoindex.out
	./target/custom_autoindex.out test
	cat target/tree.html

full_test:
	make -s clean
	make -s compile
	make -s install
	make -s test
