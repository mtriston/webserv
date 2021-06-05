#ifndef ASOCKET_HPP
# define ASOCKET_HPP

#include <sys/types.h>

class ConfigParser;

class IWork;

class ASocket {
protected:
    int socket_;
    int port_;
    ConfigParser *config_;
    bool isBusy_;

    ASocket();

    ASocket(ASocket const &);

public:
    ASocket(int socket, int port, ConfigParser *config);

    virtual ~ASocket();

    int getSocket() const;

    int getPort() const;

    ConfigParser *getConfig() const;

    void setIsBusy(bool);

    virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;

    virtual bool isReady(fd_set *readfds, fd_set *writefds) = 0;

    virtual IWork *getWork() = 0;
};

#endif
