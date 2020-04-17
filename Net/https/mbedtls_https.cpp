typedef struct _MBEDTLS_SESSION {
    mbedtls_net_context      mbedNetCtx;
    mbedtls_ssl_context      mbedSslCtx;
    mbedtls_ssl_config       mbedSslConf;
    mbedtls_ctr_drbg_context mbedDrbgCtx;
    mbedtls_entropy_context  mbedEtpyCtx;
    mbedtls_x509_crt         mbedX509Crt;
}mbedtlsSession_t;

typedef struct SSL_SESSION {
    MbedTLSSession_st    mbedIntf;
} SSLSession_st;

MbedTlsClientInit(MbedTLSSession_st *session, void *entropy, size_t entropyLen)
{
    mbedtls_net_init(&(session->mbedNetCtx));
    mbedtls_ssl_init(&(session->mbedSslCtx));
    mbedtls_ssl_config_init(&(session->mbedSslConf));
    mbedtls_ctr_drbg_init(&(session->mbedDrbgCtx));
    mbedtls_x509_crt_init(&(session->mbedX509Crt));

    mbedtls_entropy_init(&(session->mbedEtpyCtx));
    mbedtls_ctr_drbg_seed(&(session->mbedDrbgCtx), 
                            mbedtls_entropy_func, 
                            &(session->mbedEtpyCtx), 
                            (unsigned char *)entropy, 
                            entropyLen);
}


AMCMbedTlsClientConnect(MbedTLSSession_st *session, const char *serverHost, int serverPort)
{
    int callStat;
    char portStrBuff[16];

    snprintf(portStrBuff, sizeof(portStrBuff), "%d", serverPort);
    callStat = mbedtls_net_connect(&(session->mbedNetCtx), 
                                    serverHost, 
                                    portStrBuff, 
                                    MBEDTLS_NET_PROTO_TCP);
    _RETURN_IF_ERROR(callStat);

    callStat = mbedtls_ssl_config_defaults(&(session->mbedSslConf), 
                                            MBEDTLS_SSL_IS_CLIENT, 
                                            MBEDTLS_SSL_TRANSPORT_STREAM, 
                                            MBEDTLS_SSL_PRESET_DEFAULT);
    _RETURN_IF_ERROR(callStat);

    mbedtls_ssl_conf_authmode(&(session->mbedSslConf), MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&(session->mbedSslConf), &(session->mbedX509Crt), NULL);

    mbedtls_ssl_conf_rng(&(session->mbedSslConf), 
                            mbedtls_ctr_drbg_random, 
                            &(session->mbedDrbgCtx));
    //edtls_ssl_conf_dbg(&(session->mbedSslConf), NULL, NULL);

    mbedtls_ssl_set_bio(&(session->mbedSslCtx), 
                        &(session->mbedNetCtx), 
                        mbedtls_net_send, 
                        mbedtls_net_recv, 
                        mbedtls_net_recv_timeout);

    callStat = mbedtls_ssl_setup(&(session->mbedSslCtx), &(session->mbedSslConf));
    _RETURN_IF_ERROR(callStat);

    while((callStat = mbedtls_ssl_handshake(&(session->mbedSslCtx))) != 0)
    {
        if ((callStat != MBEDTLS_ERR_SSL_WANT_READ)
            && (callStat != MBEDTLS_ERR_SSL_WANT_WRITE))
        {
            return callStat;
        }
    }

    return 0;
}
static mbedtlsSession_t *g_session = NULL;
int main(){

    SSLSession_st session;
    mbedtlsClientInit(&(session.mbedIntf), pers, strlen(pers));
    mbedtls_ssl_write();
}