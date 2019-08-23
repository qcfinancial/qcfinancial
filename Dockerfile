FROM  docker-registry.default.svc:5000/frontdesk/centos-builder:latest as builder
WORKDIR /src/qcdvaluationengine/
ADD ./QC_DVE_CORE/ /src/qcdvaluationengine
RUN cmake3 . 
RUN make QC_Financial 
RUN make QC_DVE_PYBIND 
RUN mkdir -p /output/ \
&& cp /src/qcdvaluationengine/QC_Financial.so /output/QC_Financial.so	\
&& cp /src/qcdvaluationengine/QC_DVE_PYBIND.so /output/QC_DVE_PYBIND.so	

FROM nginx:stable-alpine
WORKDIR /output
COPY --from=builder /output /usr/share/nginx/html
EXPOSE 80

