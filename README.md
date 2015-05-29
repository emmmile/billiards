## Billiards

An old university project that implements a Billiards game. No high level directive has been used here, everything has been done from scratch just using lines, circles and boxes. Collisions are detected in an exact manner (basically the time goes forward from one collision to another).

## Installation

You need to have wxwidgets 2.8 installed on your system. On Archlinux you actually need bath `wxgtk` and `wxgtx2.8`:

```
pacman -S wxgtk wxgtk2.8
```

Then just use `make`.

## Screenshot

![screenshot](https://raw.githubusercontent.com/emmmile/billiards/master/screenshot.png)