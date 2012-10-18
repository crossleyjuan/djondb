package djondb;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Loader {

    public static void initialize() throws LibraryException {
        // try linux first
        InputStream resourceAsStream = Loader.class.getResourceAsStream("/libdjon-javadriver.so");
        String libraryExtension = ".so";
        if (resourceAsStream == null) {
            resourceAsStream = Loader.class.getResourceAsStream("/djonjavadriver.dll");
            libraryExtension = ".dll";
			System.out.println("Resource as Stream dll: " + resourceAsStream);
        }
        if (resourceAsStream != null) {
            FileOutputStream fos = null;
            try {
                String tempPath = System.getProperty("java.io.tmpdir");
                String libraryTempFile = tempPath + File.separator + "templibrary" + libraryExtension;
                fos = new FileOutputStream(libraryTempFile);
                byte[] buffer = new byte[1024];
                int readed;
                while ((readed = resourceAsStream.read(buffer)) > 0) {
                    fos.write(buffer, 0, readed);
                }
                fos.close();
                System.load(libraryTempFile);
            } catch (IOException ex) {
                throw new LibraryException(ex);
            } finally {
                try {
                    fos.close();
                } catch (IOException ex) {
                    Logger.getLogger(Loader.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        } else {
            throw new LibraryException("djondb javadriver not found for this operating system");
        }
    }
}
