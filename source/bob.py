import socket
import time
import re

SERVER = "127.0.0.1"
PORT = 6667
NICK = "bob"

def send(sock, msg):
    sock.sendall((msg + "\n").encode())

def recv_line(sock):
    data = b""
    while not data.endswith(b"\n"):
        data += sock.recv(1)
    line = data.decode(errors="ignore").strip()
    return line

if NICK == "alice":
    time.sleep(2)
elif NICK == "bob":
    time.sleep(10)

sock = socket.create_connection((SERVER, PORT))

sock.recv(4096)


time.sleep(1)

send(sock, f"{NICK}")


JOIN_RE = re.compile(r"^\*\*\* ([^\s]+) joined the channel$")
MSG_RE = re.compile(r"^\[(?:ADMIN)?\[?([^\]]+)\]?\]?\s+(.*)$")
target = None
conversation_started = False
conversation_finished = False
first_login = True

while True:
    line = recv_line(sock)

    line = line.strip()

    # 1) Detect joins
    m = JOIN_RE.match(line)
    if m:
        nick = m.group(1)

        # Ignore self
        if nick == "bob":
            time.sleep(2)
            if first_login == False:
                send(sock, f"lmao why did you chose the same nickname as me? lowkey cringe")
            first_login = False
        

        else:
            target = nick
            time.sleep(6)
            send(sock, f"who are you {target}")

        continue

    # 2) (Optional) react to messages from target
    m = MSG_RE.match(line)
    if m:
        nick, msg = m.groups()

        if nick == target:
            print(f"[target said]: {msg}")
            # time.sleep(2)
            # send(sock, f"\"{msg[:20]}...\", damn {nick} I can't remember who asked??")

        elif nick == "alice" and not(conversation_finished):
            time.sleep(3)
            send(sock, f"hey not much whatsup with u")
            time.sleep(6)
            time.sleep(7)
            send(sock, f"i can solve this chall")
            time.sleep(8)
            send(sock, f"...")
            conversation_finished = True

            

    time.sleep(0.05)