/*****************************************************************************************
 * WebSocket Handler                                                                     *
 *                                                                                       *
 * Copyright (c) Alexander Bock 2020                                                     *
 ****************************************************************************************/

#ifndef __WEBSOCKETHANDLER_H__
#define __WEBSOCKETHANDLER_H__

#include <functional>
#include <memory>
#include <string>

struct WebSocketHandlerImpl;

class WebSocketHandler {
public:
    WebSocketHandler(std::string address, int port,
        std::function<void()> connectionEstablished,
        std::function<void()> connectionClosed,
        std::function<void(const void*, size_t)> messageReceived);

    ~WebSocketHandler();

    bool connect(std::string protocolName, int bufferSize);
    void tick();

    void queueMessage(std::string message);
    int queueSize() const;

private:
    std::unique_ptr<WebSocketHandlerImpl> _pImpl;
};

#endif // __WEBSOCKETHANDLER_H__
