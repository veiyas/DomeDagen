function setPlayerColours(primary, secondary) {
    var svg = document.getElementById('playerCharacterSVG');
    var bodyElement = svg.getElementById('Body');
    var detailElement = svg.getElementById('Details');
    var ringElement = svg.getElementById('Ring').children[0];

    bodyElement.style.fill = "rgb(" + primary + ")";
    detailElement.style.fill = "rgb(" + secondary + ")";
    ringElement.style.fill = "rgb(" + primary + ")";

    // Set colours on game running page as well
    var primaryElement = document.querySelector('#primaryColour');
    var secondaryElement = document.querySelector('#secondaryColour');
    primaryElement.style.backgroundColor = "rgb(" + primary + ")";
    secondaryElement.style.backgroundColor = "rgb(" + secondary + ")";
}
