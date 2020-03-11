const serverAddress = 'ws://127.0.0.1/';
var socket;

function log(msg) {
    document.getElementById('debug-output').innerHTML = msg;
    console.log(msg);
}

function initialize() {
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
    log(event.data);
  }

  //Hide welcome screen
  var welcomePage = document.querySelector('#welcomeScreen')
  welcomePage.style.display = "none";

  //Display waiting to connect
  var waitingPage = document.querySelector('#waitingScreen')
  waitingPage.style.display = "flex";
}

function handleTextInputChange() {
  var connectButton = document.querySelector('#connect');
  var inputTextForm = document.querySelector('#lname');
  
  if (inputTextForm.value === '') {
    connectButton.disabled = true;
    connectButton.classList.add('disabled');
  } else {
    connectButton.disabled = false;
    connectButton.classList.remove('disabled');
  }
}
