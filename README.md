# SLOG
SLOG stands for serializable, low-latency, geo-replicated transactions.

SLOG is a geographically distributed data store that achieves high-throughput and low-latency transactions while guaranteeing strict serializability. SLOG achieves high-throughput, strictly serializable ACID transactions at geo-replicated distance and scale for all transactions submitted across the world while achieving low latency for transactions that initiate from a location close to the home region for data they access.

[Research Paper](https://dl.acm.org/citation.cfm?id=3360377)
## Install Dependencies
> Note: All steps mentioned are suitable for Ubuntu 20.04 LTS

### Install gcc,cmake and other essential libraries
```
./prev-install.sh
```
### Install dependencies for the code and building code
Clone this repo and Run in root directory of repository
```
./install-deps.sh
```
Build the code
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```
