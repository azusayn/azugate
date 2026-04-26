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
	"errors"
	"strings"
	"unsafe"

	"github.com/azusayn/azugate/internal/config"
)

type Azugate struct{}

func NewAzugate(config *config.Config, publicKey *rsa.PublicKey) (*Azugate, error) {
	azugate := &Azugate{}

	pem, err := publicKeyToPEM(publicKey)
	if err != nil {
		return nil, err
	}
	cPEM := C.CString(pem)
	defer C.free(unsafe.Pointer(cPEM))

	bindingConfig := C.BindingServerConfig{
		port:               C.uint16_t(config.Port),
		jwt_public_key_pem: cPEM,
	}
	C.azugate_load_config(bindingConfig)

	for _, route := range config.Routes {
		match := route.Match
		action := route.RouteAction
		if path := match.Path; path != nil {
			azugate.AddPathMatchRoute(*path, action.To, action.Local)
			continue
		}
		if prefix := route.Match.Prefix; prefix != nil {
			azugate.AddPrefixMatchRoute(*prefix, action.To, action.Local)
			continue
		}
		return nil, errors.New("neither prefix nor path was found in the route element")
	}

	return azugate, nil
}

func (a *Azugate) Start() {
	C.azugate_start()
}

func (*Azugate) AddPrefixMatchRoute(sourceURL, targetURL string, isLocal bool) {
	sourceURL = normalizeURL(sourceURL)
	targetURL = normalizeURL(targetURL)
	cSourceURL := C.CString(sourceURL)
	cTargetURL := C.CString(targetURL)
	defer C.free(unsafe.Pointer(cSourceURL))
	defer C.free(unsafe.Pointer(cTargetURL))
	C.azugate_add_prefix_match_route(cSourceURL, cTargetURL, C.int(boolToInt(isLocal)))
}

func (*Azugate) AddPathMatchRoute(sourceURL, targetURL string, isLocal bool) {
	sourceURL = normalizeURL(sourceURL)
	targetURL = normalizeURL(targetURL)
	cSourceURL := C.CString(sourceURL)
	cTargetURL := C.CString(targetURL)
	defer C.free(unsafe.Pointer(cSourceURL))
	defer C.free(unsafe.Pointer(cTargetURL))
	C.azugate_add_path_match_route(cSourceURL, cTargetURL, C.int(boolToInt(isLocal)))
}

func boolToInt(b bool) int {
	if b {
		return 1
	}
	return 0
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

// nomalizeURL normalizes an URL according to the following rules:
//  1. "" is returned directly.
//  2. "/" is converted to "".
//  3. Leading and trailing slashes is not required, but the result
//     will always be formatted as "/a/b/c" (except for Rule 2).
func normalizeURL(url string) string {
	url = strings.Trim(url, "/")
	if url == "" {
		return ""
	}
	return "/" + url
}
