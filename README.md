# ME-System_m5stickC
A server which shows your ME system items to you on a M5StickC.
Let me start off and tell you, this stack is a mess. Anyhow when everything is connected, the screen will cycle through your ME system for large quantities of items(like what you're mining).

## The projects
There are 3 projects in this repo
* The computercraft/ME System connector
* A fairly simple websocket server which glues them together(probably could've been built in minecraft or the M5, but for simplicity's sake)
* The websocket message client on the M5 (You could run this independently for other projects with ease)

## The stack
To connect the device to a block in-game, you have to connect the device like so.

The M5StickC will connect to your computer using the Windows Wi-Fi hotspot feature, or you can change the IP addresses to use your home wifi.

The M5, will then connect to the websocket server and send a registration message, and recieves a confirmation which tells it to leave the connection interface.

Computercraft will be hooked to your ME Controller and will connect and broadcast items as it iterates through your system. 

## Making this yourself
The tools are in front of you, but here's some details.
This was built on the [The 1.12.2 Pack: Perfectly Complex](https://www.curseforge.com/minecraft/modpacks/the-1122-pack) modpack, but I think it just needs Applied Energistics, Computercraft and Plethora.

To flash your M5, open the M5Code directory in platformIO. Change the Wi-Fi creds and IP address information as needed, you can remove the static IP stuff, but it was needed for my implementation.

On the python end, just `pip install -r requirements.txt` and `python ws.py`. Verify this is working by booting up the M5 and see if it connects. If it fails you can use the side button to reset it.

On the computercraft end, download the lua file, place the ME controller on the right side(or change the code) and fire it up. 

If all goes well, it should show "CC Connected" and then start displaying items. 


## What it looks like
<img src=demo.jpg width="381" height="806">

## Hacking and reusing code
The M5 and websocket code can be used fairly easially in other projects, I originally designed the M5 part for general stuff and decided to hook it to the ME system. 

With the M5 connected to the websocket, you can send out a message in this comma-delimited format,

```LINE 1,FONT SIZE 1,FONT 1,LINE 2,FONT SIZE 2,FONT 2```

```Example,2,1,This is an example,1,1```

If you use a font size of 1, the M5 will do the best it can to fit your text. So any strings that exceed 12 characters should use a font size of 1.

Happy hacking!

