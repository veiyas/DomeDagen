/*****************************************************************************************
 * WebSocket Handler                                                                     *
 *                                                                                       *
 * Copyright (c) Alexander Bock 2020                                                     *
 ****************************************************************************************/

#include "websockethandler.h"

#include "libwebsockets.h"
#include <assert.h>
#include <exception>
#include <mutex>
#include <string_view>
#include <vector>

struct WebSocketHandlerImpl {
    std::string address;
    int port = 0;

    std::mutex messageMutex;
    std::vector<std::string> messageQueue;

    std::function<void()> connectionEstablished;
    std::function<void(const void*, size_t)> messageReceived;
    std::function<void()> connectionClosed;

    lws_context* context;
};

int callback(lws* wsi, lws_callback_reasons reason, void*, void* in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        {
            WebSocketHandlerImpl* pImpl = reinterpret_cast<WebSocketHandlerImpl*>(
                lws_get_protocol(wsi)->user
            );
            pImpl->connectionEstablished();
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
        {
            WebSocketHandlerImpl* pImpl = reinterpret_cast<WebSocketHandlerImpl*>(
                lws_get_protocol(wsi)->user
            );
            pImpl->messageReceived(in, len);
            break;
        }
        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            WebSocketHandlerImpl* pImpl = reinterpret_cast<WebSocketHandlerImpl*>(
                lws_get_protocol(wsi)->user
            );
            std::lock_guard lock(pImpl->messageMutex);
            if (pImpl->messageQueue.empty()) {
                break;
            }

            // Get a copy of first message from the queue
            std::string msg = pImpl->messageQueue.front();
            pImpl->messageQueue.erase(pImpl->messageQueue.begin());

            // Send the message
            lws_write(
                wsi,
                reinterpret_cast<unsigned char*>(msg.data()),
                msg.size(),
                LWS_WRITE_TEXT
            );
            break;
        }
        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        {
            WebSocketHandlerImpl* pImpl = reinterpret_cast<WebSocketHandlerImpl*>(
                lws_get_protocol(wsi)->user
            );
            pImpl->connectionClosed();
            pImpl->context = nullptr;
            return -1; // close the connection
        }
        default:
            break;
    }

    return 0;
}

WebSocketHandler::WebSocketHandler(std::string address, int port,
                                   std::function<void()> connectionEstablished,
                                   std::function<void()> connectionClosed,
                                   std::function<void(const void*, size_t)> msgReceived)
{
    // Check whether there is a valid address
    assert(!address.empty());

    // Check whether the port is a valid (>0) number
    assert(port > 0);

    // Check whether the callbacks provided are non-empty
    assert(connectionEstablished);
    assert(msgReceived);

    _pImpl = std::make_unique<WebSocketHandlerImpl>();
    _pImpl->address = std::move(address);
    _pImpl->port = port;
    _pImpl->connectionEstablished = std::move(connectionEstablished);
    _pImpl->connectionClosed = std::move(connectionClosed);
    _pImpl->messageReceived = std::move(msgReceived);
}

WebSocketHandler::~WebSocketHandler() {}

bool WebSocketHandler::connect(std::string protocolName, int bufferSize) {
    assert(bufferSize >= 0);

    lws_context_creation_info info;
    std::memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN;

    // This is a bit ugly;  we pass in the address to the callbacks structure so that we
    // can access it as the user pointer of the protocol inside the callback function
    // whenever something interesting happens in the websocket connection
    const size_t bufSize = static_cast<size_t>(bufferSize);
    const lws_protocols protocols[] = {
        { protocolName.c_str(), callback, 0, bufSize, 0, _pImpl.get() },
        { nullptr, nullptr, 0, 0, 0, nullptr } // terminal value
    };

    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    _pImpl->context = lws_create_context(&info);
    lws_client_connect_info ccinfo;
    std::memset(&ccinfo, 0, sizeof(ccinfo));
    ccinfo.context = _pImpl->context;
    ccinfo.address = _pImpl->address.c_str();
    ccinfo.port = _pImpl->port;
    ccinfo.path = "/";
    ccinfo.host = lws_canonical_hostname(_pImpl->context);
    ccinfo.origin = "origin";
    ccinfo.protocol = protocols[0].name;

    lws* connection = lws_client_connect_via_info(&ccinfo);
    return connection != nullptr;
}

void WebSocketHandler::tick() {
    lws_service(_pImpl->context, 0);
}

void WebSocketHandler::queueMessage(std::string message) {
    std::lock_guard lock(_pImpl->messageMutex);
    _pImpl->messageQueue.push_back(std::move(message));
}

int WebSocketHandler::queueSize() const {
    std::lock_guard lock(_pImpl->messageMutex);
    return static_cast<int>(_pImpl->messageQueue.size());
}
