# a list of features, idea logs
### Notation 
- TODO has higher priority than IMPROVE, higher than CONCERN
- CONCERNs are questions, not necessarily issues that need to be fixed
- Issues with `!` are critical that should be prioritized, the more `!` the more critical

### TODO
- ~~TODO 1.0: fix a neutral-dark theme to make terminal looks more pleasant to the eye~~``
- TODO 1.1: Ctrl+L to clear screen but can scroll back for past results 
- ~~TODO 1.2: implement a command history that can be accessed by up/down arrow keys~~ --> only works in non-root mode. needs fixes for `/root/.bash_history`, preferably rewrite the 2 load functions in c++ instead of qml to avoid using QFile? 
- TODO 1.3!: implement SplitView that WORKS 
- TODO 1.4: handle screen exiting -- set screen invisible if other screens exist, Qt.quit() if only screen left 
- TODO 1.5: implement tab-completion 
- TODO 1.6: buffer the readBuffer to wait for output end beforeemitting the WHOLE output to screen -- which would fix the output of programs like `pstree` 
- TODO 1.7!: cannot send signals (SIGINT, SIGTSTP, etc.) when in root mode

| Issue | Reference file | 
| --- | --- | 
| TODO 1.0 | `screencontroller.cpp` |
| TODO 1.1 | `ScreenView.qml` |
| TODO 1.2 | `ScreenView.qml` |
| TODO 1.3 | `SplitScreenView.qml` |
| TODO 1.4 | `ScreenView.qml` |
| TODO 1.5 | `ScreenView.qml` |
| TODO 1.6 | `ptty.cpp` |
| TODO 1.7 | `ptty.cpp` |

### IMPROVE
- IMPROVE 1.0: a better way to handle password input detection
- IMPROVE 1.1: allowing bracketed pasting 
- IMPROVE 1.2: support ascii art rendering like the output of sl -- maybe swap RichText-PlainText mode depends on the command? 
- ~~IMPROVE 1.3: color root prompt differently~~

| Issue | Reference file |
| --- | --- |
| IMPROVE 1.0 | `ScreenView.qml` |
| IMPROVE 1.1 | `screencontroller.cpp` |
| IMPROVE 1.2 | `screencontroller.cpp` |
| IMPROVE 1.3 | `screencontroller.cpp` |

### CONCERN
- CONCERN 1.0: do we need to handle the case where the command is not written in full? like how can this happen? 
- CONCERN 1.1: readBuffer count = 0 means the pty has been closed by some child process, but how exactly? 

| Issue | Reference file |
| --- | --- |
| CONCERN 1.0 | `ptty.cpp` |
| CONCERN 1.1 | `ptty.cpp` |