#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <nanorq.h>

void precode_matrix_print(params *P, pc *W, FILE *stream)
{
    u32 m = W->rows, n = W->cols;
    u8 tmp[n];
    fprintf(stream, "A[%ux%u],S_H[%u|%u]\n", m, n, P->S, P->H);
    for (u32 row = 0; row < m; row++) {
        u32 nz = 0;
        u8 tmp[n];
        for (u32 x = 0; x < n; x++)
            tmp[x] = 0;
        for (u32 it = 0; it < uv_size(W->A[row]); it++) {
            u32 col = uv_A(W->A[row], it);
            tmp[col] = 1;
        }
        for (u32 col = 0; col < n; col++)
            if (row >= P->S && row < (P->S + P->H))
                fprintf(stream, "x");
            else
                fprintf(stream, "%d", tmp[col]);

        fprintf(stream, "\n");
    }
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    nanorq rq;
    void *sched = NULL;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <K>\n", argv[0]);
        return -1;
    }

    int K = strtol(argv[1], NULL, 10);
    if (K < 5 || K > 56403 || 0 != nanorq_encoder_new(K, 0, &rq)) {
        fprintf(stderr, "failed to init codec\n");
        return -1;
    }

    size_t prep_len = nanorq_calculate_prepare_memory(&rq);
    uint8_t *prep_mem = malloc(prep_len);
    nanorq_prepare(&rq, prep_mem, prep_len);

    nanorq_replace_symbol(&rq, 0, K + 3);
    nanorq_replace_symbol(&rq, 1, K + 5);
    nanorq_replace_symbol(&rq, 2, K + 7);

    precode_matrix_print(&rq.P, &rq.W, stdout);

    free(prep_mem);

    return 0;
}
