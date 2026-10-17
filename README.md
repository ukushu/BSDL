# BSDL: Better Simple DirectMedia Layer

BSDL is a cross-platform library designed to make it easy to write multimedia software, such as games and emulators.

BSDL is based on [SDL](https://github.com/libsdl-org/SDL)

Installation instructions and a quick introduction are available in [INSTALL.md](INSTALL.md).

_____________

# Why does BSDL exist if SDL already exists?

BSDL was created primarily for my own development needs.

There are a few areas where my requirements differ from SDL:

* SDL has accumulated legacy code and architectural constraints that can make some changes and bug fixes difficult.
* SDL and BSDL have different policies regarding AI-assisted code.
* Some features and fixes I need are difficult to implement cleanly within the current SDL architecture (BSDL fixes this)
* Some features may eventually appear in SDL, but I need them now.

But still SDL is an amazing and extremely fast library. 
BSDL simply explores a different architectural and development approach.

_____________

# License

The original SDL code remains copyrighted by its respective authors and contributors. Especially Sam Lantinga (slouken@libsdl.org). 

BSDL is distributed under the terms of the zlib license, available in [LICENSE.txt](LICENSE.txt) - same license as SDL.

I do not claim authorship of the original SDL code.

If improvements from BSDL are ever useful to SDL, I would be happy to see them adopted upstream.

# History

* BSDL was forked from SDL 3.4.18.
* No fixes from newer SDL releases are included unless explicitly stated.
