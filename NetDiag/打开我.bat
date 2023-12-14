@echo OFF
setlocal enabledelayedexpansion
REM ��ȡ Batch �ļ�����Ŀ¼
chcp 936
set "SCRIPT_DIR=%~dp0"

REM ������Ŀ¼����Ϊ Batch �ļ�����Ŀ¼
cd /d "%SCRIPT_DIR%"
SET PATH=%PATH%;"%SCRIPT_DIR%\DiagUtils"
cls

rem ����DNS��ѯ����
:DNSlookup
if not "%~1"=="" (
(echo.
echo ">>>>>>>>>>>>���ڲ�ѯ %1 ��DNS����<<<<<<<<<<<<"
echo.
nslookup %1 )| wtee.exe -a %Output_File%
goto :eof)

REM ��ȡ��ǰ����ʱ����Ϣ
for /f "delims=" %%a in ('wmic OS Get localdatetime ^| find "."') do set DateTime=%%a
set "YYYY=%DateTime:~0,4%"
set "MM=%DateTime:~4,2%"
set "DD=%DateTime:~6,2%"
set "HH=%DateTime:~8,2%"
set "Min=%DateTime:~10,2%"
set "Sec=%DateTime:~12,2%"

REM ��������ʱ������ļ���
set "Timestamp=%YYYY%%MM%%DD%_%HH%%Min%%Sec%"
set Output_File=output_%Timestamp%.bin
set Count=100
NetDiag.exe | wtee.exe -a %Output_File%


REM ����Ҫ����� PHP �ļ��� URL
set PHP_URL=http://st.ddxnb.cn/api/getip.php

REM ��������ͷ
set REQUEST_UA="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"

REM ���� HTTP ���󲢽�������浽��������ֹ��ʾ��ϸ����
for /f "tokens=2,4,6 delims=:," %%a in ('curl -s %PHP_URL% ^| find "IP" ^| find "Gateway" ^| find "drcomIP"') do (
    set MY_IP=%%a
    set GATEWAY_IP=%%b
    set DRCOM_IP=%%c
)

REM ȥ�����ܴ��ڵĶ����ַ������� "}"��
set DRCOM_IP=%DRCOM_IP:~0,-1%

REM ��ӡ��ȡ����Ϣ

(echo My IP: %MY_IP%
echo Gateway IP: %GATEWAY_IP%
echo Drcom IP: %DRCOM_IP%
echo.) | wtee.exe -a %Output_File%

(echo.
echo ###################����Ϊ·�ɱ����Ϣ###################
echo.
netsh interface ipv4 show int|findstr /v disconnected
route print) >> %Output_File%

(echo.
echo ###################����ΪDNS��������###################
echo.) >> %Output_File%

call :DNSlookup www.xpu.edu.cn
call :DNSlookup www.baidu.com
call :DNSlookup blog.ddxnb.cn

(echo.
echo ###################����ΪDRCOM��Ϣ####################
echo.)>> %Output_File%

REM ʹ�� curl �������� drcomIP ������ User-Agent ͷ��
REM curl -s -A "%REQUEST_UA%" http://%DRCOM_IP%/
REM ���� HTTP ���󲢽�������浽��������ֹ��ʾ��ϸ����
@REM (for /f "tokens=* delims=" %%d in ('curl -s -A %REQUEST_UA% http://%DRCOM_IP%/ ^| findstr "pwm= oltime= NID="') do (
(for /f "tokens=* delims=" %%d in ('curl -s http://%DRCOM_IP%/ ^| findstr "pwm= oltime= NID="') do (
    echo %%d
)) >> %Output_File%

(echo.
echo #################����ΪPing������Ϣ####################
echo.) >> %Output_File%

REM ʹ�� ping �����������Ƿ�ɴ�
echo ���Ե� ...
ping  %GATEWAY_IP% -r 9 -n 2 >> %Output_File%
tracert -d -w 200  %GATEWAY_IP% >> %Output_File%

(echo.
echo #################����Ϊ��Ping������Ϣ####################
echo.) >> %Output_File%
echo Pinging %GATEWAY_IP% ...
ping  %GATEWAY_IP% -n 10 | wtee.exe -a %Output_File%

echo.
echo �뽫������Ϣ����
transfer.exe wss %Output_File% | wtee.exe clip.txt
echo ����������Խ����ݸ��Ƶ�������...
pause > nul
type clip.txt | clip
del clip.txt
echo ���Ƴɹ�����������˳�...
pause > nul