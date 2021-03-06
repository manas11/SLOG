#pragma once

#include <vector>

#include "workload/workload_generator.h"
#include "common/configuration.h"
#include "common/types.h"
#include "proto/transaction.pb.h"

using std::vector;

namespace slog {

class BasicWorkload : public WorkloadGenerator {
public:
  BasicWorkload(
      const ConfigurationPtr config,
      const std::string& data_dir,
      const std::string& params_str,
      const RawParamMap extra_default_params = {});

  std::pair<Transaction*, TransactionProfile> NextTransaction();

protected:
  ConfigurationPtr config_;

  // This is an index of keys by their partition and home.
  // Each partition holds a vector of homes, each of which
  // is a list of keys.
  vector<vector<KeyList>> partition_to_key_lists_;

  std::mt19937 rg_;

  TxnId client_txn_id_counter_;
};

} // namespace slog