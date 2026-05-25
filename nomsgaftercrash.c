#include <windows.h>

#define TARGET_FILENAME "Anarchy.exe"
#define BACKUP_FILENAME "Anarchy.bak"
#define FILE_OFFSET 0xB13E
#define BYTE_COUNT 6

void print(const char* text) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteFile(out, text, lstrlenA(text), &written, NULL);
}

int compare_bytes(const unsigned char* a, const unsigned char* b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

// Light wrapper to copy standard strings manually
void copy_string(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

void __main__(void) {
    const unsigned char original_bytes[BYTE_COUNT] = {0x0F, 0x86, 0x2C, 0x03, 0x00, 0x00};
    const unsigned char patched_bytes[BYTE_COUNT]  = {0xE9, 0x2D, 0x03, 0x00, 0x00, 0x90};

    char filePath[MAX_PATH] = {0};
    char backupPath[MAX_PATH] = {0};
    unsigned char buffer[BYTE_COUNT] = {0};
    DWORD bytes_read = 0;
    DWORD bytes_written = 0;

    print("Please select your Anarchy.exe file using the browser window...\n");

    // Configure the lightweight File Selection Dialog using standard Windows headers
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Anarchy Executable (Anarchy.exe)\0Anarchy.exe\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Locate Anarchy.exe to apply patch";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    // Prompt the user to select the file path
    if (!GetOpenFileNameA(&ofn)) {
        print("Notice: Selection cancelled by user.\nPress any key to exit...");
        goto wait_and_exit;
    }

    // Isolate path string length manually to build backup target destination path
    int path_len = lstrlenA(filePath);
    int target_len = lstrlenA(TARGET_FILENAME);
    
    if (path_len < target_len) {
        print("Error: Invalid file path target chosen.\n");
        goto wait_and_exit;
    }

    // Replace "Anarchy.exe" at the end of the selected string path with "Anarchy.bak"
    for (int i = 0; i < path_len - target_len; i++) {
        backupPath[i] = filePath[i];
    }
    copy_string(backupPath + (path_len - target_len), BACKUP_FILENAME);

    // 1. Create a safe backup copy
    if (!CopyFileA(filePath, backupPath, FALSE)) {
        print("Warning: Could not save safety backup file.\n");
    } else {
        print("Success: Backup saved as Anarchy.bak inside target directory\n");
    }

    // 2. Open file for patching
    HANDLE hFile = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        print("Error: Target file could not be opened for read/write.\n");
        goto wait_and_exit;
    }

    SetFilePointer(hFile, FILE_OFFSET, NULL, FILE_BEGIN);

    if (ReadFile(hFile, buffer, BYTE_COUNT, &bytes_read, NULL) && bytes_read == BYTE_COUNT) {
        if (compare_bytes(buffer, original_bytes, BYTE_COUNT)) {
            SetFilePointer(hFile, FILE_OFFSET, NULL, FILE_BEGIN);
            if (WriteFile(hFile, patched_bytes, BYTE_COUNT, &bytes_written, NULL)) {
                print("Success: File patched successfully!\n");
            } else {
                print("Error: Patch write operation failed.\n");
            }
        } 
        else if (compare_bytes(buffer, patched_bytes, BYTE_COUNT)) {
            print("Notice: Selected file target is already patched.\n");
        } 
        else {
            print("Error: Selected file version mismatch or unexpected data signature found.\n");
        }
    } else {
        print("Error: Could not read target data bytes from destination binary.\n");
    }

    CloseHandle(hFile);
    print("\nPress any key to exit...");

wait_and_exit:
    // Custom clean console wait
    {
        HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(in, &mode);
        SetConsoleMode(in, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
        ReadFile(in, buffer, 1, &bytes_read, NULL);
    }
    ExitProcess(0);
}
