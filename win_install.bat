:: short Windows install script for the native host of our extension
 
:: Change HKLM to HKCU if you want to install globally.
:: %~dp0 is the directory containing this bat script and ends with a backslash.
reg add "HKCU\SOFTWARE\Google\Chrome\NativeMessagingHosts\com.cogcommtl.hardware" /ve /t REG_SZ /d "%~dp0com.cogcommtl.hardware.host.json" /f
