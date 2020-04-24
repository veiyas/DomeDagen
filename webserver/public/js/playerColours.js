function setPlayerColours(primary, secondary) {
    var svg = document.getElementById('playerCharacterSVG').contentDocument;
    var primaryElement = svg.getElementById('Body');
    var detailElement = svg.getElementById('Details');
    var ringElement = svg.getElementById('Ring');

    primaryElement.style.fill = primary;
    detailElement.style.fill = secondary;
    ringElement.style.fill = primary;
}