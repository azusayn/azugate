package binding

//#include "azugate.h"
import "C"
import "crypto/rsa"

type Azugate struct{}

func NewAzugate(_ *rsa.PublicKey) *Azugate {
	return &Azugate{}
}

func (a *Azugate) Start() {
	C.azugate_start()
}
