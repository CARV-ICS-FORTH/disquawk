package com.sun.squawk.builder.commands;

import java.io.File;
import java.io.PrintStream;

import com.sun.squawk.builder.Build;
import com.sun.squawk.builder.Command;

/**
 * @author Dave
 * @author Foivos S. Zakkak
 *
 */
public abstract class FormicCommand extends Command {

	protected String platformName;

	/**
	 * @param env
	 * @param name
	 */
	public FormicCommand(Build env, String name) {
		super(env, name);
	}

	public void usage(String errMsg) {
	    PrintStream out = System.out;

	    out.println();
	    out.println(errMsg);
	    out.println();
	    out.println(getDescription());
	    out.println("usage: " + getName() + " [ -h | -clean | -platform:[host | formic] ]");
	}

	/**
	 * {@inheritDoc}
	 */
	public void run(String[] args) {
	    if (args.length == 0) {
	    	usage("");
	    } else {
	        if (args[0].equals("-clean")) {
	            clean();
	        } else if (args[0].startsWith("-platform:")) {
	        	platformName = args[0].substring(10);
	        	build();
	        } else {
	        	usage("");
	        }
	    }
	}

	protected abstract void build();

}
