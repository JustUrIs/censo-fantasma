<#
  CENSO FANTASMA - flasheador de placas ESP32

  USO: enchufa UNA placa y corre:

       .\flash.ps1 1     SNIFFER   (censo de dispositivos)
       .\flash.ps1 3     EMISOR    (solo necesita corriente)
       .\flash.ps1 2     RECEPTOR  (va por USB a la laptop)

  Detecta el puerto solo. No toca el archivo fuente: trabaja sobre una copia.

  ANTES DE CORRERLO: si tenes la app abierta con la placa conectada,
  toca "Desconectar" en el modulo 07. El navegador se queda con el puerto.

  NOTA: este archivo esta escrito solo en ASCII a proposito. Windows PowerShell
  5.1 lee los .ps1 como ANSI, y los acentos guardados en UTF-8 sin BOM le rompen
  el parseo.
#>

param(
  [Parameter(Mandatory=$true)]
  [ValidateSet("1","2","3")]
  [string]$Role
)

$ErrorActionPreference = "Stop"

$nombres = @{
  "1" = "SNIFFER (censo de dispositivos)"
  "2" = "RECEPTOR CSI (por USB a la laptop)"
  "3" = "EMISOR CSI (solo corriente)"
}

$cli    = Join-Path $env:USERPROFILE "arduino-cli\arduino-cli.exe"
$fuente = Join-Path $PSScriptRoot "censo_fantasma_esp32\censo_fantasma_esp32.ino"
$tmp    = Join-Path $env:TEMP "censo_role$Role\censo_role$Role"

Write-Host ""
Write-Host "  CENSO FANTASMA - ROLE $Role : $($nombres[$Role])" -ForegroundColor Yellow
Write-Host "  ==========================================================="

if (-not (Test-Path $cli))    { Write-Host "  X  No encuentro arduino-cli en $cli" -ForegroundColor Red; exit 1 }
if (-not (Test-Path $fuente)) { Write-Host "  X  No encuentro el sketch en $fuente" -ForegroundColor Red; exit 1 }

# --- 1. buscar la placa ----------------------------------------------------
$puertos = @([System.IO.Ports.SerialPort]::GetPortNames())

if ($puertos.Count -eq 0) {
  Write-Host "  X  No hay ninguna placa conectada." -ForegroundColor Red
  Write-Host "     Enchufala con un cable de DATOS (no de solo carga)."
  exit 1
}
if ($puertos.Count -gt 1) {
  Write-Host "  !  Hay varias placas conectadas: $($puertos -join ', ')" -ForegroundColor Yellow
  Write-Host "     Deja UNA sola enchufada para no flashear la equivocada."
  exit 1
}
$puerto = $puertos[0]
Write-Host "  >  Placa encontrada en $puerto"

# --- 2. verificar que el puerto este libre ---------------------------------
$libre = $true
try {
  $probe = New-Object System.IO.Ports.SerialPort($puerto, 115200)
  $probe.Open()
  $probe.Close()
  $probe.Dispose()
} catch {
  $libre = $false
}
if (-not $libre) {
  Write-Host "  X  El puerto $puerto esta ocupado." -ForegroundColor Red
  Write-Host "     Casi seguro es el navegador: anda a la app, modulo 07,"
  Write-Host "     y toca 'Desconectar'. O cerra esa pestania."
  exit 1
}

# --- 3. copia con el rol pedido --------------------------------------------
New-Item -ItemType Directory -Force $tmp | Out-Null
$codigo = Get-Content $fuente -Raw -Encoding UTF8
$codigo = $codigo -replace '(?m)^#define ROLE \d+', "#define ROLE $Role"
$destino = Join-Path $tmp "censo_role$Role.ino"
Set-Content -LiteralPath $destino -Value $codigo -Encoding UTF8

# --- 4. compilar y subir ----------------------------------------------------
Write-Host "  >  Compilando (tarda ~40 segundos)..."
& $cli compile --fqbn esp32:esp32:esp32 $tmp 2>&1 | Select-Object -Last 2
if ($LASTEXITCODE -ne 0) { Write-Host "  X  Fallo la compilacion." -ForegroundColor Red; exit 1 }

Write-Host "  >  Grabando en $puerto ..."
& $cli upload -p $puerto --fqbn esp32:esp32:esp32 $tmp 2>&1 | Select-String "Hash of data|Wrote |rror|ailed"
if ($LASTEXITCODE -ne 0) { Write-Host "  X  Fallo la grabacion." -ForegroundColor Red; exit 1 }

# --- 5. confirmar que arranco ----------------------------------------------
Write-Host "  >  Verificando..."
Start-Sleep -Seconds 2
$sp = New-Object System.IO.Ports.SerialPort($puerto, 921600, "None", 8, "One")
$sp.ReadTimeout = 2500
try {
  $sp.Open()
  Start-Sleep -Seconds 2
  for ($i = 0; $i -lt 8; $i++) {
    try { $l = $sp.ReadLine().Trim() } catch { break }
    if ($l) { Write-Host "      $l" -ForegroundColor DarkGray }
  }
} catch {
} finally {
  if ($sp.IsOpen) { $sp.Close() }
  $sp.Dispose()
}

Write-Host ""
Write-Host "  OK - placa lista como $($nombres[$Role])" -ForegroundColor Green
if ($Role -eq "3") {
  Write-Host "     Desenchufala de la compu y pasala a un power bank."
  Write-Host "     El LED late = esta emitiendo. No necesita datos, solo corriente."
}
if ($Role -eq "2") {
  Write-Host "     Esta va por USB a la laptop. Necesita que el EMISOR este prendido."
}
Write-Host ""
