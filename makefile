clean:
	rm -rf target/

build: src/html_printer.h src/dir_lister.h src/custom_autoindex.h src/custom_autoindex.c template/header.html template/footer.html
	make -s clean
	mkdir target
	gcc -O2 -o target/custom_autoindex src/custom_autoindex.c
	make -s install_html_templates

test_compile: test/html_printer_test.c test/dir_lister_test.c test/custom_autoindex_test.c
	mkdir target
	gcc -g -o target/html_printer_test.out test/html_printer_test.c
	gcc -g -o target/dir_lister_test.out test/dir_lister_test.c
	gcc -g -o target/custom_autoindex_test.out test/custom_autoindex_test.c

install_html_templates: template/header.html template/footer.html
	cp template/header.html target/header.html
	cp template/footer.html target/footer.html

dir_lister_test: target/dir_lister_test.out
	./target/dir_lister_test.out test/tree

html_printer_test: target/html_printer_test.out
	make -s install_html_templates
	./target/html_printer_test.out test/tree >> target/tree_one_level.html
	cat target/tree.html
	cat target/tree_one_level.html

custom_autoindex_test: target/custom_autoindex_test.out
	./target/custom_autoindex_test.out test/tree

valgrind_test: target/html_printer_test.out target/dir_lister_test.out target/custom_autoindex_test.out
	echo "---------------------------------------------------------------------------"
	valgrind --leak-check=full --error-exitcode=1 --track-origins=yes --show-leak-kinds=all -s target/dir_lister_test.out test/tree/
	echo "---------------------------------------------------------------------------"
	valgrind --leak-check=full --error-exitcode=1 --track-origins=yes --show-leak-kinds=all -s target/html_printer_test.out test/tree/
	echo "---------------------------------------------------------------------------"
	valgrind --leak-check=full --error-exitcode=1 --track-origins=yes --show-leak-kinds=all -s target/custom_autoindex_test.out test/tree/

full_test:
	make -s clean
	make -s test_compile
	make -s dir_lister_test
	make -s html_printer_test
	make custom_autoindex_test
	make -s valgrind_test

package: target/custom_autoindex target/header.html target/footer.html
	mkdir target/custom_autoindex_root
	cp target/custom_autoindex target/custom_autoindex_root/custom_autoindex
	cp target/header.html target/custom_autoindex_root/header.html
	cp target/footer.html target/custom_autoindex_root/footer.html
	tar -cvzf "target/custom_autoindex_$(shell date +build_%y%m%d%H%m%S).tar.gz" target/custom_autoindex_root
