@echo off
set file_path="%ProgramData%\Microsoft\Windows\WlanReport\wlan-report-latest.html"

if exist "%file_path%" (
    echo File exists
) else (
    echo File does not exist
)
