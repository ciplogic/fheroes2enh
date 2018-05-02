using System;
using System.IO;
using System.IO.Compression;
using System.Net;

namespace FH2Launcher
{
    static class ZipExtractorUtilies
    {
        public static void DownloadItems(string[] itemsToDownload, string destinationPath, Action<string> onFinished)
        {
            var wc = new WebClient();
            foreach (var itemToDownload in itemsToDownload)
            {
                var urlToDownload = ConstStrings.FullUrlToPacks + itemToDownload;
                var destinationFile = Path.Combine(destinationPath, itemToDownload);
                ZipExtractorUtilies.CreateDirectoryForFile(destinationFile);
                wc.DownloadFile(urlToDownload, destinationFile);
                using (var zipStream = File.Open(destinationFile, FileMode.Open))
                {
                    ZipExtractorUtilies.UnzipFromStream(zipStream, destinationPath);
                }

                File.Delete(destinationFile);
                onFinished(itemToDownload);
            }
        }

        public static void CopyStream(Stream input, Stream output)
        {
            byte[] buffer = new byte[8 * 1024];
            int len;
            while ((len = input.Read(buffer, 0, buffer.Length)) > 0)
            {
                output.Write(buffer, 0, len);
            }
        }
        public static void UnzipFromStream(Stream zipStream, string outFolder)
        {
            Directory.CreateDirectory(outFolder);
            var za = new ZipArchive(zipStream, ZipArchiveMode.Read);
            foreach (var zipArchiveEntry in za.Entries)
            {
                var targetFile = Path.Combine(outFolder, zipArchiveEntry.FullName);
                CreateDirectoryForFile(targetFile);
            }

            foreach (var zipArchiveEntry in za.Entries)
            {
                if (zipArchiveEntry.Length == 0)
                    continue;
                using (var childStream = zipArchiveEntry.Open())
                {
                    using (Stream file = File.Create(Path.Combine(outFolder, zipArchiveEntry.FullName)))
                    {
                        CopyStream(childStream, file);
                    }
                }
            }
        }

        public static void CreateDirectoryForFile(string targetFile)
        {
            var fileInfo = new FileInfo(targetFile);
            if (!fileInfo.Directory.Exists)
            {
                Directory.CreateDirectory(fileInfo.DirectoryName);
            }
        }
    }
}