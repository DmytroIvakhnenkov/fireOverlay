Productivity Overlay, Tested on Arch Linux + Hyprland, written in GTK4

Make sure to compile overlay.c with "gcc overlay.c -o overlay $(pkg-config --cflags --libs gtk4 gtk4-layer-shell-0) -lm"

Prerequisites: "yay -S gtk4-layer-shell"

Add this to UserConfigs/UserKeybinds.conf for quick usage and data logging:

bind = $mainMod SHIFT, D, exec, tmux new-session -d -s overlaySession $HOME/fireOverlay/deep.sh
bind = $mainMod SHIFT, D, exec, python3 $HOME/fireOverlay/startWork.py
bind = $mainMod ALT, D, exec, tmux kill-session -t overlaySession
bind = $mainMod ALT, D, exec, python3 $HOME/fireOverlay/endWork.py
