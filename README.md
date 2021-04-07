# Arudino timer
=======

Arduino relay timer

#### Components:
* lcd 1602 display with i2c adapter
* relay (low level)
* 220 to 5v 0.7A adapter
* rosette
* 2 switch buttons
* 4 push buttons
* 1 big push button
* 5 resistors 10кОм
* ac-11 power connector 
* a lot of cabels
* box

#### Pinouts
###### LCD 1602 with i2c
* SDA - A4
* SCL - A5

###### relay
* Sig pin - D5 

###### Buttons
* D2, D3, D4, D5, D7 pins

### Scheme
//TODO

### Assembly
//TODO

### Usage
D2/D3 buttons to increase/decrease current value
D4 change mode button
D5 start timer with current settings
D7 change current submenu item (for "Delay with stop" mode)
switch1 - powerup source without relay
switch2 - lcd backlight switch

### Modes
* **Delay** mode - simple timer
    - setup delay time with D2/D3 buttons
    - D5 to start
    - D4 - switch to **Delay with stop** mode
* **Delay with stop** mode - timer with intervals and stop time. Timer will trigger for "D" seconds
every "S" time with stop intervals of "T" seconds. 
    - D2/D3 to change current setting value
    - D7 to change setting type
    - D5 to start
    - D4 - switch to **Relay** mode

