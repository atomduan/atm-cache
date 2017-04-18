#include <atm_config.h>
#include <atm_core.h>


static uint8_t siphash_seed[16];


void atm_siphash_init() 
{
    atm_str_t p = (atm_str_t) siphash_seed;
    atm_uint_t len = sizeof(siphash_seed);
    atm_str_t charset = "0123456789abcdef";
    atm_uint_t j;

    atm_str_t x = p;
    atm_uint_t l = len;
    struct timeval tv;
    pid_t pid = getpid();

    gettimeofday(&tv,NULL);
    if (l >= sizeof(tv.tv_usec)) {
        memcpy(x,&tv.tv_usec,sizeof(tv.tv_usec));
        l -= sizeof(tv.tv_usec);
        x += sizeof(tv.tv_usec);
    }
    if (l >= sizeof(tv.tv_sec)) {
        memcpy(x,&tv.tv_sec,sizeof(tv.tv_sec));
        l -= sizeof(tv.tv_sec);
        x += sizeof(tv.tv_sec);
    }
    if (l >= sizeof(pid)) {
        memcpy(x,&pid,sizeof(pid));
        l -= sizeof(pid);
        x += sizeof(pid);
    }
    for (j = 0; j < len; j++) {
        p[j] ^= rand();
        p[j] = charset[p[j] & 0x0F];
    }
}


/* Fast tolower() alike function that does not care about locale
 * but just returns a-z insetad of A-Z. */
static atm_int_t siptlw(atm_int_t c) 
{
    if (c >= 'A' && c <= 'Z') {
        return c+('a'-'A');
    } else {
        return c;
    }
}


/* Test of the CPU is Little Endian and supports not aligned accesses.
 * Two interesting conditions to speedup the function that happen to be
 * in most of x86 servers. */
#if defined(__X86_64__) || defined(__x86_64__) || defined (__i386__)
#define UNALIGNED_LE_CPU
#endif

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v)                                                        \
    U32TO8_LE((p), (uint32_t)((v)));                                           \
    U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#ifdef UNALIGNED_LE_CPU
#define U8TO64_LE(p) (*((uint64_t*)(p)))
#else
#define U8TO64_LE(p)                                                           \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |                        \
     ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) |                 \
     ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) |                 \
     ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))
#endif

#define U8TO64_LE_NOCASE(p)                                                    \
    (((uint64_t)(siptlw((p)[0]))) |                                           \
     ((uint64_t)(siptlw((p)[1])) << 8) |                                      \
     ((uint64_t)(siptlw((p)[2])) << 16) |                                     \
     ((uint64_t)(siptlw((p)[3])) << 24) |                                     \
     ((uint64_t)(siptlw((p)[4])) << 32) |                                              \
     ((uint64_t)(siptlw((p)[5])) << 40) |                                              \
     ((uint64_t)(siptlw((p)[6])) << 48) |                                              \
     ((uint64_t)(siptlw((p)[7])) << 56))

#define SIPROUND                                                               \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = ROTL(v1, 13);                                                     \
        v1 ^= v0;                                                              \
        v0 = ROTL(v0, 32);                                                     \
        v2 += v3;                                                              \
        v3 = ROTL(v3, 16);                                                     \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = ROTL(v3, 21);                                                     \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = ROTL(v1, 17);                                                     \
        v1 ^= v2;                                                              \
        v2 = ROTL(v2, 32);                                                     \
    } while (0)


uint64_t atm_siphash(atm_str_t input, atm_uint_t inlen) 
{
    uint8_t *in = (uint8_t *) input;
    uint8_t *k = siphash_seed;
#ifndef UNALIGNED_LE_CPU
    uint64_t hash;
    uint8_t *out = (uint8_t*) &hash;
#endif
    uint64_t v0 = 0x736f6d6570736575ULL;
    uint64_t v1 = 0x646f72616e646f6dULL;
    uint64_t v2 = 0x6c7967656e657261ULL;
    uint64_t v3 = 0x7465646279746573ULL;
    uint64_t k0 = U8TO64_LE(k);
    uint64_t k1 = U8TO64_LE(k + 8);
    uint64_t m;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    const atm_int_t left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    for (; in != end; in += 8) {
        m = U8TO64_LE(in);
        v3 ^= m;

        SIPROUND;

        v0 ^= m;
    }

    switch (left) {
    case 7: b |= ((uint64_t)in[6]) << 48;
    case 6: b |= ((uint64_t)in[5]) << 40;
    case 5: b |= ((uint64_t)in[4]) << 32;
    case 4: b |= ((uint64_t)in[3]) << 24;
    case 3: b |= ((uint64_t)in[2]) << 16;
    case 2: b |= ((uint64_t)in[1]) << 8;
    case 1: b |= ((uint64_t)in[0]); break;
    case 0: break;
    }

    v3 ^= b;

    SIPROUND;

    v0 ^= b;
    v2 ^= 0xff;

    SIPROUND;
    SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
#ifndef UNALIGNED_LE_CPU
    U64TO8_LE(out, b);
    return hash;
#else
    return b;
#endif
}

uint64_t atm_siphash_nocase(atm_str_t input, atm_uint_t inlen) 
{
    uint8_t *in = (uint8_t *) input;
    uint8_t *k = siphash_seed;
#ifndef UNALIGNED_LE_CPU
    uint64_t hash;
    uint8_t *out = (uint8_t*) &hash;
#endif
    uint64_t v0 = 0x736f6d6570736575ULL;
    uint64_t v1 = 0x646f72616e646f6dULL;
    uint64_t v2 = 0x6c7967656e657261ULL;
    uint64_t v3 = 0x7465646279746573ULL;
    uint64_t k0 = U8TO64_LE(k);
    uint64_t k1 = U8TO64_LE(k + 8);
    uint64_t m;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    const atm_int_t left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    for (; in != end; in += 8) {
        m = U8TO64_LE_NOCASE(in);
        v3 ^= m;

        SIPROUND;

        v0 ^= m;
    }

    switch (left) {
    case 7: b |= ((uint64_t)siptlw(in[6])) << 48;
    case 6: b |= ((uint64_t)siptlw(in[5])) << 40;
    case 5: b |= ((uint64_t)siptlw(in[4])) << 32;
    case 4: b |= ((uint64_t)siptlw(in[3])) << 24;
    case 3: b |= ((uint64_t)siptlw(in[2])) << 16;
    case 2: b |= ((uint64_t)siptlw(in[1])) << 8;
    case 1: b |= ((uint64_t)siptlw(in[0])); break;
    case 0: break;
    }

    v3 ^= b;

    SIPROUND;

    v0 ^= b;
    v2 ^= 0xff;

    SIPROUND;
    SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
#ifndef UNALIGNED_LE_CPU
    U64TO8_LE(out, b);
    return hash;
#else
    return b;
#endif
}
