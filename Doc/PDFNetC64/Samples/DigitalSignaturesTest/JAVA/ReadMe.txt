In order to run the DigitalSignatureTest for Java with the custom Signature Handler, download the BouncyCastle
libraries here: http://www.bouncycastle.org/.

For Android, there is a repackaged version of BouncyCastle that is intended for the Dalvik VM called SpongyCastle. More
information here: http://rtyley.github.com/spongycastle/

Place the following files in the Lib folder where PDFNetC.dll and PDFNet.jar resides:
    - bcpkix-jdk15on-xxx.jar
    - bcprov-jdk15on-xxx.jar

    * where "xxx" is the version number of bouncycastle.

Rename BCSignatureHandler.java.txt to BCSignatureHandler.java and modify the code inside as necessary.

Uncomment some of the lines in DigitalSignatureTest.java so that the sample uses BCSignatureHandler class.
    
Modify the script files (RunTest.sh or RunTest.bat) to include the library in the build process.

Execute the the script to build and run the sample.
