function setPlayerColours(primary, secondary) {
    var svg = document.getElementById('playerCharacterSVG');
    var primaryElement = svg.getElementById('Body');
    var detailElement = svg.getElementById('Details');
    var ringElement = svg.getElementById('Ring').children[0];

    primaryElement.style.fill = "rgb(" + primary + ")";
    detailElement.style.fill = "rgb(" + secondary + ")";
    ringElement.style.fill = "rgb(" + primary + ")";
}
