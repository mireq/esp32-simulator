FROM ubuntu:19.10

RUN cd ~ && \
	apt-get -y update && \
	apt install -y git wget flex bison gperf python3 python3-pip python3-setuptools python3-serial python3-click python3-cryptography python3-future python3-pyparsing python3-pyelftools cmake ninja-build ccache libffi-dev libssl-dev libusb-1.0-0 && \
	update-alternatives --install /usr/bin/python python /usr/bin/python3 10 && \
	git clone -b release/v4.1 --recursive https://github.com/espressif/esp-idf.git && \
	./esp-idf/install.sh && \
	echo ". `pwd`/esp-idf/export.sh" >> ~/.bashrc

CMD "/bin/bash"
