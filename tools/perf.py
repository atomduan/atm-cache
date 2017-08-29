#!/usr/bin/env python


#
# Still have Problem!!!
# Still have Problem!!!
# Still have Problem!!!
# Still have Problem!!!
# Still have Problem!!!

import socket

MSGLEN = 4096

class atmsocket:
    '''demonstration class only
      - coded for clarity, not efficiency
    '''

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def send(self, msg):
        totalsent = 0
        while totalsent < MSGLEN:
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

    def receive(self):
        chunks = []
        bytes_recd = 0
        while bytes_recd < MSGLEN:
            chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
            if chunk == '':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        return ''.join(chunks)


def main(argv):
    sc = atmsocket()
    sc.connect("127.0.0.1", 8088);
    while True:
        sc.send("set abc 123");
        print sc.receive();
    

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
