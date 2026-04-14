# ARCs

<p>
  <a href="https://apps.repebble.com/40d48d07d398413fbe331dc3"><img src="https://img.shields.io/static/v1?label=App%20Listing&message=%20&logoColor=white&color=833FD6&style=flat"/></a>
  <a href="https://developer.repebble.com/dashboard/"><img src="https://img.shields.io/static/v1?label=Pebble%20Developer%20Portal&message=%20&logoColor=white&color=FC6251&style=flat"/></a> * 
  <a href="https://github.com/thementalgoose/pebble-arcs/releases"><img src="https://img.shields.io/github/v/release/thementalgoose/pebble-arcs"/></a> 
</p>

Simple pebble watchface with metric arcs displayed in each corner

| Codename | Pebble name       | Watchface                                               |
|----------|-------------------|---------------------------------------------------------|
| aplite   | Pebble            | <img src="appstore/basalt/watchface.png" width="120"/>  |
| basalt   | Pebble Time       | <img src="appstore/basalt/watchface.png" width="120"/>  |
| chalk    | Pebble Time Round | <img src="appstore/chalk/watchface.png" width="120"/>   |
| diorite  | Pebble 2          | <img src="appstore/diorite/watchface.png" width="120"/> |
| emery    | Pebble Time 2     | <img src="appstore/emery/watchface.png" width="150"/>   |
| flint    | Pebble 2 Duo      | <img src="appstore/flint/watchface.png" width="120"/>   |
| gabbro   | Pebble Round 2    | <img src="appstore/gabbro/watchface.png" width="150"/>  |

### Currently supported

- [x] Adaptive to all supported pebble sizes
- [x] Configure the colour of each corner
- [x] Configure the metric for each corner
- [x] Add weather based indicators
- [x] Add health based indicators

#### Building

- Python 3.10.x required

```bash
# Booting the emulator
pebble install --emulator flint

# Building
pebble build

# Running
pebble install
pebble install --cloudpebble 
pebble install --emulator flint --logs
```

#### Useful Links

- [Hardware information](https://developer.rebble.io/guides/tools-and-resources/hardware-information/)
- [UI Samples](https://github.com/pebble-examples/ui-patterns/)
- [Modular Architecture](https://github.com/pebble-examples/modular-app-example/blob/master/src/windows/main_window.h)
- [Best Practices](https://developer.rebble.io/guides/best-practices/modular-app-architecture/)
