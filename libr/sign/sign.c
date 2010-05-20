/* radare - LGPL - Copyright 2009-2010 pancake<nopcode.org> */

#include <r_sign.h>
#include <r_anal.h>

R_API RSign *r_sign_new() {
	RSign *sig;

	sig = R_NEW (RSign);
	if (sig) {
		sig->s_byte = sig->s_anal = 0;
		sig->prefix[0] = '\0';
		sig->printf = (FunctionPrintf) printf;
		INIT_LIST_HEAD (&(sig->items));
	}
	return sig;
}

R_API void r_sign_prefix(RSign *sig, const char *str) {
	strncpy (sig->prefix, str, sizeof (sig->prefix));
	sig->prefix[sizeof (sig->prefix)] = '\0';
}

R_API int r_sign_add(RSign *sig, RAnal *anal, int type, const char *name, const char *arg) {
	int len, ret = R_FALSE;
	char *data;
	RSignItem *si; // TODO: like in r_search.. we need r_sign_item_new ()
			// TODO: but..we need to use a pool here..

	if (!name || !arg || !anal)
		return R_FALSE;

	switch (type) {
	case R_SIGN_BYTES:
	case R_SIGN_FUNC:
		si = R_NEW (RSignItem);
		if (si == NULL)
			break;
		si->type = type;
		snprintf (si->name, sizeof (si->name), "%s.%s",
			*sig->prefix?sig->prefix:"sign", name);
		data = r_anal_strmask (anal, arg);
		if (data == NULL)
			break;
		len = strlen (data);
		si->bytes = (ut8 *)malloc (len);
		si->mask = (ut8 *)malloc (len);
		if (si->bytes == NULL || si->mask == NULL) {
			eprintf ("Cannot malloc\n");
			free (si->mask);
			free (si->bytes);
			free (si);
			break;
		}
		si->size = r_hex_str2binmask (data, si->bytes, si->mask);
		if (si->size<1) {
			free (si->bytes);
			free (si);
		} else list_add_tail (&(si->list), &(sig->items));
		sig->s_byte++;
		free (data);
		break;
	default:
	case R_SIGN_ANAL:
		eprintf ("r_sign_add: TODO\n");
		break;
	}
	return ret;
}

R_API void r_sign_list(RSign *sig, int rad) {
	if (rad) {
		struct list_head *pos;
		sig->printf ("zp-");
		list_for_each (pos, &sig->items) {
			RSignItem *si = list_entry (pos, RSignItem, list);
			sig->printf ("z%c %s ...\n", si->type, si->name); // TODO : show bytes
		}
	} else {
		sig->printf ("Loaded %d signatures\n", sig->s_byte + sig->s_anal);
		sig->printf ("  %d byte signatures\n", sig->s_byte);
		sig->printf ("  %d anal signatures\n", sig->s_anal);
	}
}

R_API void r_sign_reset(RSign *sig) {
	struct list_head *pos, *n;
	list_for_each_safe (pos, n, &sig->items) {
		RSignItem *i = list_entry (pos, RSignItem, list);
		free (i->bytes);
		free (i);
	}
	INIT_LIST_HEAD (&(sig->items));
}

R_API RSign *r_sign_free(RSign *sig) {
	r_sign_reset (sig);
	free (sig);
	return NULL;
}

R_API RSignItem *r_sign_check(RSign *sig, const ut8 *buf, int len) {
	struct list_head *pos;
	list_for_each (pos, &sig->items) {
		RSignItem *si = list_entry (pos, RSignItem, list);
		if (si->type == R_SIGN_BYTES) {
			int l = (len>si->size)?si->size:len;
			if (!r_mem_cmp_mask (buf, si->bytes, si->mask, l))
				return si;
		}
	}
	return NULL;
}
