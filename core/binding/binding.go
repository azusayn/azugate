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
#cgo LDFLAGS: -lboost_atomic -lboost_chrono -lboost_container -lboost_url -lssl -lcrypto -ldl -lstdc++ -lm
#include "azugate.h"
#include <stdlib.h>
*/
import "C"
import (
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"
	"unsafe"
)

type Azugate struct{}

func NewAzugate(port uint16, publicKey *rsa.PublicKey) (*Azugate, error) {
	pem, err := publicKeyToPEM(publicKey)
	if err != nil {
		return nil, err
	}
	cPEM := C.CString(pem)
	defer C.free(unsafe.Pointer(cPEM))

	config := C.BindingServerConfig{
		port:               C.uint16_t(port),
		jwt_public_key_pem: cPEM,
	}
	C.azugate_load_config(config)

	return &Azugate{}, nil
}

func (a *Azugate) Start() {
	C.azugate_start()
}

func publicKeyToPEM(publicKey *rsa.PublicKey) (string, error) {
	bytes, err := x509.MarshalPKIXPublicKey(publicKey)
	if err != nil {
		return "", err
	}
	pemBlock := &pem.Block{
		Type:  "PUBLIC KEY",
		Bytes: bytes,
	}
	return string(pem.EncodeToMemory(pemBlock)), nil
}
