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
#include "oneflow/core/framework/framework.h"
#include "oneflow/core/common/data_type.h"
#include "oneflow/core/kernel/util/cuda_half_util.h"
#include "oneflow/core/cuda/elementwise.cuh"

namespace oneflow {

namespace user_op {

template<typename T>
struct SEluFunctor {
  __host__ __device__ explicit SEluFunctor(T lambda_, T alpha_) : lambda_(lambda_), alpha_(alpha_) {}
  __device__ T operator()(T x) const {
    return ((x > static_cast<T>(0)) ? x : alpha_ * (exp(x) - static_cast<T>(1))) * lambda_;
  }
  const T lambda_;
  const T alpha_;
};

template<typename T>
struct SEluGradFunctor {
  __host__ __device__ explicit SEluGradFunctor(T lambda_, T alpha_) : lambda_(lambda_), alpha_(alpha_) {}
  __device__ T operator()(T x, T dy) const {
    return ((x > static_cast<T>(0)) ? dy : dy * alpha_ * (exp(x))) * lambda_;
  }
  const T lambda_;
  const T alpha_;
};

template<>
struct SEluFunctor<half> {
  __host__ __device__ explicit SEluFunctor(float lambda_, float alpha_)
      : lambda_(lambda_), alpha_(alpha_), float_functor(SEluFunctor<float>(lambda_, alpha_)) {}
  __device__ half operator()(half x) const { return __float2half(float_functor(__half2float(x))); }
  const float lambda_;
  const float alpha_;
  SEluFunctor<float> float_functor;
};

template<>
struct SEluGradFunctor<half> {
  __host__ __device__ explicit SEluGradFunctor(float lambda_, float alpha_)
      : lambda_(lambda_), alpha_(alpha_), float_functor(SEluGradFunctor<float>(lambda_, alpha_)) {}
  __device__ half operator()(half x, half dy) const {
    return __float2half(float_functor(__half2float(x), __half2float(dy)));
  }
  const float lambda_;
  const float alpha_;
  SEluGradFunctor<float> float_functor;
};

template<DeviceType device_type, typename T>
class GpuSEluKernel final : public OpKernel {
 public:
  GpuSEluKernel() = default;
  ~GpuSEluKernel() = default;

 private:
  void Compute(KernelComputeContext* ctx) const override {
    const Tensor* in_tensor = ctx->Tensor4ArgNameAndIndex("in", 0);
    Tensor* out_tensor = ctx->Tensor4ArgNameAndIndex("out", 0);
    const T lambda_ = static_cast<T>(ctx->Attr<double>("lambda_"));
    const T alpha_ = static_cast<T>(ctx->Attr<double>("alpha_"));
    const T* in_ptr = in_tensor->dptr<T>();
    T* out_ptr = out_tensor->mut_dptr<T>();

    const int32_t elem_cnt = in_tensor->shape().elem_cnt();
    OF_CUDA_CHECK((oneflow::cuda::elementwise::Unary(SEluFunctor<T>(lambda_, alpha_), elem_cnt, out_ptr,
                                                     in_ptr, ctx->device_ctx()->cuda_stream())));
  }
  bool AlwaysComputeWhenAllOutputsEmpty() const override { return false; }
};

#define REGISTER_GPU_SELU_KERNEL(device, dtype)                                            \
  REGISTER_USER_KERNEL("selu").SetCreateFn<GpuSEluKernel<device, dtype>>().SetIsMatchedHob( \
      (HobDeviceTag() == device) & (HobDataType("out", 0) == GetDataType<dtype>::value));

REGISTER_GPU_SELU_KERNEL(DeviceType::kGPU, half);
REGISTER_GPU_SELU_KERNEL(DeviceType::kGPU, float);
REGISTER_GPU_SELU_KERNEL(DeviceType::kGPU, double);

template<DeviceType device_type, typename T>
class GpuSEluGradKernel final : public OpKernel {
 public:
  GpuSEluGradKernel() = default;
  ~GpuSEluGradKernel() = default;

 private:
  void Compute(KernelComputeContext* ctx) const override {
    const Tensor* x_tensor = ctx->Tensor4ArgNameAndIndex("x", 0);
    const Tensor* dy_tensor = ctx->Tensor4ArgNameAndIndex("dy", 0);
    Tensor* dx_tensor = ctx->Tensor4ArgNameAndIndex("dx", 0);
    const T lambda_ = static_cast<T>(ctx->Attr<double>("lambda_"));
    const T alpha_ = static_cast<T>(ctx->Attr<double>("alpha_"));
    const T* x_ptr = x_tensor->dptr<T>();
    const T* dy_ptr = dy_tensor->dptr<T>();
    T* dx_ptr = dx_tensor->mut_dptr<T>();
    const int32_t elem_cnt = x_tensor->shape().elem_cnt();
    OF_CUDA_CHECK(
        (oneflow::cuda::elementwise::Binary(SEluGradFunctor<T>(lambda_, alpha_), elem_cnt, dx_ptr, x_ptr,
                                            dy_ptr, ctx->device_ctx()->cuda_stream())));
  }
  bool AlwaysComputeWhenAllOutputsEmpty() const override { return false; }
};

#define REGISTER_GPU_SELU_BACKWARD_KERNEL(device, dtype) \
  REGISTER_USER_KERNEL("selu_grad")                      \
      .SetCreateFn<GpuSEluGradKernel<device, dtype>>()   \
      .SetIsMatchedHob((HobDeviceTag() == device)       \
                       & (HobDataType("dx", 0) == GetDataType<dtype>::value));

REGISTER_GPU_SELU_BACKWARD_KERNEL(DeviceType::kGPU, half);
REGISTER_GPU_SELU_BACKWARD_KERNEL(DeviceType::kGPU, float);
REGISTER_GPU_SELU_BACKWARD_KERNEL(DeviceType::kGPU, double);

}  // namespace user_op

}  // namespace oneflow
