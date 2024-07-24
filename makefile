clean:
	rm -rf target/

test_compile: src/custom_autoindex.c src/dir_lister_test.c
	mkdir target
	gcc -g -o target/custom_autoindex.out src/custom_autoindex.c
	gcc -g -o target/dir_lister_test.out src/dir_lister_test.c

install_html_templates: target/custom_autoindex.out
	cp template/header.html target/header.html
	cp template/footer.html target/footer.html

dir_lister_test:
	make -s clean
	make -s test_compile
	./target/dir_lister_test.out testTree

html_printer_test:
	make -s clean
	make -s test_compile
	make -s install_html_templates
	./target/custom_autoindex.out testTree
	cat target/tree.html
