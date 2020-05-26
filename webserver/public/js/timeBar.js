var bar, startingHeight;

function initTimeBar() {
    bar = document.querySelector('#timeBar');
    startingHeight = bar.getBoundingClientRect().height;
}

function setTimeBarHeight(percentageRemaining) {
    bar.style.height = `${startingHeight * percentageRemaining}px`;
}