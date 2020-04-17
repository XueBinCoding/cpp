#include <errno.h>
#include <stdlib.h>

#include "tinySSL.h"
// "buffer.h"

int tiny_ssl_init(struct tiny_ssl_ctx *tmpctx, int sock, char *host) {
	
	//struct tiny_ssl_ctx *tmpctx = calloc(1, sizeof(struct tiny_ssl_ctx));

	//if (!ctx) {
	//	//uwsc_log_err("calloc failed: %s\n", strerror(errno));
	//	return -1;
	//}
	mbedtls_net_init(&tmpctx->net);
	mbedtls_ssl_init(&tmpctx->ssl);
	mbedtls_ssl_config_init(&tmpctx->cfg);
	mbedtls_ctr_drbg_init(&tmpctx->drbg);
	mbedtls_x509_crt_init(&tmpctx->x509);

	mbedtls_entropy_init(&tmpctx->etpy);
	mbedtls_ctr_drbg_seed(&tmpctx->drbg, mbedtls_entropy_func, &tmpctx->etpy, NULL, 0);

	//mbedtls connect
	char portStrBuff[16];
	mbedtls_ssl_config_defaults(&tmpctx->cfg, MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

	mbedtls_ssl_conf_authmode(&tmpctx->cfg, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_rng(&tmpctx->cfg, mbedtls_ctr_drbg_random, &tmpctx->drbg);
	mbedtls_ssl_set_bio(&tmpctx->ssl, &tmpctx->net,mbedtls_net_send, mbedtls_net_recv, NULL);

	mbedtls_ssl_set_hostname(&tmpctx->ssl, host);
	mbedtls_ssl_conf_renegotiation(&tmpctx->cfg, MBEDTLS_SSL_RENEGOTIATION_ENABLED);
	mbedtls_ssl_setup(&tmpctx->ssl, &tmpctx->cfg);
	tmpctx->net.fd = sock;

	return 0;
}

int tiny_ssl_handshake(struct tiny_ssl_ctx *ctx) {
	//while (mbedtls_ssl_handshake(&ctx->ssl)) != 0)
	//{
	//	if ((callStat != MBEDTLS_ERR_SSL_WANT_READ) && (callStat != MBEDTLS_ERR_SSL_WANT_WRITE)) {
	//		return NULL;
	//	}
	//}
	int ret = mbedtls_ssl_handshake(&ctx->ssl);
	if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
		return 0;
	if (ret == 0)
		return 1;
	return -1;
}

void tiny_ssl_free(struct tiny_ssl_ctx *ctx) {
	mbedtls_ssl_free(&ctx->ssl);
	mbedtls_ssl_config_free(&ctx->cfg);
	free(ctx);
}

int tiny_ssl_read(struct tiny_ssl_ctx *ctx, unsigned char *buf, size_t count) {
	int ret;
	ret = mbedtls_ssl_read(&ctx->ssl, buf, count);
	return ret;
}

int tiny_ssl_write(struct tiny_ssl_ctx *ctx, const char *buf, size_t count) {
	int ret;
	ret = mbedtls_ssl_write(&ctx->ssl, buf, count);
	return ret;
}