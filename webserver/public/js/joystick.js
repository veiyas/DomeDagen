// joystick.js
// Handles joystick-like input for character controls on a mobile device
// This is a back-up solution to the device orientation controls idea
// Excuse the somewhat messy code for now 

// Coordinates of inital touch
var x0, y0;

// Canvas
var c = document.querySelector("#joystickArea");
c.addEventListener("touchstart", handleTouchStart);
c.addEventListener("touchmove", handleTouchMove);
c.addEventListener("touchend", handleTouchEnd);

// TODO: Move into CSS
c.style.width = "100%";
c.style.height = "100%";
c.style.position = "absolute";

c.width = window.innerWidth;
c.height = window.innerHeight;

var ctx = c.getContext("2d");

function handleTouchStart(event) {
    var { touches } = event;

    x0 = touches[0].pageX;
    y0 = touches[0].pageY;

    ctx.fillStyle = "rgba(255, 0, 0, 0.33)";
    clearAll();
    drawCircle(x0, y0, 30, "rgba(255, 0, 0, 0.33)");
}

function handleTouchMove(event) {
    event.preventDefault(); // Stops scrolling on some phones

    var { touches } = event;

    var x = touches[0].pageX;
    var y = touches[0].pageY;

    var xDiff = x - x0;
    var yDiff = y - y0;

    clearAll();
    drawCircle(x0, y0, 30, "rgba(255, 0, 0, 0.33)");
    drawCircle(x, y, 15, "rgb(255, 0, 0)");

    // Calculate angle to send via socket,
    // might be interesting to calculate and send distance too
    // TODO: Send data via socket
    console.log(Math.atan2(yDiff, xDiff));
}

function handleTouchEnd(event) {
    ctx.clearRect(0, 0, c.width, c.height);
}

function drawCircle(x, y, r, color) {
    ctx.fillStyle = color;
    ctx.beginPath();
    ctx.arc(x, y, r, 0, 2 * Math.PI);
    ctx.fill();
}

function clearAll() {
    ctx.clearRect(0, 0, c.width, c.height);
}
