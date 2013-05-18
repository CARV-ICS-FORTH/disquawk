/*
 * Copyright 2004-2008 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Sun Microsystems, Inc., 16 Network Circle, Menlo
 * Park, CA 94025 or visit www.sun.com if you need additional
 * information or have any questions.
 */

package com.sun.squawk;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.io.Connector;

import com.sun.squawk.pragma.HostedPragma;
/*if[VERIFY_SIGNATURES]*/
import com.sun.squawk.security.verifier.SignatureVerifier;
import com.sun.squawk.security.verifier.SignatureVerifierException;
/*end[VERIFY_SIGNATURES]*/
import com.sun.squawk.util.Arrays;
import com.sun.squawk.util.Assert;
import com.sun.squawk.util.BitSet;
import com.sun.squawk.util.StringTokenizer;
import com.sun.squawk.util.StructuredFileInputStream;
import com.sun.squawk.util.Tracer;
import com.sun.squawk.vm.ChannelConstants;
import com.sun.squawk.vm.HDR;


/**
 * This class facilitates loading a serialized object graph from a URI
 * and relocating it.
 *
 */
public class ObjectMemoryLoader {

/*if[VERIFY_SIGNATURES]*/
    private static boolean signatureVerifierInitialised = false;
    private static boolean noPublicKeyInstalled = false;

    private static byte[] getPublicKey() {
		byte[] result = new byte[256];
		int numberOfBytesRead = VM.execSyncIO(ChannelConstants.GET_PUBLIC_KEY, result.length, 0,0,0,0,0, result, null);
		result = Arrays.copy(result, 0, numberOfBytesRead, 0, numberOfBytesRead);
		return result;
	}

    // Can't be a static block, as we must only call this on SPOTs
    private static void ensurePublicKeyInitialised() {
        if (!VM.isHosted() && !signatureVerifierInitialised && !noPublicKeyInstalled) {
            try {
            	byte[] publicKeyBytes = getPublicKey();
            	if (publicKeyBytes.length > 0) {
            		SignatureVerifier.initialize(publicKeyBytes, 0, publicKeyBytes.length);
            		signatureVerifierInitialised = true;
            	} else {
            		noPublicKeyInstalled = true;
            	}
           } catch (SignatureVerifierException e) {
                throw new RuntimeException("Failed to initialize SignatureVerifier. " + e.getMessage());
            }
        }
    }
/*end[VERIFY_SIGNATURES]*/

    /**
     * Calculates the hash of an array of bytes.
     *
     * @param arr   the byte array to hash
     * @return      the hash of <code>arr</code>
     */
    protected int getHash(byte[] arr) {
    	return hash(arr);
    }

    public static int hash(byte[] arr) {
        int hash = arr.length;
        for (int i = 0; i != arr.length; ++i) {
            hash += arr[i];
        }
        return hash;
    }


    /*---------------------------------------------------------------------------*\
     *                                 Loading                                   *
    \*---------------------------------------------------------------------------*/

    /**
     * The validating reader used to read the components of an object memory from
     * an input stream.
     */
    final ObjectMemoryReader reader;

    /**
     * Specifies if the object memory is to be moved to read-only memory once it
     * has been loaded and relocated.
     */
    private final boolean loadIntoReadOnlyMemory;

    /**
     * Specifies if the suite data is required to be changed to conform
     * with the endianess of this platform.
     */
    private boolean requiresEndianSwap;

    /**
     * Specifies if "-traceoms" was enabled.
     */
    private boolean tracing;

    /**
     * Constructor.
     *
     * @param reader
     * @param loadIntoReadOnlyMemory
     */
    ObjectMemoryLoader(ObjectMemoryReader reader, boolean loadIntoReadOnlyMemory) {
        this.reader = reader;
        this.loadIntoReadOnlyMemory = loadIntoReadOnlyMemory;
        if (Klass.TRACING_ENABLED) {
            this.tracing = Tracer.isTracing("oms");
        }
    }

    /**
     * Loads the header of an object memory file from a given input stream.
     *
     * @param dis                     the data input stream from which to read
     * @param uri                     a URI identifying the object memory being loaded
     * @return the ObjectMemoryFile instance encapsulating only the header information in the stream
     *         (i.e. the {@link ObjectMemoryFile#objectMemory} field is null)
     */
    public static ObjectMemoryFile loadHeader(DataInputStream dis, String uri) {
        return load0(dis, uri, false, true);
    }

    /**
     * Loads an object memory from a given input stream. If the URI describing the source of the input
     * stream corresponds to the URI of an object memory already present in the system, then that object
     * memory is returned instead.
     *
     * @param dis                     the data input stream from which to read
     * @param uri                     a URI identifying the object memory being loaded
     * @param loadIntoReadOnlyMemory  specifies if the object memory should be put into read-only memory
     * @return the ObjectMemoryFile instance encapsulating the loaded/resolved object memory
     */
    public static ObjectMemoryFile load(DataInputStream dis, String uri, boolean loadIntoReadOnlyMemory) {
        try {
            return load0(dis, uri, loadIntoReadOnlyMemory, false);
        } catch (ObjectMemory.GCDuringRelocationError e) {
            throw new OutOfMemoryError("garbage collection occured while loading object memory from " + uri);
        }
    }

    /**
     * Loads an object memory from a given input stream. If the URI describing the source of the input
     * stream corresponds to the URI of an object memory already present in the system, then that object
     * memory is returned instead.
     *
     * @param uri                     a URI identifying the object memory being loaded
     * @param loadIntoReadOnlyMemory  specifies if the object memory should be put into read-only memory
     * @return the ObjectMemoryFile instance encapsulating the loaded/resolved object memory
     * @throws java.io.IOException
     */
    public static ObjectMemoryFile load(String uri, boolean loadIntoReadOnlyMemory) throws IOException {
        String url;
        if (VM.isHosted()) {
            url = convertURIHosted(uri);
        } else {
            url = uri;
        }
        if (url.startsWith("file://") && filePathelements != null) {
        	url += ";" + filePathelements;
        }
        try {
            DataInputStream dis = Connector.openDataInputStream(url);
            ObjectMemoryFile result = load(dis, uri, loadIntoReadOnlyMemory);
            dis.close();
            return result;
        } catch (ConnectionNotFoundException e) {
System.out.println("filePathelements=" + filePathelements);
            throw e;
        }
    }

    /**
     * The restartable entry point to loading.
     *
     * @param dis                     the data input stream from which to read
     * @param uri                     a URI identifying the object memory being loaded
     * @param loadIntoReadOnlyMemory  specifies if the object memory should be put into read-only memory
     * @param headerOnly              only loads the header of the object memory file if true
     * @return the ObjectMemoryFile instance encapsulating the loaded/resolved object memory
     */
    private static ObjectMemoryFile load0(DataInputStream dis, String uri, boolean loadIntoReadOnlyMemory, boolean headerOnly) {
        ObjectMemoryLoader loader;
        if (VM.isVerbose())
          VM.print("[DIAG]  load0 for "+uri+"\n");

        if (uri.startsWith("spotsuite:")) {
/*if[VERIFY_SIGNATURES]*/
        	ensurePublicKeyInitialised();
            InputStream suiteIn;
            String signatureVerificationErrorMessage = "";
            if (uri.endsWith("library")) {
            	signatureVerificationErrorMessage = "Signature verification of the library ("+uri+") failed.\n" +
            		"Use \"ant flashlibrary\" to reinstall the library from the same SDK used to flash the " +
            		"current application. ";
            } else {
            	signatureVerificationErrorMessage = "Signature verification of the application ("+uri+") failed.\n" +
            		"Use \"ant deploy\" to reinstall the application via USB.";
            }
            try {
            	suiteIn = Connector.openInputStream(uri);
                try {
                	if (!SignatureVerifier.isVerifiedSuite(suiteIn) && !noPublicKeyInstalled) {
                		if (VM.isVerbose()) {
                			System.out.println("Verifying signature of suite ("+uri+")");
                        }
                		SignatureVerifier.verifySuite(suiteIn);
                	} else {
                		//System.out.println("NOT verifying suite with uri " + uri);
                	}

				} catch (SignatureVerifierException e) {
                    throw new Error(signatureVerificationErrorMessage+((VM.isVerbose())?("\n\t"+e.getMessage()):""));
                } finally {
                    suiteIn.close();
                }

            } catch (IOException e) {
            	throw new Error(signatureVerificationErrorMessage+((VM.isVerbose())?("\n\t"+e.getMessage()):""));
            }
/*end[VERIFY_SIGNATURES]*/
            ObjectMemoryReader reader = new FlashObjectMemoryReader(dis, uri);
            loader = new FlashObjectMemoryLoader(reader, loadIntoReadOnlyMemory);
        } else
        {
            ObjectMemoryReader reader = new ObjectMemoryReader(dis, uri);
            loader = new ObjectMemoryLoader(reader, loadIntoReadOnlyMemory);
        }
        ObjectMemoryFile omf = loader.load(headerOnly);

        if (VM.isVerbose()) {
            VM.print("[loaded object memory from '");
            VM.print(uri);
            VM.println("']");
        }

        if (loadIntoReadOnlyMemory) {
            GC.registerReadOnlyObjectMemory(omf.objectMemory);
        }
        return omf;
    }

/*if[TYPEMAP]*/
    /**
     * Loads the type map describing the type of every address in an object memory.
     *
     * @param start   the start address of the object memory
     * @param size    the size address of the object memory
     */
    private void loadTypeMap(Address start, int size) {
        Address p = start;
        for (int i = 0; i != size; ++i) {
            byte type = (byte)reader.readByte(null);
            NativeUnsafe.setType(p, type, 1);
            p = p.add(1);
        }
        if (Klass.TRACING_ENABLED && Tracer.isTracing("oms")) {
            Tracer.traceln("typemap:{size = " + size + "}");
        }
    }
/*end[TYPEMAP]*/

    /**
     * Loads the complete object memory from the input stream.
     *
     * @param headerOnly  only loads the header of the object memory file if true
     */
    private ObjectMemoryFile load(boolean headerOnly) {

        if (Klass.TRACING_ENABLED && Tracer.isTracing("oms")) {
            Tracer.traceln("Loading object memory from " + reader.getFileName());
        }

        // Load magic
        int magic = reader.readInt("magic");
        if (magic != 0xdeadbeef) {
            throw new Error("invalid magic file identifier: expected 0xdeadbeef, received 0x" + Integer.toHexString(magic));
        }

        // Load and ignore version numbers for now
        int minor = reader.readShort("minor_version");
        int major = reader.readShort("major_version");

        // Load attributes
        int attributes = reader.readInt("attributes");
        boolean hasTypemap = (attributes & ObjectMemoryFile.ATTRIBUTE_TYPEMAP) != 0;
        boolean is32Bit = (attributes & ObjectMemoryFile.ATTRIBUTE_32BIT) != 0;
        boolean isBigEndian = (attributes & ObjectMemoryFile.ATTRIBUTE_BIGENDIAN) != 0;

        // Verify the word size
        if (is32Bit != (HDR.BYTES_PER_WORD == 4)) {
            throw new Error("invalid word size in object memory: expected " +
                                   (is32Bit ? "32 bit" : "64 bit") + ", received " + (is32Bit ? "64 bit" : "32 bit"));
        }

        // Check whether the suite is in the correct endianness.
        requiresEndianSwap = (VM.isBigEndian() != isBigEndian);

        int parentHash = reader.readInt("parent_hash");
        String parentURI = reader.readUTF("parent_uri");

        // Return now if only loading the object memory file header
        if (headerOnly) {
            return new ObjectMemoryFile(minor, major, attributes, parentHash, parentURI, null);
        }

        if (VM.isVerbose()) {
          VM.print("[DIAG]  in load(bool)\n");
          VM.print("        Minor     : "+minor+"\n");
          VM.print("        Major     : "+major+"\n");
          VM.print("        Attributes: "+attributes+"\n");
          VM.print("        parentHash: 0x"+Integer.toHexString(parentHash)+"\n");
          VM.print("        parentURI : "+parentURI+"\n");
        }

        // Load the parent of this object memory file
        ObjectMemory parent;
        if (parentURI.length() == 0) {
            Assert.always(VM.isHosted());
            parent = null;
        } else {
            parent = loadParent(parentHash, parentURI);
        }

        // Load the object memory file and relocate the object memory
        ObjectMemory om = loadThis(parent, hasTypemap);

        // Tracing
        if (Klass.TRACING_ENABLED && Tracer.isTracing("oms")) {
            Tracer.traceln("Loaded object memory from " + reader.getFileName());
        }

        return new ObjectMemoryFile(minor, major, attributes, parentHash, parentURI, om);
    }

    /**
     * An output stream that counts and then discards the bytes written to it.
     */
    static final class OutputStreamSink extends OutputStream {
        private int length;

        public int getLength() {
            return length;
        }

        public void write(int b) throws IOException {
            ++length;
        }
    }

    /**
     * Calculates the number of bytes that are required for representing a given string
     * in a DataOutputStream of DataInputStream.
     *
     * @param s   the string value to test
     * @return    the number of bytes in the UTF8 representation of <code>s</code>
     */
    static int getUTF8Length(String s) {
        OutputStreamSink oss = new OutputStreamSink();
        DataOutputStream dos = new DataOutputStream(oss);
        try {
            dos.writeUTF(s);
        } catch (IOException ioe) {
            Assert.shouldNotReachHere();
        }
        return oss.getLength();
    }

    /**
     * Calculates the padding that precedes the 'memory' item to ensure that it is word aligned
     * with respect to the start of the object memory file.
     *
     * @param parentURI   the value of the 'parent_uri' item in the object memory file
     * @param memorySize  the value of the 'size' item in the object memory file
     */
    public static int calculateMemoryPadding(String parentURI, int memorySize) {
        int sizeSoFar = 4 +   // u4 magic
                        2 +   // u2 minor_version
                        2 +   // major_version
                        4 +   // u4 attributes
                        4 +   // u4 parent_hash
                        getUTF8Length(parentURI) + // utf8 parent_uri
                        4 +   // u4 root
                        4 +   // u4 size
                        GC.calculateOopMapSizeInBytes(memorySize); // u1 oopmap[]

        int pad = sizeSoFar % HDR.BYTES_PER_WORD;
        if (pad != 0) {
            pad = HDR.BYTES_PER_WORD - pad;
        }
        return pad;
    }

    /**
     * Skips the padding that precedes the 'memory' item to ensure that it is word aligned
     * with respect to the start of the object memory file.
     *
     * @param parentURI   the value of the 'parent_uri' item in the object memory file
     * @param memorySize  the value of the 'size' item in the object memory file
     */
    protected void skipMemoryPadding(String parentURI, int memorySize) {
        int pad = calculateMemoryPadding(parentURI, memorySize);
        reader.skip(pad, "padding");
    }

    /**
     * Loads the non-parent components of an object memory from the input stream.
     *
     * @param parent     the parent object memory
     */
    private ObjectMemory loadThis(ObjectMemory parent, boolean hasTypemap) {

        String url = reader.getFileName();

        // Load the offset to the root object
        int root = reader.readInt("root");

        // Load the size of the memory
        int size = reader.readInt("size");

        // Load the oop map
        BitSet oopMap = loadOopMap(size);

        // Skip the padding
        skipMemoryPadding(parent == null ? "" : parent.getURI(), size);

        // Load the object memory
        byte[] buffer = loadMemory(size);

        // Calculate the hash of the object memory while it is in canonical form
        int hash = getHash(buffer);

        // Run the collector to prevent a collection being run during relocation which
        // will screw a RAM buffer
        if (!VM.isHosted()) {
          VM.collectGarbage(true);
        }

        if (VM.isVerbose()) {
          VM.print("[DIAG]  in loadThis\n");
          VM.print("        Read:"+reader.getBytesRead()+"\n");
          VM.print("        rootOffset: "+root+"\n");
          VM.print("        memorySize: "+size+"\n");
          VM.print("        hash      : 0x"+Integer.toHexString(hash)+"\n");
        }

        // Relocate the pointers in the memory and move the buffer into read-only memory if necessary
        Address relocatedBuffer = relocateMemory(parent, buffer, oopMap);

        // Need to do this one more time
        if (!VM.isHosted() && !loadIntoReadOnlyMemory) {
            if (buffer != relocatedBuffer.toObject()) {
                throw new ObjectMemory.GCDuringRelocationError();
            }
        }

        // Set the pointer to the root object
        Object rootObject = relocatedBuffer.add(root).toObject();

        // Load the typemap
        if (hasTypemap) {
/*if[TYPEMAP]*/
            if (VM.usingTypeMap()) {
                loadTypeMap(relocatedBuffer, size);
            }
/*end[TYPEMAP]*/
        }

//if(!VM.isHosted()) GC.traceMemory(relocatedBuffer, relocatedBuffer.add(size), true);
        if (Klass.TRACING_ENABLED && !VM.isHosted() && Tracer.isTracing("oms")) {
            GC.traceMemory(relocatedBuffer, relocatedBuffer.add(size), true);
        }

        ObjectMemory om = new ObjectMemory(relocatedBuffer, size, url, rootObject, hash, parent);
        return om;
    }

    static String filePathelements;

    /**
     * Expecting a string that looks something like "c:\dev\1${File.separatorChar}c:\windows".
     * @param path entries
     */
    public static void addFilePath(String path) {
        if (path == null) {
            return;
        }
        StringBuffer buffer = new StringBuffer();
        if (filePathelements != null) {
            buffer.append(filePathelements);
        }
        StringTokenizer tokenizer = new StringTokenizer(path, "" + VM.getPathSeparatorChar());
        while (tokenizer.hasMoreTokens()) {
            if (buffer.length() > 0) {
                buffer.append(';');
            }
            buffer.append("pathelement=");
            buffer.append(tokenizer.nextToken());
        }
        filePathelements = buffer.toString();
    }

    /**
     * Expecting a string that looks something like "c:\dev\1${File.separatorChar}c:\windows".
     * @param path entries
     */
    public static void setFilePath(String path) {
        if (path == null) {
            filePathelements = null;
            return;
        }
        StringBuffer buffer = new StringBuffer(path.length());
        StringTokenizer tokenizer = new StringTokenizer(path, "" + VM.getPathSeparatorChar());
        while (tokenizer.hasMoreTokens()) {
            if (buffer.length() > 0) {
                buffer.append(';');
            }
            buffer.append("pathelement=");
            buffer.append(tokenizer.nextToken());
        }
        filePathelements = buffer.toString();
    }

    /**
     * Converts ObjectMemory.BOOTSTRAP_URI to the bootstrap URI given by the
     * system property <code>bootstrap.suite.url</code>.  Any other URI is
     * returned unchanged.
     */
    private static String convertURIHosted(String uri) throws HostedPragma {
        String result = uri;
        if (uri.equals(ObjectMemory.BOOTSTRAP_URI)) {
            result = System.getProperty(ObjectMemory.BOOTSTRAP_URI_PROPERTY);
            if (result == null) {
                result = "file://squawk.suite";
            }
        }
        return result;
    }

    /**
     * Loads the parent components of an object memory from the input stream. If the
     * parent components specify that there is a parent, then this parent and its
     * parents are loaded recursively.
     *
     * @param hash   the hash expected of the parent object memory
     * @param uri    the URL from which to load the parent object memory
     * @return the loaded object memory
     */
    private ObjectMemory loadParent(int hash, String uri) {
        ObjectMemory parent = GC.lookupReadOnlyObjectMemoryBySourceURI(uri);
        if (parent == null) {
            try {
                parent = load(uri, loadIntoReadOnlyMemory).objectMemory;
            } catch (IOException e) {
                throw new Error("IO error loading object memory from '" + uri + "': " + e);
            }
        }

        if (parent.getHash() != hash) {
        	String helpText = "";
/*if[FLASH_MEMORY]*/
        	if ("spotsuite://library".equals(parent.getURI())) {
        		helpText += "The application you are trying to run was not built against the library that is installed\n";
        		helpText += "Either rebuild your application and re-deploy, or install the correct library\n";
        	} else if ("memory:bootstrap".equals(parent.getURI())) {
        		helpText += "The installed library was not built against the installed version of the Java VM\n";
        	}
/*end[FLASH_MEMORY]*/
            throw new Error(helpText + "invalid hash for parent (" + parent.getURI() + "): expected " + hash + ", received " + parent.getHash());
        }
        return parent;
    }

    /**
     * Loads the 'oopmap' component of the object memory from the input stream.
     *
     * @param size   the size of memory as specified by the 'size' element of the objct memory
     * @return the bit set encapsulating the oop map
     */
    protected BitSet loadOopMap(int size) {
        // Load the oop map
        byte[] bits = new byte[GC.calculateOopMapSizeInBytes(size)];
        reader.readFully(bits, "oopmap");
        BitSet oopMap = new BitSet(bits);
        if (Klass.TRACING_ENABLED && Tracer.isTracing("oms")) {
            Tracer.traceln("oopmap:{cardinality = " + oopMap.cardinality() + "}");
        }
        return oopMap;
    }

    /**
     * Loads the 'memory' component of the object memory from the input stream.
     *
     * @param size      the size of memory as specified by the 'size' element of the objct memory
     * @return          the contents of the 'memory' component
     */
    protected byte[] loadMemory(int size) {
       // Load the 'memory' component into a byte array
        byte[] buffer = new byte[size];
        reader.readFully(buffer, "memory");
        return buffer;
    }

    /**
     * Relocates the memory.
     *
     * @param parent     the loaded/resolved parent object memory
     * @param buffer     the contents of the 'memory' component
     * @param oopMap     the bit set encapsulating the 'oopmap' component
     * @return the address of the relocated memory buffer
     */
    protected Address relocateMemory(ObjectMemory parent, byte[] buffer, BitSet oopMap) {

        String url = reader.getFileName();
        int size = buffer.length;

        // Calculate the canonical starting address of the memory about to be loaded
        // based on the canonical starting address and size of the parent
        Address canonicalStart = parent == null ? Address.zero() : parent.getCanonicalEnd();

        // If this is the mapper, then the memory model in com.sun.squawk.Address needs to be initialized/appended to
        if (VM.isHosted()) {
            NativeUnsafe.initialize(buffer, oopMap, parent != null);
        }

        // Set up the address at which the object memory will finally reside
        final Address bufferAddress = VM.isHosted() ? canonicalStart : Address.fromObject(buffer);
        final Address relocatedBufferAddress = (loadIntoReadOnlyMemory) ? GC.allocateNvmBuffer(size) : bufferAddress;

        // Null the buffer object as there is no need for the relocation to test whether
        // or not the relocated buffer has moved which it won't have if it is in read-only
        // memory or this host environment is not Squawk
        if (VM.isHosted() || loadIntoReadOnlyMemory) {
            buffer = null;
        }

        // Relocate the pointers to the other object memories against which this object memory is bound
        ObjectMemory.relocateParents(url, buffer, bufferAddress, oopMap, parent, false, requiresEndianSwap, tracing);

        // Relocate the pointers within this object memory
        ObjectMemory.relocate(url, buffer, bufferAddress, oopMap, relocatedBufferAddress, canonicalStart, size, false, requiresEndianSwap, tracing, true);

        Assert.always(oopMap.cardinality() == 0); // "some pointers were not relocated"

        // Swap the endianess if necessary
        if (requiresEndianSwap) {
            ObjectMemory om = new ObjectMemory(bufferAddress, size, "", null, 0, parent);
            ObjectMemoryEndianessSwapper.swap(om, true, false);
        }

        // Copy a relocated object memory into read-only memory if necessary
        if (loadIntoReadOnlyMemory) {
            VM.copyBytes(bufferAddress, 0, relocatedBufferAddress, 0, size, true);
/*if[TYPEMAP]*/
            if (VM.usingTypeMap()) {
                NativeUnsafe.copyTypes(bufferAddress, relocatedBufferAddress, size);
            }
/*end[TYPEMAP]*/
        }

        // Fix up the object memory buffer if it is in RAM so that it now looks
        // like a zero length byte array to the garbage collector
        if (!loadIntoReadOnlyMemory && !VM.isHosted()) {
            GC.setHeaderLength(Address.fromObject(buffer), 0);
        }

        return relocatedBufferAddress;
    }
}




/**
 * An instance of <code>ObjectMemoryReader</code> is used to read an input
 * stream opened on an object memory file.
 *
 */
class ObjectMemoryReader extends StructuredFileInputStream {



    /**
     * Creates a <code>ObjectMemoryReader</code> that reads object memory file components
     * from a given input stream.
     *
     * @param   in        the input stream
     * @param   filePath  the file from which <code>in</code> was created
     */
    public ObjectMemoryReader(InputStream in, String filePath) {
        super(in, filePath, "oms");
    }


    /**
     * {@inheritDoc}
     */
    public Error formatError(String msg) {
        if (msg == null) {
            throw new Error(getFileName());
        } else {
            throw new Error(getFileName() + ": " + msg);
        }
    }
}
