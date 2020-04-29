
var serverAddress;

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
  if (!serverAddress) {
    log('Error: Connection tried to open before adress was set');
  }

  socket = new WebSocket(serverAddress);
  socket.onopen = function(event) {
    log('Connection is opened');
  };

  socket.onerror = function (err) {
    log(`Error: ${err}`);
  }

  socket.onclose = function() {
    log('Connection is closed');
  }

  socket.onmessage = function() {
    if (event.data === "Connected") log(event.data);

    // Receive player-colours
    if (event.data[0] == 'A' || event.data[0] == 'B')
    {
      var primary;
      var secondary;
      if (event.data[0] === 'A') {
        primary = event.data.substring(2);
        log("Colour 1: " + primary);
      } else if (event.data[0] === 'B') {
        secondary = event.data.substring(2);
        log("Colour 2: " + secondary);
      }
      setPlayerColours(primary, secondary);
    }
  }

  // Push all screens into a map of [screen ID, screen]
  document.querySelectorAll('.screen').forEach(screen => {
    screens.set(screen.id, screen)
  });

  // Set up event listeners
  var connectButton = document.querySelector('#connect');
  connectButton.addEventListener('click', () => {
    connected = true; // Mock connection state, should probably be handled in conjunction with the back-end

    document.querySelector('#orientationPrompt').classList.remove('hidden');
    document.querySelector('#promptBackground').classList.remove('hidden');

    setCurrentScreen('waitingScreen');
    handleInformationMessages();
  })
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

// Send client's name to server
function sendName() {
  // TODO Add input validation (no spaces, char limit, etc)
  // Should ofc be validated server-side as well
  name = document.getElementById("lname").value.trim();
  if (socket.readyState === WebSocket.OPEN)
  {
    var stringToSend = `N ${name}`;
    socket.send(stringToSend);
  }
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
