import socket
import time
import re

SERVER = "127.0.0.1"
PORT = 6667
NICK = "alice"

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
first_login = True

while True:
    line = recv_line(sock)
    line = line.strip()

    # 1) Detect joins
    m = JOIN_RE.match(line)
    if m:
        nick = m.group(1)

        # Ignore self
        if nick == "alice":
            time.sleep(2)
            if first_login == False:
                send(sock, f"lmao why did you chose the same nickname as me? lowkey cringe")
            first_login = False
        
        elif nick == "bob":
            time.sleep(3)
            send(sock, f"heeey bob whatsup")
            time.sleep(9)
            send(sock, f"not much")
            time.sleep(3)
            send(sock, f"hey did u know one out of 3 people cant solve this chall")
            time.sleep(4)
            time.sleep(4)
            send(sock, f"me too")

        else:
            conversation_started = True
            target = nick

            time.sleep(2)
            send(sock, f"{target} new guy in town? u play fortnite?")

        continue

    # 2) (Optional) react to messages from target
    m = MSG_RE.match(line)
    if m:
        nick, msg = m.groups()

        if nick == target:
            print(f"[target said]: {msg}")
            # time.sleep(2)
            # send(sock, f"\"{msg[:20]}...\", damn {nick} I can't remember who asked??")


            

    time.sleep(0.05)