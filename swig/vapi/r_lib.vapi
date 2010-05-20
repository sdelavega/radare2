/* radare - LGPL - Copyright 2009-2010 pancake<nopcode.org> */

namespace Radare {
	[Compact]
	[CCode (cheader_filename="r_lib.h", cprefix="r_lib_", cname="struct r_lib_t", free_function="r_lib_free")]
	public class RLib {
		public RLib (string symname);
		public bool close(string file);
		public int opendir(string path);
		//public string types_get(int idx);

		/* lowlevel api */
		public static void* dl_open(string libname);
		public void* dl_sym(string symname);
		public static bool dl_close(void *handle);
		public static bool dl_check_filename(string file);
		/* handlers */
	// we need delegates here (function pointerz)
	//	public bool add_handler(int type, string desc, /* */, void* user);
		public bool del_handler(int type);
		public Handler get_handler(int type);
		//public struct Struct { }
		[Compact]
		[CCode (cname="struct r_lib_handler_t*")]
		public struct Handler {
			int type;
			string desc;
			void* user;
			// constructor
			// destructor
		}
	}

	[Compact]
	[CCode (cheader_filename="r_lib.h", cprefix="r_lib_struct_", cname="struct r_lib_struct_t", free_function="r_lib_free")]
	public struct RLibStruct {
		public RLibType foo;
		public void *data;
	}

	[CCode (cprefix="R_LIB_TYPE_", cname="int")]
	public enum RLibType {
		IO,
		DBG,
		LANG,
		ASM,
		ANAL,
		PARSE,
		BIN,
		BININFO,
		BP,
		SYSCALL,
		FASTCALL,
		CRYPTO,
		CMD,
		LAST
	}
}
