/*
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2023 Zhaolan Huang <zhaolan.huang@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for uTVM models
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <tvmgen_default.h>
#include "xtimer.h"

char input[TVMGEN_DEFAULT_SERVING_DEFAULT_INPUT_0_SIZE];
char output[TVMGEN_DEFAULT_PARTITIONEDCALL_0_SIZE];

int main(void)
{
    (void) puts("uTVM test application");
    struct tvmgen_default_inputs default_inputs = {
        .serving_default_input_0 = input
    };
    struct tvmgen_default_outputs default_outputs = {
        .PartitionedCall_0 = output
    };

    uint32_t start, end;
    start =  xtimer_now_usec();
    int ret_val = tvmgen_default_run(&default_inputs, &default_outputs);
    end =  xtimer_now_usec();
    printf("usec: %ld ret: %d \n", (end - start),ret_val);
    printf("model default ret_val: %d \n", ret_val);

    return 0;
}
