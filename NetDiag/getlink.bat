@echo OFF
setlocal enabledelayedexpansion
set "SCRIPT_DIR=%~dp0"
REM ������Ŀ¼����Ϊ Batch �ļ�����Ŀ¼
cd /d "%SCRIPT_DIR%"
SET PATH=%PATH%;"%SCRIPT_DIR%\DiagUtils"

transfer.exe wss -s output_20231211_215925.bin | wtee.exe clip.txt
for /f "tokens=1,* delims=:" %%a in ('findstr "Download Link:" clip.txt') do (
    set "link=%%b"
    @REM echo !link:~1!
    goto :done
)
:done
curl -X POST -F "url=!link:~1!" http://st.ddxnb.cn/api/savediag.php
del clip.txt
echo.
echo ����������Խ����ݸ��Ƶ�������...
pause > nul
echo !link! | clip
echo ���Ƴɹ�����������˳�...
pause > nul