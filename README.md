# BSDL: Better Simple DirectMedia Layer
BSDL is a cross-platform lib designed to make it easy to write multimedia software, such as games and emulators.

BSDL is based on SDL ( https://github.com/libsdl-org/SDL )

Installation instructions and a quick introduction are available in [INSTALL.md](INSTALL.md)

_____________

# Why was BSDL needed if SDL already exists?
First of all it's needed for my personal needs.

There were a few issues with the SDL lib for me:

* SDL has a lot of legacy code and problems at the architectural level. This creates a situation where SDL is difficult to support and fixing bugs takes too much time. In some cases, hacks are needed to fix a bug. (Important: even so — SDL is amazing and ultrafast!)
* In SDL, some features/bugs will not be fixed even if they are important, like testing/automation support. Not because they are not important, but because they are difficult to fix properly with the SDL codebase.
* Some features will be implemented in the SDL lib in the distant future, but I need them now :)
* Sam Lantinga rejects AI-generated code (even if it is high-quality and not AI slop). It doesn't mean that Sam is wrong -- we just have two incompatible visions/ideologies.

_____________

# License

### All rights to the code belong to Sam Lantinga (slouken@libsdl.org),
not me (ukushu).

This library is distributed under the terms of the zlib license, available in [LICENSE.txt](LICENSE.txt).

I don't want to own even a single line of code in this lib.

If one day Sam Lantinga comes and sees that this repo is better, I hope he will take it and make it the new base of SDL :) It's OK for me.

# History

* This fork was made from SDL 3.4.18. No fixes from newer releases have been taken at the moment.
