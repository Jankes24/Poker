#pragma once
#include "Asio/asio.hpp"
#include <iostream>
#include <deque>

class Server {
public:
    Server(asio::io_context& io, const std::string& host, const std::string& port)
        : io(io), resolver(io), socket(io), host(host), port(port) {
    }

    void start() {
        resolver.async_resolve(host, port, [this](auto ec, auto results) {
                if (!ec) { connect(results); }
                else { std::cerr << "Resolve failed: " << ec.message() << "\n"; }});
    }

    void send(const std::string& msg) {
        asio::post(io, [this, msg]() {
            bool writing = !write_msgs.empty();
            write_msgs.push_back(msg);
            if (!writing) { do_write(); }});
    }

private:
    void connect(const asio::ip::tcp::resolver::results_type& endpoints) {
        asio::async_connect(socket, endpoints,
            [this](auto ec, auto) {
                if (!ec) { std::cout << "Connected!\n"; do_read(); }
                else { std::cerr << "Connect failed: " << ec.message() << "\n"; }
            });
    }

    void do_read() {
        socket.async_read_some(asio::buffer(read_buf),
            [this](auto ec, auto len) {
                if (!ec) {
                    std::cout << "Server: " << std::string(read_buf.data(), len) << "\n";
                    do_read();
                }
            });
    }

    void do_write() {
        asio::async_write(socket,
            asio::buffer(write_msgs.front()),
            [this](auto ec, auto) {
                if (!ec) {
                    write_msgs.pop_front();
                    if (!write_msgs.empty()) { do_write(); }
            }});
    }

    asio::io_context& io;
    asio::ip::tcp::resolver resolver;
    asio::ip::tcp::socket socket;
    std::string host, port;

    std::array<char, 1024> read_buf;
    std::deque<std::string> write_msgs;
};
