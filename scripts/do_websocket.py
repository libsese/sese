import websocket
import sys


def test(port: str):
    ws = websocket.WebSocket()
    ws.connect("ws://localhost:{}/chat".format(port))
    ws.send("Hello")
    buf = ws.recv()
    print(buf)
    ws.close()


if __name__ == '__main__':
    p = sys.argv[1]
    test(p)
