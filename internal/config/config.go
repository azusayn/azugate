package config

import (
	"fmt"
	"os"

	"gopkg.in/yaml.v3"
)

type Config struct {
	Port   int     `yaml:"port"`
	Routes []Route `yaml:"routes"`
}

// ref: https://www.envoyproxy.io/docs/envoy/latest/configuration/http/http_conn_man/route_matching.
// TODO: cluster.
type Route struct {
	Match       Match       `yaml:"match"`
	RouteAction RouteAction `yaml:"route"`
}

type RouteAction struct {
	// route to local file.
	Local bool   `yaml:"local"`
	To    string `yaml:"to"`
}

type Match struct {
	Prefix *string `yaml:"prefix,omitempty"`
	Path   *string `yaml:"path,omitempty"`
}

func Load(path string) (*Config, error) {
	var cfg Config
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read config file: %w", err)
	}

	if err := yaml.Unmarshal(data, &cfg); err != nil {
		return nil, fmt.Errorf("failed to parse config file: %w", err)
	}

	return &cfg, nil
}
