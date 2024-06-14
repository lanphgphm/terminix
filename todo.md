# a list of features, idea logs
### Notation 
- TODO has higher priority than IMPROVE 
- TODO i has higher priority than TODO j if i < j

### TODO
- ~~TODO 1.0: fix a neutral-dark theme to make terminal looks more pleasant to the eye~~``
- TODO 1.1: Ctrl+L to clear screen but can scroll back for past results 
- TODO 1.2: implement a command history that can be accessed by up/down arrow keys 
- TODO 1.3: implement SplitView that WORKS 
- TODO 1.4: handle screen exiting -- set screen invisible if other screens exist, Qt.quit() if only screen left 
- TODO 1.5: implement tab-completion 
- TODO 1.6: buffer the readBuffer to wait for output end beforeemitting the WHOLE output to screen -- which would fix the output of programs like `pstree` 

| Issue | Reference file | 
| --- | --- | 
| TODO 1.0 | `screencontroller.cpp` |
| TODO 1.1 | `ScreenView.qml` |
| TODO 1.2 | `ScreenView.qml` |
| TODO 1.3 | `SplitScreenView.qml` |
| TODO 1.4 | `ScreenView.qml` |
| TODO 1.5 | `ScreenView.qml` |
| TODO 1.6 | `ptty.cpp` |

### IMPROVE
- IMPROVE 0.1: a better way to handle password input detection
- IMPROVE 0.2: allowing bracketed pasting 
- IMPROVE 0.3: support ascii art rendering like the output of sl -- maybe swap RichText-PlainText mode depends on the command? 
- IMPROVE 0.4: color root prompt differently  

| Issue | Reference file |
| --- | --- |
| IMPROVE 0.1 | `ScreenView.qml` |
| IMPROVE 0.2 | `screencontroller.cpp` |
| IMPROVE 0.3 | `screencontroller.cpp` |