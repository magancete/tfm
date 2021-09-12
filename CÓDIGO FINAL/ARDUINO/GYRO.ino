bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[128]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]
VectorInt16 aa;         // [x, y, z]
VectorInt16 aaReal;     // [x, y, z]
VectorInt16 aaWorld;    // [x, y, z]
VectorFloat gravity;    // [x, y, z]
float ypr[3];           // [yaw, pitch, roll]

volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

void setupMpu(){

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.println("Inicializando MPU...");
    mpu.initialize();
    pinMode(PIN_MPU_INTERRUPT, INPUT);

    Serial.println(mpu.testConnection() ? ("MPU6050 connection successful") : ("MPU6050 connection failed"));

    Serial.println("Inicializando DMP...");
    devStatus = mpu.dmpInitialize();

    // Valores de calibracion
    mpu.setXAccelOffset(-2950);
    mpu.setYAccelOffset(-3120);
    mpu.setZAccelOffset(-3250);

    mpu.setXGyroOffset(35);
    mpu.setYGyroOffset(125);
    mpu.setZGyroOffset(205);

    // Activar DMP
    if (devStatus == 0) {

        Serial.println("Activando DMP...");
        mpu.setDMPEnabled(true);

        // Activar interrupcion
        attachInterrupt(digitalPinToInterrupt(PIN_MPU_INTERRUPT), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        Serial.println("DMP listo!");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print("DMP Error (code ");
        Serial.print(devStatus);
        Serial.println(")");
    }

}

void get_gyro(){  
   
  String gyro = String(ypr[0] * 180/M_PI) + "|" + String(ypr[1] * 180/M_PI) + "|" + String(ypr[2] * 180/M_PI) + "|" + String(aaReal.x) + "|" + String(aaReal.y) + "|" + String(aaReal.z);
  
  publisToTopic("gyro", gyro);   

}