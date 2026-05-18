# Ejercicio 3 - Pizarra colaborativa

Aplicacion de dibujo en Qt/C++ con lienzo interactivo y sincronizacion remota de trazos.

## Descripcion

El proyecto permite dibujar sobre un canvas, elegir colores, modificar el grosor del pincel y sincronizar los trazos con un servidor. La logica separa el modelo de dibujo, la vista del lienzo y el administrador de sincronizacion.

## Como funciona

- `main.cpp` inicia la ventana principal.
- `MainWindow` arma la interfaz, la paleta de colores y el panel superior.
- `CanvasView` captura el mouse y el teclado para dibujar trazos.
- `DrawingModel` guarda los trazos locales y pendientes de sincronizar.
- `SyncManager` consulta y envia datos al servidor remoto.

El servidor base configurado en el codigo es:

```text
http://173.212.209.61:5002
```

## Controles

- Click y arrastre: dibujar.
- Rueda del mouse: cambiar grosor del pincel.
- Teclas `1` a `9`: cambiar color de la paleta.
- Tecla de borrador segun la logica del lienzo: permite crear trazos en modo borrado cuando esta activo.
- Boton de guardado: fuerza la sincronizacion de trazos pendientes.

## Requisitos

- Qt con modulos `core`, `gui`, `widgets` y `network`.
- Compilador C++ con soporte para C++17.
- Conexion al servidor de sincronizacion si se quiere usar el modo remoto.

## Compilacion

Desde Qt Creator:

1. Abrir `ejercicio3.pro`.
2. Seleccionar el kit de Qt.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake ejercicio3.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Dibujar sobre el lienzo con el mouse.
3. Cambiar color o grosor segun sea necesario.
4. Usar el guardado/sincronizacion para enviar los trazos pendientes al servidor.
