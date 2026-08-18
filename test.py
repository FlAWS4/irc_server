#!/usr/bin/env python3
import socket
import subprocess
import time
import os
import signal
import sys

PORT = 6677
PASSWORD = "changeme"
SERVER = "./ircserv"

class IRCClient:
    def __init__(self, name):
        self.name = name
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(0.4)
        self.sock.connect(("127.0.0.1", PORT))
        self.buffer = ""

    def send(self, line):
        self.sock.sendall((line + "\r\n").encode())

    def read_all(self, delay=0.15):
        time.sleep(delay)
        out = ""
        while True:
            try:
                data = self.sock.recv(4096)
                if not data:
                    break
                out += data.decode(errors="replace")
            except socket.timeout:
                break
            except OSError:
                break
        self.buffer += out
        return out

    def expect(self, text, label):
        out = self.read_all()
        if text not in out:
            print("\n[FAIL]", label)
            print("Expected fragment:", repr(text))
            print("Got:", repr(out))
            raise SystemExit(1)
        print("[PASS]", label)

    def expect_no(self, text, label):
        out = self.read_all()
        if text in out:
            print("\n[FAIL]", label)
            print("Unexpected fragment:", repr(text))
            print("Got:", repr(out))
            raise SystemExit(1)
        print("[PASS]", label)

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def wait_server():
    for _ in range(30):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(0.2)
            s.connect(("127.0.0.1", PORT))
            s.close()
            return
        except OSError:
            time.sleep(0.1)
    raise RuntimeError("server did not start")


def register(c, nick, user):
    c.send("PASS " + PASSWORD)
    c.send("NICK " + nick)
    c.send("USER " + user + " 0 * :" + user)
    c.expect(":ircserv 001 " + nick, "register " + nick)


def main():
    print("[INFO] building...")
    if subprocess.call(["make", "fclean"]) != 0:
        raise SystemExit("[FAIL] make fclean failed")
    if subprocess.call(["make"]) != 0:
        raise SystemExit("[FAIL] make failed")

    print("[INFO] starting server...")
    server = subprocess.Popen(
        [SERVER, str(PORT), PASSWORD],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    try:
        wait_server()

        # DAY 1 / BASIC SERVER + PING + UNKNOWN + QUIT
        c0 = IRCClient("c0")
        c0.send("HELLO")
        c0.expect(":ircserv 421 * HELLO :Unknown command", "unknown command before registration")

        c0.send("PING")
        c0.expect(":ircserv 409 * :No origin specified", "PING missing origin")

        c0.send("PING :123")
        c0.expect("PONG :123", "PING with origin")

        c0.send("QUIT")
        time.sleep(0.2)
        c0.close()
        print("[PASS] QUIT disconnect basic")

        # DAY 2 / REGISTRATION
        a = IRCClient("abrar")

        a.send("PASS wrong")
        a.expect(":ircserv 464 * :Password incorrect", "wrong PASS")

        a.send("PASS")
        a.expect(":ircserv 461 * PASS :Not enough parameters", "PASS missing param")

        a.send("NICK")
        a.expect(":ircserv 431 * :No nickname given", "NICK missing param")

        a.send("NICK #bad")
        a.expect(":ircserv 432 * #bad :Erroneous nickname", "bad nickname")

        register(a, "abrar", "abrar")

        a.send("PASS changeme")
        a.expect(":ircserv 462 abrar :You may not reregister", "PASS after registration")

        a.send("USER abrar 0 * :Again")
        a.expect(":ircserv 462 abrar :You may not reregister", "USER after registration")

        b = IRCClient("bob")
        b.send("PASS changeme")
        b.send("NICK abrar")
        b.expect(":ircserv 433 * abrar :Nickname is already in use", "duplicate nickname")

        b.send("NICK bob")
        b.send("USER bob 0 * :Bob")
        b.expect(":ircserv 001 bob", "register bob")

        # DAY 3 / JOIN
        a.send("JOIN")
        a.expect(":ircserv 461 abrar JOIN :Not enough parameters", "JOIN missing param")

        a.send("JOIN room")
        a.expect(":ircserv 403 abrar room :No such channel", "JOIN invalid channel name")

        a.send("JOIN #test")
        a.expect(":abrar!abrar@127.0.0.1 JOIN #test", "abrar JOIN #test")

        a.send("JOIN #test")
        a.expect_no("No such", "duplicate JOIN does not error")

        b.send("JOIN #test")
        b.expect(":bob!bob@127.0.0.1 JOIN #test", "bob JOIN #test")

        # DAY 4 / PRIVMSG
        u = IRCClient("unreg")
        u.send("PRIVMSG bob :hello")
        u.expect(":ircserv 451 * :You have not registered", "PRIVMSG before registration")
        u.close()

        a.send("PRIVMSG")
        a.expect(":ircserv 411 abrar :No recipient given (PRIVMSG)", "PRIVMSG missing recipient")

        a.send("PRIVMSG bob")
        a.expect(":ircserv 412 abrar :No text to send", "PRIVMSG missing text")

        a.send("PRIVMSG ghost :hello")
        a.expect(":ircserv 401 abrar ghost :No such nick/channel", "PRIVMSG unknown nick")

        a.send("PRIVMSG #ghost :hello")
        a.expect(":ircserv 403 abrar #ghost :No such channel", "PRIVMSG unknown channel")

        c = IRCClient("charlie")
        register(c, "charlie", "charlie")

        c.send("PRIVMSG #test :hello")
        c.expect(":ircserv 404 charlie #test :Can't send to channel", "PRIVMSG channel while not joined")

        a.send("PRIVMSG bob :direct hello")
        b.expect(":abrar!abrar@127.0.0.1 PRIVMSG bob :direct hello", "direct PRIVMSG delivered")

        b.send("PRIVMSG #test :room hello")
        a.expect(":bob!bob@127.0.0.1 PRIVMSG #test :room hello", "channel PRIVMSG delivered")

        # DAY 5 / KICK
        b.send("KICK #test abrar :lol")
        b.expect(":ircserv 482 bob #test :You're not channel operator", "non-op cannot KICK")

        a.send("KICK #ghost bob :x")
        a.expect(":ircserv 403 abrar #ghost :No such channel", "KICK unknown channel")

        a.send("KICK #test ghost :x")
        a.expect(":ircserv 401 abrar ghost :No such nick/channel", "KICK unknown nick")

        a.send("KICK #test charlie :x")
        a.expect(":ircserv 441 abrar charlie #test :They aren't on that channel", "KICK user not in channel")

        a.send("KICK #test bob :bye")
        a.expect(":abrar!abrar@127.0.0.1 KICK #test bob :bye", "KICK visible to operator")
        b.expect(":abrar!abrar@127.0.0.1 KICK #test bob :bye", "KICK visible to kicked user")

        b.send("PRIVMSG #test :am I still here")
        b.expect(":ircserv 404 bob #test :Can't send to channel", "kicked user removed from channel")

        b.send("JOIN #test")
        b.expect(":bob!bob@127.0.0.1 JOIN #test", "bob can rejoin after KICK")

        b.send("KICK #test abrar :again")
        b.expect(":ircserv 482 bob #test :You're not channel operator", "rejoined user is still not op")

        print("\nALL DAY 1-5 TESTS PASSED")

        a.close()
        b.close()
        c.close()

    finally:
        try:
            server.terminate()
            server.wait(timeout=1)
        except Exception:
            try:
                server.kill()
            except Exception:
                pass


if __name__ == "__main__":
    main()