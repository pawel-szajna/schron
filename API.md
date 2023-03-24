# Schron scripting API

## Dialogue mode

### `choice(options)`

**Arguments**:

* `options`: a table of pairs `{label, caption}`, where:
  * `label` *string*: the value returned when the player selects this option
  * `caption` *string*: the option as presented to the player

The function displays a selection of options to the player. When the player selects an
option, the variable `result` is set to contain the `label` of the chosen option.

**Example**:

```lua
choice({{'death', 'I hope everything you love dies!'},
        {'penalty', 'Behaviour like yours should be penalized.'}})
-- The script execution will be resumed after the player selects
-- one of the options. The variable "resut" will contain either the
-- string "death" or "penalty".
if result == 'death' then
    -- Player chose the first option
else
    -- Player chose the second option
end
```

### `dialogue_end()`

**Arguments**: none

The function exits the dialogue mode, restoring the player capability to move and the
default camera angle.

### `dialogue_start()`

**Arguments**: none

The function enters the dialogue mode, preventing player movement and zooming the camera
in. This mode is retained until `dialogue_end()` is called.

### `speech(person, caption)`

* `person` *string*: the name of the person speaking the text
* `caption` *string*: the text to be presented

This function behaves exactly like `text()`, but the value of `person` argument is
printed before the `caption` with a different font.

### `text(caption)`

**Arguments**:

* `caption` *string*: the text string which should be presented to the player

The function presents the text to the player. The text is animated, the animation can
be skipped by pressing the Enter key. After the entire text is displayed, the script
execution is only resumed after player presses the Enter key.

## Player-related functionality

### Variable: `sanity`

Holds the current value of player sanity. Automatically updated whenever player sanity
changes. The value is always within the range of `0`–`100`, where `100` means absolutely
sane, and `0` means totally insane.

Should the value ever become less than zero or exceed  one hundred, it will be clamped
to fit within those boundaries.

### `sanity_mod(delta)`

**Arguments**:

* `delta` *int*: delta of player sanity

Sets the value of player sanity to `sanity + delta`. For instance, calling `sanity_mod(3)`
increases the value of player sanity by 3, whereas calling `sanity_mod(-5)` decreases it
by 5. The final value is clamped to fit within the range of `0`–`100`.

### `sanity_set(value)`

**Arguments**:

* `value` *int*: desired value of player sanity

Sets the value of player sanity to the absolute value of `value`. The value is clamped
to fit within the range of `0`–`100`.
