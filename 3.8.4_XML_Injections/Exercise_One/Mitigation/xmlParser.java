import org.xml.sax.helpers.XMLReaderFactory;
import java.lang.Exception;
import org.xml.sax.InputSource;
import org.xml.sax.*;
import java.io.IOException;
import java.util.concurrent.*;

public class xmlParser {

    public static void main(String[] args) throws Exception {
        if (args.length != 1)
            throw new IOException("Need a valid xml file name.");
            
        // define a runXMLParser Runnable for parsing XML files
        Runnable runXMLParser = () -> { 
            try {
                XMLReader xmlReader = XMLReaderFactory.createXMLReader();
                xmlReader.setContentHandler(new MyContentHandler());
                xmlReader.parse(new InputSource(args[0]));
            } catch (Exception e) {
                e.printStackTrace();
            }
        };
        
        // create an executor to run a new thread for XML parsing
        ExecutorService executor = Executors.newSingleThreadExecutor();

        // start the thread XMLParser in the second thread
        Future future = executor.submit(runXMLParser); 
        try {
            // set a timeout for the thread
            future.get(1, TimeUnit.SECONDS); 
        } catch (TimeoutException e) {
            System.err.println("Error: Time limit reached.");
        }

        // shut down the executor
        executor.shutdownNow();
    }
}

// print out node content
final class MyContentHandler extends org.xml.sax.helpers.DefaultHandler implements org.xml.sax.ContentHandler {

    final private static void print(final String context, final String text) {
        // exit the thread when interrupted
        if (Thread.currentThread().isInterrupted())
            System.exit(1);
        java.lang.System.out.println(context + ":\"" + text + "\".");
    }

    final public void startElement(final String namespace, final String localname, final String type,
            final org.xml.sax.Attributes attributes) throws org.xml.sax.SAXException {
        print("startElement", type);
    }

    final public void endElement(final String namespace, final String localname, final String type)
            throws org.xml.sax.SAXException {
        print("endElement", type);
    }

    final public void characters(final char[] ch, final int start, final int len) {
        final String text = new String(ch, start, len);
        final String text1 = text.trim();
        if (text1.length() > 0)
            print("characters ", text1);
    }
}
