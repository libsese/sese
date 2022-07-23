# Socket

## ~~Option~~（已弃用）

- 选项对照表

  | Socket::Option               | ::setsockopt |
  | ---------------------------- | ------------ |
  | REUSE_ADDRESS[[0]](#jmp0)    | SO_REUSEADDR |
  | LINGER[[1]](#jmp1)           | SO_LINGER    |
  | SEND_TIMEOUT[[2]](#jmp2)     | SO_SNDTIMEO  |
  | RECV_TIMEOUT[[3]](#jmp3)     | SO_RCVTIMEO  |
  | SEND_BUFFER_SIZE[[4]](#jmp4) | SO_SNDBUF    |
  | RECV_BUFFER_SIZE[[5]](#jmp5) | SO_RCVBUF    |
  | KEEPALIVE[[6]](#jmp6)        | SO_KEEPALIVE |
  | OOB_INLINE[[7]](#jmp7)       | SO_OOBINLINE |
  | BROADCAST[[8]](#jmp8)        | SO_BROADCAST |
  | DEBUG[[9]](#jmp9)            | SO_DEBUG     |

- 选项释义

  <span id="jmp0"/>

  - REUSE_ADDRESS：允许端口被释放后可被立即重新使用、多个Socket不同 IP 的同一端口，UDP协议可绑定同一IP的同一端口进行多播。

  <span id="jmp1"/>

  - LINGER：发送延迟选项，Windows 和 Linux 等平台可能含义不一致，请参考[MSDN](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-setsockopt)和[Linux Manual Pages](https://www.man7.org/linux/man-pages/man7/socket.7.html)。

  <span id="jmp2"/>

  - SEND_TIMEOUT：发送超时时间

  <span id="jmp3"/>

  - RECV_TIMEOUT：接收超时时间

  <span id="jmp4"/>

  - SEND_BUFFER_SIZE：发送缓冲区大小，Windows 每个版本默认值不一致

  <span id="jmp5"/>

  - RECV_BUFFER_SIZE：接收缓冲区大小，Windows 每个版本默认值不一致

  <span id="jmp6"/>

  - KEEPALIVE：启用 TCP 内嵌心跳包

  <span id="jmp7"/>

  - OOB_INLINE：指示外绑定数据应与常规数据一起返回，此选项仅对支持带外数据的面向连接的协议有效

  <span id="jmp8"/>

  - BROADCAST：配置发送广播数据的套接字，此选项仅对支持广播的协议(如IPX和UDP)有效

  <span id="jmp9"/>

  - DEBUG：启用调式输出