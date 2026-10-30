#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/* 把 count 个无符号整数写入 out，成功返回 0。 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t a = (uint32_t)challenge;
    uint32_t b = (uint32_t)(challenge >> 32);
    uint32_t m = (uint32_t)(challenge >> 16);
    uint32_t x = (5u * a + b) ^ 0xe161u;
    uint32_t y = 3u * (b - m) + 0xcce9u;
    values[0] = x ^ (y + 0xe12cu);
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    values[0] = (uint32_t)((challenge >> 26) & 127u) + 7u;
    for (unsigned i = 1; i < count; ++i) {
        uint32_t n = (uint32_t)((challenge >> (3u * i + 2u)) & 63u);
        values[i] = (5u * values[i - 1]) ^ (n + i);
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    uint32_t x = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    values[0] = (uint32_t)(challenge & 7u);
    // 跳转表的顺序不同于 case 函数的编号。
    switch (values[0]) {
        case 0: values[1] = (x ^ 0x317cu) + (x >> 2); break;
        case 1: values[1] = 5u * x + 0x456e1u; break;
        case 2: values[1] = (2u * x) ^ 0x4593u; break;
        case 3: values[1] = 5u * x + 0x28532u; break;
        case 4: values[1] = 3u * x + 0x362fu; break;
        case 5: values[1] = 3u * x + 0x2a51u; break;
        case 6: values[1] = (2u * x) ^ 0x2321u; break;
        default: values[1] = (x ^ 0xee4fu) + (x >> 1); break;
    }
    return write_answer(out, cap, values, count);
}

static uint32_t walk(uint32_t t, uint32_t l, uint32_t r, uint32_t s) {
    uint32_t m = l + ((r - l) >> 1);
    if (t == m) return (m + 0xf2u) ^ s;
    if (t < m) return (3u * walk(t, l, m - 1, s)) ^ (m + s) ^ 0x18u;
    return (3u * walk(t, m + 1, r, s)) ^ (m + s) ^ 0x17u;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t t = (uint32_t)(challenge & 127u);
    uint32_t s = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    values[0] = walk(t, 0, 127, s);
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    struct node {
        uint32_t v, id;
        const struct node *next;
    };
    static const struct node n[12] = {
        {0xfbf2, 0x47d, &n[1]}, {0xeb79, 0x84c, &n[7]},
        {0xfd54, 0x609, NULL},  {0x8925, 0xa21, &n[10]},
        {0x2216, 0x224, &n[6]}, {0x30dd, 0x750, &n[8]},
        {0x81fc, 0x14e, &n[2]}, {0xe2b2, 0x958, &n[5]},
        {0x59a8, 0x563, &n[3]}, {0xba41, 0xb5d, &n[11]},
        {0xebc6, 0x30b, &n[4]}, {0xbefc, 0x04f, &n[0]}
    };
    static const unsigned head[8] = {9, 11, 0, 1, 7, 5, 8, 3};
    const struct node *p = &n[head[challenge & 7u]];
    unsigned steps = 1u + (unsigned)((challenge >> 3) & 3u);
    values[0] = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32) ^ 0x1b0eu;
    for (unsigned i = 0; i < steps; ++i) {
        values[0] = ((values[0] + p->v) * 5u) ^ p->id;
        values[1] = p->id;
        p = p->next;
    }
    return write_answer(out, cap, values, count);
}

static uint32_t op0(uint32_t x) {
    return ((x << 5) + 0x1d85u) ^ (5u * x);
}

static uint32_t op1(uint32_t x) {
    return ((x << 1) | (x >> 31)) ^ 0x820fu;
}

static uint32_t op2(uint32_t x) {
    return (x ^ (x >> 1)) + 0xa523u;
}

static uint32_t op3(uint32_t x) {
    return (x ^ 0x887u) + (x << 5);
}

static uint32_t op4(uint32_t x) {
    return (x >> 2) | (x << 30);
}

static uint32_t op5(uint32_t x) {
    return (x + 0xb5bau) ^ (x >> 2);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    static uint32_t (*const op[8])(uint32_t) = {
        op1, op1, op3, op2, op0, op5, op2, op4
    };
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    x = op[challenge & 7u](x) ^ 0xa6cu;
    values[0] = op[(challenge >> 3) & 7u](x) + 0x1c61u;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t a = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t b = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t k = (uint32_t)(context & 3u);
    uint32_t x = a ^ b;
    uint32_t y = ((x >> 5) ^ 0xd4efu) + (x ^ 0xb548u);
    y ^= (k * 0xb548u) ^ (uint32_t)(context >> 16);
    uint32_t z = y ^ ((a >> 5) + b + k + 0xd4efu);
    // 32 字节后是 OPEN 标记，再接三个整数。
    int size = snprintf(out, cap,
                        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                        k, y, z);
    return size < 0 || (size_t)size >= cap;
}
