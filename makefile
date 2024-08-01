clean:
	rm -rf target/


compile: test/html_printer_test.c test/dir_lister_test.c
	mkdir target
	gcc -O2 target/html_printer_test test/html_printer_test.c

test_compile: test/html_printer_test.c test/dir_lister_test.c
	mkdir target
	gcc -g -o target/html_printer_test.out test/html_printer_test.c
	gcc -g -o target/dir_lister_test.out test/dir_lister_test.c

install_html_templates: target/html_printer_test.out
	cp template/header.html target/header.html
	cp template/footer.html target/footer.html

dir_lister_test:
	make -s clean
	make -s test_compile
	./target/dir_lister_test.out test/tree

html_printer_test:
	make -s clean
	make -s test_compile
	make -s install_html_templates
	./target/html_printer_test.out test/tree
	cat target/tree.html

valgrind_test: target/html_printer_test.out target/dir_lister_test.out
	echo "---------------------------------------------------------------------------"
	valgrind --leak-check=full --error-exitcode=1 --track-origins=yes --show-leak-kinds=all -s target/dir_lister_test.out test/tree/
	echo "---------------------------------------------------------------------------"
	valgrind --leak-check=full --error-exitcode=1 --track-origins=yes --show-leak-kinds=all -s target/html_printer_test.out test/tree/
	echo "---------------------------------------------------------------------------"

full_test:
	make -s dir_lister_test
	make -s html_printer_test
	make -s valgrind_test
