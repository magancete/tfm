# Coche robótico controlado mediante MQTT

El objetivo principal del trabajo es el diseño de un coche robótico controlado mediante el protocolo MQTT que sea capaz de realizar tareas autónomas, responder ante estímulos externos detectados por sus sensores y ser controlado manualmente. Este vehículo será uno de los componentes de un taller sobre IoT impartido en la universidad Carlos III, dirigidos a jóvenes estudiantes de bachillerato con el objetivo principal de introducirles en el mundo de la ciencia y la ingeniería, animándoles a estudiar una carrera tecnológica en la propia universidad.

## Requisitos

* Arduino IDE
* Arduino MEGA
* Raspberry Pi
* OSOYOO V2.1 ROBOT CAR KIT
* Servidor MQTT (Mosquitto https://mosquitto.org/) 
* Librería WiFiEsp (https://github.com/bportaluri/WiFiEsp)
* Librería PubSub (https://github.com/knolleary/pubsubclient)
* Librería Servo (https://www.arduino.cc/reference/en/libraries/servo/)
* Librería tinyGPS (https://github.com/mikalhart/TinyGPS)
* Librería MPU-6050 (https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)
* Librería I2Cdev (https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/I2Cdev)

## Ejercicios

### Sesión 1

##### Ejercicio 1
> Utilizando el LED incorporado en la placa (pin 13) escribe un código que haga que el led se encienda durante 1 segundo y se repita cada 5 segundos.
##### Ejercicio 2
> Partiendo del ejercicio anterior, que cada vez que se encienda el LED, se escriba por la consola: “LED ENCENDIDO”.
##### Ejercicio 3
> Escribe un código que encienda el LED durante si la placa recibe el comando serial ‘Y’ y que lo apague si recibe el comando ‘N’.

### Sesión 2

##### Ejercicio 1
> Completa el código con todos los movimientos del coche (girar a la derecha, a la izquierda y dar marcha atrás) siguiendo el diagrama de la figura.
##### Ejercicio 2
> Haz pruebas con distintas velocidades en cada lado del coche para comprobar los ángulos de giro que realiza.
##### Ejercicio 3
> Utilizando la comunicación serial, escribe un código que sea capaz de modificar la velocidad del coche en función del comando recibido. “G” -> Aumentar la velocidad y “J” Disminuirla.
##### Ejercicio 4
> Utilizando la comunicación serial, escribe un código que sea capad de mover el vehículo en función del comando recibido. “A” -> Avanzar, “B” -> Volver hacia atrás, “R” -> Girar a la derecha, “L” -> Girar a la izquierda y “S” -> Detenerse.

### Sesión 3

##### Ejercicio 1
> Escribe un código que emita un pitido cada vez que se reciba un paquete UDP.
##### Ejercicio 2
> Escribe un código que reciba paquetes UDP y que modifique el movimiento del coche en función del comando recibido. Puedes apoyarte en el código de la sesión 2. Por ejemplo, puedes utilizar esta combinación “A” -> Ir hacia adelante, “L” -> Girar a la izquierda, “R” -> Girar a la derecha, “B” -> Ir hacia atrás, “S” -> Detenerse, “G” -> Aumentar la velocidad, “J” -> Disminuir la velocidad.

### Sesión 4

##### Ejercicio 1
> Utilizando la aplicación WireShark, filtra todas las trazas relacionadas con MQTT para analizar qué tipo de mensajes se envían entre los clientes y el broker. Prueba con diferentes QoS. 
##### Ejercicio 2
> Abre dos sesiones de la consola, suscríbete al topic “test” en una de ellas y envía mensajes en la otra. 
##### Ejercicio 3
> Suscríbete al topic "test” desde Arduino y envía mensajes desde la consola de la Raspberry. Emite un pitido cada vez que se reciba un mensaje. 
##### Ejercicio 4
> Envía un mensaje MQTT desde el coche cada 5 segundos y comprueba desde la consola de la Raspberry que los mensajes llegan correctamente. 
##### Ejercicio 5
> Replica el ejercicio 2 de la sesión 3 pero con MQTT en vez de UDP. 

### Sesión 5

##### Ejercicio 1
> Escribe un código capaz de seguir líneas utilizando el sensor óptico frontal. 
##### Ejercicio 2
> Escribe un código que haga lo siguiente: El coche debe andar durante 2 segundos; debe parar; debe analizar los obstáculos en 180º, 120º, 90º, 40º y 0º; Si encuentra un obstáculo más cerca de 30cm de emitir un pitido, girar durante 2 segundos en dirección opuesta; volver a comprobar si hay obstáculos, si no hay, repetir desde el principio, si encuentra algo, volver a girar. 
##### Ejercicio 3
> Crea una máquina de estados con 3 estados: Control manual, seguir líneas y evitar obstáculos. Mediante MQTT cambia de estado al recibir un mensaje en el topic “mode” y ejecuta dicho estado hasta recibir un nuevo cambio de estado.

### Sesión 6

##### Ejercicio 1
> En un fichero nuevo, envía los datos de posición del GPS a un nuevo topic MQTT llamado ‘gps’.
##### Ejercicio 2
> En un fichero nuevo, envía los datos del acelerómetro y el giroscopio a un nuevo topic MQTT llamado ‘gyro’ utilizando el DMP del sensor.
##### Ejercicio 3
> Combina los ejercicios 1 y 2 con todo lo que hemos visto en sesiones anteriores para tener el coche completo (WIFI, MQTT, seguir líneas y evitar obstáculos…). 

### Sesión 7

##### Ejercicio 1
> Crea un nuevo proyecto de NodeRed en donde aparezcan botones capaces de controlar el movimiento del coche, modificar su velocidad y cambiar de modo de conducción. Debes utilizar comandos MQTT. 
##### Ejercicio 2
> Añade un mapa que visualice la posición del coche obteniendo los datos del topic MQTT “gps”, añade también el número de satélites empleado y la precisión. 
##### Ejercicio 3
> Añade una vista donde se muestren los datos del giroscopio y del acelerómetro mediante el topic MQTT “gyro”. 

## Autor

Carlos Magán López, Agosto 2021
