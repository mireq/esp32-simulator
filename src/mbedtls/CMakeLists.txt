set(MBEDTLS_SOURCES
	"aes.c"
	"aesce.c"
	"aesce.h"
	"aesni.c"
	"aesni.h"
	"alignment.h"
	"aria.c"
	"asn1parse.c"
	"asn1write.c"
	"base64.c"
	"bignum.c"
	"bignum_core.c"
	"bignum_core.h"
	"bignum_mod.c"
	"bignum_mod.h"
	"bignum_mod_raw.c"
	"bignum_mod_raw.h"
	"bignum_mod_raw_invasive.h"
	"bn_mul.h"
	"camellia.c"
	"ccm.c"
	"cipher.c"
	"cipher_wrap.c"
	"cipher_wrap.h"
	"cmac.c"
	"CMakeLists.txt"
	"common.h"
	"constant_time.c"
	"constant_time_internal.h"
	"constant_time_invasive.h"
	"ctr_drbg.c"
	"debug.c"
	"des.c"
	"dhm.c"
	"ecdh.c"
	"ecdsa.c"
	"ecjpake.c"
	"ecp.c"
	"ecp_curves.c"
	"ecp_internal_alt.h"
	"ecp_invasive.h"
	"entropy.c"
	"entropy_poll.c"
	"entropy_poll.h"
	"error.c"
	"gcm.c"
	"hash_info.c"
	"hash_info.h"
	"hkdf.c"
	"hmac_drbg.c"
	"chachapoly.c"
	"chacha20.c"
	"check_crypto_config.h"
	"lmots.c"
	"lmots.h"
	"lms.c"
	"Makefile"
	"md.c"
	"md_wrap.h"
	"md5.c"
	"memory_buffer_alloc.c"
	"mps_common.h"
	"mps_error.h"
	"mps_reader.c"
	"mps_reader.h"
	"mps_trace.c"
	"mps_trace.h"
	"net_sockets.c"
	"nist_kw.c"
	"oid.c"
	"padlock.c"
	"padlock.h"
	"pem.c"
	"pk.c"
	"pkcs12.c"
	"pkcs5.c"
	"pkcs7.c"
	"pkparse.c"
	"pk_wrap.c"
	"pk_wrap.h"
	"pkwrite.c"
	"pkwrite.h"
	"platform.c"
	"platform_util.c"
	"poly1305.c"
	"psa_crypto_aead.c"
	"psa_crypto_aead.h"
	"psa_crypto.c"
	"psa_crypto_cipher.c"
	"psa_crypto_cipher.h"
	"psa_crypto_client.c"
	"psa_crypto_core.h"
	"psa_crypto_driver_wrappers.c"
	"psa_crypto_driver_wrappers.h"
	"psa_crypto_ecp.c"
	"psa_crypto_ecp.h"
	"psa_crypto_hash.c"
	"psa_crypto_hash.h"
	"psa_crypto_invasive.h"
	"psa_crypto_its.h"
	"psa_crypto_mac.c"
	"psa_crypto_mac.h"
	"psa_crypto_pake.c"
	"psa_crypto_pake.h"
	"psa_crypto_random_impl.h"
	"psa_crypto_rsa.c"
	"psa_crypto_rsa.h"
	"psa_crypto_se.c"
	"psa_crypto_se.h"
	"psa_crypto_slot_management.c"
	"psa_crypto_slot_management.h"
	"psa_crypto_storage.c"
	"psa_crypto_storage.h"
	"psa_its_file.c"
	"psa_util.c"
	"ripemd160.c"
	"rsa_alt_helpers.c"
	"rsa_alt_helpers.h"
	"rsa.c"
	"sha1.c"
	"sha256.c"
	"sha512.c"
	"ssl_cache.c"
	"ssl_ciphersuites.c"
	"ssl_client.c"
	"ssl_client.h"
	"ssl_cookie.c"
	"ssl_debug_helpers_generated.c"
	"ssl_debug_helpers.h"
	"ssl_misc.h"
	"ssl_msg.c"
	"ssl_ticket.c"
	"ssl_tls.c"
	"ssl_tls12_client.c"
	"ssl_tls12_server.c"
	"ssl_tls13_client.c"
	"ssl_tls13_generic.c"
	"ssl_tls13_invasive.h"
	"ssl_tls13_keys.c"
	"ssl_tls13_keys.h"
	"ssl_tls13_server.c"
	"threading.c"
	"timing.c"
	"version.c"
	"version_features.c"
	"x509.c"
	"x509_create.c"
	"x509_crl.c"
	"x509_crt.c"
	"x509_csr.c"
	"x509write_crt.c"
	"x509write_csr.c"
)
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src/include"
)
list(TRANSFORM MBEDTLS_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/library/)
add_library(mbedtls STATIC
	${MBEDTLS_SOURCES}
)
