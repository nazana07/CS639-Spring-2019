import org.xml.sax.helpers.XMLReaderFactory;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.StringReader;
import java.lang.Exception;

import org.xml.sax.InputSource;
import org.xml.sax.*;

import java.io.IOException;
import java.util.Scanner;

public class xmlParser {

    public static void main(String[] args) {
        try {

            if (args.length != 1)
                throw new IOException("Need a valid xml file name.");

            XMLReader xmlReader = XMLReaderFactory.createXMLReader();
            xmlReader.setEntityResolver(new MyResolver());

            xmlReader.setContentHandler(new MyContentHandler());

            xmlReader.parse(new InputSource(args[0]));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

// print out node content
final class MyContentHandler extends org.xml.sax.helpers.DefaultHandler implements org.xml.sax.ContentHandler {

    final private static void print(final String context, final String text) {
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

class MyResolver implements EntityResolver {
    public InputSource resolveEntity(String publicId, String systemId) {
        try {
            File file = new File("whiteListForXMLXXEAccess.txt");
            final Scanner scanner = new Scanner(file);
            while (scanner.hasNextLine()) {
                String lineFromFile = scanner.nextLine();
                if (systemId.contains(lineFromFile)) {
                    return null;
                }
            }
            return new InputSource(new StringReader(""));
        } catch (FileNotFoundException ex) {
        }
        return null;
    }
}