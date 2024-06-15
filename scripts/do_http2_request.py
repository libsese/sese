import os
import sys
from http.client import HTTPConnection


def on_nt(p: int):
    conn = HTTPConnection('127.0.0.1', p)
    conn.request('GET', '/')
    resp = conn.getresponse()
    print(resp.status)
    print(resp.headers)
    print(resp.read())


def on_unix(p: int):
    os.system('curl --http2 http://127.0.0.1:{} -v -s'.format(p))


port = int(sys.argv[1])
if os.name == 'nt':
    on_nt(port)
else:
    on_unix(port)
