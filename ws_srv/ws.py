from SimpleWebSocketServer import WebSocket, SimpleWebSocketServer
import time

clients = []


class SimpleEcho(WebSocket):

    def handleMessage(self):

        if self.data is None:
            self.data = ''

        # echo message back to client
        if self.data == 'register':
            clients.append(self)
            print(self.address, 'registered')
            time.sleep(1)
            self.sendMessage("sure thing buddy")
        else:
            for client in clients:
                try:
                    client.sendMessage(str(self.data))
                except Exception:
                    print("Dead flies")

    def handleConnected(self):
        print(self.address, 'connected')

    def handleClose(self):
        print(self.address, 'closed')


server = SimpleWebSocketServer('0.0.0.0', 8000, SimpleEcho)
server.serveforever()
