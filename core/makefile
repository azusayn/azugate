MAKEFLAGS += --no-print-directory
MAKEFLAGS += --silent

.PHONY: config build run clean all

config:
	@cmake -B build \
	-DCMAKE_MAKE_PROGRAM="/usr/bin/make" \
	-DCMAKE_C_COMPILER="/usr/bin/gcc" \
	-DCMAKE_CXX_COMPILER="/usr/bin/g++" \
	--preset=default .

build:
	@cmake --build build --parallel $(nproc)

run:
	@./build/azugate

clean:
	@rm -rf build/

all: config build