# Ejercicio 2 - Monitor de metricas

Aplicacion de escritorio en Qt/C++ para consultar un endpoint de metricas y visualizar el estado general de un servicio.

## Descripcion

El proyecto muestra un panel de monitoreo con estado, uptime, CPU, memoria, disco y eventos recientes. Permite configurar la URL del endpoint, el intervalo de actualizacion y el umbral de alerta.

## Como funciona

- `main.cpp` inicia la ventana principal.
- `MainWindow` construye la interfaz y actualiza las tarjetas de metricas.
- `MonitorService` realiza peticiones HTTP al endpoint configurado usando `QNetworkAccessManager`.
- `AnimatedProgressBar` anima los cambios de porcentaje en las barras de CPU, memoria y disco.

El endpoint por defecto es:

```text
http://173.212.209.61:5000/metrics
```

La aplicacion espera una respuesta JSON con datos de sistema o metricas. Si el endpoint no responde o devuelve un formato invalido, se muestra un estado de error.

## Requisitos

- Qt con modulos `core`, `gui`, `widgets` y `network`.
- Compilador C++ con soporte para C++17.
- Acceso de red al endpoint configurado.

## Compilacion

Desde Qt Creator:

1. Abrir `ejercicio2.pro`.
2. Seleccionar un kit compatible.
3. Compilar y ejecutar.

Desde terminal con qmake:

```bash
qmake ejercicio2.pro
make
```

En Windows con MinGW puede usarse `mingw32-make`.

## Uso

1. Ejecutar la aplicacion.
2. Revisar el estado general y las metricas principales.
3. Cambiar la URL si se quiere consultar otro endpoint.
4. Ajustar el intervalo de refresco y el umbral de alerta.
5. Usar `Refrescar ahora` para forzar una consulta manual.
