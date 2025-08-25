#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "digest.h"

static void test_md_sha256_correctness(void **state) {
    (void) state;

    MessageDigest *md = MD_digest_init();
    assert_non_null(md);

    const unsigned char msg[] = "abc";
    unsigned char out[md->md_size];
    unsigned int len = 0;

    assert_int_equal(MD_update_message(md, (void*)msg, strlen((const char*)msg)), 0);
    MD_calculate_digest(md, out, &len);

    const unsigned char expected[32] = {
        0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,
        0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,
        0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad
    };

    assert_int_equal(len, 32);
    assert_memory_equal(out, expected, 32);

    MD_digest_free(&md);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_md_sha256_correctness),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
