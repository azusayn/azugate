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
	azugate := binding.NewAzugate(&secret.PublicKey)
	azugate.Start()
	for {
		time.Sleep(time.Hour)
	}
}
