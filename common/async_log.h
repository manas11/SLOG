#pragma once

#include <queue>
#include <unordered_map>

#include "common/types.h"
#include "proto/internal.pb.h"

using std::queue;
using std::unordered_map;

namespace slog {

using BatchPtr = std::unique_ptr<internal::Batch>;

class AsyncLog {
public:
  AsyncLog();

  void AddBatch(BatchPtr&& batch);
  void AddSlot(SlotId slot_id, BatchId batch_id);

  bool HasNextBatch() const;
  BatchPtr NextBatch();

private:
  void UpdateReadyBatches();

  unordered_map<SlotId, BatchId> pending_slots_;
  unordered_map<BatchId, BatchPtr> unordered_batches_;
  SlotId next_slot_;
  queue<BatchPtr> ready_batches_;
};

} // namespace slog