# sese-http

## Simple test command

http/1.1 to h2

```shell
curl --http2 -k http://127.0.0.1:80/haha --noproxy "*" -v
```

without upgrade

```shell
curl --http2-prior-knowledge -k https://127.0.0.1:80/haha --noproxy "*" -v
```

## Todos

- [x] servlet
- [x] controller
- [x] filter
- [x] static resources mounting
- [x] ALPN
- [x] http/1.1
- [x] http/2 (exclude the priority algorithm)
- [ ] merge into sese-core

## Others

verify via [h2spec](https://github.com/summerwind/h2spec)