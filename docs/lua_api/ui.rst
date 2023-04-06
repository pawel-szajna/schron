UI functions
============

The UI API allows to manipulate the user interface widgets from LUA scripts.
Some of those functions are registered as soon as :cpp:class:`scripting::Scripting`
is constructed, while others have some preconditions for being available.

Generic UI functions
--------------------

Those functions are available as soon, as :cpp:class:`scripting::Scripting` is
created.

.. lua:function:: ui_createText()

   Creates a full-screen :cpp:class:`text widget <ui::Text>`.

   :return: Text widget ID.
   :rtype: number

.. lua:function:: ui_textClear(id)

   Clears the contents of a text widget.

   :param id: Text widget ID.
   :type id: number

.. lua:function:: ui_textWrite(id, message, font, speed)

   Appends text to a text widget. Internally, finds the text widget with given
   ID, and calls :cpp:func:`ui::Text::write`.

   :param id: Text widget ID.
   :type id: number
   :param message: Text contents to append.
   :type message: str
   :param font: Font face to be used.
   :type font: str
   :param speed: Text typing speed.

Dialogue mode
-------------

Dialogue mode allows for displaying a messages, optionally providing the speaker
name, and presenting player with choices. Despite the name, they may be used for
any interaction, for example observing an object, as long as a zoomed in view and
a message are needed.

Mode management functions
`````````````````````````

Those functions are available while the :cpp:class:`in-game mode <game::ModeInGame>`
is active.

.. lua:function:: dialogue_start()

   Starts the dialogue mode.

   Game :cpp:class:`Dialogue <game::Dialogue>` sub-mode is activated.

   When the dialogue mode is entered, a zoom-in animation is activated, and the
   screen remains zoomed in for the remainder of dialogue.

   This registers the dialogue-related functions, described below. The standard
   player controls (movement, interaction, inventory access, etc.) are disabled
   until this mode is finished. The mode does *not* finish automatically, so if
   the script does not call the :lua:func:`dialogue_end()` function, player will
   be stuck in this mode forever.

.. lua:function:: dialogue_end()

   Ends the dialogue mode.

   A zoom-out animation is activated, and standard player controls are restored.

   Has no effect if the dialogue mode is not active.

Dialogue-related functions
``````````````````````````

Those functions are available while the dialogue mode is active.

.. lua:function:: choice(choices)

   Presents the player with a choice.

   The control is returned to the script only after player selects one of the
   options. The chosen option identifier is stored in a variable called ``result``.

   :param choices: A table of string pairs.

                   The first pair element is the identifier of an option, which
                   will be stored in ``result`` after the player makes the choice.

                   The second pair element is the caption assigned to this option,
                   which is displayed on the choice selection screen.
   :type choices: table\<str, str>

.. lua:function:: text(caption)

   Presents the player with a text message.

   The control is returned to the script only after player presses the enter key.

   :param caption: The text to be presented.
   :type caption: str

.. lua:function:: speech(person, caption)

   Presents the player with a text message along with the speaking person name,
   which is rendered in a different font face.

   The control is returned to the script only after player presses the enter key.

   :param person: The speaker name.
   :type person: str
   :param caption: The text to be presented.
   :type caption: str
