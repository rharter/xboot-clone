/*
 * kernel/core/menu.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <charset.h>
#include <malloc.h>
#include <xml.h>
#include <xboot/list.h>
#include <xboot/menu.h>

static struct menu_list __menu_list = {
	.entry = {
		.next	= &(__menu_list.entry),
		.prev	= &(__menu_list.entry),
	},
};
static struct menu_list * menu_list = &__menu_list;

static x_bool menu_add_item(const char * title, const char * command)
{
	struct menu_list * list;
	struct menu_item * item;

	if(!title || !command)
		return FALSE;

	list = malloc(sizeof(struct menu_list));
	if(!list)
		return FALSE;

	item = malloc(sizeof(struct menu_item));
	if(!item)
	{
		free(list);
		return FALSE;
	}

	item->title = (char *)utf8_strdup((const x_s8 *)title);
	item->command = (char *)utf8_strdup((const x_s8 *)command);

	list->item = item;
	list_add_tail(&list->entry, &menu_list->entry);

	return TRUE;
}

x_bool menu_load(char * file)
{
	struct menu_list * pos, * n;
	struct xml * root, * menu;
	struct xml * title, * command;

	/*
	 * check the xml file contained menu
	 */
	root = xml_parse_file(file);
	if(!root || !root->name)
		return FALSE;

	if(strcmp((const x_s8 *)root->name, (const x_s8 *)"menu") != 0)
	{
		xml_free(root);
		return FALSE;
	}

	/*
	 * delete all of the menu item
	 */
	list_for_each_entry_safe(pos, n, &menu_list->entry, entry)
	{
		free(pos->item->title);
		free(pos->item->command);
		free(pos->item);
		list_del(&pos->entry);
		free(pos);
	}

	/*
	 * add menu item
	 */
	for(menu = xml_child(root, "menu"); menu; menu = menu->next)
	{
		title = xml_child(menu, "title");
		command = xml_child(menu, "command");

		if(title && command)
			menu_add_item(title->txt, command->txt);
	}

	xml_free(root);
	return TRUE;
}

inline struct menu_list * get_menu_list(void)
{
	return menu_list;
}

struct menu_item * get_menu_indexof_item(x_s32 index)
{
	struct menu_list * list;
	struct list_head * pos;
	x_s32 i;

	for(i = 0, pos = (&menu_list->entry)->next; pos != (&menu_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct menu_list, entry);
		if(i++ == index)
			return list->item;
	}

	return NULL;
}