package oauth

import (
	"context"
	"crypto/rand"
	"crypto/sha256"
	"encoding/base64"
	"errors"

	"golang.org/x/oauth2"
)

type IdentityProvider struct {
	Config *oauth2.Config
}

// Why redirect_uri in Exchange-Token stage:
/*
 * "As an added measure of security, the server should verify that the redirect
 * URL in this request matches exactly the redirect URL that was included in
 * the initial authorization request for this authorization code. If the redirect
 * URL does not match, the server rejects the request with an error."
 *
 * https://www.oauth.com/oauth2-servers/redirect-uris/redirect-uri-validation/
 */

func NewIdentityProvider(config *oauth2.Config) *IdentityProvider {
	return &IdentityProvider{Config: config}
}

// GetAuthCodeURL returns authURL, state and verifier.
func (p *IdentityProvider) GetAuthCodeURL() (string, string, string) {
	// use state to prevent CSRF attack.
	bytes16 := genRandomBytes(16)
	state := base64.RawURLEncoding.EncodeToString(bytes16)

	// use PKCE to prevent Authorization Code Interception Attack (RFC 7636).
	bytes32 := genRandomBytes(32)
	verifier := base64.RawURLEncoding.EncodeToString(bytes32)

	// challenge = Base64(Sha256(verifier))
	hasher := sha256.New()
	hasher.Write([]byte(verifier))
	challenge := base64.RawURLEncoding.EncodeToString(hasher.Sum(nil))

	authURL := p.Config.AuthCodeURL(
		state,
		oauth2.SetAuthURLParam("code_challenge", challenge),
		oauth2.SetAuthURLParam("code_challenge_method", "S256"),
	)

	return authURL, state, verifier
}

// TODO: azutils...
func genRandomBytes(length int) []byte {
	bytes := make([]byte, length)
	_, _ = rand.Read(bytes)
	return bytes
}

// ExchangeToken retrieves the access token from the authorization response,
// returning it if no error occurs.
func (p *IdentityProvider) ExchangeToken(ctx context.Context, code string) (string, error) {
	token, err := p.Config.Exchange(ctx, code)
	if err != nil {
		return "", err
	}
	accessToken, ok := token.Extra("access_token").(string)
	if !ok {
		return "", errors.New("missing access token from authorization response")
	}
	return accessToken, nil
}
