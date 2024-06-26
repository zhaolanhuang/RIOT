/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       TensorFlow Lite MNIST MLP inference functions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdio.h>
#include "kernel_defines.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"

#include "tensorflow/lite/schema/schema_generated.h"

#include "blob/digit.h"
#include "blob/model.tflite.h"

#define THRESHOLD       (0.5)

// Globals, used for compatibility with Arduino-style sketches.
namespace {
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;

    // Create an area of memory to use for input, output, and intermediate arrays.
    // Finding the minimum value for your model may require some trial and error.
    constexpr int kTensorArenaSize = 6 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup()
{

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(model_tflite);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model provided is schema version %d not equal "
               "to supported version %d.",
               static_cast<uint8_t>(model->version()), TFLITE_SCHEMA_VERSION);
        return;
    }

    // This pulls in all the operation implementations we need.
    static tflite::MicroMutableOpResolver<4> resolver;
    if (resolver.AddFullyConnected() != kTfLiteOk) {
        return;
    }
    if (resolver.AddQuantize() != kTfLiteOk) {
        return;
    }
    if (resolver.AddDequantize() != kTfLiteOk) {
        return;
    }
    if (resolver.AddSoftmax() != kTfLiteOk) {
        return;
    }

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        puts("AllocateTensors() failed");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);

    // Copy digit array in input tensor
    for (unsigned i = 0; i < digit_len; ++i) {
        input->data.f[i] = static_cast<float>(digit[i]) / 255.0;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        puts("Invoke failed");
        return;
    }

    // Get the best match from the output tensor
    float val = 0;
    uint8_t digit = 0;
    for (unsigned i = 0; i < 10; ++i) {
        float current = output->data.f[i];
        if (current > THRESHOLD && current > val) {
            val = current;
            digit = i;
        }
    }

    // Output the prediction, if there's one
    if (val > 0) {
        printf("Digit prediction: %d\n", digit);
    }
    else {
        puts("No match found");
    }
}

// The name of this function is important for Arduino compatibility.
void loop() {}
