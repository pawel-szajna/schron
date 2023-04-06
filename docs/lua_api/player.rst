Player functions
================

Functions related to the player state. They are registered when the
:cpp:class:`game::Player` is constructed, which essentially means that they
are available while the :cpp:class:`in-game <game::ModeInGame>` mode is active.

General
-------

.. lua:function:: place(sector, x, y, z, a, fovH, fovV)

   Changes the player position.

   :param sector: ID of the sector.
   :type sector: number
   :param x: X-coordinate of the position.
   :type x: number
   :param y: Y-coordinate of the position.
   :type y: number
   :param z: Z-coordinate of the position.
   :type z: number
   :param a: Angle of the position.
   :type a: number
   :param fovH: Horizontal field of view.
   :type fovH: number
   :param fovV: Vertical field of view.
   :type fovV: number

Sanity
------

The current value of player sanity is always available in the ``sanity`` variable.

The sanity value is always contained between ``0`` (which means absolutely insane)
and ``100`` (meaning full psychical strength). If a change is requested which would
break those limits, the maximum or minimum value is set instead.

.. lua:function:: sanity_set(value)

   Sets the player sanity to an absolute value.

   :param value: Target sanity value.
   :type value: number

.. lua:function:: sanity_mod(delta)

   Changes the player sanity by a given amount.

   :param delta: Desired change in sanity.
   :type delta: number

Inventory
---------

.. lua:function:: item_add(name, description)

   Provides player an item.

   :param name: Name of the item.
   :type name: str
   :param description: Description of the item.
   :type description: str
   :return: ID of the item.
   :rtype: number

.. lua:function:: item_check(name)

   Check player inventory for an item.

   :param name: Name of the item to check.
   :type name: str
   :return: Item ID when player has the item, ``-1`` otherwise.
   :rtype: number

.. lua:function:: item_remove(id)

   Removes item from player's inventory.

   :param id: ID of the item to remove.
   :type id: number
