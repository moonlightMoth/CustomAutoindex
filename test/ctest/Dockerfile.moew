FROM alpine as build
RUN apk add --no-cache build-base
WORKDIR /app
COPY . .
RUN gcc hello.c
FROM alpine
COPY --from=build /app/a.out /app/a.out
WORKDIR /app
CMD ["./a.out"]
