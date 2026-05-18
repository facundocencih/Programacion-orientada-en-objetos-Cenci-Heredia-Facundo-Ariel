# Ejercicio 8 - Editor multilenguaje

Aplicacion de escritorio en Qt/C++ con login, editor de codigo, validacion simple de sintaxis y exportacion a imagen.

## Descripcion

El proyecto implementa un editor para escribir codigo en C++, Python o Java. Incluye una pantalla de login, bloqueo temporal por intentos fallidos, registro de eventos, validacion de la linea actual y exportacion del contenido a JPG.

## Como funciona

- `main.cpp` copia `config.ini` junto al ejecutable si hace falta y arranca `AppController`.
- `AppController` cambia entre login, editor y modo bloqueado.
- `Login` valida las credenciales configuradas.
- `EditorPrincipal` contiene las pestañas de edicion, seleccion de lenguaje, diagnosticos y exportacion.
- `CodeEditor` detecta cambios de linea, foco y teclado para validar codigo.
- `ValidadorSintaxis` define validadores simples para C++, Python y Java.
- `ModoBloqueado` impide nuevos intentos durante unos segundos luego de varios errores.
- `Logger` registra eventos en el archivo configurado.

## Configuracion

El archivo `config.ini` permite cambiar:

- Usuario y password.
- Duracion del bloqueo.
- Lenguaje por defecto.
- Ruta de exportacion JPG.
- Ruta del log.

Credenciales por defecto:

```text
Usuario: admin
Password: 1234
```

## Requisitos

- Qt con modulos `core`, `gui` y `widgets`.
- Compilador C++ con soporte para C++17.

## Compilacion

Desde Qt Creator:

1. Abrir `ejercicio-8.pro`.
2. Seleccionar un kit compatible.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake ejercicio-8.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Ingresar con las credenciales configuradas.
3. Elegir el lenguaje de trabajo.
4. Escribir codigo en el editor.
5. Revisar el diagnostico de la linea validada.
6. Usar la correccion simple o exportar el codigo a JPG.
7. Si hay tres intentos fallidos de login, se muestra el modo bloqueado.
