'use strict'

//
// This template for this file is heavily inspired by the 2019 TNM094 group;
// @gunnarsdotter @Anondod @ylvaselling @SimonKallberg
// When you see them, buy them a beer; they saved you about 2 weeks of work
//
const fs = require('fs');
const express = require('express');
const app = express();
const server = require('http').Server(app);
const WebSocketServer = require('websocket').server;
const assert = require('assert').strict;

//Give each player a unique id, gets incremented on every new connection
global.uniqueId = 0;

//Store all players and their id
global.playerList = new Map(); // {"ip", id}

//
//
var config = JSON.parse(fs.readFileSync('config.json'));
console.log(config.gameAddress);
console.log(config.serverAddress);
const port = config.serverPort;
const gameAddress = config.gameAddress;


app.use(express.static(__dirname + '/public'));

app.get('/', function (req, res) {
  res.sendFile(__dirname + '/public/index.html');
});

app.get('/config', function(req, res, next) {
  res.json(config);
});

server.listen(port, function () {
  console.log(`Server is listening on: ${server.address().address}:${port}`);
})

var gameSocket = null;
var connectionArray = [];

var wsServer = new WebSocketServer({ httpServer: server });
wsServer.on('request', function (req) {
  console.log(req.remoteAddress);
  if (req.remoteAddress === gameAddress) {
    console.log('Game connection established');

    gameSocket = req.accept("example-protocol", req.origin);

    gameSocket.on('message', function(msg) {
      if (msg.type === 'utf8') {
        if (msg.utf8Data === 'ping') {
          gameSocket.send('pong');
        }
      }
    });

    gameSocket.on('message', function(msg) {
      if (msg.type === 'utf8') {
        if (msg.utf8Data === 'game_connect') {
          gameSocket.send('Game connection established');
        }
      }
    });

    gameSocket.on('close', function(reason, desc) {
      console.log(`Game connection lost. Reason: ${reason}.  Description: ${desc}`);
      gameSocket = null;
    });
  }
  else {
    console.log(`Other connection from ${req.remoteAddress}`);

    const addresses = connectionArray.map(c => c.socket.remoteAddress);
    if (addresses.indexOf(req.remoteAddress) === -1 || connectionArray.length === 0) {
      // Save the connection for later use
      var connection = req.accept(null, req.origin);
      connectionArray.push(connection);

      connection.send('Connected');

      // Do something with the connection
      connection.on('message', function(msg) {
        assert(gameSocket, 'Tried to pass message to game but no connection was open');

        if (msg.type === 'utf8') {
          var temp = msg.utf8Data.split(' ');

          // Testing if first slot has value "N", if so --> send name
          if (temp[0] === "N") {
            console.log("Sending name: " + temp);

            playerList.set(connection.socket.remoteAddress, uniqueId);
            console.log(playerList);
            // gameSocket.send("N " + temp[1] + "|" + uniqueId);
            gameSocket.send(`N ${uniqueId} ${temp[1]}`);
            uniqueId++;
          }

          // Testing if first slot has value "C", if so --> send rotation data
          else if (temp[0] === "C") {
            // Test sending some rotation data from the user's mobile device
            const playerId = playerList.get(req.remoteAddress);
            gameSocket.send(`C ${playerId} ${temp[1]}`);
          }
        }
      });

      // When the connection closes
      connection.on('close', function(reason, desc) {
        const remoteAddress = connection.socket.remoteAddress;
        const addresses = connectionArray.map(c => c.socket.remoteAddress);
        connectionArray.splice(addresses.indexOf(remoteAddress), 1);
        const id = playerList.get(remoteAddress);

        // TODO Let the game know a player has disconnected
        if (playerList.delete(remoteAddress)) {
          gameSocket.send(`D ${id}`);
          console.log(`Removed player ${id} with ip ${remoteAddress}`);
        }
      });
    }
    // More connections form same device
    else {
      console.log('Same IP address connected twice');
    }
    // First connection message with game online
    if (gameSocket) {
      gameSocket.send("Remote connection from: " + req.remoteAddress);
    }
  }
});
