# Testing on 1000 active clients
For ($i=0; $i -lt 1000; $i++) {
    Start-Process -NoNewWindow ..\bin\client -ArgumentList "-n User", "-r" -RedirectStandardOutput "NUL"
}