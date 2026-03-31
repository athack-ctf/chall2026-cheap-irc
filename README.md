# @Hack 2026: cHeap IRC

> Authored by [Hugo](https://github.com/hkerma).

- **Category**: `Pwn`
- **Solves**: `25/120`
- **Tags**: `sponsored`
- **Protocol**: `tcp`

> POV: it's 2004
>
> I build a little IRC for me and my friend 'cause Discord hasn't been invented yet.
>
> We're gonna hang out there right now, come check around!
>

## Files

- **[Download: server.c](https://github.com/athack-ctf/chall2026-cheap-irc/raw/refs/heads/main/offline-artifacts/server.c)**

## Access a dockerized instance

Run challenge container using docker compose

```
docker compose up -d
```

Connect using netcat

```
nc localhost 53002
```

<details>
<summary>
How to stop/restart challenge?
</summary>

To stop the challenge run

```
docker compose stop
```

To restart the challenge run

```
docker compose restart
```

</details>

## Reveal Flag(s)

Did you try solving this challenge?
<details>
<summary>
Yes
</summary>

Did you **REALLY** try solving this challenge?

<details>
<summary>
Yes, I promise!
</summary>

- Flag 1: `ATHACKCTF{ReallyBroThatsUrIRC???}`

</details>
</details>


---

## About @Hack

[@Hack](https://athackctf.com/) is an annual CTF (Capture The Flag) competition hosted
by [HEXPLOIT ALLIANCE](https://hexploit-alliance.com/) and [TECHNATION](https://technationcanada.ca/) at Concordia
University in Montreal, Canada.

---
[Check more challenges from @Hack 2026](https://github.com/athack-ctf/AtHackCTF-2026-Challenges).
