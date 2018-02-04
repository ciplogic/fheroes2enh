using System.IO;
using System.IO.Compression;

namespace Installer.Commons
{
    class ZipExtractorUtilies
    {

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
                if(zipArchiveEntry.Length==0)
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