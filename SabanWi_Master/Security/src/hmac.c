#include "hmac.h"

/* LOCAL FUNCTIONS */

// Concatenate X & Y, return hash.
static void* H(const void* x,
               const size_t xlen,
               const void* y,
               const size_t ylen,
               void* out,
               const size_t outlen);

// Wrapper for sha256
static void* sha256(const void* data,
                    const size_t datalen,
                    void* out,
                    const size_t outlen);

// Declared in hmac_sha256.h
size_t hmac_sha256(const void* key,
                   const size_t keylen,
                   const void* data,
                   const size_t datalen,
                   void* out,
                   const size_t outlen) {
  uint8_t k[SHA256_BLOCK_SIZE];
  uint8_t k_ipad[SHA256_BLOCK_SIZE];
  uint8_t k_opad[SHA256_BLOCK_SIZE];
  uint8_t ihash[SHA256_HASH_SIZE];
  uint8_t ohash[SHA256_HASH_SIZE];
  size_t sz;
  int i;

  memset(k, 0, sizeof(k));
  memset(k_ipad, 0x36, SHA256_BLOCK_SIZE);
  memset(k_opad, 0x5c, SHA256_BLOCK_SIZE);

  if (keylen > SHA256_BLOCK_SIZE) {
    // If the key is larger than the hash algorithm's
    // block size, we must digest it first.
    sha256(key, keylen, k, sizeof(k));
  } else {
    memcpy(k, key, keylen);
  }

  for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
    k_ipad[i] ^= k[i];
    k_opad[i] ^= k[i];
  }

  // Perform HMAC algorithm: ( https://tools.ietf.org/html/rfc2104 )
  //      `H(K XOR opad, H(K XOR ipad, data))`
  H(k_ipad, sizeof(k_ipad), data, datalen, ihash, sizeof(ihash));
  H(k_opad, sizeof(k_opad), ihash, sizeof(ihash), ohash, sizeof(ohash));

  sz = (outlen > SHA256_HASH_SIZE) ? SHA256_HASH_SIZE : outlen;
  memcpy(out, ohash, sz);
  return sz;
}

static void* H(const void* x,
               const size_t xlen,
               const void* y,
               const size_t ylen,
               void* out,
               const size_t outlen) {
  void* result;
  size_t buflen = (xlen + ylen);
  uint8_t* buf = (uint8_t*)malloc(buflen);

  memcpy(buf, x, xlen);
  memcpy(buf + xlen, y, ylen);
  result = sha256(buf, buflen, out, outlen);

  free(buf);
  return result;
}

static void* sha256(const void* data,
                    const size_t datalen,
                    void* out,
                    const size_t outlen) {
  size_t sz;
  Sha256Context ctx;
  SHA256_HASH hash;

  Sha256Initialise(&ctx);
  Sha256Update(&ctx, data, datalen);
  Sha256Finalise(&ctx, &hash);

  sz = (outlen > SHA256_HASH_SIZE) ? SHA256_HASH_SIZE : outlen;
  return memcpy(out, hash.bytes, sz);
}
                    

//void TestHMAC(void)
//{
//      int timestart = 0 ;
//      const char* str_data = "0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f";
//      const char* str_key = "0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f";
//      uint8_t out[SHA256_HASH_SIZE];
//      char out_str[SHA256_HASH_SIZE * 2 + 1];
//      unsigned i;
//      
//      Timer3_SetTickMs();
//      timestart = Timer3_GetTickMs();
//      // Call hmac-sha256 function
//      hmac_sha256(str_key, strlen(str_key), str_data, strlen(str_data), &out, sizeof(out));
//      printf("DES encrypt TIME:%d ms\n\r",timestart);
//      // Convert `out` to string with printf
//      memset(&out_str, 0, sizeof(out_str));
//      for (i = 0; i < sizeof(out); i++) 
//      {
//            snprintf(&out_str[i*2], 3, "%02x", out[i]);
//      }

//      // Print out the result
//      printf("Message: %s\n", str_data);
//      printf("Key: %s\n", str_key);
//      printf("HMAC: %s\n", out_str);

//}

