all:
	make clean
	@mkdir -p bin/
	javac -d bin/ ketama/test1.java
	javac -d bin/ ketama/test2.java
	@cd bin/ && java ketama/test1
	@cd bin/ && java ketama/test2

clean:
	rm -rf ./bin/
