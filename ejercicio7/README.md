# Ejercicio 7 - Login con clima y perfil

Aplicacion de escritorio en Qt/C++ que combina login local, consulta de clima, fondo descargado y pantalla de perfil personal.

## Descripcion

El proyecto muestra una pantalla de ingreso con validacion local. Mientras el usuario inicia sesion, la aplicacion consulta el clima de la ciudad configurada y descarga una imagen de fondo. Luego abre una ventana principal con informacion de perfil.

## Como funciona

- `main.cpp` crea la aplicacion y arranca `AppController`.
- `AppController` coordina login, clima, fondo, proxy y apertura de la ventana principal.
- `Login` muestra el formulario de ingreso y el estado del clima.
- `Clima` consulta OpenWeatherMap usando los datos de `config.ini`.
- `WallpaperManager` descarga o reutiliza una imagen de fondo.
- `Ventana` muestra la pantalla principal con informacion personal.
- `Logger` registra eventos relevantes de la aplicacion.

## Configuracion

El archivo `config.ini` contiene:

- URL base de la API de clima.
- API key de OpenWeatherMap.
- Ciudad y unidades.
- URL de la imagen de fondo.
- Usuario y clave locales.
- Tiempo de bloqueo por intentos fallidos.
- Datos opcionales de proxy.

Credenciales locales por defecto:

```text
Usuario: admin
Clave: 1234
```

Si no hay una API key valida, la aplicacion muestra datos simulados de clima.

## Requisitos

- Qt con modulos `core`, `gui`, `widgets` y `network`.
- Compilador C++ con soporte para C++17.
- Conexion a internet para clima e imagen de fondo.
- API key de OpenWeatherMap para clima real.

## Compilacion

Desde Qt Creator:

1. Abrir `ejercicio7.pro`.
2. Verificar que `config.ini` este junto al ejecutable o en la carpeta del proyecto.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake ejercicio7.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Ingresar con el usuario y clave configurados.
3. Ver el clima cargado en la pantalla de login.
4. Acceder a la ventana principal de perfil.
5. Si se fallan varios intentos, el login se bloquea temporalmente.
