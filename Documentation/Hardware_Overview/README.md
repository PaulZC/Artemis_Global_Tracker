# Artemis Iridium Tracker: Hardware Overview

The Artemis Iridium Tracker is an open source satellite tracker utilising the [SparkFun Artemis module](https://www.sparkfun.com/products/15484),
[Iridium 9603N satellite transceiver](https://www.iridium.com/products/iridium-9603/) and [u-blox ZOE-M8Q GNSS
](https://www.u-blox.com/en/product/zoe-m8-series).

The hardware design is based extensively on the:
- [SparkFun Artemis module](https://www.sparkfun.com/products/15484)
- [SparkFun Thing Plus - Artemis](https://www.sparkfun.com/products/15574) battery charging circuit and USB-C interface
- SparkFun Artemis design block [AP3429](https://www.diodes.com/part/view/AP3429) 3.3V buck regulator circuit
- [Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N) Iridium 9603N, LTC3225 supercapacitor charger and ADM4210 inrush current circuit
- [SparkFun GPS Breakout - ZOE-M8Q](https://www.sparkfun.com/products/15193) ZOE connections and backup battery circuit

The design also makes use of the:
- [Skyworks AS179-92LF GaAs RF Switch](https://www.skyworksinc.com/products/switches/as179-92lf) as used on the [Iridium 9603N Solar Beacon](https://github.com/PaulZC/Iridium_9603N_Solar_Beacon)
- [Maxtena M1600HCT-P-SMA antenna](https://www.maxtena.com/products/f-gps/m1600hct-p-sma/) which is tuned for Iridium, GPS and GLONASS
- [TE / MEAS Switzerland MS8607](https://www.te.com/usa-en/product-CAT-BLPS0018.html) combined pressure, humidity and temperature sensor

The full schematic for the tracker can be found [here](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/Hardware/Schematic.pdf)

## The Artemis Module

The heart of the tracker is, of course, the most excellent Artemis module from SparkFun. The pad allocation is defined [here](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/Documentation/Hardware_Overview/ARTEMIS_PINS.md).

![Artemis](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Artemis.JPG)

## Power Options

The tracker can be powered from:
- the USB-C interface
- a LiPo battery (recharged via the USB-C interface)
- three Energizer® Ultimate Lithium AAA cells (which will work down to -40C)
- an external solar panel or battery pack (5.5V maximum)

Low-forward-voltage diodes isolate the power sources from each other. You can have the USB, LiPo and AAA cells connected simultaneously, it will do no harm.
The tracker will preferentially draw power from USB if it is connected.

If the USB is disconnected, the tracker will preferentially draw power from the AAA cells. If you have the AAA cells installed, you may as well disconnect the LiPo.

The connector for the external solar panel is connected in parallel with the three AAA cells. Please use either the AAA cells or the external supply, not both!

![Power_Select](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Power_Select.JPG)

J4 can be used to measure the current draw or to connect a power switch after you open the MEAS split pad.

## USB Interface

The USB interface is taken directly from the [SparkFun Thing Plus - Artemis](https://www.sparkfun.com/products/15574).

![USB](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/USB.JPG)

## LiPo Charger

The LiPo charger circuit is taken directly from the [SparkFun Thing Plus - Artemis](https://www.sparkfun.com/products/15574).

![LiPo](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/LiPo.JPG)

## 3.3V Buck Regulator

3.3V power for the tracker is regulated by an AP3429A buck regulator. Its 90uA quiescent current draw means it can be powered continuously without
depleting the batteries during sleep.

![Buck_Reg](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Buck_Reg.JPG)

If you want to completely disable the buck regulator to minimise the current draw, you can do so by opening the 3V3EN split pad. The EN pin can then be used to enable/disable the regulator.
Pull EN low to disable the regulator, pull it up to VIN (labelled as VBUS on J4) to enable it. EN could be driven by a logic signal from an external timer circuit.

![Buck_Reg_EN](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Buck_Reg_EN.JPG)

## ZOE-M8Q

GNSS data is provided by the u-blox ZOE-M8Q as used on the [SparkFun GPS Breakout - ZOE-M8Q](https://www.sparkfun.com/products/15193).

Connection to the Artemis is via I2C port 1. The serial and safeboot pins are available on test pads to allow the ZOE firmware to be updated.

Geofence alerts from the ZOE can be produced on PIO14, which is connected to Artemis pin D10.

![ZOE](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/ZOE.JPG)

Back-up power for the ZOE is drawn preferentially from the 3.3V rail, but there is a small back-up battery too to keep the ZOE's clock running
when all other power sources have been disconnected. The battery recharges only when USB power is connected (to help minimise the 3.3V current
draw during sleep).

![VBCKP](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/VBCKP.JPG)

3.3V power for the ZOE is switched via a FET. The same switched power also feeds the antenna switch when the GNSS is in use.

![GPS_EN](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/GPS_EN.JPG)

## Iridium 9603N

The tracker uses the same Iridium 9603N transceiver as the [Qwiic Iridium 9603N](https://github.com/PaulZC/Qwiic_Iridium_9603N).

![9603N](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/9603N.JPG)

Power for the 9603N is provided by the same LTC3225 supercapacitor charger and ADM4210 in-rush current limit circuit as used by the Qwiic Iridium.

![LTC3225](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/LTC3225.JPG)

## Antenna Switch

The ZOE and Iridium 9603N share the antenna via a [Skyworks AS179-92LF GaAs RF Switch](https://www.skyworksinc.com/products/switches/as179-92lf)
which has been flight tested on the [Iridium 9603N Solar Beacon](https://github.com/PaulZC/Iridium_9603N_Solar_Beacon).

Care needs to be taken that the 3.3V GNSS and 5.3V 9603N are not powered up simultaneously as _bad things might happen to the AS179_.

![RF](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/RF.JPG)

## Pressure, Humidity and Temperature Sensor

Pressure, humidity and temperature readings are provided by a [TE / MEAS Switzerland MS8607](https://www.te.com/usa-en/product-CAT-BLPS0018.html) combined sensor.
The MS8607 shares I2C port 1 with the ZOE.

The sensor will provide pressure readings as low as 10mbar which is equivalent to an altitude of approximately 31,000m.

![PHT](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/PHT.JPG)

## I/O Pins

The Artemis' SPI and I2C (port 1) pins are broken out on pin headers so the user can connect external peripherals.

![Pins](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Pins.JPG)

I2C port 4 is broken out on a standard SparkFun Qwiic connector.

![Qwiic](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Qwiic.JPG)

## Bus Voltage

The bus voltage (from the USB, LiPo or AAA cells) can be measured via the Artemis pin AD13. A simple two resistor divider divides the bus voltage by three.
Power to the resistor divider is switched by an N-FET so the power draw can be minimised during sleep.

![Bus_V](https://github.com/PaulZC/Artemis_Iridium_Tracker/blob/master/img/Bus_V.JPG)








