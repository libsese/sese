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
- [ ] static resources mounting
- [x] ALPN
- [x] http/1.1
- [ ] http/2 (exclude the priority algorithm)
- [ ] merge into sese-core

## HTTP2 Server

- 1.Starting HTTP/2
    - [x] 1: Sends a client connection preface
- 2.Streams and Multiplexing
    - [x] 1: Sends a PRIORITY frame on idle stream
    - [ ] 2: Sends a WINDOW_UPDATE frame on half-closed (remote) stream
    - [ ] 3: Sends a PRIORITY frame on half-closed (remote) stream
    - [x] 4: Sends a RST_STREAM frame on half-closed (remote) stream
    - [x] 5: Sends a PRIORITY frame on closed stream

- 3.Frame Definitions
    - 3.1. DATA
        - [x] 1: Sends a DATA frame
        - [x] 2: Sends multiple DATA frames
        - [x] 3: Sends a DATA frame with padding
    - 3.2. HEADERS
        - [x] 1: Sends a HEADERS frame
        - [x] 2: Sends a HEADERS frame with padding
        - [x] 3: Sends a HEADERS frame with priority
    - 3.3. PRIORITY
        - [x] 1: Sends a PRIORITY frame with priority 1
        - [x] 2: Sends a PRIORITY frame with priority 256
        - [x] 3: Sends a PRIORITY frame with stream dependency
        - [x] 4: Sends a PRIORITY frame with exclusive
        - [x] 5: Sends a PRIORITY frame for an idle stream, then send a HEADER frame for a lower stream ID
    - 3.4. RST_STREAM
        - [x] 1: Sends a RST_STREAM frame
    - 3.5. SETTINGS
        - [x] 1: Sends a SETTINGS frame
    - 3.7. PING
        - [x] 1: Sends a PING frame
    - 3.8. GOAWAY
        - [x] 1: Sends a GOAWAY frame
    - 3.9. WINDOW_UPDATE
        - [x] 1: Sends a WINDOW_UPDATE frame with stream ID 0
        - [x] 2: Sends a WINDOW_UPDATE frame with stream ID 1
    - 3.10. CONTINUATION
        - [x] 1: Sends a CONTINUATION frame
        - [x] 2: Sends multiple CONTINUATION frames
- 4.HTTP Message Exchanges
    - [x] 1: Sends a GET request
    - [x] 2: Sends a HEAD request
    - [x] 3: Sends a POST request
    - [x] 4: Sends a POST request with trailers

## [RFC7541](https://www.rfc-editor.org/rfc/rfc7541.txt)

- 5.HPACK
    - [x] 1: Sends a indexed header field representation
    - [x] 2: Sends a literal header field with incremental indexing - indexed name
    - [x] 3: Sends a literal header field with incremental indexing - indexed name (with Huffman coding)
    - [x] 4: Sends a literal header field with incremental indexing - new name
    - [x] 5: Sends a literal header field with incremental indexing - new name (with Huffman coding)
    - [x] 6: Sends a literal header field without indexing - indexed name
    - [x] 7: Sends a literal header field without indexing - indexed name (with Huffman coding)
    - [x] 8: Sends a literal header field without indexing - new name
    - [x] 9: Sends a literal header field without indexing - new name (huffman encoded)
    - [x] 10: Sends a literal header field never indexed - indexed name
    - [x] 11: Sends a literal header field never indexed - indexed name (huffman encoded)
    - [x] 12: Sends a literal header field never indexed - new name
    - [x] 13: Sends a literal header field never indexed - new name (huffman encoded)
    - [x] 14: Sends a dynamic table size update
    - [x] 15: Sends multiple dynamic table size update

## [RFC7540](https://www.rfc-editor.org/rfc/rfc7540.txt)

- 3.Starting HTTP/2
    - 3.5. HTTP/2 Connection Preface
        - [x] 1: Sends client connection preface
        - [x] 2: Sends invalid connection preface

- 4.HTTP Frames
    - 4.1. Frame Format
        - [ ] 1: Sends a frame with unknown type
        - [ ] 2: Sends a frame with undefined flag
        - [ ] 3: Sends a frame with reserved field bit

- 4.2. Frame Size
    - [x] 1: Sends a DATA frame with 2^14 octets in length
    - [x] 2: Sends a large size DATA frame that exceeds the SETTINGS_MAX_FRAME_SIZE
    - [x] 3: Sends a large size HEADERS frame that exceeds the SETTINGS_MAX_FRAME_SIZE

- 4.3. Header Compression and Decompression
    - [x] 1: Sends invalid header block fragment
    - [x] 2: Sends a PRIORITY frame while sending the header blocks
    - [ ] 3: Sends a HEADERS frame to another stream while sending the header blocks

- 5.Streams and Multiplexing
    - 5.1. Stream States
        - [x] 1: idle: Sends a DATA frame
        - [x] 2: idle: Sends a RST_STREAM frame
        - [x] 3: idle: Sends a WINDOW_UPDATE frame
        - [x] 4: idle: Sends a CONTINUATION frame
        - [ ] 5: half closed (remote): Sends a DATA frame
        - [ ] 6: half closed (remote): Sends a HEADERS frame
        - [ ] 7: half closed (remote): Sends a CONTINUATION frame
        - [ ] 8: closed: Sends a DATA frame after sending RST_STREAM frame
        - [ ] 9: closed: Sends a HEADERS frame after sending RST_STREAM frame
        - [x] 10: closed: Sends a CONTINUATION frame after sending RST_STREAM frame
        - [ ] 11: closed: Sends a DATA frame
        - [ ] 12: closed: Sends a HEADERS frame
        - [ ] 13: closed: Sends a CONTINUATION frame
    - 5.1.1. Stream Identifiers
        - [ ] 1: Sends even-numbered stream identifier
        - [ ] 2: Sends stream identifier that is numerically smaller than previous
    - 5.1.2. Stream Concurrency
        - [ ] 1: Sends HEADERS frames that causes their advertised concurrent stream limit to be exceeded
    - 5.3. Stream Priority
        - 5.3.1. Stream Dependencies
            - [ ] 1: Sends HEADERS frame that depends on itself
            - [ ] 2: Sends PRIORITY frame that depend on itself
    - 5.4. Error Handling
        - 5.4.1. Connection Error Handling
            - [ ] 1: Sends an invalid PING frame for connection close
    - 5.5. Extending HTTP/2
        - [ ] 1: Sends an unknown extension frame
        - [ ] 2: Sends an unknown extension frame in the middle of a header block
- 6.Frame Definitions
    - 6.1. DATA
        - [x] 1: Sends a DATA frame with 0x0 stream identifier
        - [ ] 2: Sends a DATA frame on the stream that is not in "open" or "half-closed (local)" state
        - [x] 3: Sends a DATA frame with invalid pad length
    - 6.2. HEADERS
        - [x] 1: Sends a HEADERS frame without the END_HEADERS flag, and a PRIORITY frame
        - [x] 2: Sends a HEADERS frame to another stream while sending a HEADERS frame
        - [x] 3: Sends a HEADERS frame with 0x0 stream identifier
        - [ ] 4: Sends a HEADERS frame with invalid pad length
    - 6.3. PRIORITY
        - [x] 1: Sends a PRIORITY frame with 0x0 stream identifier
        - [x] 2: Sends a PRIORITY frame with a length other than 5 octets
    - 6.4. RST_STREAM
        - [x] 1: Sends a RST_STREAM frame with 0x0 stream identifier
        - [x] 2: Sends a RST_STREAM frame on a idle stream
        - [x] 3: Sends a RST_STREAM frame with a length other than 4 octets
    - 6.5. SETTINGS
        - [x] 1: Sends a SETTINGS frame with ACK flag and payload
        - [x] 2: Sends a SETTINGS frame with a stream identifier other than 0x0
        - [x] 3: Sends a SETTINGS frame with a length other than a multiple of 6 octets
        - 6.5.2. Defined SETTINGS Parameters
            - [x] 1: SETTINGS_ENABLE_PUSH (0x2): Sends the value other than 0 or 1
            - [x] 2: SETTINGS_INITIAL_WINDOW_SIZE (0x4): Sends the value above the maximum flow control window size
            - [x] 3: SETTINGS_MAX_FRAME_SIZE (0x5): Sends the value below the initial value
            - [x] 4: SETTINGS_MAX_FRAME_SIZE (0x5): Sends the value above the maximum allowed frame size
            - [ ] 5: Sends a SETTINGS frame with unknown identifier
        - 6.5.3. Settings Synchronization
            - [ ] 1: Sends multiple values of SETTINGS_INITIAL_WINDOW_SIZE
            - [x] 2: Sends a SETTINGS frame without ACK flag
    - 6.7. PING
        - [x] 1: Sends a PING frame
        - [x] 2: Sends a PING frame with ACK
        - [x] 3: Sends a PING frame with a stream identifier field value other than 0x0
        - [x] 4: Sends a PING frame with a length field value other than 8
    - 6.8. GOAWAY
        - [x] 1: Sends a GOAWAY frame with a stream identifier other than 0x0
    - 6.9. WINDOW_UPDATE
        - [x] 1: Sends a WINDOW_UPDATE frame with a flow control window increment of 0
        - [x] 2: Sends a WINDOW_UPDATE frame with a flow control window increment of 0 on a stream
        - [x] 3: Sends a WINDOW_UPDATE frame with a length other than 4 octets
        - 6.9.1. The Flow-Control Window
            - [ ] 1: Sends SETTINGS frame to set the initial window size to 1 and sends HEADERS frame
            - [x] 2: Sends multiple WINDOW_UPDATE frames increasing the flow control window to above 2^31-1
            - [x] 3: Sends multiple WINDOW_UPDATE frames increasing the flow control window to above 2^31-1 on a stream
        - 6.9.2. Initial Flow-Control Window Size
            - [ ] 1: Changes SETTINGS_INITIAL_WINDOW_SIZE after sending HEADERS frame
            - [ ] 2: Sends a SETTINGS frame for window size to be negative
            - [ ] 3: Sends a SETTINGS_INITIAL_WINDOW_SIZE settings with an exceeded maximum window size value
    - 6.10. CONTINUATION
        - [x] 1: Sends multiple CONTINUATION frames preceded by a HEADERS frame
        - [x] 2: Sends a CONTINUATION frame followed by any frame other than CONTINUATION
        - [x] 3: Sends a CONTINUATION frame with 0x0 stream identifier
        - [x] 4: Sends a CONTINUATION frame preceded by a HEADERS frame with END_HEADERS flag
        - [x] 5: Sends a CONTINUATION frame preceded by a CONTINUATION frame with END_HEADERS flag
        - [x] 6: Sends a CONTINUATION frame preceded by a DATA frame

- 7.Error Codes
    - [ ] 1: Sends a GOAWAY frame with unknown error code
    - [ ] 2: Sends a RST_STREAM frame with unknown error code

- 8.HTTP Message Exchanges
    - 8.1. HTTP Request/Response Exchange
        - [ ] 1: Sends a second HEADERS frame without the END_STREAM flag
    - 8.1.2. HTTP Header Fields
        - 1: Sends a HEADERS frame that contains the header field name in uppercase letters
            - 8.1.2.1. Pseudo-Header Fields
                - [ ] 1: Sends a HEADERS frame that contains a unknown pseudo-header field
                - [ ] 2: Sends a HEADERS frame that contains the pseudo-header field defined for response
                - [ ] 3: Sends a HEADERS frame that contains a pseudo-header field as trailers
                - [ ] 4: Sends a HEADERS frame that contains a pseudo-header field that appears in a header block after
                  a regular header field
            - 8.1.2.2. Connection-Specific Header Fields
                - [ ] 1: Sends a HEADERS frame that contains the connection-specific header field
                - [ ] 2: Sends a HEADERS frame that contains the TE header field with any value other than "trailers"
            - 8.1.2.3. Request Pseudo-Header Fields
                - [ ] 1: Sends a HEADERS frame with empty ":path" pseudo-header field
                - [ ] 2: Sends a HEADERS frame that omits ":method" pseudo-header field
                - [ ] 3: Sends a HEADERS frame that omits ":scheme" pseudo-header field
                - [ ] 4: Sends a HEADERS frame that omits ":path" pseudo-header field
                - [ ] 5: Sends a HEADERS frame with duplicated ":method" pseudo-header field
                - [ ] 6: Sends a HEADERS frame with duplicated ":scheme" pseudo-header field
                - [ ] 7: Sends a HEADERS frame with duplicated ":path" pseudo-header field
            - 8.1.2.6. Malformed Requests and Responses
                - [ ] 1: Sends a HEADERS frame with the "content-length" header field which does not equal the DATA
                  frame payload length
                - [ ] 2: Sends a HEADERS frame with the "content-length" header field which does not equal the sum of
                  the multiple DATA frames payload length
    - 8.2. Server Push
        - [ ] 1: Sends a PUSH_PROMISE frame

## HPACK: Header Compression for HTTP/2

- 2.Compression Process Overview
    - [x] 2.3. Indexing Tables
    - 2.3.3. Index Address Space
        - [x] 1: Sends a indexed header field representation with invalid index
        - [x] 2: Sends a literal header field representation with invalid index

- 4.Dynamic Table Management
    - [x] 4.2. Maximum Table Size
    - [x] 1: Sends a dynamic table size update at the end of header block

- 5.Primitive Type Representations
    - 5.2. String Literal Representation
        - [ ] 1: Sends a Huffman-encoded string literal representation with padding longer than 7 bits
        - [ ] 2: Sends a Huffman-encoded string literal representation padded by zero
        - [ ] 3: Sends a Huffman-encoded string literal representation containing the EOS symbol

- 6.Binary Format
    - 6.1. Indexed Header Field Representation
        - [x] 1: Sends a indexed header field representation with index 0
    - 6.3. Dynamic Table Size Update
        - [x] 1: Sends a dynamic table size update larger than the value of SETTINGS_HEADER_TABLE_SIZE

## Others

verify via [h2spec](https://github.com/summerwind/h2spec)