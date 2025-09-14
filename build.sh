#!/bin/sh
g++ -std=c++20 -Wno-return-type $(find Poker -name '*.cpp') -I Poker/include -I Poker/include/Asio -o PokerApp -lglfw -lGL -ldl -lpthread -lm
