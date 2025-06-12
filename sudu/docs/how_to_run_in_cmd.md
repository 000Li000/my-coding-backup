# 如何在外部終端機執行數獨程式

1. 開啟命令提示字元（cmd）
2. 切換到專案資料夾：
   ```
   cd /d d:\YY\sudu_start\sudu
   ```
3. 執行程式（確保 main.exe 已經編譯好）：
   ```
   main.exe
   ```
   或
   ```
   .\main.exe
   ```
4. 若要重新編譯（需安裝 gcc）：
   ```
   gcc -g *.c -o main.exe
   ```
5. 若顏色顯示異常，可先執行：
   ```
   chcp 65001
   ```
