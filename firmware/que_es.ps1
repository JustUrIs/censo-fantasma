<#
  CENSO FANTASMA - identificador de placas

  USO: enchufa UNA placa y corre:  .\que_es.ps1

  Resetea la placa, lee lo que dice al arrancar y te dice que rol tiene grabado.
  No modifica nada.

  (ASCII puro a proposito: Windows PowerShell 5.1 lee los .ps1 como ANSI.)
#>

$ErrorActionPreference = "Stop"

$puertos = @([System.IO.Ports.SerialPort]::GetPortNames())
if ($puertos.Count -eq 0) {
  Write-Host "  X  No hay ninguna placa conectada." -ForegroundColor Red
  exit 1
}
if ($puertos.Count -gt 1) {
  Write-Host "  !  Hay varias: $($puertos -join ', '). Deja UNA sola." -ForegroundColor Yellow
  exit 1
}
$puerto = $puertos[0]
Write-Host ""
Write-Host "  Leyendo la placa en $puerto ..." -ForegroundColor Yellow

$sp = New-Object System.IO.Ports.SerialPort($puerto, 921600, "None", 8, "One")
$sp.ReadTimeout = 2000
$rol = $null
$muestras = @()
$crudas = @()
try {
  $sp.Open()
  # Reset por hardware: en las NodeMCU, RTS controla el pin EN.
  $sp.DtrEnable = $false; $sp.RtsEnable = $true; Start-Sleep -Milliseconds 150
  $sp.RtsEnable = $false; Start-Sleep -Seconds 2
  for ($i = 0; $i -lt 25; $i++) {
    try { $l = $sp.ReadLine().Trim() } catch { break }
    if (-not $l) { continue }
    if ($l -match '#ROLE,(\w+)')   { $rol = $matches[1] }
    if ($l -match '^(S|C|Q|R|D),') { $muestras += $l }
    $crudas += $l
  }
} finally {
  if ($sp.IsOpen) { $sp.Close() }
  $sp.Dispose()
}

# Si no llego a agarrar el cartel de arranque, lo deduce por el tipo de dato.
if (-not $rol -and $muestras.Count -gt 0) {
  switch -Regex ($muestras[0]) {
    "^[SD],"  { $rol = "SNIFFER" }
    "^R,"     { $rol = "RADAR" }
    default   { $rol = "CSI_RX" }
  }
}

Write-Host ""
switch ($rol) {
  "SNIFFER" {
    Write-Host "  ESTA PLACA ES: SNIFFER - censo de dispositivos" -ForegroundColor Green
    Write-Host "  Funciona sola. Va por USB a la laptop, modulo 07 de la app."
  }
  "CSI_RX" {
    Write-Host "  ESTA PLACA ES: RECEPTORA CSI" -ForegroundColor Green
    Write-Host "  Va por USB a la laptop. Necesita la EMISORA prendida cerca."
  }
  "CSI_TX" {
    Write-Host "  ESTA PLACA ES: EMISORA CSI" -ForegroundColor Green
    Write-Host "  Solo necesita corriente. Power bank y listo. El LED late = emite."
  }
  "RADAR" {
    Write-Host "  ESTA PLACA ES: RADAR LD2450" -ForegroundColor Green
    Write-Host "  Cableado: 5V->VIN  GND->GND  TXdelSensor->GPIO16  RXdelSensor->GPIO17"
    Write-Host "  Si las lineas R vienen todas en cero: el radar habla pero no ve a nadie."
  }
  default {
    Write-Host "  No pude identificarla." -ForegroundColor Yellow
    Write-Host "  Puede estar vacia, o ser una EMISORA (solo habla al arrancar)."
    Write-Host "  Si el LED late cada medio segundo, es EMISORA."
    Write-Host "  Si no late, esta vacia: flashheala con .\flash.ps1"
  }
}
if ($muestras.Count -gt 0) {
  Write-Host ""
  Write-Host "  Datos en vivo:" -ForegroundColor DarkGray
  $muestras | Select-Object -First 4 | ForEach-Object { Write-Host "      $_" -ForegroundColor DarkGray }
}
if ($crudas.Count -gt 0) {
  Write-Host ""
  Write-Host "  Todo lo que dijo la placa (crudo):" -ForegroundColor DarkGray
  $crudas | Select-Object -First 10 | ForEach-Object {
    $t = $_; if ($t.Length -gt 96) { $t = $t.Substring(0,96) + " ..." }
    Write-Host "      $t" -ForegroundColor DarkGray
  }
} else {
  Write-Host ""
  Write-Host "  La placa no dijo NADA en 2 segundos." -ForegroundColor Yellow
  Write-Host "  Probablemente no tiene firmware, o la grabacion fallo." -ForegroundColor Yellow
}
Write-Host ""
