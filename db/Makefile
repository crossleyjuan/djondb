dependencies:
	rm -rf temp_dependencies
	mkdir temp_dependencies
	curl -o temp_dependencies/gtest-1.6.0.zip https://googletest.googlecode.com/files/gtest-1.6.0.zip
	unzip -o temp_dependencies/gtest-1.6.0.zip -d temp_dependencies/;	cd temp_dependencies/gtest-1.6.0; \
		./configure;make
	cp temp_dependencies/gtest-1.6.0/lib/.libs/libgtest*.a third_party/libs/
	

