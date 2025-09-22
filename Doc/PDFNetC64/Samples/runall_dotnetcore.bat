@echo off
for /D %%s in (*) do (
    if exist %%s\CS (
        cd %%s\CS
        echo %%s starting...
        call dotnet run
        cd ..\..
        echo %%s finished.
    )
)

echo Run all tests finished.
