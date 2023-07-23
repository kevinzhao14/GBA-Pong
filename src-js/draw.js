const canvas = document.getElementById("canvas");
const c = canvas.getContext("2d");

// canvas settings
c.imageSmoothingEnabled = false;

function setFont() {
    let size = FONT_HEIGHT * SCALE;
    c.font = size + "px monospace";
}
setFont();

function drawRect(x, y, width, height, color) {
    c.fillStyle = color;
    c.fillRect(x * SCALE, y * SCALE, width * SCALE, height * SCALE);
}

function drawFullscreenRect(color) {
    drawRect(0, 0, WIDTH, HEIGHT, color);
}

function drawFullscreenImage(image) {
    c.drawImage(image, 0, 0, WIDTH * SCALE, HEIGHT * SCALE);
}

function drawString(x, y, str, color) {
    setFont();
    c.fillStyle = color;
    c.fillText(str, x * SCALE, (y + FONT_HEIGHT / 2) * SCALE);
}

function init() {
    canvas.width = WIDTH * SCALE;
    canvas.height = HEIGHT * SCALE;
}
init();