MAKEFLAGS += --no-print-directory
MAKEFLAGS += --silent

.PHONY: build clean

build:
	@cd core && make build
	@cd ..
	@go build -asan ./cmd/azugate

go:
	@go build  ./cmd/azugate

clean:
	@rm -f *.exe

