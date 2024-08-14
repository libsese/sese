# sese-http

## test command

http/1.1 to h2

```shell
curl --http2 -k http://127.0.0.1:8080/haha --noproxy "*" -v
```

without upgrade

```shell
curl --http2-prior-knowledge -k https://127.0.0.1:8080/haha --noproxy "*" -v
```

## todos

- servlet
- controller
- filter
- static resources mounting
- ALPN
- http/1.1
- http/2 (exclude the priority algorithm)
- merge into sese-core