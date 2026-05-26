using System;
using System.IO;
using System.Windows.Forms;

class Program
{
    private const string TARGET_FILENAME = "Anarchy.exe";
    private const string BACKUP_FILENAME = "Anarchy.bak";
    private const long FILE_OFFSET = 0xB13E;

    [STAThread]
    static void Main()
    {
        byte[] originalBytes = new byte[] { 0x0F, 0x86, 0x2C, 0x03, 0x00, 0x00 };
        byte[] patchedBytes  = new byte[] { 0xE9, 0x2D, 0x03, 0x00, 0x00, 0x90 };

        Console.WriteLine("Please select your Anarchy.exe file using the browser window...");

        // Configure and open the File Selection Dialog
        using (OpenFileDialog ofn = new OpenFileDialog())
        {
            ofn.Filter = "Anarchy Executable (Anarchy.exe)|Anarchy.exe|All Files (*.*)|*.*";
            ofn.Title = "Locate Anarchy.exe to apply patch";
            ofn.CheckFileExists = true;
            ofn.CheckPathExists = true;

            if (ofn.ShowDialog() != DialogResult.OK)
            {
                Console.WriteLine("Notice: Selection cancelled by user.");
                WaitAndExit();
                return;
            }

            string filePath = ofn.FileName;
            string directory = Path.GetDirectoryName(filePath);
            string backupPath = Path.Combine(directory, BACKUP_FILENAME);

            // 1. Create a safe backup copy
            try
            {
                File.Copy(filePath, backupPath, true);
                Console.WriteLine("Success: Backup saved as Anarchy.bak inside target directory");
            }
            catch (Exception)
            {
                Console.WriteLine("Warning: Could not save safety backup file.");
            }

            // 2. Open file for patching
            try
            {
                using (FileStream fs = new FileStream(filePath, FileMode.Open, FileAccess.ReadWrite))
                {
                    if (fs.Length < FILE_OFFSET + originalBytes.Length)
                    {
                        Console.WriteLine("Error: Selected file version mismatch or file too small.");
                        WaitAndExit();
                        return;
                    }

                    byte[] buffer = new byte[originalBytes.Length];
                    fs.Seek(FILE_OFFSET, SeekOrigin.Begin);
                    int bytesRead = fs.Read(buffer, 0, buffer.Length);

                    if (bytesRead == buffer.Length)
                    {
                        if (CompareBytes(buffer, originalBytes))
                        {
                            fs.Seek(FILE_OFFSET, SeekOrigin.Begin);
                            fs.Write(patchedBytes, 0, patchedBytes.Length);
                            Console.WriteLine("Success: File patched successfully!");
                        }
                        else if (CompareBytes(buffer, patchedBytes))
                        {
                            Console.WriteLine("Notice: Selected file target is already patched.");
                        }
                        else
                        {
                            Console.WriteLine("Error: Selected file version mismatch or unexpected data signature found.");
                        }
                    }
                    else
                    {
                        Console.WriteLine("Error: Could not read target data bytes from destination binary.");
                    }
                }
            }
            catch (UnauthorizedAccessException)
            {
                Console.WriteLine("Error: Target file could not be opened. Try running as Administrator.");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: {0}", ex.Message);
            }
        }

        WaitAndExit();
    }

    private static bool CompareBytes(byte[] a, byte[] b)
    {
        if (a.Length != b.Length) return false;
        for (int i = 0; i < a.Length; i++)
        {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    private static void WaitAndExit()
    {
        Console.WriteLine("\nPress any key to exit...");
        Console.ReadKey(true);
    }
}
