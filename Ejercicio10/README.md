# Ejercicio 10 - T-Rex Extremo

Juego de escritorio en Qt/C++ inspirado en el T-Rex runner, con obstaculos, pajaros, puntaje y dificultad progresiva.

## Descripcion

El proyecto implementa un juego arcade donde el jugador controla un T-Rex que debe esquivar cactus y pajaros. La velocidad aumenta con el puntaje y el juego termina cuando se detecta una colision.

## Como funciona

- `main.cpp` inicia la aplicacion y muestra `Widget`.
- `Widget` controla el ciclo principal del juego, puntaje, obstaculos, colisiones y dibujado del fondo.
- `TRex` maneja los estados del personaje: correr, saltar, agacharse, adelantar, frenar y game over.
- `Pajaro` representa obstaculos aereos con movimiento y animacion propia.

Los sprites se generan por codigo con `QPainter`, por lo que no necesita archivos externos de imagen para funcionar.

## Controles

- `Espacio`: saltar.
- `Flecha abajo`: agacharse.
- `Flecha derecha`: adelantarse.
- `Flecha izquierda`: frenarse.
- `R` o `Espacio` en game over: reiniciar.

## Requisitos

- Qt con modulo `widgets`.
- Compilador C++ con soporte para C++17.

## Compilacion

Desde Qt Creator:

1. Abrir `Ejercicio10.pro`.
2. Seleccionar un kit compatible.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake Ejercicio10.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Usar los controles para evitar obstaculos.
3. Acumular puntos sobreviviendo el mayor tiempo posible.
4. Reiniciar cuando aparezca la pantalla de game over.
