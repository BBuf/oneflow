#include "oneflow/core/job/runtime.h"
#include "oneflow/core/common/str_util.h"
#include "oneflow/core/common/protobuf.h"
#include "oneflow/core/persistence/file_system.h"
#include "oneflow/core/job/plan.pb.h"
#include "oneflow/core/graph/task_node.h"
#include "oneflow/core/evaluator/eval_thread_manager.h"

namespace oneflow {

namespace {

void HandoutTasks(const std::vector<const TaskProto*>& tasks) {
  for (const TaskProto* task : tasks) {
    Global<EvalThreadMgr>::Get()->GetThrd(task->thrd_id())->AddTask(*task);
  }
  SendCmdMsg(tasks, ActorCmd::kConstructActor);
}

}  // namespace

class Evaluator final {
 public:
  OF_DISALLOW_COPY_AND_MOVE(Evaluator);
  ~Evaluator() = default;

  Evaluator(const Plan& plan, const int64_t actor_id);

 private:
  void NewAllGlobal();
  void DeleteAllGlobal();
};

Evaluator::Evaluator(const Plan& plan, const int64_t actor_id) {
  NewAllGlobal();
  std::vector<const TaskProto*> eval_tasks;
  int64_t this_machine_task_num = 0;

  for (const TaskProto& task : plan.task()) {
    if (task.machine_id() != 0) { continue; }
    if (task.task_id() == actor_id) { eval_tasks.push_back(&task); }
    this_machine_task_num += 1;
  }
  RuntimeCtx* runtime_ctx = Global<RuntimeCtx>::Get();
  runtime_ctx->NewCounter("constructing_actor_cnt", this_machine_task_num);
  HandoutTasks(eval_tasks);
  runtime_ctx->WaitUntilCntEqualZero("constructing_actor_cnt");
  LOG(INFO) << "All actor on this machine are constructed";

  DeleteAllGlobal();
}

void Evaluator::NewAllGlobal() {
  int64_t piece_num = 0;
  Global<RuntimeCtx>::New(piece_num, false);
  Global<EvalThreadMgr>::New();
}

void Evaluator::DeleteAllGlobal() {
  Global<RuntimeCtx>::Delete();
  Global<EvalThreadMgr>::Delete();
}

}  // namespace oneflow

DEFINE_string(plan_filepath, "", "");
DEFINE_string(actor_id, "", "");

int main(int argc, char** argv) {
  using namespace oneflow;
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  LocalFS()->RecursivelyCreateDirIfNotExist(LogDir());
  RedirectStdoutAndStderrToGlogDir();
  LOG(INFO) << "Evaluation Starting Up";

  Plan plan;
  LOG(INFO) << "Parse Plan File";
  ParseProtoFromTextFile(FLAGS_plan_filepath, &plan);
  Evaluator eval(plan, std::stoi(FLAGS_actor_id));

  LOG(INFO) << "Evaluation Shutting Down";
  CloseStdoutAndStderr();
  return 0;
}
