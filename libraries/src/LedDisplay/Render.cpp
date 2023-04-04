/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

Render::Render() {
    Serial.println("Render");
};

void Render::setPixel(uint8_t col, uint8_t row, uint8_t color) {
    if (!BUF_NUM_VALID(_renderBufNum)) {
        _renderBufNum = rp2040.fifo.pop();
//        Serial.println("_renderBufNum: " + String(_renderBufNum));
    }
    _frameBuffers[_renderBufNum][row][col] = color;
};

void Render::show() {
//    Serial.print("Render Buf#: " + String(_renderBufNum) + "; ");
    rp2040.fifo.push(_renderBufNum);
//    Serial.println("free: " + String(_renderBufNum));
    _renderBufNum = -1;
};
