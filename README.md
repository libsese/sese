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

## [rfc7540](https://www.rfc-editor.org/rfc/rfc7540.txt)

- [ ] 6.1 Data
  - [x] 6.1.1
  - [ ] 6.1.2
  - [x] 6.1.3
- [ ] 6.2 HEADERS
  - [x] 6.2.1
  - [x] 6.2.2
  - [x] 6.2.3
  - [ ] 6.2.4
- [ ] 6.3 PRIORITY
  - [x] 6.3.1
  - [ ] 6.3.2
- [ ] 6.4 RST_STREAM
  - [x] 6.4.1
  - [x] 6.4.2
  - [ ] 6.4.3