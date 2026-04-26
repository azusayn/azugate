package main

import (
	"fmt"
	"log/slog"
	"os"
	"os/signal"
	"path/filepath"
	"syscall"

	"github.com/azusayn/azugate/core/binding"
	"github.com/azusayn/azugate/internal/config"
	"github.com/azusayn/azutils/auth"
)

func main() {
	exePath, err := os.Executable()
	if err != nil {
		panic(err.Error())
	}
	// TODO: cmd flag for config file path.
	cfgPath := filepath.Join(filepath.Dir(exePath), "config.yaml")
	cfg, err := config.Load(cfgPath)
	if err != nil {
		panic(err.Error())
	}

	secret, err := auth.GeneratePrivateKey()
	if err != nil {
		slog.Error(err.Error())
		return
	}

	azugate, err := binding.NewAzugate(cfg, &secret.PublicKey)
	if err != nil {
		panic(err.Error())
	}

	c := make(chan os.Signal, 1)
	signal.Notify(c, syscall.SIGINT, syscall.SIGTERM)

	// TODO: ensure C++ threads are stopped before process exit.
	go azugate.Start()

	fmt.Printf("\n%s received, shutting down azugate...\n", (<-c).String())
}
