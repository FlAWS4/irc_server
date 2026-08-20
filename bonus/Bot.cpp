#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

static std::string intToString(int n)
{
    std::stringstream ss;

    ss << n;
    return (ss.str());
}

static void sendLine(int fd, const std::string &line)
{
    std::string msg;
    ssize_t     sent;

    msg = line + "\r\n";
    sent = send(fd, msg.c_str(), msg.size(), 0);
    if (sent == -1)
        std::cerr << "bot send: " << std::strerror(errno) << std::endl;
}

static int connectBot(const std::string &host, int port)
{
    int                 fd;
    struct sockaddr_in  addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        return (-1);
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
    {
        close(fd);
        return (-1);
    }
    if (connect(fd, reinterpret_cast<struct sockaddr *>(&addr),
            sizeof(addr)) == -1)
    {
        close(fd);
        return (-1);
    }
    return (fd);
}

static std::string getNickFromPrefix(const std::string &prefix)
{
    size_t bang;

    if (prefix.empty() || prefix[0] != ':')
        return ("unknown");
    bang = prefix.find('!');
    if (bang == std::string::npos)
        return (prefix.substr(1));
    return (prefix.substr(1, bang - 1));
}

static bool parsePrivmsg(const std::string &line, std::string &sender,
    std::string &target, std::string &text)
{
    size_t firstSpace;
    size_t targetStart;
    size_t secondSpace;
    size_t colon;

    if (line.empty() || line[0] != ':')
        return (false);
    firstSpace = line.find(' ');
    if (firstSpace == std::string::npos)
        return (false);
    if (line.substr(firstSpace + 1, 7) != "PRIVMSG")
        return (false);
    targetStart = firstSpace + 9;
    secondSpace = line.find(' ', targetStart);
    if (secondSpace == std::string::npos)
        return (false);
    target = line.substr(targetStart, secondSpace - targetStart);
    colon = line.find(" :", secondSpace);
    if (colon == std::string::npos)
        return (false);
    text = line.substr(colon + 2);
    sender = getNickFromPrefix(line.substr(0, firstSpace));
    return (true);
}

static std::string getTimeString()
{
    time_t      now;
    struct tm   *info;
    char        buffer[64];

    now = time(NULL);
    info = localtime(&now);
    if (info == NULL)
        return ("time unavailable");
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", info);
    return (std::string(buffer));
}

static bool startsWith(const std::string &str, const std::string &prefix)
{
    return (str.size() >= prefix.size()
        && str.substr(0, prefix.size()) == prefix);
}

static std::string getReplyTarget(const std::string &botNick,
    const std::string &sender, const std::string &target)
{
    if (target == botNick)
        return (sender);
    return (target);
}

static void handleCommand(int fd, const std::string &botNick,
    const std::string &sender, const std::string &target,
    const std::string &text)
{
    std::string replyTarget;
    int         roll;

    if (text.empty() || text[0] != '!')
        return ;
    replyTarget = getReplyTarget(botNick, sender, target);
    if (text == "!help")
    {
        sendLine(fd, "PRIVMSG " + replyTarget
            + " :Commands: !help !ping !hello !time !roll !echo <text>");
    }
    else if (text == "!ping")
        sendLine(fd, "PRIVMSG " + replyTarget + " :pong");
    else if (text == "!hello")
        sendLine(fd, "PRIVMSG " + replyTarget + " :hello " + sender);
    else if (text == "!time")
    {
        sendLine(fd, "PRIVMSG " + replyTarget
            + " :current time is " + getTimeString());
    }
    else if (text == "!roll")
    {
        roll = (std::rand() % 6) + 1;
        sendLine(fd, "PRIVMSG " + replyTarget + " :" + sender
            + " rolled " + intToString(roll));
    }
    else if (startsWith(text, "!echo "))
        sendLine(fd, "PRIVMSG " + replyTarget + " :" + text.substr(6));
    else
        sendLine(fd, "PRIVMSG " + replyTarget
            + " :unknown command. Try !help");
}

static void handleLine(int fd, const std::string &botNick,
    const std::string &line)
{
    std::string sender;
    std::string target;
    std::string text;

    std::cout << line << std::endl;
    if (line.substr(0, 4) == "PING")
    {
        if (line.size() > 5)
            sendLine(fd, "PONG " + line.substr(5));
        else
            sendLine(fd, "PONG :ircserv");
        return ;
    }
    if (parsePrivmsg(line, sender, target, text))
        handleCommand(fd, botNick, sender, target, text);
}

static void processBuffer(int fd, const std::string &botNick,
    std::string &data)
{
    size_t      pos;
    std::string line;

    while ((pos = data.find('\n')) != std::string::npos)
    {
        line = data.substr(0, pos);
        data.erase(0, pos + 1);
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (!line.empty())
            handleLine(fd, botNick, line);
    }
}

static int runBot(const std::string &host, int port,
    const std::string &password, const std::string &channel)
{
    int         fd;
    ssize_t     bytes;
    char        buffer[512];
    std::string data;
    std::string botNick;

    botNick = "ftbot";
    fd = connectBot(host, port);
    if (fd == -1)
    {
        std::cerr << "Bot: connection failed" << std::endl;
        return (1);
    }
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    sendLine(fd, "PASS " + password);
    sendLine(fd, "NICK " + botNick);
    sendLine(fd, "USER ftbot 0 * :ftbot");
    sendLine(fd, "JOIN " + channel);
    sendLine(fd, "PRIVMSG " + channel + " :ftbot online. Try !help");
    while (true)
    {
        bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
            break ;
        buffer[bytes] = '\0';
        data += buffer;
        processBuffer(fd, botNick, data);
    }
    close(fd);
    return (0);
}

int main(int argc, char **argv)
{
    int port;

    if (argc != 5)
    {
        std::cerr << "Usage: ./ircbot <host> <port> <password> <channel>"
            << std::endl;
        std::cerr << "Example: ./ircbot 127.0.0.1 6667 123 '#bot'"
            << std::endl;
        return (1);
    }
    signal(SIGPIPE, SIG_IGN);
    port = std::atoi(argv[2]);
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Bot: invalid port" << std::endl;
        return (1);
    }
    return (runBot(argv[1], port, argv[3], argv[4]));
}
