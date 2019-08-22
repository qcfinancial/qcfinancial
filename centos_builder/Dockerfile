FROM centos:7
RUN yum install -y epel-release centos-release-scl
RUN yum install -y  boost169-python2.x86_64 python27-python-libs.x86_64 cmake3 boost169-devel-1.69.0-2.el7.x86_64 python-devel devtoolset-7-gcc-c++ make python27-python-devel.x86_64
&& \
ln -s /usr/lib64/boost169/libboost_python27.so /usr/local/lib/libboost_python27.so && \
ln -s /usr/lib64/boost169/libboost_python27.so /usr/lib/libboost_python27.so       && \
ln -s /usr/include/boost169/boost /usr/local/include/boost

ENV CC=/opt/rh/devtoolset-7/root/usr/bin/gcc


