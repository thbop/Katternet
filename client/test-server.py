import socket
from threading import Thread


def connection( conn: socket.socket, conn_addr):
    print(f'Established connection from {conn_addr[0]}:{conn_addr[1]}.')
    while True:
        try:
            data: bytes = conn.recv(1024)
            if not data: break
            print(f'{conn_addr[0]}:{conn_addr[1]}:', data)
            conn.send(b'100')
        except ConnectionAbortedError:
            print(f'Disconnected from {conn_addr[0]}:{conn_addr[1]}.')
            break
        except ConnectionResetError:
            print(f'Connection reset error from {conn_addr[0]}:{conn_addr[1]}.')
            break
    
    conn.close()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('127.0.0.1', 6410))

print('Listening...')
while True:
    sock.listen()
    conn, conn_addr = sock.accept()
    Thread(target=connection, args=(conn, conn_addr)).start()