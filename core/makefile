MAKEFLAGS += --no-print-directory
MAKEFLAGS += --silent

.PHONY: config build run clean all

config:
	@cmake -B build \
	-G "Unix Makefiles" \
	-DCMAKE_MAKE_PROGRAM="/usr/bin/make" \
	-DCMAKE_C_COMPILER="/usr/bin/gcc" \
	-DCMAKE_CXX_COMPILER="/usr/bin/g++" \
	-DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
	.

# NOTE: changes to libazugate.a will not trigger the Go compiler to relink the static library.
# To force relinking manually, you have two options:
#   a. modify any .go files.
#   b. pass the '-a' flag to force recompilation of all packages (i.e. your source files and
#      dependencies in go.mod, excluding standard Go libraries).
# This target handles it automatically by generating a unique buildID in binding/version.go,
# which causes the Go compiler to detect a source change and relink.
build:
	@if [ "$$(uname)" = "Darwin" ]; then \
		printf 'package binding\n\nconst buildID = "%s"\n' $$(uuidgen | tr '[:upper:]' '[:lower:]') > binding/version.go; \
	else \
		printf 'package binding\n\nconst buildID = "%s"\n' $$(cat /proc/sys/kernel/random/uuid) > binding/version.go; \
	fi
	@cmake --build build --parallel $(nproc)

run:
	@./build/azugate

clean:
	@rm -rf build/

all: config build