var bar, startingHeight;

function initTimeBar() {
    bar = document.querySelector('#timeBar');
    startingHeight = bar.getBoundingClientRect().height;
}

function setTimeBarHeight(percentage) {
    if (startingHeight > 0) {
        bar.style.height = `${startingHeight * (1-percentage)}px`;
    } else {
        initTimeBar();
    }
}