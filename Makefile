COMPILER_DIR = squelette-pld-comp/compiler

all:
	$(MAKE) -C $(COMPILER_DIR)

test: all
	python3 squelette-pld-comp/ifcc-test.py --verbose squelette-pld-comp/testfiles

testfail: all
	python3 squelette-pld-comp/ifcc-test.py --verbose squelette-pld-comp/testfilesfail

run: all
	$(COMPILER_DIR)/ifcc

clean:
	$(MAKE) -C $(COMPILER_DIR) clean