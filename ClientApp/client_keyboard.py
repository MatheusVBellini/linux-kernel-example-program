import socket
import struct
import os

# Server details
SERVER_HOST = '192.168.0.14'
SERVER_PORT = 50000

# File to store received information
FILE_NAME = 'log/log.txt'

def save_to_file(data):
    with open(FILE_NAME, 'a') as file:
        file.write(str(data) + '\n')

def client():
    # Create a socket object
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:

        os.remove(FILE_NAME)

        # Connect to the server
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        print(f"Connected to {SERVER_HOST}:{SERVER_PORT}")

        # Receive data from the server and save it to a file
        while True:
            received_data = client_socket.recv(4)  # Assuming 4 bytes for the keycode
            if not received_data:
                break
            keycode = struct.unpack('!i', received_data)[0]  # Unpack the binary data
            save_to_file(keycode)

    except ConnectionRefusedError:
        print("The server is not available.")

    finally:
        # Close the client socket
        client_socket.close()
        print("Connection closed.")


if __name__ == '__main__':
    client()