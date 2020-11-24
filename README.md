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

### Install dependencies for admin tool.
```
sudo apt-get install python3-pip -y
pip3 install -r tools/requirements.txt
```

### Install docker
Follow [Docker engine install on Ubuntu](https://docs.docker.com/engine/install/ubuntu/). Then, configure docker to run without Sudo using [Link](https://docs.docker.com/engine/install/linux-postinstall/#manage-docker-as-a-non-root-user)

### Build docker image and push the image to a remote repository:
https://hub.docker.com/repository/docker/ctring/slog
```
docker build . -t manas11/manas
docker login
# check image id using 
docker images
docker tag <image-id> manas11/manas:latest

docker push manas11/manas
```

### Create local cluster:
Use the file present in examples/cluster.conf as template for creating a local cluster
```
python3 tools/admin.py local --start --image manas11/manas /home/ubuntu/SLOG/examples/cluster.conf
```
This creates 4 docker containers with SLOG running, now these containers are ready to exchange data.

### Create dataset
```
python3 tools/gen_data.py -c /var/tmp/slog.conf -s 100 -su mb /var/tmp
```

### Run benchmark tests
```
build/benchmark -data_dir /var/tmp -config /var/tmp/slog.conf -num_txns=1000 -wl=basic -params="mh=50"
```
