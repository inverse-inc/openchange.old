/* 
   OpenChange MAPI implementation testsuite

   Create a Exchange User

   Copyright (C) Julien Kerihuel 2007
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <libmapi/libmapi.h>
#include <gen_ndr/ndr_exchange.h>
#include <param.h>
#include <credentials.h>
#include <torture/mapi_torture.h>
#include <torture/torture.h>
#include <torture/torture_proto.h>
#include <samba/popt.h>
#include <dcerpc/samr.h>

bool torture_mapi_createuser(struct torture_context *torture)
{
	NTSTATUS		ntstatus;
	enum MAPISTATUS		retval;
	TALLOC_CTX		*mem_ctx;
	struct mapi_profile	*profile;
	struct test_join        *user_ctx = (struct test_join *) NULL;
	const char		*username = lp_parm_string(-1, "exchange", "username");
	const char		*user_password = lp_parm_string(-1, "exchange", "password");

	/* sanity checks */
	if (!username) {
		printf("Specify the username to create with exchange:username\n");
		return False;
	}

	/* init mapi */
	mem_ctx = talloc_init("torture_mapi_createuser");
	retval = torture_load_profile(mem_ctx);
	if (retval != MAPI_E_SUCCESS) return False;

	profile = global_mapi_ctx->session->profile;

	/* Create the user in the AD */
	user_ctx = torture_create_testuser(username, profile->domain, 
					   ACB_NORMAL,
					   (const char **)&user_password);

	if (!user_ctx) {
		printf("Failed to create the user\n");
		return False;
	}

       /* We extend the user with Exchange attributes */
	ntstatus = torture_exchange_createuser(mem_ctx, username, torture_join_user_sid(user_ctx));
       if (!NT_STATUS_IS_OK(ntstatus)) {
	       torture_leave_domain(user_ctx);
	       talloc_free(mem_ctx);
	       return False;
       }

       return True;
}
