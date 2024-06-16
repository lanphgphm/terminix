# a list of features, idea logs
### Notation 
- TODO has higher priority than IMPROVE, higher than CONCERN
- CONCERNs are questions, not necessarily issues that need to be fixed
- Issues with `!` are critical that should be prioritized, the more `!` the more critical

### TODO
- ~~TODO 1.0: fix a neutral-dark theme to make terminal looks more pleasant to the eye~~``
- TODO 1.1: Ctrl+L to clear screen but can scroll back for past results 
    - i have learned that most terminals handle this feature with ANSI escape codes, not padding the screen or adding empty lines
    - i have tried to do screenpadding but it always pushes new text off of the screen which is even worse than not having autoscroll 
    - the lesson is to parse ANSI escape codes correctly and handle a variety of them, not just discarding non-visual characters 
- ~~TODO 1.2: implement a command history that can be accessed by up/down arrow keys~~ 
    - currently, this works with the user that runs the app. switching user (like to root, or another user) should switch to a new `.bash_history`, but the corresponding history file is often reported to be not found or not readable. 
    - most commented out code from screencontroller.cpp is related to this issue. the buggy code is commented out to improve the load time of the app but still left breadcrumbs for future attempts 
- ~~TODO 1.3!!: implement SplitView that WORKS~~
    - current view splits but not automatically resizing. issue is due to the fact that all splitviews are children of the rootspitview
    - the view family tree has height = 1, all children of rootSplitView 
    - due to this, the text is not wrapping as expected either 
- ~~TODO 1.4: handle screen exiting -- set screen invisible if other screens exist, Qt.quit() if only screen left~~
- TODO 1.5: implement tab-completion 
- TODO 1.6: buffer the readBuffer to wait for output end beforeemitting the WHOLE output to screen -- which would fix the output of programs like `pstree` 
- TODO 1.7!: cannot send signals (SIGINT, SIGTSTP, etc.) when in root mode 
    - needs to crosscheck with TODO 1.2 to see if it's a problem of switching user in general and not just root 

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