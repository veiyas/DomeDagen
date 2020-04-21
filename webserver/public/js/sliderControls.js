function handleSliderControls(event) {
    if (connected && socket.readyState === WebSocket.OPEN) {
        // Maybe do some processing of the data first?
        // This feels good enough for testing
        socket.send(`C ${event.target.value}`);
    }
}
