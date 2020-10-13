import sys
import socket
import time
from multiprocessing import Process

# function to send partial request, sleep for 5 seconds, then complete request
def send_partial_request():
    port_num = int(sys.argv[1])

    # open connection to socket
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(('127.0.0.1', port_num))

    # send first part of request
    print('Sending first part')
    partial_request = 'GET /echo HTTP/1.1\r\nHost: www.partial.com\r\n'
    client.send(partial_request.encode())

    # sleep for 5 seconds
    time.sleep(5)

    # send remainder of request
    print('Sending remainder')
    finished_request = '\r\n'
    client.send(finished_request.encode())

    response = client.recv(1024)
    http_response = repr(response)

    print(http_response)


# function to send complete request and get immediate response
def send_complete_request():
    port_num = int(sys.argv[1])

    # open connection to socket
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(('127.0.0.1', port_num))

    # send complete request
    print('Sending complete request')
    complete_request = 'GET /echo HTTP/1.1\r\nHost: www.complete.com\r\n\r\n'
    client.send(complete_request.encode())

    response = client.recv(1024)
    http_response = repr(response)

    print(http_response)


# spawn two processes, send partial request, send complete request, then finish first request
if __name__ == '__main__':
    try:
        partial_process = Process(target=send_partial_request)
        partial_process.start()

        # make sure to send complete request after partial request starts sending
        time.sleep(1)

        complete_process = Process(target=send_complete_request)
        complete_process.start()

        partial_process.join()
        complete_process.join()
    except:
        print('An exception occurred.')
        sys.exit(1)

    sys.exit(0)