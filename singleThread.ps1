param(
    [int64]$count
)

for ($i = 0; $i -lt $count; $i++) {
    .\tests\tests.exe -j 1 -n "*Time*" *>$null
    $percent = [int64](($i / $count) * 100)
    Write-Progress -Activity "Running Time tests" -Status "Time tests $percent% complete." -PercentComplete $percent
}
