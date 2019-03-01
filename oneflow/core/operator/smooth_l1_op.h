#ifndef ONEFLOW_CORE_OPERATOR_SMOOTH_L1_OP_H_
#define ONEFLOW_CORE_OPERATOR_SMOOTH_L1_OP_H_

#include "oneflow/core/operator/operator.h"

namespace oneflow {

class SmoothL1Op final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(SmoothL1Op);
  SmoothL1Op() = default;
  ~SmoothL1Op() = default;

  void InitFromOpConf() override;
  const PbMessage& GetCustomizedConf() const override;
  bool NeedOutBlobWhenBackward() const override { return false; }
  void InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                      const ParallelContext* parallel_ctx) const override;

 private:
  bool IsInputBlobAllowedModelSplit(const std::string& ibn) const override { return false; }
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_OPERATOR_SMOOTH_L1_OP_H_
