package main

import (
	"log/slog"
	"time"

	"github.com/azusayn/azugate/core/binding"
	"github.com/azusayn/azutils/auth"
)

func main() {
	secret, err := auth.GeneratePrivateKey()
	if err != nil {
		slog.Error(err.Error())
		return
	}
	azugate, err := binding.NewAzugate(8080, &secret.PublicKey)
	if err != nil {
		panic(err.Error())
	}

	azugate.Start()

	for {
		time.Sleep(time.Hour)
	}

}
