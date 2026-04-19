param(
    [string]$Dir = "."
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $Dir -PathType Container)) {
    Write-Host "Error: '$Dir' is not a directory"
    exit 1
}

$Root = (Resolve-Path -LiteralPath $Dir).Path

function Is-ExcludedPath {
    param([string]$Path)

    $p = $Path.Replace('/', '\')

    return (
        $p -match '(^|\\)build(\\|$)' -or
        $p -match '(^|\\)\.git(\\|$)' -or
        $p -match '(^|\\)\.vs(\\|$)' -or
        $p -match '(^|\\)png(\\|$)' -or
        $p -match '(^|\\)drawio(\\|$)' -or
        $p -match '(^|\\)legacy(\\|$)'
    )
}

function Is-ExcludedFile {
    param([System.IO.FileInfo]$File)

    return (
        $File.Extension -ieq ".png" -or
        $File.Extension -ieq ".drawio"
    )
}

function Is-TextFile {
    param([string]$Path)

    try {
        $bytes = [System.IO.File]::ReadAllBytes($Path)

        if ($bytes.Length -eq 0) {
            return $true
        }

        $limit = [Math]::Min($bytes.Length, 2048)

        for ($i = 0; $i -lt $limit; $i++) {
            $b = $bytes[$i]

            if ($b -eq 0) {
                return $false
            }

            if (($b -lt 9) -or (($b -gt 13) -and ($b -lt 32))) {
                return $false
            }
        }

        return $true
    }
    catch {
        return $false
    }
}

Write-Output "=== TREE: $Root ==="
Write-Output ""

$items = Get-ChildItem -LiteralPath $Root -Recurse -Force |
    Where-Object { -not (Is-ExcludedPath $_.FullName) } |
    Sort-Object FullName

foreach ($item in $items) {
    $relative = $item.FullName.Substring($Root.Length).TrimStart('\')

    if ([string]::IsNullOrWhiteSpace($relative)) {
        continue
    }

    $depth = ($relative -split '\\').Count - 1
    $indent = ("|   " * $depth)

    if ($item.PSIsContainer) {
        Write-Output "$indent[$($item.Name)]"
    }
    else {
        Write-Output "$indent$($item.Name)"
    }
}

Write-Output ""
Write-Output "=== FILE CONTENTS ==="

$files = Get-ChildItem -LiteralPath $Root -Recurse -Force -File |
    Where-Object {
        -not (Is-ExcludedPath $_.FullName) -and
        -not (Is-ExcludedFile $_)
    } |
    Sort-Object FullName

foreach ($file in $files) {
    Write-Output ""
    Write-Output "----- $($file.FullName) -----"

    if (Is-TextFile $file.FullName) {
        Get-Content -LiteralPath $file.FullName -Raw
    }
    else {
        Write-Output "[binary file skipped]"
    }
}