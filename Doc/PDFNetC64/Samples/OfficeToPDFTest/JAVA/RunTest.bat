@echo off
setlocal
set TEST_NAME=OfficeToPDFTest
IF EXIST %TEST_NAME%.class del %TEST_NAME%.class
javac.exe -cp .;../../../Lib/PDFNet.jar *.java
java.exe -Djava.library.path=../../../Lib -classpath .;../../../Lib/PDFNet.jar %TEST_NAME%
endlocal
