FROM  docker-registry-default.apps.pre.ocp.binter.cl/frontdesk/web2py-framework/centos7_builder:latest as builder

WORKDIR /src/qcdvaluationengine/
ADD ./QC_DVE_CORE/ /src/qcdvaluationengine
RUN cmake3 . \
&&  make all \
&& mkdir -p /output/ \
&& cp /src/qcdvaluationengine/QC_DVE_CORE/libQC_Financial.so /output/QC_Financial.so	\
&& cp /src/qcdvaluationengine/QC_DVE_CORE/libQC_DVE_PYBIND.so /output/QC_DVE_PYBIND.so	

FROM alpine:latest
WORKDIR /output
COPY --from=builder /output /output

