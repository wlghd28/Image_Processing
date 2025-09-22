@echo off
for /D %%s in (*) do (
    if exist %%s\JAVA\RunTest.bat (
        cd %%s\JAVA
        echo %%s starting...
        call RunTest.bat
        cd ..\..
        echo %%s finished.
        pause
    )
)

echo Run all tests finished.
