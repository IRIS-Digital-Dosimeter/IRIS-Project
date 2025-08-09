// Example 4 - Receive a number as text and convert it to an int

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

int dataNumber = 0;             // new for this version

void setup() {
    Serial.begin(9600);
    while (!Serial) { // Need this to run properly 
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithEndMarker();
    showNewNumber();
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    if (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewNumber() {
    if (newData == true) {
        dataNumber = 0;             // new for this version
        dataNumber = atoi(receivedChars);   // new for this version
        // This function stops reading further when encountered by a first character that is not a number. 
        // In atoi(), you cannot convert exponents or decimal numbers.
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        Serial.print("Data as Number ... ");    // new for this version
        Serial.println(dataNumber);     // new for this version
        newData = false;
    }
}