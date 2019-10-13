# Artemis Iridium Tracker: Hardware Overview

The Artemis Iridium Tracker is an open source satellite tracker utilising the [SparkFun Artemis module](https://www.sparkfun.com/products/15484),
[Iridium 9603N satellite transceiver](https://www.iridium.com/products/iridium-9603/) and [u-blox ZOE-M8Q GNSS
](https://www.u-blox.com/en/product/zoe-m8-series).

The hardware design is based extensively on:
- [the SparkFun Artemis module](https://www.sparkfun.com/products/15484)
- [the SparkFun Thing Plus - Artemis](https://www.sparkfun.com/products/15574) battery charging circuit
- [the SparkFun RedBoard Artemis](https://www.sparkfun.com/products/15444) USB-C interface
- [the SparkFun Artemis design block]() AP3429 3.3V buck regulator circuit
- [the Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N) Iridium 9603N, LTC3225 supercapacitor charger and ADM4210 inrush current circuit
- [the SparkFun GPS Breakout - ZOE-M8Q](https://www.sparkfun.com/products/15193) ZOE connections and backup battery circuit

The design also makes use of:
- [the Skyworks AS179-92LF GaAs RF Switch](https://www.skyworksinc.com/products/switches/as179-92lf) as used on the [Iridium 9603N Solar Beacon](https://github.com/PaulZC/Iridium_9603N_Solar_Beacon)
- [the Maxtena M1600HCT-P-SMA antenna](https://www.maxtena.com/products/f-gps/m1600hct-p-sma/) which is tuned for Iridium, GPS and GLONASS
- [the TE / MEAS Switzerland MS8607](https://www.te.com/usa-en/product-CAT-BLPS0018.html) combined pressure, humidity and temperature sensor
- [the AP3429 buck converter](https://www.diodes.com/part/view/AP3429)

The full schmatic for the tracker can be found [here](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/Hardware/Schematic.pdf)

## The Artemis Module

The heart of the tracker is, of course, the most excellent Artemis module from SparkFun. The pad allocation is defined [here](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/Documentation/Hardware_Overview/ARTEMIS_PINS.md).

![Artemis](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Artemis.JPG)

## Power Options

The tracker can be powered from:
- the USB-C interface
- a LiPo battery (recharged via the USB-C interface)
- three Energizer® Ultimate Lithium AA cells (which will work down to -40C)

Low forward-voltage diodes isolate the power sources from each other. You can have all three connected simultaneously, it will do no harm.
The tracker will preferentially draw power from USB if it is connected.

Please be aware that if you have only the AA cells and LiPo connected simultaneously, the tracker will preferentially draw power from the AA cells.
If you have the AA cells installed, you may as well disconnect the LiPo.

![Power_Select](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Power_Select.JPG)

J4 can be used to measure the current draw or to connect a power switch after you open the MEAS split pad.

## USB Interface

The USB interface is taken directly from [the SparkFun RedBoard Artemis](https://www.sparkfun.com/products/15444).

![USB](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/USB.JPG)

## LiPo Charger

The LiPo charger circuit is taken directly from [the SparkFun RedBoard Artemis Nano](https://www.sparkfun.com/products/15443).

![LiPo](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/LiPo.JPG)

## 3.3V Buck Regulator

Power for the tracker is regulated by an AP3429A buck regulator. Its 90uA quiescent current draw means it can be powered continuously without
depleting the batteries.

![Buck_Reg](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Buck_Reg.JPG)










