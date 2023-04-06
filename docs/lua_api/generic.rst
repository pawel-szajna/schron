General purpose functions
=========================

Those functions, which serve as general purpose functions, are registered
as soon as :cpp:class:`scripting::Scripting` is constructed.

Logging utility functions
-------------------------

Those functions are used to display messages using the ``spdlog`` module.

.. lua:function:: debug(msg)

   Display a debug level message.

   :param msg: Debug message contents.
   :type msg: str

.. lua:function:: info(msg)

   Display an info level message.

   :param msg: Info message contents.
   :type msg: str

.. lua:function:: warning(msg)

   Display a warning message.

   :param msg: Warning message contents.
   :type msg: str
