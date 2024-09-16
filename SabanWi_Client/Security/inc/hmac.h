#ifndef _HMAC_H_
#define _HMAC_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "sha.h"

#define SHA256_BLOCK_SIZE 64

size_t  // Returns the number of bytes written to `out`
hmac_sha256(
    // [in]: The key and its length.
    //      Should be at least 32 bytes long for optimal security.
    const void* key,
    const size_t keylen,

    // [in]: The data to hash alongside the key.
    const void* data,
    const size_t datalen,

    // [out]: The output hash.
    //      Should be 32 bytes long. If it's less than 32 bytes,
    //      the resulting hash will be truncated to the specified length.
    void* out,
    const size_t outlen);

void TestHMAC(void);

#endif // _HMAC_H_
