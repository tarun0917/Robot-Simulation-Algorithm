var websocket = null; // websocket instance
var localhost = "192.168.1.65";
var b = document.getElementById('btnWS');
var buttonClicked = false;
var websocket_connected = false;
// Initialize the websocket
function init() {
    if (window.location.hostname != "") {
        localhost = window.location.hostname;
    }

    doConnect();
}

function doConnect() { // makes a connection and defines callbacks
    if (b.innerText == "Start Webcam") {
        writeToScreen("Connecting to ws://" + localhost + "/stream ...");
        b.disabled = true;
        websocket = new WebSocket("ws://" + localhost + "/stream");
        websocket.onopen = function (evt) {
            onOpen(evt)
        };
        websocket.onclose = function (evt) {
            onClose(evt)
        };
        websocket.onmessage = function (evt) {
            onMessage(evt)
        };
        websocket.onerror = function (evt) {
            onError(evt)
        };
    } else {
        writeToScreen("Disconnecting ...");
        websocket.close();
    }
}

function onOpen(evt) { // when handshake is complete:
    writeToScreen("Connected.");
    //*** Change the text of the button to read "Stop Webcam" ***//
    b.textContent = "Stop Webcam"
    //*** Change the title attribute of the button to display "Click to stop webcam" ***//
    b.title = "Click to stop webcam"
    //*** Enable the button ***//
    b.disabled = false;

    buttonClicked = false;
    websocket_connected = true;
}

function onClose(evt) { // when socket is closed:
    writeToScreen("Disconnected. Error: " + evt);
    //*** Change the text of the button to read "Start Webcam" ***//
    b.textContent = "Start Webcam"
    //*** Change the title attribute of the button to display "Click to start webcam" ***//
    b.title = "Click to start webcam"
    //*** Enable the button ***//
    b.disabled = false;

    // If the user never actually clicked the button to stop the webcam, reconnect.
    if (buttonClicked == false) {
        doConnect();
    }
    buttonClicked = false;
    websocket_connected = false;
}

function onMessage(msg) { // when client receives a WebSocket message because a new image is ready:
    //*** Display a new timestamp ***//
    var now = new Date();
    document.getElementById("timestamp").textContent = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds();
    // Get the image just taken from WiFi chip's RAM.
    var image = document.getElementById('image');
    var reader = new FileReader();
    reader.onload = function (e) {
        var img_test = new Image();
        img_test.onload = function () {
            image.src = e.target.result;
        };
        img_test.onerror = function () {;
        };
        img_test.src = e.target.result;
    };
    reader.readAsDataURL(msg.data);
}

function onError(evt) { // when an error occurs
    websocket.close();
    writeToScreen("Websocket error");

    //*** Change the text of the button to read "Start Webcam" ***//
    b.textContent = "Start Webcam"
    //*** Change the title attribute of the button to display "Click to start webcam" ***//
    b.title = "Click to start webcam"
    //*** Enable the button ***//
    b.disabled = false;

    buttonClicked = false;
}

// Set up event listeners
//*** When the button is clicked, disable it and set the 'buttonClicked' variable to true, and depending on whether a Websocket is open or not, either run "doConnect()" or "websocket.close()" ***//
b.onclick = function () {
    b.disabled = true;
    buttonClicked = true;
    if (websocket_connected)
        websocket.close();
    else
        doConnect();
}

// Function to display to the message box
function writeToScreen(message) {
    document.getElementById("msg").innerHTML += message + "\n";
    document.getElementById("msg").scrollTop = document.getElementById("msg").scrollHeight;
}

// Open Websocket as soon as page loads
window.addEventListener("load", init, false);