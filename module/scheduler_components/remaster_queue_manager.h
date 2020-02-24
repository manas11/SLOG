#pragma once

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/configuration.h"
#include "common/constants.h"
#include "common/types.h"
#include "common/transaction_utils.h"

#include "storage/storage.h"

using std::list;
using std::shared_ptr;
using std::pair;
using std::unordered_map;
using std::unordered_set;
using std::vector;

namespace slog {

enum class VerifyMasterResult {VALID, WAITING, ABORT};


/**
 * The remaster queue manager also conducts the check of master metadata.
 * If a remaster has occured since the transaction was forwarded, it may
 * need to be restarted. If the transaction arrived before a remaster that
 * the forwarder included in the metadata, then it will need to wait.
 */
class RemasterQueueManager {
public:
  RemasterQueueManager(
    ConfigurationPtr config,
    shared_ptr<Storage<Key, Record>> storage,
    shared_ptr<unordered_map<TxnId, TransactionHolder>> all_txns);

  /**
   * Checks the counters of the transaction's master metadata.
   * 
   * @param txn Transaction to be checked
   * @return The result of the check.
   * - If Valid, the transaction can be sent for locks.
   * - If Waiting, the counters were ahead (meaning that a remaster
   * has occured at another region before the local). The
   * transaction will be put in a queue to wait for the remaster
   * to be executed locally.
   * - If Aborted, the counters were behind and the transaction
   * needs to be aborted.
   */
  VerifyMasterResult VerifyMaster(const TransactionHolder& txn_holder);

  /**
   * Updates the queue of transactions waiting for remasters,
   * and returns any newly unblocked transactions.
   * 
   * @param key The key that has been remastered
   * @return A queue of transactions that are now unblocked, in the
   * order they were submitted
   */
  list<TxnId> RemasterOccured(Key key, const uint32_t remaster_counter);

private:
  void InsertIntoBlockedQueue(const Key key, const uint32_t counter, const Transaction& txn);
  void TryToUnblock(const Key unblocked_key, list<TxnId>& unblocked);

  ConfigurationPtr config_;
  shared_ptr<Storage<Key, Record>> storage_;
  shared_ptr<unordered_map<TxnId, TransactionHolder>> all_txns_;
  
  // Priority queues for the transactions waiting for each key. Lowest counters first, earliest
  // arrivals break tie
  unordered_map<Key, list<pair<TxnId, uint32_t>>> blocked_queue;

  // Queues of keys that are blocked waiting for other keys of the transaction to be remastered, or for other
  // transactions ahead in order.
  unordered_map<Key, list<TxnId>> indirectly_blocked_queue;
};

} // namespace slog