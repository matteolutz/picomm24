import socket
import time
import math
import wave
import struct
from scipy.io import wavfile as wave

DEST_IP = "255.255.255.255"
DEST_PORT = 8192

SAMPLE_RATE = 48000
PACKET_SAMPLE_COUNT = 128
PACKET_RATE = SAMPLE_RATE / PACKET_SAMPLE_COUNT

def send_sine(sock: socket.socket):
    while True:
        start = time.time()
        buffer = bytearray([0xFF])
        for i in range(PACKET_SAMPLE_COUNT):
            buffer.append(int(math.sin(i / PACKET_SAMPLE_COUNT * 2 * math.pi) * 127 + 128))

        sock.sendto(buffer, (DEST_IP, DEST_PORT))
        took = time.time() - start
        time.sleep(max(0, (1 / PACKET_RATE) - took))

def send_wav(sock: socket.socket, wav_file: str):
    _, data = wave.read(wav_file)

    left_channel = data[:, 0] if data.ndim > 1 else data

    buffer = bytearray([0xFF])
    started = time.time()
    # iterate over all frames in the wav file
    while True:
        for val in left_channel:
            # val is a signed 16 bit integer, we need to convert it to unsigned 8 bit
            unsigned_val = int((int(val) + 32768) * 255 / 65535)

            buffer.append(unsigned_val)

            if len(buffer) >= PACKET_SAMPLE_COUNT + 1:
                sock.sendto(buffer, (DEST_IP, DEST_PORT))
                buffer = bytearray([0xFF])
                took = time.time() - started
                time.sleep(max((1 / PACKET_RATE) - took, 0))
                started = time.time()

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    send_wav(sock, "piano2.wav")

if __name__ == "__main__":
    main()