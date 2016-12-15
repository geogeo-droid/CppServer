/*!
    \file server.h
    \brief TCP server definition
    \author Ivan Shynkarenka
    \date 14.12.2016
    \copyright MIT License
*/

#ifndef CPPSERVER_TCP_SERVER_H
#define CPPSERVER_TCP_SERVER_H

#include "errors/fatal.h"
#include "system/uuid.h"

#include "../asio.h"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace CppServer {

//! TCP server
/*!
    TCP server is used to connect, disconnect and manage TCP sessions.
    It is implemented based on Asio C++ Library and use a separate
    thread to perform all TCP communications.

    Not thread-safe.
*/
template <class TServer, class TSession>
class TCPServer
{
    template <class TSomeServer, class TSomeSession>
    friend class TCPSession;

public:
    //! Initialize TCP server with a given protocol and port number
    /*!
        \param protocol - Protocol type
        \param port - Port number
    */
    explicit TCPServer(InternetProtocol protocol, uint16_t port);
    //! Initialize TCP server with a given IP address and port number
    /*!
        \param address - IP address
        \param port - Port number
    */
    explicit TCPServer(const std::string& address, uint16_t port);
    TCPServer(const TCPServer&) = delete;
    TCPServer(TCPServer&&) noexcept = default;
    virtual ~TCPServer() { Stop(); }

    TCPServer& operator=(const TCPServer&) = delete;
    TCPServer& operator=(TCPServer&&) noexcept = default;

    //! Is server started?
    bool IsStarted() const noexcept { return _started; }

    //! Start server
    void Start();
    //! Stop server
    void Stop();

protected:
    //! Initialize thread handler
    /*!
         This handler can be used to initialize priority or affinity of the server thread.
    */
    virtual void onThreadInitialize() {}
    //! Cleanup thread handler
    /*!
         This handler can be used to cleanup priority or affinity of the server thread.
    */
    virtual void onThreadCleanup() {}

    //! Handle server starting notification
    virtual void onStarting() {}
    //! Handle server started notification
    virtual void onStarted() {}

    //! Handle server stopping notification
    virtual void onStopping() {}
    //! Handle server stopped notification
    virtual void onStopped() {}

    //! Handle new session connected notification
    /*!
        \param session - Connected session
    */
    virtual void onConnected(std::shared_ptr<TSession> session) {}
    //! Handle session disconnected notification
    /*!
        \param session - Disconnected session
    */
    virtual void onDisconnected(std::shared_ptr<TSession> session) {}

    //! Handle error notification
    /*!
        \param error - Error code
        \param category - Error category
        \param message - Error message
    */
    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    std::atomic<bool> _started;
    std::thread _thread;
    asio::io_service _service;
    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
    std::map<CppCommon::UUID, std::shared_ptr<TSession>> _sessions;
    std::mutex _sessions_lock;

    //! Server accept
    void ServerAccept();
    //! Server loop
    void ServerLoop();

    //! Register a new session
    std::shared_ptr<TSession> RegisterSession();
    //! Unregister the given session
    void UnregisterSession(const CppCommon::UUID& id);
};

/*! \example tcp_server.cpp TCP server example */

} // namespace CppServer

#include "server.inl"

#endif // CPPSERVER_TCP_SERVER_H