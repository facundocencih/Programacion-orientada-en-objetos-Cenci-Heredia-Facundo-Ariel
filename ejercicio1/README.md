# Ejercicio 1 - Login basico

Aplicacion de escritorio en Qt/C++ que muestra una ventana simple de inicio de sesion con campos de usuario y clave.

## Descripcion

El proyecto implementa un formulario de login usando widgets basicos de Qt. La interfaz contiene etiquetas, campos de texto y un boton para ingresar. El campo de clave usa modo password para ocultar el texto escrito.

## Como funciona

- `main.cpp` inicia la aplicacion Qt y muestra la ventana `Login`.
- `Login` construye la interfaz con `QLabel`, `QLineEdit`, `QPushButton` y `QGridLayout`.
- Al presionar el boton `Ingresar`, se ejecuta el slot de ingreso y la ventana se cierra.

## Requisitos

- Qt con soporte para Widgets.
- Compilador C++ compatible con el kit de Qt instalado.

## Compilacion

Desde Qt Creator:

1. Abrir `ejerciciologin.pro`.
2. Seleccionar un kit de compilacion.
3. Ejecutar el proyecto.

Desde terminal con qmake:

```bash
qmake ejerciciologin.pro
make
```

En Windows con MinGW puede usarse `mingw32-make` en lugar de `make`.

## Uso

1. Ejecutar la aplicacion.
2. Escribir usuario y clave.
3. Presionar `Ingresar` para cerrar la ventana.
