# zubrobot-ws

C++ [ZUBR exchange](https://zubr.io/) trading robot sample
<br>
Orders price calculated as:
```
BUY = (current best purchase price + current best sale price) / 2 - interest - shift * position
SELL = (current best purchase price + current best sale price) / 2 + interest - shift * position
```
<br>
**For educational purpose ONLY. Don't use for real trading!**
<br>

## dependencies

- WebSocket++
- Boost.System
- Boost.Log
- RapidJSON
- OpenSSL
- pthreads

## build tools
- CMake
