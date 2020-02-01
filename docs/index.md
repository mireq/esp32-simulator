This repository contains ESP32 simulator for linux.

# How does it work?

ESP-IDF contains small amount of hardware specific code, FreeRTOS and few
multi-platform libraries.

This project uses FreeRTOS port for POSIX, LWIP for TCP/IP layer and mbedtls for
cryptography functions.

In addition, several useful APIs from esp-idf are implemented eg. logging.

With this project you can write a program, which will be runnable and testable
on Linux.

# Running

## Network

This project requires tun kernel module and configured tap0 interface to work.
To configure tap0 run following commands:

```
sudo ip tuntap add dev tap0 mode tap user `whoami`
sudo ip link set tap0 up
sudo ip addr add 10.0.0.1/24 dev tap0
export PRECONFIGURED_TAPIF=tap0
```

Simulator can be connected to internet using masquerade. First we need to enable
forwarding.

```
sudo echo 1 > /proc/sys/net/ipv4/ip_forward
```

Simple nftables forwarding rules:

```
#!/sbin/nft -f

flush ruleset


table inet filter {
	chain input {
		type filter hook input priority 0; policy drop;
		ct state invalid counter drop
		ct state {established, related} counter accept
		iif lo accept
		iif != lo ip daddr 127.0.0.1/8 counter drop
		iif != lo ip6 daddr ::1/128 counter drop
		ip protocol icmp counter accept
		ip6 nexthdr icmpv6 counter accept
		iifname tap0 accept
	}

	chain forward {
		type filter hook forward priority 0; policy accept;
	}

	chain output {
		type filter hook output priority 0; policy accept;
	}
}


table ip nat {
	chain input {
		type nat hook input priority 0; policy accept;
		ip protocol icmp accept
	}

	chain prerouting {
		type nat hook prerouting priority 0; policy accept;
	}

	chain postrouting {
		type nat hook postrouting priority 100; policy accept;
		ip daddr != 10.0.0.0/24 ip saddr 10.0.0.0/24 masquerade;
	}

	chain output {
		type nat hook output priority 0; policy accept;
	}
}
```

## Using ubuntu

```
sudo apt install -y git wget flex bison gperf python3 python3-pip python3-setuptools python3-serial python3-click python3-cryptography python3-future python3-pyparsing python3-pyelftools cmake ninja-build ccache libffi-dev libssl-dev libusb-1.0-0
mkdir esp
cd esp
git clone -b release/v4.1 --recursive https://github.com/espressif/esp-idf.git
./esp-idf/install.sh
. ./esp-idf/export.sh
git clone https://github.com/mireq/esp32-simulator
cd simulator/example
idf.py build
./build/example
```

## Inside docker

```
docker build -t esp32-simulator .
docker run --cap-add=NET_ADMIN --device /dev/net/tun:/dev/net/tun --name esp32-simulator -v `pwd`:/root/simulator -d esp32-simulator

docker exec -i -t esp32-simulator bash
cd ~/simulator/example
idf.build
./build/example
```
