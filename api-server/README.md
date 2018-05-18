# API Server

This file describes whatever I can document of the REST backend server provided
in this directory.

## Why?

The idea behind having a REST api that can control the LED strip is to be able
to write interfaces that interact with this like Web pages or even a mobile app
(which is the original goal).

It is written in ruby at the moment because I wanted to learn ruby and I want
to test out in the future having this API run on [COAP](http://coap.technology/)
*and* HTTP and to the extent that I've tested so far,
 [david](https://rubygems.org/gems/david/versions/0.4.5) appears to work like
a drop in, so adding COAP to the server whenever I manage to write a COAP
library in Dart for the app should be easy.

## Current State

Currently, there is a simple wrapper around what's available with XPC at the
`/strip*` endpoints. Not much of what's available there is supposed to be used:
there is a way to control each LED's color by PUTing an integer array to
`/strip/:id/contents`, but it is not a good idea to use it for anything but
testing, really. It might be useful, though, for a user to configure their
strip's length, which can be done by PUTing it to `/strips/:id`.

There is also a mode API, which exposes the zeromq interface as a REST API a
well. At the moment, there is little to no validation of objects posted to
the mode API and that is a problem that should be addressed as I further
develop this. My aim is to have a safe system with descriptive error messages
and any PRs that might help me get there are welcome.

## Current API (will change)

## Strip

### GET `/strips`

Returns an array with data for each strip available on the device. Something
like:

```
[{
	"state": "on",
	"length": 50
},
{...},
...
]
```

### GET `/strips/:id`

Returns an object with data for the strip identified by its index `id`. The
strip object looks like:

```
{
	"state": "off",
	"length": 50
}
```

### PUT `/strips/:id`

Used for setting the state or the strip's length.

If the request's body is something like
```
{
	"state": "on",
	"length": 50
}
```

the result would be turning the strip on and setting its length to 50 LEDs.

### GET `/strips/:id/contents`

Returns the contents of the strip at index `id`.

```
[ 0, 0, 0, 0, ..., 0 ]
```

being it an array of color integers.

### PUT `/strips/:id/contents`

**note:** don't use for anything other than testing.

Posting an array like

```
[ 0, ..., 0 ]
\-----------/
length elements long
```

## Mode

### GET `/modes`

Returns an array of all stored modes as mode objects.

### POST `/modes`

If the request body is a valid mode (check below to see more on modes), it
adds that mode to the stored modes.

### GET `/modes/:id`

Returns a specific mode, identified by `id`. At the moment, mode objects look
like the following.

```
{
	"id": "mode@id",
	"name": "Example mode name.",
	"engine": <recognizable by the engine>,
	"data": <recognizable by the engine>,
	"params": {
		<parameters for the mode and their metadata>
	}
}
```

The fields shown above are mandatory and trying to add a mode without any of
those will yield a bad request.

`engine` and `data` fileds must be recognizable by the target mode runner
(also called mode/animation engine))m), but not necessarily need to be
parsed by the API server

The fields shown above are mandatory and trying to add a mode without any of
those will yield a bad request.

`engine` and `data` fileds must be recognizable by the target mode runner
(also called mode/animation engine), but will not necessarily be parsed by
the API server.

`params` must be a map where the keys are parameter names and the values
are objects like:

```
{
	"label": "Useful Param",
	"description": "The most useful param",
	"type": "integer",
	"default": 1234,
	"value": 0
}
```

`label` and `description` will be used when the app generates an UI to control
the parameters; `type` describes the type expected for the parameter; `default`
is the default value for the parameter and `value` is the current value for
the parameter.

#### A note on strings

For the purposes of localization of things such as the names, labels and
descriptions, inside the mode object, there may also be a `strings` map that
looks like

```
"strings" : {
	"en": {
		"useful-param-label": "Useful Param",
		"useful-param-desc": "The most useful param"
	},
	"pt-br": {
		"useful-param-label": "Parametro util",
		"useful-param-desc": "O parametro mais util"
	}
}
```

and then the a parameter could look like this:

```
{
	"label": "$useful-param-label",
	"description": "$useful-param-desc",
	"type": "integer",
	"default": 1234,
	"value": 0
}
```

The interface generated will then use the correct locale based on user settings.
By default, the first available locale is normally used when the preferred by
the user is unavailable.

### PUT `/modes/:id`

Allows the user to update data on a mode.

### DELETE `/modes/:id`

Deletes the mode identified by `id` from the database.

### GET `/modes/current`

Gets the current running mode. This is not necessarily saved.

### POST `/modes/current`

Sets the current running mode, sending a notification to the subscriber network.

The posted mode can be either just an object containing an ID that already
exists in the mode database or it can be a mode object that will be set to
the current mode, but **NOT** saved.

### PUT `/modes/current/params`

Sets the parameters for the mode. There is no validation for this yet. The
PUT object should be a map where the keys are parameter names and the values
are the values for the parameters. In the future both the API server and the
engine should run some validations against the types specified in the mode
object.

Example:

```
{
	"color": "#FF000000",
	"other": 123,
	"another": <maybe something the engine recognizes>
}
```

## Finally a somewhat of a mode example

```
{
  "id": "base#static",
  "strings": {
    "en": {
      "name": "Static",
      "description": "Static color mode",
      "color-label": "Color",
      "color-description": "Strip color"
    },
    "pt-br": {
      "name": "Estático",
      "description": "Modo estático",
      "color-label": "Cor",
      "color-description": "Cor para a faixa"
    }
  },

  "name": "$name",
  "description": "$description",
  "author": "Leonardo",
  "engine": "Lua",

  "params": {
    "color": {
      "label":"#color-label",
      "description": "#color-description",
      "type": "color",
      "default": "#FFFFFF",
      "value": "#FF0000"
    }
  },

  "data": {
    "realtime": false,
    "data": "under development"
  }
}
```
