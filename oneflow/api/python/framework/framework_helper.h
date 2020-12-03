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
#ifndef ONEFLOW_API_PYTHON_FRAMEWORK_FRAMEWORK_HELPER_H_
#define ONEFLOW_API_PYTHON_FRAMEWORK_FRAMEWORK_HELPER_H_

#include <string>
#include <google/protobuf/text_format.h>
#include "oneflow/core/common/protobuf.h"
#include "oneflow/core/job/machine_context.h"
#include "oneflow/core/job/job_build_and_infer_ctx_mgr.h"
#include "oneflow/core/job/oneflow.h"
#include "oneflow/core/job/placement.pb.h"
#include "oneflow/core/framework/config_def.h"
#include "oneflow/core/framework/load_library.h"

namespace oneflow {

Maybe<std::string> GetSerializedStructureGraph() {
  const auto* job_ctx_mgr = Global<LazyJobBuildAndInferCtxMgr>::Get();
  CHECK_NOTNULL_OR_RETURN(job_ctx_mgr);
  return job_ctx_mgr->structure_graph();
}

Maybe<std::string> GetSerializedInterUserJobInfo() {
  CHECK_OR_RETURN(Global<MachineCtx>::Get()->IsThisMachineMaster());
  CHECK_NOTNULL_OR_RETURN(Global<Oneflow>::Get());
  CHECK_NOTNULL_OR_RETURN(Global<InterUserJobInfo>::Get());
  std::string ret;
  google::protobuf::TextFormat::PrintToString(*Global<InterUserJobInfo>::Get(), &ret);
  return ret;
}

Maybe<std::string> GetSerializedJobSet() {
  const auto* job_ctx_mgr = Global<LazyJobBuildAndInferCtxMgr>::Get();
  CHECK_NOTNULL_OR_RETURN(job_ctx_mgr);
  return PbMessage2TxtString(job_ctx_mgr->job_set());
}

Maybe<std::string> GetFunctionConfigDef() {
  std::string ret;
  google::protobuf::TextFormat::PrintToString(GlobalFunctionConfigDef(), &ret);
  return ret;
}

Maybe<std::string> GetScopeConfigDef() {
  std::string ret;
  google::protobuf::TextFormat::PrintToString(GlobalScopeConfigDef(), &ret);
  return ret;
}

Maybe<std::string> GetSerializedMachineId2DeviceIdListOFRecord(
    const std::string& parallel_conf_str) {
  ParallelConf parallel_conf;
  CHECK_OR_RETURN(TxtString2PbMessage(parallel_conf_str, &parallel_conf))
      << "parallel conf parse failed";
  return PbMessage2TxtString(*JUST(ParseMachineAndDeviceIdList(parallel_conf)));
}

Maybe<void> LoadLibraryNow(const std::string& lib_path) { return LoadLibrary(lib_path); }

}  // namespace oneflow

#endif  // ONEFLOW_API_PYTHON_FRAMEWORK_FRAMEWORK_HELPER_H_
