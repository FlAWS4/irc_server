# Bonus IRC Bot

This directory contains a simple IRC bot used as a bonus client for the `ft_irc` project.

The bot is a separate executable. It connects to the IRC server like a normal client, registers with `PASS`, `NICK`, and `USER`, joins one channel, listens for `PRIVMSG`, and replies to commands that start with `!`.

It does not change the mandatory server behavior.

---

## Build

From the project root:

```bash
make bonus
```

This builds both executables:

```text
ircserv
ircbot
```

The normal mandatory build still works with:

```bash
make
```

---

## Run

Start the server first:

```bash
./ircserv 6667 123
```

In another terminal, start the bot:

```bash
./ircbot 127.0.0.1 6667 123 '#bot'
```

Arguments:

```text
./ircbot <host> <port> <password> <channel>
```

Example:

```text
host     = 127.0.0.1
port     = 6667
password = 123
channel  = #bot
```

---

## Test with netcat

Open another client:

```bash
nc -C 127.0.0.1 6667
```

Register and join the bot channel:

```irc
PASS 123
NICK ab
USER ab 0 * :AB
JOIN #bot
```

Send bot commands:

```irc
PRIVMSG #bot :!help
PRIVMSG #bot :!ping
PRIVMSG #bot :!hello
PRIVMSG #bot :!time
PRIVMSG #bot :!roll
PRIVMSG #bot :!echo bonus works
```

---

## Commands

| Command | Description |
|--------|-------------|
| `!help` | Lists available bot commands |
| `!ping` | Replies with `pong` |
| `!hello` | Greets the sender |
| `!time` | Shows the bot machine local time |
| `!roll` | Rolls a random number from 1 to 6 |
| `!echo <text>` | Sends the text back to the channel |

---

## Implementation notes

The bot follows the same IRC protocol rules as a normal client:

- it connects using a TCP socket;
- it sends IRC messages terminated by `\r\n`;
- it registers with `PASS`, `NICK`, and `USER`;
- it joins a channel with `JOIN`;
- it answers server `PING` messages with `PONG`;
- it reads server messages using `recv`;
- it parses complete IRC lines from the received buffer;
- it handles only `PRIVMSG` commands whose message text starts with `!`.

The bot is intentionally simple so that the mandatory server stays stable.
