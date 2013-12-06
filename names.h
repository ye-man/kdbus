/*
 * Copyright (C) 2013 Kay Sievers
 * Copyright (C) 2013 Greg Kroah-Hartman <gregkh@linuxfoundation.org>
 * Copyright (C) 2013 Daniel Mack <daniel@zonque.org>
 * Copyright (C) 2013 Linux Foundation
 *
 * kdbus is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 */

#ifndef __KDBUS_NAMES_H
#define __KDBUS_NAMES_H

#include <linux/hashtable.h>

#include "internal.h"

/**
 * struct kdbus_name_registry - names resgistered for a bus
 * @kref:		Reference count
 * @entries_hash:	Map of entries
 * @entries_lock:	Registry data lock
 */
struct kdbus_name_registry {
	struct kref		kref;
	DECLARE_HASHTABLE(entries_hash, 6);
	struct mutex		entries_lock;
};

/**
 * struct kdbus_name_entry - well-know name entry
 * @name:		The well-known name
 * @flags:		KDBUS_NAME_* flags
 * @queue_list:		List of queued waiters for the well-known name
 * @conn_entry:		Entry in connection
 * @hentry:		Entry in registry map
 * @conn:		Connection owning the name
 * @starter:		Connection of the starter queuing incoming messages
 */
struct kdbus_name_entry {
	char			*name;
	u64			flags;
	struct list_head	queue_list;
	struct list_head	conn_entry;
	struct hlist_node	hentry;
	struct kdbus_conn	*conn;
	struct kdbus_conn	*starter;
};

int kdbus_name_registry_new(struct kdbus_name_registry **reg);
void kdbus_name_registry_unref(struct kdbus_name_registry *reg);

int kdbus_name_acquire(struct kdbus_name_registry *reg,
		       struct kdbus_conn *conn,
		       const char *name, u64 flags,
		       struct kdbus_name_entry **entry);
int kdbus_cmd_name_acquire(struct kdbus_name_registry *reg,
			   struct kdbus_conn *conn,
			   void __user *buf);
int kdbus_cmd_name_release(struct kdbus_name_registry *reg,
			   struct kdbus_conn *conn,
			   void __user *buf);
int kdbus_cmd_name_list(struct kdbus_name_registry *reg,
			struct kdbus_conn *conn,
			void __user *buf);

struct kdbus_name_entry *kdbus_name_lookup(struct kdbus_name_registry *reg,
					   const char *name);
void kdbus_name_remove_by_conn(struct kdbus_name_registry *reg,
			       struct kdbus_conn *conn);

bool kdbus_name_is_valid(const char *p);
#endif
