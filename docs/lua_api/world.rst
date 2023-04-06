World functions
===============

Those functions are used to manipulate the environment. They are registered when
:cpp:class:`scripting::Scripting` is created.

Layout functions
----------------

Those functions are used to describe the layout of the level. While they may be
called from within scripts, they main intent is to be used in the ``map.lua`` files
to describe the initial layout of the level, and they do not validate if the
identifiers are in fact unique, which may cause a crash when engine attempts to
render an invalid geometry.

The calls are automatically generated when a level is saved in the level editor.

.. lua:function:: sector_create(sectorId, floor, floorTexture, ceiling, ceilingTexture)

   Creates an empty sector in the currently loaded level.

   :param sectorId: ID of the sector to be created. Has to be unique.
   :type sectorId: number
   :param floor: Z-coordinate of the sector floor.
   :type floor: number
   :param floorTexture: Texture of the sector floor.
   :type floorTexture: str
   :param ceiling: Z-coordinate of the sector ceiling.
   :type ceiling: number
   :param ceilingTexture: Texture of the sector ceiling.
   :type ceilingTexture: str

.. lua:function:: sector_wall(sectorId, texture, x1, y1, x2, y2)

   Adds a wall to the sector. The walls should be added in clockwise vertex direction
   so that the engine renders them properly.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param texture: Texture of the wall.
   :type texture: str
   :param x1: X-coordinate of the wall start vertex.
   :type x1: number
   :param y1: Y-coordinate of the wall start vertex.
   :type y1: number
   :param x2: X-coordinate of the wall end vertex.
   :type x2: number
   :param y2: Y-coordinate of the wall end vertex.
   :type y2: number

.. lua:function:: sector_portal(sectorId, texture, x1, y1, x2, y2, target)

   Adds a wall in the sector with a portal to the target sector. Behaves exactly as
   :lua:func:`sector_wall` with the addition of:

   :param target: ID of the portal target sector.
   :type target: number

.. lua:function:: sector_transform(sectorId, texture, x1, y1, x2, y2, target, transformX, transformY, transformZ, transformAngle)

   Adds a wall in the sector with a portal, which transforms the coordinates and viewing
   angle, to the target sector. Behaves exactly as :lua:func:`sector_portal` with the
   addition of:

   :param transformX: X-coordinate transformation when passing the portal.
   :type transformX: number
   :param transformY: Y-coordinate transformation when passing the portal.
   :type transformY: number
   :param transformZ: Z-coordinate transformation when passing the portal.
   :type transformZ: number
   :param transformAngle: Angle transformation when passing the portal.
   :type transformX: number

.. lua:function:: sprite_create(sectorId, spriteId, texture, x, y, z, offset, shadows, lightCenter, blocking)

   Adds a sprite to the sector.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param spriteId: Sprite ID to be added. Has to be unique.
   :type spriteId: number
   :param texture: Texture of the sprite.
   :type texture: str
   :param x: X-coordinate of the sprite.
   :type x: number
   :param y: Y-coordinate of the sprite.
   :type y: number
   :param z: Z-coordinate of the sprite.
   :type z: number
   :param offset: Movement of rendered sprite texture along the Z-axis.
   :type offset: number
   :param shadows: Whether the sprite casts shadows.
   :type shadows: boolean
   :param lightCenter: Point along the Z-axis used to calculate sprite lighting.
   :type lightCenter: number
   :param blocking: Whether the sprite prevents player from entering a sector.
   :type blocking: boolean

.. lua:function:: sprite_texture(sectorId, spriteId, angle, texture)

   Adds a texture to the sprite.

   The textures are displayed based on the viewing angle of player. When the player position
   angle reaches the ``angle`` value of the sprite texture, but is smaller than the ``angle``
   value of the next sprite texture, this texture is used for the sprite.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param spriteId: ID of the sprite.
   :type spriteId: number
   :param angle: Player position angle value needed for the texture to activate.
   :type angle: number
   :param texture: The texture file.
   :type texture: str

.. lua:function:: light_create(sectorId, x, y, z, r, g, b)

   Adds a light source to the sector.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param x: X-coordinate of the light.
   :type x: number
   :param y: Y-coordinate of the light.
   :type y: number
   :param z: Z-coordinate of the light.
   :type z: number
   :param r: Intensity of the red light.
   :type r: number
   :param g: Intensity of the green light.
   :type g: number
   :param b: Intensity of the blue light.
   :type b: number

Interaction functions
---------------------

Those functions are meant to be used in the scripts.

.. lua:function:: load_texture(texture)

   Causes the texture to be loaded when the engine is initialized for the current level.
   Has no effect when the level is already loaded.

   This function is intended to be used in the main level script file to mark the need to
   pre-load a texture which is dynamically added to the level during some later script
   execution (for example: a sprite texture will be changed). While any non-loaded texture
   loads automatically when needed, pre-loading all textures can reduce delays related to
   I/O operations.

   :param texture: The texture file.
   :type texture: str

.. lua:function:: change_texture(sectorId, spriteId, texture)

   Changes the base texture of a sprite.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param spriteId: ID of the sprite.
   :type spriteId: number
   :param texture: The texture file.
   :type texture: str

.. lua:function:: interactive_point(sectorId, x, y, script)

   Marks a point as interactive.

   When facing an interactive point, player will be prompted to interact. When player chooses
   to interact, a script will be launched.

   The interaction prompt is not dependent on the Z-coordinate of the player. As long as player
   is in the sector and is facing the proper X/Y-coordinates, the prompt will be displayed.

   :param sectorId: ID of the sector.
   :type sectorId: number
   :param x: X-coordinate of the interactive point.
   :type x: number
   :param y: Y-coordinate of the interactive point.
   :type y: number
   :param script: The script filename to be launched on interaction.
   :type script: str
