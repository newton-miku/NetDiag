@echo OFF
setlocal enabledelayedexpansion
REM 获取 Batch 文件所在目录
chcp 936
set "SCRIPT_DIR=%~dp0"

REM 将工作目录更改为 Batch 文件所在目录
cd /d "%SCRIPT_DIR%"
SET PATH=%PATH%;"%SCRIPT_DIR%\DiagUtils"
cls

rem 定义DNS查询函数
:DNSlookup
if not "%~1"=="" (
(echo.
echo ">>>>>>>>>>>>正在查询 %1 的DNS解析<<<<<<<<<<<<"
echo.
nslookup %1 )| wtee.exe -a %Output_File%
goto :eof)

REM 获取当前日期时间信息
for /f "delims=" %%a in ('wmic OS Get localdatetime ^| find "."') do set DateTime=%%a
set "YYYY=%DateTime:~0,4%"
set "MM=%DateTime:~4,2%"
set "DD=%DateTime:~6,2%"
set "HH=%DateTime:~8,2%"
set "Min=%DateTime:~10,2%"
set "Sec=%DateTime:~12,2%"

REM 构建带有时间戳的文件名
set "Timestamp=%YYYY%%MM%%DD%_%HH%%Min%%Sec%"
set Output_File=output_%Timestamp%.bin
set Count=100
NetDiag.exe | wtee.exe -a %Output_File%


REM 设置要请求的 PHP 文件的 URL
set PHP_URL=http://st.ddxnb.cn/api/getip.php

REM 设置请求头
set REQUEST_UA="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"

REM 发送 HTTP 请求并将结果保存到变量，禁止显示详细过程
for /f "tokens=2,4,6 delims=:," %%a in ('curl -s %PHP_URL% ^| find "IP" ^| find "Gateway" ^| find "drcomIP"') do (
    set MY_IP=%%a
    set GATEWAY_IP=%%b
    set DRCOM_IP=%%c
)

REM 去掉可能存在的额外字符（例如 "}"）
set DRCOM_IP=%DRCOM_IP:~0,-1%

REM 打印提取的信息

(echo My IP: %MY_IP%
echo Gateway IP: %GATEWAY_IP%
echo Drcom IP: %DRCOM_IP%
echo.) | wtee.exe -a %Output_File%

(echo.
echo ###################以下为路由表的信息###################
echo.
netsh interface ipv4 show int|findstr /v disconnected
route print) >> %Output_File%

(echo.
echo ###################以下为DNS解析测试###################
echo.) >> %Output_File%

call :DNSlookup www.xpu.edu.cn
call :DNSlookup www.baidu.com
call :DNSlookup blog.ddxnb.cn

(echo.
echo ###################以下为DRCOM信息####################
echo.)>> %Output_File%

REM 使用 curl 发送请求到 drcomIP 并带上 User-Agent 头部
REM curl -s -A "%REQUEST_UA%" http://%DRCOM_IP%/
REM 发送 HTTP 请求并将结果保存到变量，禁止显示详细过程
@REM (for /f "tokens=* delims=" %%d in ('curl -s -A %REQUEST_UA% http://%DRCOM_IP%/ ^| findstr "pwm= oltime= NID="') do (
(for /f "tokens=* delims=" %%d in ('curl -s http://%DRCOM_IP%/ ^| findstr "pwm= oltime= NID="') do (
    echo %%d
)) >> %Output_File%

(echo.
echo #################以下为Ping测试信息####################
echo.) >> %Output_File%

REM 使用 ping 命令检查网关是否可达
echo 请稍等 ...
ping  %GATEWAY_IP% -r 9 -n 2 >> %Output_File%
tracert -d -w 200  %GATEWAY_IP% >> %Output_File%

(echo.
echo #################以下为长Ping测试信息####################
echo.) >> %Output_File%
echo Pinging %GATEWAY_IP% ...
ping  %GATEWAY_IP% -n 10 | wtee.exe -a %Output_File%

echo.
echo 请将以下信息发送
transfer.exe wss %Output_File% | wtee.exe clip.txt
echo 按任意键尝试将内容复制到剪贴板...
pause > nul
type clip.txt | clip
del clip.txt
echo 复制成功，按任意键退出...
pause > nul