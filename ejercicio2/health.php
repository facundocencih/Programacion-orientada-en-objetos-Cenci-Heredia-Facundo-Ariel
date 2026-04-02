<?php
header('Content-Type: application/json');

// Obtener uptime
$uptime = shell_exec('uptime -p');
$uptime = trim($uptime);

// Obtener carga de CPU (load average)
$load = shell_exec('uptime | awk -F\'load average:\' \'{ print $2 }\' | cut -d, -f1');
$load = trim($load) . '%';

// Memoria usada
$memory_used = shell_exec('free -h | grep Mem | awk \'{print $3}\'');
$memory_used = trim($memory_used);

// Espacio libre en disco
$disk_free = shell_exec('df -h / | tail -1 | awk \'{print $4}\'');
$disk_free = trim($disk_free);

// Estado general (puedes agregar lógica para determinar basado en umbrales)
$status = 'OK';

// Respuesta JSON
echo json_encode([
    'uptime' => $uptime,
    'load' => $load,
    'memory_used' => $memory_used,
    'disk_free' => $disk_free,
    'status' => $status
]);
?>