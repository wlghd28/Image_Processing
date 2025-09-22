@echo off
setlocal
set TEST_NAME=U3DTest
javac.exe -cp .;../../../Lib/PDFNet.jar *.java
java.exe -Djava.library.path=../../../Lib -classpath .;../../../Lib/PDFNet.jar %TEST_NAME%
endlocal
