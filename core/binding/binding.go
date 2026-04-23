package binding

/*
#cgo CXXFLAGS: -std=gnu++20 -O3 -march=native -fno-fat-lto-objects -DNDEBUG
#cgo CXXFLAGS: -DBOOST_ATOMIC_NO_LIB -DBOOST_ATOMIC_STATIC_LINK
#cgo CXXFLAGS: -DBOOST_CHRONO_NO_LIB -DBOOST_CHRONO_STATIC_LINK
#cgo CXXFLAGS: -DBOOST_CONTAINER_NO_LIB -DBOOST_CONTAINER_STATIC_LINK
#cgo CXXFLAGS: -DBOOST_CONTEXT_EXPORT="" -DBOOST_CONTEXT_NO_LIB="" -DBOOST_CONTEXT_STATIC_LINK=""
#cgo CXXFLAGS: -DBOOST_COROUTINE_NO_LIB -DBOOST_COROUTINE_STATIC_LINK
#cgo CXXFLAGS: -DBOOST_DATE_TIME_NO_LIB -DBOOST_DATE_TIME_STATIC_LINK
#cgo CXXFLAGS: -DBOOST_THREAD_NO_LIB -DBOOST_THREAD_STATIC_LINK -DBOOST_THREAD_USE_LIB
#cgo CXXFLAGS: -DBOOST_URL_NO_LIB=1 -DBOOST_URL_STATIC_LINK=1
#cgo CXXFLAGS: -DSPDLOG_COMPILED_LIB -DSPDLOG_FMT_EXTERNAL -DYAML_CPP_STATIC_DEFINE
#cgo CXXFLAGS: -I../include -I.
#cgo CXXFLAGS: -I../third_party
#cgo CXXFLAGS: -isystem ../build/vcpkg_installed/x64-linux/include
#cgo LDFLAGS: -L../build/vcpkg_installed/x64-linux/lib -L../build
#cgo LDFLAGS: -lazugate_core -lspdlog -lfmt -lz -lboost_coroutine -lboost_context -lboost_thread -lboost_date_time -lboost_regex
#cgo LDFLAGS: -lboost_atomic -lboost_chrono -lboost_container -lboost_url -lyaml-cpp -lssl -lcrypto -ldl -lstdc++ -lm
#include "azugate.h"
*/
import "C"
import "crypto/rsa"

type Azugate struct{}

func NewAzugate(_ *rsa.PublicKey) *Azugate {
	return &Azugate{}
}
func (a *Azugate) Start() {
	C.azugate_start()
}
