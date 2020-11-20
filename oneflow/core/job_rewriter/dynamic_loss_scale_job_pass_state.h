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
#ifndef ONEFLOW_CORE_JOB_REWRITER_DYNAMIC_LOSS_SCALE_JOB_PASS_STATE_H_
#define ONEFLOW_CORE_JOB_REWRITER_DYNAMIC_LOSS_SCALE_JOB_PASS_STATE_H_

#include "oneflow/core/job_rewriter/job_pass.h"

namespace oneflow {

class DynamicLossScaleJobPassState : public JobPassState {
 public:
  OF_DISALLOW_COPY_AND_MOVE(DynamicLossScaleJobPassState);
  DynamicLossScaleJobPassState() = default;
  ~DynamicLossScaleJobPassState() override = default;

  const std::string& train_step_assign_condition() const { return train_step_assign_condition_; }
  void set_train_step_assign_condition(const std::string& op_name) {
    train_step_assign_condition_ = op_name;
  }

 private:
  std::string train_step_assign_condition_;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_JOB_REWRITER_DYNAMIC_LOSS_SCALE_JOB_PASS_STATE_H_
