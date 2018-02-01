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
                var words = zipArchiveEntry.FullName.Split('/');
                if (words.Length < 2)
                    continue;
                Directory.CreateDirectory(Path.Combine(outFolder, words[0]));
            }

            foreach (var zipArchiveEntry in za.Entries)
            {

                using (var childStream = zipArchiveEntry.Open())
                {
                    using (Stream file = File.Create(Path.Combine(outFolder, zipArchiveEntry.FullName)))
                    {
                        CopyStream(childStream, file);
                    }
                }
            }
        }

    }
}