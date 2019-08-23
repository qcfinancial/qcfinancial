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
COPY --from=builder /output /usr/share/nginx/html
RUN chmod -R g+rwx /var/cache/nginx /run /var/log/nginx /usr/share/nginx/html
# users are not allowed to listen on priviliged ports
RUN sed -i.bak 's/listen\(.*\)80;/listen 8081;/' /etc/nginx/conf.d/default.conf
EXPOSE 8081
# # comment user directive as master process is run as user in OpenShift anyhow
RUN sed -i.bak 's/^user/#user/' /etc/nginx/nginx.conf
