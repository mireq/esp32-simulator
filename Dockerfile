FROM ubuntu:24.04

ENV PRECONFIGURED_TAPIF=tap0

COPY ./docs/nftables.rules /etc/nftables.rules

RUN apt-get -y update && \
	apt install -y git wget flex bison gperf python3 python3-pip python3-setuptools python3-serial python3-click python3-cryptography python3-future python3-pyparsing python3-pyelftools python3-venv cmake ninja-build ccache libffi-dev libssl-dev libusb-1.0-0 iproute2 nftables dfu-util
RUN cd ~ &&\
	git clone -b release/v5.2 --recursive https://github.com/espressif/esp-idf.git && \
	./esp-idf/install.sh && \
	echo ". `pwd`/esp-idf/export.sh" >> ~/.bashrc && \
	echo "ip tuntap add dev tap0 mode tap;\nip link set tap0 up;\nip addr add 10.0.0.1/24 dev tap0;\nnft -f /etc/nftables.rules;\ntail -f /dev/null" > /usr/bin/container_start && \
	chmod +x /usr/bin/container_start

CMD "/usr/bin/container_start"
