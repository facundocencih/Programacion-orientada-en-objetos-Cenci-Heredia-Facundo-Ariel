# Ejercicio 9 - Paint con SQLite

Aplicacion de escritorio en Qt/C++ para dibujar en un lienzo y guardar los trazos en una base de datos SQLite.

## Descripcion

El proyecto implementa un paint sencillo con login. Cada trazo realizado en el lienzo se guarda en SQLite junto con su color, grosor y puntos. Al volver a iniciar sesion, los dibujos del usuario se cargan desde la base de datos.

## Como funciona

- `main.cpp` abre la base de datos, muestra el login y luego abre la ventana de dibujo.
- `DatabaseManager` crea el esquema SQLite, valida usuarios, registra accesos y guarda trazos.
- `LoginDialog` valida usuario y clave.
- `Paint` arma la interfaz principal y el panel de herramientas.
- `Pintura` maneja el dibujo, el cambio de color, el grosor, deshacer y limpiar el lienzo.

La base de datos se crea junto al ejecutable con el nombre:

```text
ejercicio9.sqlite
```

Usuario por defecto:

```text
Usuario: admin
Clave: admin
```

## Controles

- Click y arrastre: dibujar.
- Rueda del mouse: cambiar grosor del pincel.
- `R`: color rojo.
- `G`: color verde.
- `B`: color azul.
- `Ctrl+Z`: deshacer ultimo trazo.
- `Escape`: limpiar el lienzo.

## Requisitos

- Qt con modulos `widgets` y `sql`.
- Driver SQLite disponible en la instalacion de Qt.
- Compilador C++ con soporte para C++17.

## Compilacion

Desde Qt Creator:

1. Abrir `ejercicio9.pro`.
2. Seleccionar un kit compatible.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake ejercicio9.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Ingresar con `admin / admin`.
3. Dibujar sobre el lienzo.
4. Cambiar color o grosor segun sea necesario.
5. Cerrar y volver a abrir la aplicacion para comprobar que los trazos quedaron guardados.
