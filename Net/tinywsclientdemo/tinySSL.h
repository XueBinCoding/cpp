
#ifndef TINY_SSL_H
#define TINY_SSL_H

#include <stdint.h>
#include <sys/types.h>
//#include "config.h"
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>

struct tiny_ssl_ctx {
	mbedtls_net_context      net;
	mbedtls_ssl_context      ssl;
	mbedtls_ssl_config       cfg;
	mbedtls_ctr_drbg_context drbg;
	mbedtls_entropy_context  etpy;
	mbedtls_x509_crt         x509;
	//bool last_read_ok;
};

#ifdef __cplusplus
extern "C" {
#endif
int tiny_ssl_init(struct tiny_ssl_ctx *ctx, int sock, char *host);
int tiny_ssl_handshake(struct tiny_ssl_ctx *ctx);
void tiny_ssl_free(struct tiny_ssl_ctx *ctx);

int tiny_ssl_read(struct tiny_ssl_ctx *ctx, unsigned char *buf, size_t count);
int tiny_ssl_write(struct tiny_ssl_ctx *ctx, const char *buf, size_t count);
#ifdef __cplusplus
}
#endif

#endif
