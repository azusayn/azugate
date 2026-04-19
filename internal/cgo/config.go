package cgo

//#include "config_wrapper.h"
import "C"
import "encoding/pem"

type ServerConfig struct {
	Port         uint16
	JWTPublicKey []byte
}

func ConfigServer(config *ServerConfig) {
	block := pem.EncodeToMemory(&pem.Block{
		Type:  "PUBLIC KEY",
		Bytes: config.JWTPublicKey,
	})
	c := C.ServerConfig{
		port:               C.uint16_t(config.Port),
		jwt_public_key_pem: C.CString(string(block)),
	}
	C.ConfigServer(&c)
}
