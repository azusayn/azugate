package oauth

type IdentityProvider struct {
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

func NewIdentityProvider() {}
