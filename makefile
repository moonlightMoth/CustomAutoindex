clean:
	rm -rf target/
test:
	mkdir target
	gcc -g -o target/customAutoindex.out customAutoindex.c
