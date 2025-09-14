#pragma once
#include "Asio/asio.hpp"
#include <iostream>

class Server
{
    asio::io_context io;
    asio::ip::tcp::acceptor acceptor;

public:
    Server(short port) : acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

    void Start()
    {
        Accept();
        io.run();
    }

private:
    void Accept()
    {
        auto socket = std::make_shared<asio::ip::tcp::socket>(io);
        acceptor.async_accept(*socket, [this, socket](std::error_code ec)
        {
            if (!ec)
            {
                std::cout << "New connection from: " << socket->remote_endpoint() << "\n";
                auto msg = std::make_shared<std::string>("Hello!\n");
                asio::async_write(*socket, asio::buffer(*msg),
                [socket, msg](std::error_code ec, std::size_t length)
                {
                    if (!ec)
                        std::cout << "Sent\n";
                });
            }
            Accept();
        });
    }
};

class Client
{
    asio::io_context io;
    asio::ip::tcp::socket socket{ io };

public:
    Client(const std::string& host, short port)
    {
        asio::ip::tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket, endpoints);
        std::cout << "Connected to server!\n";
    }

    void Run()
    {
        std::string msg = "Hello from Client!\n";
        asio::write(socket, asio::buffer(msg));
        char buf[128];
        size_t len = socket.read_some(asio::buffer(buf));
        std::cout << "Server says: " << std::string(buf, len) << "\n";
    }
};