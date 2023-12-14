@echo OFF
setlocal enabledelayedexpansion
set "SCRIPT_DIR=%~dp0"
REM 将工作目录更改为 Batch 文件所在目录
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
echo 按任意键尝试将内容复制到剪贴板...
pause > nul
echo !link! | clip
echo 复制成功，按任意键退出...
pause > nul