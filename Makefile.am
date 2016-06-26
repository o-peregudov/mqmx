ACLOCAL_AMFLAGS = -I m4
nodist_doc_DATA = README

SUBDIRS = mqmx test doc

@CODE_COVERAGE_RULES@

clean-local:
	-find . -name "*.gcda" -o -name "*.gcov" -o -name "*.gcno" -delete

EXTRA_DIST = CMakeLists.txt
