FROM alpine:latest AS build
RUN apk add gcc make valgrind build-base
WORKDIR /
RUN mkdir app
WORKDIR app
COPY . .
RUN make clean test_compile install_html_templates valgrind_test
RUN make clean compile install_html_templates

FROM alpine:latest
RUN mkdir /app
WORKDIR /app
COPY --from=build /app/target/ /app
RUN mkdir /vol
VOLUME /vol
EXPOSE 8080

CMD ["./custom_autoindex", "-s", "/vol"]
