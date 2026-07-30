param (
    [Parameter(Mandatory=$true)]
    [string]$file
)

$map = @{}
Get-Content "$file" | 
    ForEach-Object {
        $items = $_ -split ':'
        $key = $items[0].Trim()
        if (-not $map.ContainsKey($key)) {
            $map[$key] = [PSCustomObject]@{
                First   = [int64]0
                Second  = [int64]0
            }
        }

        if ($items.Length -eq 2) {
            $parsed = $items[1].Trim()
            [int64]$value = $parsed -replace '[^\d]', ''

            $map[$key].First  += $value
            $map[$key].Second += 1
        }
    }

Write-Host ""
ForEach ($item in $map.GetEnumerator()) {
    Write-Host "$($item.Key) average: $($item.Value.First / $item.Value.Second)ns"
}
Write-Host ""
