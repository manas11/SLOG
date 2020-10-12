#include <memory>
#include <vector>
#include <fcntl.h>

#include "common/configuration.h"
#include "common/constants.h"
#include "common/offline_data_reader.h"
#include "common/service_utils.h"
#include "common/types.h"
#include "common/proto_utils.h"
#include "connection/broker.h"
#include "connection/sender.h"
#include "module/scheduler.h"
#include "proto/internal.pb.h"
#include "storage/mem_only_storage.h"

DEFINE_string(config, "slog.conf", "Path to the configuration file");
DEFINE_string(address, "", "Address of the local machine");
DEFINE_uint32(replica, 0, "Replica number of the local machine");
DEFINE_uint32(partition, 0, "Partition number of the local machine");
DEFINE_uint32(num_txn, 1000, "");
DEFINE_uint32(wait_sec, 2, "");

using namespace slog;
using std::make_shared;

int main(int argc, char* argv[]) {
  slog::InitializeService(&argc, &argv);
  
  auto zmq_version = zmq::version();
  LOG(INFO) << "ZMQ version "
            << std::get<0>(zmq_version) << "."
            << std::get<1>(zmq_version) << "."
            << std::get<2>(zmq_version);

  auto config = slog::Configuration::FromFile(
      FLAGS_config, 
      FLAGS_address,
      FLAGS_replica,
      FLAGS_partition);
  const auto& all_addresses = config->all_addresses();
  CHECK(std::find(
      all_addresses.begin(),
      all_addresses.end(),
      config->local_address()) != all_addresses.end())
      << "The configuration does not contain the provided "
      << "local machine ID: \"" << config->local_address() << "\"";
  CHECK_LT(config->local_replica(), config->num_replicas())
      << "Replica numbers must be within number of replicas";
  CHECK_LT(config->local_partition(), config->num_partitions())
      << "Partition number must be within number of partitions";

  auto context = make_shared<zmq::context_t>(1);
  auto broker = make_shared<Broker>(config, context);

  // Create and initialize storage layer
  auto storage = make_shared<slog::MemOnlyStorage<Key, Record, Metadata>>();

  // Create the server module. This is not added to the "modules" 
  // list below because it starts differently.

  auto scheduler = MakeRunnerFor<slog::Scheduler>(config, broker, storage);
  
  // New modules cannot be bound to the broker after it starts so start 
  // the Broker only after it is used to initialized all modules.
  broker->StartInNewThread();
  scheduler->StartInNewThread();

  auto txn = slog::MakeTransaction(
      {"0", "1", "2"}, {}, "" , 
      {{"0", {0, 0}},
        {"1", {0, 0}},
        {"2", {0, 0}}});
  txn->mutable_internal()->set_type(TransactionType::SINGLE_HOME);
  internal::Request req;
  req.mutable_forward_txn()->set_allocated_txn(txn);
  
  auto sender = slog::Sender(broker);
  for (unsigned int i = 0; i < FLAGS_num_txn; i++) {
    txn->mutable_internal()->set_id(i);
    sender.Send(req, kSchedulerChannel);
  }

  std::this_thread::sleep_for(std::chrono::seconds(FLAGS_wait_sec));
  return 0;
}