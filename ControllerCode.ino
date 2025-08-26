
#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("Controller connected, index=%d\n", i);
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            break;
        }
    }
}

void processGamepad(ControllerPtr ctl) {
    int16_t x = -ctl->axisRX(); // Invert X axis so positive is right
    int16_t y = ctl->axisRY();  // Y axis is already correct (positive is forward)
    bool xPressed = ctl->x();

    // Calculate magnitude and angle
    float magnitude = sqrt(x*x + y*y);
    float angle = atan2(y, x) * 180 / M_PI; // Convert to degrees

    // Define thresholds
    const float THRESHOLD_MAX = 400;
    const float THRESHOLD_MID = 200;
    const float THRESHOLD_MIN = 50;

    // Function to map magnitude to 3-bit speed and get speed name
    auto mapSpeed = [](float mag, float threshold_max, float threshold_mid, float threshold_min) -> std::pair<uint8_t, const char*> {
        if (mag >= threshold_max) return {0b000, "MAX"};
        if (mag >= threshold_mid) return {0b100, "MID"};
        if (mag >= threshold_min) return {0b110, "MIN"};
        return {0b111, "ZERO"};
    };

    uint8_t pwmL = 0b111, pwmR = 0b111;
    uint8_t spinL = 0, spinR = 0;
    const char* commandName = "Unknown";

    // Determine direction and speed based on angle and magnitude
    if (magnitude < THRESHOLD_MIN) {
        commandName = "Stop";
    } else {
        int sector = static_cast<int>((angle + 202.5) / 45) % 8;
        auto [speed, speedName] = mapSpeed(magnitude, THRESHOLD_MAX, THRESHOLD_MID, THRESHOLD_MIN);

        switch (sector) {
            case 0: // Right
                pwmL = pwmR = speed;
                spinL = 0;
                spinR = 1;
                commandName = "Right";
                break;
            case 1: // Forward-Right (Right Steer)
                pwmL = speed;
                pwmR = 0b111; // ZERO
                spinL = spinR = 0;
                commandName = "Right Steer";
                break;
            case 2: // Forward
                pwmL = pwmR = speed;
                spinL = spinR = 0;
                commandName = "Forward";
                break;
            case 3: // Forward-Left (Left Steer)
                pwmL = 0b111; // ZERO
                pwmR = speed;
                spinL = spinR = 0;
                commandName = "Left Steer";
                break;
            case 4: // Left
                pwmL = pwmR = speed;
                spinL = 1;
                spinR = 0;
                commandName = "Left";
                break;
            case 5: // Reverse-Left
                pwmL = pwmR = speed;
                spinL = spinR = 1;
                commandName = "Reverse-Left";
                break;
            case 6: // Reverse
                pwmL = pwmR = speed;
                spinL = spinR = 1;
                commandName = "Reverse";
                break;
            case 7: // Reverse-Right
                pwmL = pwmR = speed;
                spinL = spinR = 1;
                commandName = "Reverse-Right";
                break;
        }
        
        // Append speed to command name
        char fullCommandName[32];
        snprintf(fullCommandName, sizeof(fullCommandName), "%s [%s]", commandName, speedName);
        commandName = fullCommandName;
    }

    // Construct the 8-bit command
    uint8_t command = (pwmL << 5) | (pwmR << 2) | (spinL << 1) | spinR;

    // Convert command to binary string
    char binaryCommand[9];
    for (int i = 7; i >= 0; i--) {
        binaryCommand[i] = (command & 1) ? '1' : '0';
        command >>= 1;
    }
    binaryCommand[8] = '\0';
    Serial.write(binaryCommand);


    // Output the command
    Serial.printf("Command: %s (%s), Magnitude: %.2f, Angle: %.2f, X: %4d, Y: %4d, X key: %s\n", 
                  binaryCommand, commandName, magnitude, angle, x, y, xPressed ? "Pressed" : "Not pressed");

    if (xPressed) {
        ctl->playDualRumble(0, 250, 0x80, 0x40);
    }

    // Additional gamepad processing
    if (ctl->a()) {
        static int colorIdx = 0;
        switch (colorIdx % 3) {
            case 0:
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                ctl->setColorLED(0, 255, 0);
                break;
            case 2:
                ctl->setColorLED(0, 0, 255);
                break;
        }
        colorIdx++;
    }

    if (ctl->b()) {
        static int led = 0;
        led++;
        ctl->setPlayerLEDs(led & 0x0f);
    }
}


void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData() && myController->isGamepad()) {
            processGamepad(myController);
        }
    }
}

void setup() {
    Serial.begin(9600);
    BP32.setup(&onConnectedController, &onDisconnectedController);
}

void loop() {
    if (BP32.update()) {
        processControllers();
    }
    delay(150);
}