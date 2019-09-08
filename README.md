# EPAR EMITTER HACK

## The Eagle project of the epar project + and modified firmware

**(c) Found the entire project here : http://furrtek.free.fr/?a=epar**


## For futher

As we know, only 350 cities (minimum), have been equipped with this system, (which makes 10% of the total cities in France in 2014). To have this famous cities code which is coded on 8 bits (so 255 possibilities), it is necessary to scan the frequencies 31MHz to 32MHz, which is tedious, this repo proposes to you a "brut-force", the program has been modified so that the 255 possibilities are tested. Once you have the code you can directly inform it in the remote control, it will go faster. This is just the theory, I have not tried it yet, but I would update this readme as soon as I have some pretty big breakthroughs.


## Included:

- All needed files to modify the project
- BOM (see in /pcb/)
- 🚧 Modified firmware (see in /firmware/) in WIP

## To use it :

1. Buy the pcb then solder every composants.
2. Flash the Atmega8 with *eparremote.hex* with these fuses :

- Low: *0xE4*, High: *0xD9*
## Some img's :

<img src="https://raw.githubusercontent.com/StoneSet/furrtek_ecpr/master/img/schematics.png" alt="alt" width="600">

<img src="http://files.stoneset.fr/others/ecpr_furrtek/img/transmit.JPG" alt="alt" height="500"> <img src="/img/animate.gif" alt="alt" weight="800">

### To dot it, you will need :

| Components | list |
|--|--|
| x1 Atmega8 | x1 78L05 regulator |
| x1 10k potentiometer | x4 12mm switch |
| x1 0802 screen | x1 AD9850 |
| x1 10k resistor | x1 47uF capacitor |
| x1 100uH coil | x1 Green led |
| x1 1nF capacitor | x1 10nF capacitor |
| x1 BFR96 | x1 47k resistor |
| x1 SPDT switch | x1 FM antenna |
| x1 9v clip | x1 1N4001 diode |


Tested, it will work perfectly !

#

(c) @furrtek & @StoneSet & @lefuturiste - http://furrtek.free.fr/?a=esl - http://stoneset.github.io/ (2019)

plz do not steal, i'm not the owner of this project, i'm only the owner of this pcb. Have fun !
