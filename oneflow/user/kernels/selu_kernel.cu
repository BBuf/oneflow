/*
Copyright 2020 The OneFlow Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifdef WITH_CUDA
#include "oneflow/core/kernel/util/cuda_half_util.h"
#include "oneflow/core/cuda/elementwise.cuh"
#include "oneflow/user/kernels/selu_kernel.h"
namespace oneflow {

namespace {

template<template<typename> class Opt, typename T>
struct ElemwiseSeluFunctor<DeviceType::kGPU, Opt, T> final {
  void operator()(DeviceCtx* ctx, const int64_t elem_cnt, T lambda, T alpha, T* out,
                  const T* in) {
    OF_CUDA_CHECK(oneflow::cuda::elementwise::Unary(SeluFunctor<T>(lambda, alpha), elem_cnt,
                                                    out, in, ctx->cuda_stream()));
  }
};

template<template<typename> class Opt, typename T>
struct ElemwiseSeluGradFunctor<DeviceType::kGPU, Opt, T> final {
  void operator()(DeviceCtx* ctx, const int64_t elem_cnt, T lambda, T alpha, T* dx, const T* y,
                  const T* dy) {
    OF_CUDA_CHECK(oneflow::cuda::elementwise::Binary(SeluGradFunctor<T>(lambda, alpha),
                                                     elem_cnt, dx, y, dy, ctx->cuda_stream()));
  };
};

}  // namespace

REGISTER_SELU_KERNELS(DeviceType::kGPU, half);
REGISTER_SELU_KERNELS(DeviceType::kGPU, float);
REGISTER_SELU_KERNELS(DeviceType::kGPU, double);

}  // namespace oneflow
#endif  // WITH_CUDA
