# Project Rubi-Ka Crash Popup Remover

A lightweight, zero-dependency Windows utility designed to patch the `Anarchy.exe` client. This utility forces the client to bypass the two blocking error/maintenance message boxes that appear when the application crashes.

## 🚀 Features

*   **Automation Friendly**: Replaces blocking conditional dialog branches with unconditional execution jumps (`JMP`).
*   **Automatic Backup**: Instantly archives your unmodified executable to `Anarchy.bak` prior to writing modifications.
*   **Built-in File Browser**: Automatically opens a native Windows file selection dialog to easily locate `Anarchy.exe`.
*   **Safe Checking**: Validates binary file byte signatures at the target offset (`0xB13E`) before executing changes to prevent file corruption.
*   **Ultra Lightweight**: Built completely without standard C runtime initialization bloat (`-nostdlib`). The final standalone binary footprint is roughly **4 KB**.

## 🛠️ How to Use

1. Download `nomsgaftercrash.exe` from the **Releases** tab.
2. Run the application. 
3. A Windows file selection browser will appear. Navigate to your game installation directory and select your target **`Anarchy.exe`** file.
4. The console window will display the patch status and confirm that a safe `Anarchy.bak` backup file has been created.
5. Close the patcher and launch your game.

## 💻 Compilation from Source

This utility is optimized to compile cleanly using modern open-source toolchains like **w64devkit** (GCC for Windows). 

### Prerequisites

Ensure you have a working GCC environment configured on your path (such as [w64devkit](https://github.com/skeeto/w64devkit)).

### Build Command

Navigate to the directory containing `nomsgaftercrash.c` source code file and execute the following optimization sequence inside your terminal:

```bash
gcc -Os -s -ffreestanding -nostdlib -e ___main__ -o nomsgaftercrash.exe nomsgaftercrash.c -lkernel32 -lcomdlg32
```

### Alternative

If you don't want to download the portable **w64devkit**

1. Open the **Command Prompt** (`cmd.exe`).
2. Run the following command to compile your executable:

```cmd
C:\Windows\Microsoft.NET\Framework64\v4.\0.30319\csc.\exe /target:exe /out:nomsgaftercrash.exe nomsgaftercrash.cs
```

### Compiler Flag Breakdown:
*   `-Os`: Minimizes file size above all else.
*   `-s`: Strips all debugging symbols to drastically minimize the output footprint.
*   `-ffreestanding -nostdlib`: Totally eliminates standard bloated runtime library initialization layers.
*   `-e ___main__`: Forces execution flow straight into the custom minimalist hardware entry loop.
*   `-lkernel32 -lcomdlg32`: Explicitly links only the essential native Windows system file management and dialog frameworks.

## ⚠️ Disclaimer

*This tool is intended strictly for local usability improvements and debugging purposes. Always ensure your `Anarchy.exe` file is retained to seamlessly revert your client back to its default state if necessary.*
