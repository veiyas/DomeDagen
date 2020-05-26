
var serverAddress;
var returningPlayerUserName ="";
//Request server adress when document is loaded
$($.ajax({
  url: '/config',
  complete: function(data) {
    console.log(data.responseJSON.serverAddress);
    serverAddress = data.responseJSON.serverAddress + ":" + data.responseJSON.serverPort;

    initialize();
  }
}));

// const serverAddress = 'ws://192.168.10.225:81/';
var socket;
var screens = new Map();
const nameLimit = 20;
var connected = false;

function log(msg) {
    document.getElementById('debug-output').innerHTML = msg;
    console.log(msg);
}

// Set up everything the front-end application needs.
// Called on load for <body>.
function initialize() {
  // if (!serverAddress) {
  //   log('Error: Connection tried to open before adress was set');
  // }

  socket = new WebSocket(serverAddress);
  // socket.onopen = function(event) {
  //   log('Connection is opened');
  // };

  // socket.onerror = function (err) {
  //   log(`Error: ${err}`);
  // }

  // socket.onclose = function() {
  //   log('Connection is closed');
  // }

  socket.onmessage = function(event) {
    //if (event.data === "Connected") // log(event.data);

    // Receive player-colours
    if (event.data[0] == 'A' || event.data[0] == 'B')
    {
      var primary;
      var secondary;
      if (event.data[0] === 'A') {
        primary = event.data.substring(2);
        // log("Colour 1: " + primary);
      } else if (event.data[0] === 'B') {
        secondary = event.data.substring(2);
        // log("Colour 2: " + secondary);
      }
      setPlayerColours(primary, secondary);
    }

    // Update points
    if (event.data[0] == 'P') {
      var points = event.data.substring(2);
      document.getElementById("currentScore").innerHTML = points;
    }

    if (event.data[0] == 'T'){
      var time = event.data.substring(2);
      log("TIME: " + time);
    }
  }

  // Push all screens into a map of [screen ID, screen]
  document.querySelectorAll('.screen').forEach(screen => {
    screens.set(screen.id, screen)
  });

  // if (checkCookie()) {
  //   setCurrentScreen('returningPlayerScreen');
  //   var username = document.querySelector("#returnUserName");
  //   username.innerHTML = returningPlayerUserName;
  // } else {
  //   setCurrentScreen('welcomeScreen');
  //   /*var connectButton = document.querySelector('#connect');
  //   connectButton.addEventListener('click', () => {
  //     connected = true; // Mock connection state, should probably be handled in conjunction with the back-end
  //     setCurrentScreen('gameRunningScreen');
  //   })*/
  // }
  // Set up event listeners
  var connectButton = document.querySelector('#connect');
  connectButton.addEventListener('click', () => {
    connected = true; // Mock connection state, should probably be handled in conjunction with the back-end

    // setCurrentScreen('gameRunningScreen');
    setCurrentScreen('waitingScreen');
    handleInformationMessages();
  })

  initTimeBar();
}

// Set the currently visible screen to the matching screenID argument.
// If no screen exists matching that argument, do nothing.
function setCurrentScreen(screenID) {
  if (screens.has(screenID)) {
    screens.forEach(screen => {
      if (screen.id === screenID) {
        screen.style.display = 'flex';
      }
      else {
        screen.style.display = 'none';
      }
    });
  }
}

// Send client's name to server and create a cookie
function sendName() {
  // TODO Add input validation (no spaces, char limit, etc)
  // Should ofc be validated server-side as well
  name = document.getElementById("lname").value.trim();
  // setCookie("username", name, 30);
  if (socket.readyState === WebSocket.OPEN) {
    var stringToSend = `N ${name}`;
    socket.send(stringToSend);
  }
  //go to gamescreen
  connected = true; // Mock connection state, should probably be handled in conjunction with the back-end
  setCurrentScreen('gameRunningScreen');

}

// For returning user
function returnConnection() {
  if (socket.readyState === WebSocket.OPEN) {
    var stringToSend = `E ${returningPlayerUserName}`;
    socket.send(stringToSend);
  }
  // Go to gamescreen
  connected = true; // Mock connection state, should probably be handled in conjunction with the back-end
  setCurrentScreen('gameRunningScreen');
}

// Enable the connect button if and only if the user has entered something
// into the input form.
function handleTextInputChange() {
  var connectButton = document.querySelector('#connect');
  var inputTextForm = document.querySelector('#lname');
  var charLimitMsg = document.querySelector('#charLimitMsg');

  if (inputTextForm.value !== '' && inputTextForm.value.length <= nameLimit) {
    inputTextForm.classList.remove('error');
    charLimitMsg.classList.add('hidden');
    connectButton.disabled = false;
    connectButton.classList.remove('disabled');
  } else {
    inputTextForm.classList.add('error');
    charLimitMsg.classList.remove('hidden');
    connectButton.disabled = true;
    connectButton.classList.add('disabled');
  }
}

const leftBtn = document.getElementById('turnLeft');
const rightBtn = document.getElementById('turnRight');

var leftIsPressed = false;
var rightIsPressed = false;

// Handle buttons when using mouse
leftBtn.addEventListener('mousedown', (event) => {
  leftIsPressed = true;
  sendSteeringData();
});
leftBtn.addEventListener('mouseup', (event) => {
  leftIsPressed = false;
  sendSteeringData();
});
rightBtn.addEventListener('mousedown', (event) => {
  rightIsPressed = true;
  sendSteeringData();
});
rightBtn.addEventListener('mouseup', (event) => {
  rightIsPressed = false
  sendSteeringData();
});

// Handle buttons on touch devices
leftBtn.addEventListener('touchstart', (event) => {
  leftIsPressed = true;
  sendSteeringData();
  event.preventDefault();
});
leftBtn.addEventListener('touchend', (event) => {
  leftIsPressed = false;
  sendSteeringData();
  event.preventDefault();
});
rightBtn.addEventListener('touchstart', (event) => {
  rightIsPressed = true;
  sendSteeringData();
  event.preventDefault();
});
rightBtn.addEventListener('touchend', (event) => {
  rightIsPressed = false
  sendSteeringData();
  event.preventDefault();
});

function sendSteeringData() {
  if(connected && socket.readyState === WebSocket.OPEN) {
    // console.log(direction);
    let direction = 0;
    if (rightIsPressed && leftIsPressed || (!rightIsPressed) && (!leftIsPressed)){
      direction = 0;
    } else if (rightIsPressed) {
      direction = -1;
    } else {
      direction = 1;
    }

    socket.send(`C ${direction}`);
  }
}
// function checkCookie() {
//   var user = getCookie("username");
//   if (user != "") {
//     returningPlayerUserName = user;
//     return true;
//   } else {
//     /* user = document.getElementById("lname").value.trim();
//     if (user != "" && user != null) {
//       setCookie("username", user, 30);
//       console.log("New user");
//     }*/
//     return false;
//   }
// }

// // Set a cookie
// function setCookie(cname, cvalue, exdays) {
//   var d = new Date();
//   d.setTime(d.getTime() + (exdays*24*60*60*1000));
//   var expires = "expires="+ d.toUTCString();
//   document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
// }
// Set a cookie
// function setCookie(cname, cvalue, exdays) {
//   var d = new Date();
//   d.setTime(d.getTime() + (30*60*1000));
//   var expires = "expires="+ d.toUTCString();
//   document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
// }

// // Delete a cookie
// function deleteCookie() {
//   if(getCookie("username")) {
//     document.cookie = returningPlayerUserName + '=; expires=Thu, 01 Jan 1970 00:00:01 GMT;';
//     returningPlayerUserName = "";
//   }
//   setCurrentScreen('welcomeScreen');
// }

// // Get a cookie
// function getCookie(cname) {
//   var name = cname + "=";
//   var decodedCookie = decodeURIComponent(document.cookie);
//   var ca = decodedCookie.split(';');
//   for (var i = 0; i < ca.length; i++) {
//     var c = ca[i];
//     while (c.charAt(0) == ' ') {
//       c = c.substring(1);
//     }
//     if (c.indexOf(name) == 0) {
//       return c.substring(name.length, c.length);
//     }
//   }
//   return "";
// }
