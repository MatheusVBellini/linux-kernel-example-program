import socket
import struct
import os

# Server details
SERVER_HOST = '192.168.0.14'
SERVER_PORT = 50050
VIDEO_MEMORY_SIZE = (1024 * 1024)  # 1 MB

# File to store received information
FILE_NAME = 'log/video_log.txt'

def save_to_file(data):
    with open(FILE_NAME, 'a') as file:
        for number in data:
            file.write(str(number) + '\n')

def client():
    # Create a socket object
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to the server
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        print(f"Connected to {SERVER_HOST}:{SERVER_PORT}")

        if os.path.exists(FILE_NAME):
            os.remove(FILE_NAME)

        # Receive data from the server and save it to a file

        while True:
            received_data = bytes()
            bytes_received = 0

            chunk = client_socket.recv(VIDEO_MEMORY_SIZE)
            received_data += chunk
            bytes_received += len(chunk)

            # Unpack the received data
            unpacked_data = struct.unpack('B' * bytes_received, received_data)
            save_to_file(unpacked_data)
            save_to_file("SNAPSHOT")

    except ConnectionRefusedError:
        print("The server is not available.")

    finally:
        # Close the client socket
        client_socket.close()
        print("Connection closed.")


if __name__ == '__main__':
    client()
