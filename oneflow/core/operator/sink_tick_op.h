#ifndef ONEFLOW_CORE_OPERATOR_SINK_TICK_OP_H_
#define ONEFLOW_CORE_OPERATOR_SINK_TICK_OP_H_

#include "oneflow/core/operator/operator.h"
#include "oneflow/core/graph/logical_node.h"

namespace oneflow {

class SinkTickOp final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(SinkTickOp);
  SinkTickOp() = default;
  ~SinkTickOp() = default;

  void InitFromOpConf() override;
  const PbMessage& GetCustomizedConf() const override;
  Maybe<void> InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                             const ParallelContext* parallel_ctx) const;
  LogicalNode* NewProperLogicalNode() const override { return new TickLogicalNode; }

 private:
  Maybe<void> InferBatchAxis(std::function<OptInt64*(const std::string&)> BatchAxis4BnInOp) const;
  void GetSbpSignatures(SbpSignatureList* sbp_sig_list) const override;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_OPERATOR_SINK_TICK_OP_H_
