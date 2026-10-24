/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write see:
 *             <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "test_assert.h"

#include <stdlib.h>
#include <string.h>

#include <espeak-ng/speak_lib.h>

#include "espeak_command.h"

static void test_text_is_terminated_when_size_excludes_nul(void)
{
	char text[3] = { 'a', 'b', 'c' };
	t_espeak_command *cmd = create_espeak_text(text, sizeof(text), 0, POS_CHARACTER, 0, 0, NULL);

	TEST_ASSERT(cmd != NULL);
	TEST_ASSERT(strcmp((const char *)cmd->u.my_text.text, "abc") == 0);
	delete_espeak_command(cmd);
}

static void test_mark_is_terminated_when_size_excludes_nul(void)
{
	char text[3] = { 'a', 'b', 'c' };
	t_espeak_command *cmd = create_espeak_mark(text, sizeof(text), "mark", 0, 0, NULL);

	TEST_ASSERT(cmd != NULL);
	TEST_ASSERT(strcmp((const char *)cmd->u.my_mark.text, "abc") == 0);
	delete_espeak_command(cmd);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	test_text_is_terminated_when_size_excludes_nul();
	test_mark_is_terminated_when_size_excludes_nul();
	return 0;
}
